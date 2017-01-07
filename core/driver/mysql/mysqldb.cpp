#include "mysqldb.h"
#include "mysqldriver.h"
#include "mysqltypes.h"
#include "driver/diff.h"
#include "spisfilter.h"

#include <unistd.h>

#include <QDebug>
#include <QRegularExpression>
#include <QSqlError>

using namespace spis;
using namespace spis::driver;

static void dumpError(const QSqlQuery &q, const char* file, int line)
{
	if (isatty(STDERR_FILENO))
	{
		fprintf(stderr, "\033[1mSPIS[MySQL]: \033[31mError\033[0m in \033[32m%s:%d\033[0m:\n", file, line);
		fprintf(stderr, "    \033[1;31mQuery:\033[0m %s\n", qPrintable(q.lastQuery()));
		fprintf(stderr, "    \033[1;31mError:\033[0m %s\n", qPrintable(q.lastError().text()));
	}
	else
	{
		fprintf(stderr, "SPIS[MySQL]: Error in %s:%d:\n", file, line);
		fprintf(stderr, "    Query: %s\n", qPrintable(q.lastQuery()));
		fprintf(stderr, "    Error: %s\n", qPrintable(q.lastError().text()));
	}
}
#define DUMP_ERROR(q) dumpError((q), __FILE__, __LINE__);

MySQLDatabase::MySQLDatabase(MySQLDriver *driver, const char *charset, bool usevar)
	: QtDatabase(charset, usevar, "QMYSQL")
	, driver(driver)
{
}

void MySQLDatabase::loadTableInfo()
{
#ifdef CMAKE_DEBUG
	qDebug() << "SPIS[MySQL]: Starting loadTableInfo()";
#endif
	QSqlQuery tables(db());
	if (!tables.exec("SHOW TABLES;"))
	{
		DUMP_ERROR(tables);
		return;
	}
	if (!tables.first())
		return;
	do
	{
		QByteArray tblName = tables.value(0).toByteArray();
		QList<SPISColumn> cols;
		QByteArray pk;
		
		QSqlQuery columns(db());
		if (!columns.exec("SHOW COLUMNS FROM `" + tblName + "`;"))
		{
			DUMP_ERROR(columns);
			continue;
		}
		if (!columns.first())
			continue;
		do
		{
			uint8_t constraints = SPIS::none;
			if (columns.value("Null").toString() == "NO")
				constraints |= SPIS::notnull;
			if (columns.value("Key").toString() == "PRI")
			{
				constraints |= SPIS::primarykey | SPIS::unique;
				pk = columns.value("Field").toByteArray();
			}
			else if (columns.value("Key").toString() == "UNI")
				constraints |= SPIS::unique;
			auto type = MySQLTypes::fromSQL(columns.value("Type").toByteArray());
			SPISColumn col(columns.value("Field").toByteArray(), type.first, type.second, constraints, columns.value("Default"));
			cols << col;
		}
		while (columns.next());
		
		SPISTable tbl(tblName.data(), pk, 0);
		for (auto c : cols)
			tbl.addColumn(c);
		addTable(tbl);
	}
	while (tables.next());
}

bool MySQLDatabase::ensureTable(const SPISTable &tbl)
{
	bool success = ensureTableImpl(tbl);
	if (success)
		addTable(tbl); // otherwise subsequent calls to ensureTable will repeat the changes
	return success;
}

static QString mysqlCharset(const QByteArray &charset)
{
	if (charset == "utf-8")
		return "utf8mb4";
	if (charset == "utf-16" || charset == "utf-16be")
		return "utf16";
	if (charset == "utf-16le")
		return "utf16le";
	return "";
}

QString MySQLDatabase::typeDefinition(const SPISTable &tbl, const SPISColumn &col)
{
	QString td = "`" + col.name() + "` " + MySQLTypes::fromSPIS(tbl.db(), col.type(), col.minsize(), usevar());
	if ((col.constraints() & SPIS::notnull) == SPIS::notnull)
		td += " NOT NULL";
	if ((col.constraints() & SPIS::unique) == SPIS::unique)
	{
		if (strcoll(col.type(), "text") == 0 || strcoll(col.type(), "blob") == 0)
			qWarning() << "SPIS[MySQL]: Column" << col.name() << "has a unique constraint but mysql doesn't support indexes on text/blob fields";
		else
			td += " UNIQUE";
	}
	if (col.def().isValid() && !col.def().isNull())
	{
		td += " DEFAULT ";
		if (col.type() == "date")
			td += "'" + driver->fromQDate(col.def().toDate()).toString() + "'";
		else if (col.type() == "time")
			td += "'" + driver->fromQTime(col.def().toTime()).toString() + "'";
		else if (col.type() == "datetime")
			td += "'" + driver->fromQDateTime(col.def().toDateTime()).toString() + "'";
		else if (needsEnquote(col.type()))
			td += "'" + col.def().toString().replace("'", "''") + "'";
		else
		{
#ifdef CMAKE_DEBUG
			if (QRegularExpression("[^0-9a-zA-Z\\.,\\-+]").match(col.def().toString()).hasPartialMatch())
				qDebug(__FILE__":%d: Though needsEnquote() returned false the default value contains non-alphanumerical chars, type is: %d", __LINE__, col.def().type());
#endif
			td += col.def().toString().replace(QRegularExpression("[^0-9a-zA-Z\\.,\\-+]"), "");
		}
	}
	return td;
}

bool MySQLDatabase::ensureTableImpl(const SPISTable &tbl)
{
	if (!containsTable(tbl.name()))
	{
		QString query = "CREATE TABLE `" + tbl.name() + "` (";
		for (int i = 0; i < tbl.columns().size(); i++)
		{
			if (i != 0)
				query += ",";
			query += typeDefinition(tbl, tbl.columns()[i]);
		}
		if (!tbl.primaryKey().isEmpty())
			query += ", PRIMARY KEY (`" + tbl.primaryKey() + "`)";
		for (auto c : tbl.columns())
			if (c.type()[0] == '&')
			{
				QByteArray t = c.type().mid(1);
				query += ", FOREIGN KEY (`" + c.name() + "`) REFERENCES `" + t.mid(0, t.indexOf('.')) + "`(`" + t.mid(t.indexOf('.')+1) + "`)";
			}
		query += ")";
		QString cs = mysqlCharset(charset());
		if (!cs.isEmpty())
			query += " DEFAULT CHARSET=" + cs;
		query += ";";
		
		QSqlQuery createq(db());
		if (!createq.exec(query))
		{
			DUMP_ERROR(createq);
			return false;
		}
		return true;
	}
	
	TableDiff diff(table(tbl.name()), tbl);
	QString query = "ALTER TABLE `" + tbl.name() + "` ";
	int qi = 0;
	
	for (auto col : diff.addedCols())
	{
		qi++;
		query += "ADD COLUMN " + typeDefinition(tbl, col) + ",";
	}
	
	for (auto col : diff.removedCols())
	{
		qi++;
		query += "DROP COLUMN `" + col.name() + "`,";
	}
	
	for (auto col : diff.typeChanged()) // TODO what if the new type is a fkey
	{
		qi++;
		query += "MODIFY COLUMN " + typeDefinition(tbl, col) + ",";
	}
	
	for (auto col : diff.defChanged())
	{
		qi++;
		query += "MODIFY COLUMN " + typeDefinition(tbl, col) + ",";
	}
	
	for (auto conDif : diff.constraintsChanged())
	{
		auto col = tbl.column(conDif.colName());
		QByteArray type = col.type();
		
		if ((conDif.constraintsAdded() & SPIS::notnull) == SPIS::notnull  ||
				(conDif.constraintsRemoved() & SPIS::notnull) == SPIS::notnull)
		{
			qi++;
			query += "MODIFY COLUMN " + typeDefinition(tbl, col) + ",";
			continue;
		}
		
		if (type == "text" || type == "blob") // dont need to look at unique
		{
			qWarning() << "SPIS[MySQL]: Column" << col.name() << "has a unique constraint but mysql doesn't support indexes on text/blob fields";
			continue;
		}
		
		if ((conDif.constraintsAdded() & SPIS::unique) == SPIS::unique)
		{
			qi++;
			query += "ADD UNIQUE `" + conDif.colName() + "`,";
		}
		
		if ((conDif.constraintsRemoved() & SPIS::unique) == SPIS::unique)
		{
			qi++;
			query += "DROP INDEX `" + conDif.colName() + "`,";
		}
	}
	
	if (table(tbl.name()).primaryKey() != tbl.primaryKey())
	{
		query += "DROP PRIMARY KEY,ADD PRIMARY KEY `" + tbl.primaryKey() + "`,";
		qi+=2;
	}
	
	if (qi == 0)
		return true;
#ifdef CMAKE_DEBUG
	qDebug() << "SPIS[MySQL]: Altering table" << tbl.name() << query;
#endif
	query = query.mid(0, query.size()-1) + ";";
	QSqlQuery alterq(db());
	if (!alterq.exec(query))
	{
		DUMP_ERROR(alterq);
		return false;
	}
	return true;
}

bool MySQLDatabase::needsEnquote(const QByteArray &type)
{
	QByteArray t = type.trimmed().toLower();
	return (t=="char" || t=="text" || t=="password" || t=="byte" || t=="blob" || t=="variant" || t=="date" || t=="time" || t=="datetime");
}

QString MySQLDatabase::filterSQL(const SPISTable &tbl, const SPISFilter &filter)
{
	if (filter.op() == SPISFilter::noop)
		return QString();
	QString sql;
	
	if (filter.op() < 0x20)
	{
		sql += "(`" + tbl.name() + "`.`" + filter.arg(0) + "`";
		switch (filter.op())
		{
		case SPISFilter::eq: sql += " =  "; break;
		case SPISFilter::ne: sql += " <> "; break;
		case SPISFilter::lt: sql += " <  "; break;
		case SPISFilter::le: sql += " <= "; break;
		case SPISFilter::gt: sql += " >  "; break;
		case SPISFilter::ge: sql += " >= "; break;
		case SPISFilter::like: sql += " LIKE "; break;
		default:
			fprintf(stderr, "SPIS[MySQL]: Unknown filter operator value 0x%02x\n", filter.op());
			return QString();
		}
		
		QString arg = filter.arg(1);
		if (arg.startsWith("int:") || arg.startsWith("double:"))
			sql += arg.mid(arg.indexOf(':') + 1);
		else if (arg.startsWith("'") && arg.endsWith("'"))
			sql += "'" + arg.mid(1, arg.size() - 2).replace("'", "''") + "'";
		else if (strcoll(tbl.column(arg.toUtf8()).type(), "invalid") == 0)
			sql += "'" + arg.replace("'", "''") + "'";
		else
			sql += "`" + tbl.name() + "`.`" + arg + "`";
		
		sql += ")";
	}
	
	else if (filter.op() < 0x30)
	{
		sql += "(`" + tbl.name() + "`.`" + filter.arg(0) + "`";
		switch (filter.op())
		{
		case SPISFilter::isnull: sql += " IS NULL "; break;
		case SPISFilter::notnull: sql += " IS NOT NULL "; break;
		default:
			fprintf(stderr, "SPIS[SQLite]: Unknown filter operator value 0x%02x\n", filter.op());
			return QString();
		}
	}
	
	else if (filter.op() < 0x40)
	{
		sql += "(" + filterSQL(tbl, filter.filter(0));
		switch (filter.op())
		{
		case SPISFilter::op_and: sql += " AND "; break;
		case SPISFilter::op_or: sql += " OR "; break;
		default:
			fprintf(stderr, "SPIS[SQLite]: Unknown filter operator value 0x%02x\n", filter.op());
			return QString();
		}
		sql += filterSQL(tbl, filter.filter(1)) + ")";
	}
	
	else
	{
		switch (filter.op())
		{
		case SPISFilter::op_not: sql += "NOT "; break;
		default:
			fprintf(stderr, "SPIS[SQLite]: Unknown filter operator value 0x%02x\n", filter.op());
			return QString();
		}
		sql += filterSQL(tbl, filter.filter(0));
	}
	
	return sql;
}

SelectResult* MySQLDatabase::selectTable(const SPISTable &tbl, const QList<SPISColumn> &cols, const SPISFilter &filter,
										 const QList<SPISJoinTable> &join, int limit, bool asc, const QByteArray &order)
{
	QSqlQuery q(db());
	QString qq = "SELECT ";
	for (int i = 0; i < cols.size(); i++)
	{
		if (i != 0)
			qq += ", ";
		qq += "`" + tbl.name() + "`.`" + cols[i].name() + "` AS `" + cols[i].name() + "`";
	}
	for (auto j : join)
		for (auto col : j.cols)
			qq += ", `" + j.tbl.name() + "`.`" + col.name() + "` AS `" + j.prefix + col.name() + "`";
	qq += " FROM `" + tbl.name() + "`";
	for (auto j : join)
		qq += " INNER JOIN `" + j.tbl.name() + "` ON `" + tbl.name() + "`.`" + j.on.name()
				+ "`=`" + j.tbl.name() + "`.`" + j.onTbl.name() + "`";
	
	QString fsql = filterSQL(tbl, filter);
	if (!fsql.isEmpty())
		qq += " WHERE " + fsql;
	if (!order.isEmpty() || !tbl.primaryKey().isEmpty())
	{
		qq += " ORDER BY `" + tbl.name() + "`.`" + (order.isEmpty() ? tbl.primaryKey() : order) + "`";
		if (asc)
			qq += " ASC";
		else
			qq += " DESC";
	}
	if (limit > 0)
		qq += " LIMIT " + QString::number(limit);
	qq += ";";
	if (!q.exec(qq))
	{
		DUMP_ERROR(q);
		return 0;
	}
	return new QtSelectResult(q);
}

bool MySQLDatabase::insertIntoTable(const SPISTable &tbl, const QList<SPISColumn> &cols, const QVector<QVector<QVariant> > &rows)
{
	Q_ASSERT(!cols.empty());
	
	QSqlQuery q(db());
	QString qq = "INSERT INTO `" + tbl.name() + "` (";
	for (int i = 0; i < cols.size(); i++)
	{
		if (i != 0)
			qq += ",";
		qq += "`" + cols[i].name() + "`";
	}
	qq += ") VALUES ";
	for (int i = 0; i < rows.size(); i++)
	{
		if (i != 0)
			qq += ",";
		qq += "(";
		auto row = rows[i];
		for (int j = 0; j < cols.size(); j++)
		{
			if (j != 0)
				qq += ",";
			bool enquote = needsEnquote(cols[j].type());
			if (enquote)
				qq += "'" + row[j].toString().replace("'", "''") + "'";
			else
				qq += row[j].toString().replace(QRegularExpression("[^0-9a-zA-Z\\.,\\-+]"), "");
		}
		qq += ")";
	}
	qq += ";";
	if (!q.exec(qq))
	{
		DUMP_ERROR(q);
		return false;
	}
	return true;
}

bool MySQLDatabase::updateTable(const SPISTable &tbl, const QMap<SPISColumn, QVariant> &values, const QVector<QVariant> &pks)
{
	if (pks.empty() || values.empty())
		return true;
	
	QSqlQuery q(db());
	
	QString qq = "UPDATE `" + tbl.name() + "` SET ";
	for (auto key : values.keys())
	{
		qq += "`" + key.name() + "`=";
		if (needsEnquote(key.type()))
			qq += "'" + values[key].toString().replace("'", "''") + "'";
		else
			qq += values[key].toString().replace(QRegularExpression("[^0-9a-zA-Z\\.,\\-+]"), "");
		qq += ",";
	}
	qq = qq.mid(0, qq.size()-1);
	
	qq += " WHERE ";
	for (auto pk : pks)
		qq += "(`" + tbl.primaryKey() + "`=" + QByteArray::number(pk.toInt()) + ") OR "; // note that currently only int and uint as pks are allowed
	qq = qq.mid(0, qq.size()-4);
	
	qq += ";";
	if (!q.exec(qq))
	{
		DUMP_ERROR(q);
		return false;
	}
	return true;
}

bool MySQLDatabase::deleteFromTable(const SPISTable &tbl, const SPISFilter &filter)
{
	QSqlQuery q(db());
	QString qq = "DELETE FROM `" + tbl.name() + "`";
	QString fsql = filterSQL(tbl, filter);
	if (!fsql.isEmpty())
		qq += " WHERE " + fsql;
	qq += ";";
	if (!q.exec(qq))
	{
		DUMP_ERROR(q);
		return false;
	}
	return true;
}

bool MySQLDatabase::deleteFromTable(const SPISTable &tbl, const QVector<QVariant> &pks)
{
	Q_ASSERT(!tbl.primaryKey().isEmpty());
	if (pks.empty())
		return true;
	
	QSqlQuery q(db());
	QString qq = "DELETE FROM `" + tbl.name() + "` WHERE ";
	for (int i = 0; i < pks.size(); i++)
	{
		if (i != 0)
			qq += " OR ";
		qq += "(`" + tbl.name() + "`.`" + tbl.primaryKey() + "`=" + QByteArray::number(pks[i].toInt()) + ")"; // note that currently only int and uint as pks are allowed
	}
	qq += ";";
	if (!q.exec(qq))
	{
		DUMP_ERROR(q);
		return false;
	}
	return true;
}
