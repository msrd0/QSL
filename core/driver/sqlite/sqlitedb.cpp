#include "spisfilter.h"
#include "sqlitedb.h"
#include "sqlitedriver.h"
#include "sqlitetypes.h"
#include "driver/diff.h"

#include <unistd.h>

#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QRegularExpression>
#include <QSqlError>


using namespace spis;
using namespace spis::driver;

static void dumpError(const QSqlQuery &q, const char* file, int line)
{
	if (isatty(STDERR_FILENO))
	{
		fprintf(stderr, "\033[1mSPIS[SQLite]: \033[31mError\033[0m in \033[32m%s:%d\033[0m:\n", file, line);
		fprintf(stderr, "    \033[1;31mQuery:\033[0m %s\n", qPrintable(q.lastQuery()));
		fprintf(stderr, "    \033[1;31mError:\033[0m %s\n", qPrintable(q.lastError().text()));
	}
	else
	{
		fprintf(stderr, "SPIS[SQLite]: Error in %s:%d:\n", file, line);
		fprintf(stderr, "    Query: %s\n", qPrintable(q.lastQuery()));
		fprintf(stderr, "    Error: %s\n", qPrintable(q.lastError().text()));
	}
}
#define DUMP_ERROR(q) dumpError((q), __FILE__, __LINE__);


SQLiteDatabase::SQLiteDatabase(SQLiteDriver *driver, const char *charset, bool usevar)
	: QtDatabase(charset, usevar, "QSQLITE")
	, driver(driver)
{
}

bool SQLiteDatabase::connect()
{
	if (!QtDatabase::connect())
		return false;
#ifdef CMAKE_DEBUG
	qDebug() << "SPIS[SQLite]: Conncted to" << db().databaseName();
#endif
	
	// enable fkeys
	QSqlQuery qfkey(db());
	if (!qfkey.exec("PRAGMA foreign_keys = ON;"))
	{
		DUMP_ERROR(qfkey);
		disconnect();
		return false;
	}
	
	// handle the charset
	QByteArray enc = charset();
	enc = enc.toLower();
	if (enc == "utf-8")
		enc = "UTF-8";
	else if (enc == "utf-16")
		enc = "UTF-16";
	else if (enc == "utf-16le")
		enc = "UTF-16le";
	else if (enc == "utf-16be")
		enc = "UTF-16be";
	else
	{
		qWarning() << "SPIS[SQLite]: Encoding" << enc << "is currently not supported. Please only report if";
		qWarning() << "              the encoding is listet at http://www.sqlite.org/pragma.html#pragma_encoding";
		enc = "";
	}
	if (!enc.isEmpty())
	{
		QSqlQuery qenc(db());
		if (!qenc.exec("PRAGMA encoding = \"" + enc + "\";"))
		{
			DUMP_ERROR(qenc);
			qWarning() << "SPIS[SQLite]: Failed to set encoding. Will use the default provided by SQLite.";
		}
	}
	
	return true;
}

void SQLiteDatabase::loadTableInfo()
{
#ifdef CMAKE_DEBUG
	qDebug() << "SPIS[SQLite]: Starting loadTableInfo()";
#endif
	QSqlQuery tables(db());
	if (!tables.exec("SELECT name FROM sqlite_master WHERE type='table';"))
	{
		DUMP_ERROR(tables);
		return;
	}
	
	if (!tables.first())
		return;
	do
	{ 
		QByteArray tblName = tables.value("name").toByteArray();
		if (tblName.contains("___spisbackup_"))
			continue;
		
		QByteArray pk;
#ifdef CMAKE_DEBUG
		qDebug() << "SPIS[SQLite]: Detecting table" << tblName;
#endif
		// query the table info
		QSqlQuery tblInfo(db());
		if (!tblInfo.exec("PRAGMA table_info(" + tblName + ");"))
		{
			DUMP_ERROR(tblInfo);
			continue;
		}
		if (!tblInfo.first())
			continue;
		// query all indexes of the table
		QSqlQuery indexList(db());
		if (!indexList.exec("PRAGMA index_list(" + tblName + ");"))
		{
			DUMP_ERROR(indexList);
			continue;
		}
		
		// create the columns. note that tblInfo.first() was already called
		QMap<QByteArray, MutableColumn> columns;
		do
		{
			auto type = SQLiteTypes::fromSQL(tblInfo.value("type").toByteArray());
			MutableColumn col(tblInfo.value("name").toByteArray(), type.first, type.second, tblInfo.value("dflt_value"));
#ifdef CMAKE_DEBUG
			qDebug() << "SPIS[SQLite]: Adding column" << col.name();
#endif
			if (tblInfo.value("notnull") == "1")
				col.addConstraint(SPIS::notnull);
			if (tblInfo.value("pk") == "1")
			{
				col.addConstraint(SPIS::primarykey);
				pk = col.name();
			}
			columns.insert(col.name(), col);
		}
		while (tblInfo.next());
		
		// gather information about the normal indexes
		if (indexList.first())
		{
			do
			{
#ifdef CMAKE_DEBUG
				qDebug() << "SPIS[SQLite]: Looking at index" << indexList.value("name").toString();
#endif
				if (indexList.value("unique") == "1")
				{
					// sqlite doesn't tell us which column so lets find out
					QSqlQuery q(db());
					if (!q.exec("PRAGMA index_info(\"" + indexList.value("name").toString() + "\");"))
					{
						DUMP_ERROR(q);
						continue;
					}
					if (!q.first())
						continue;
					
					if (indexList.value("origin").toByteArray() == "c") // I only care about CREATE INDEX indexes
						uniqueIndexNames.insert(tblName + "." + q.value("name").toByteArray(), indexList.value("name").toByteArray());
					
					auto it = columns.find(q.value("name").toByteArray());
					if (it != columns.end())
						it->addConstraint(SPIS::unique);
				}
			}
			while (indexList.next());
		}
		
		// create and add the table
		SPISTable tbl(tblName.data(), pk, 0);
		for (auto col : columns.values())
			tbl.addColumn(col);
		addTable(tbl);
	}
	while (tables.next());
}

bool SQLiteDatabase::ensureTable(const SPISTable &tbl)
{
	SPISTable tbl0 = tbl;
	bool success = ensureTableImpl(tbl0);
	if (success)
		addTable(tbl0); // otherwise subsequent calls to ensureTable will repeat the changes
	return success;
}

static QByteArray backupSuffixName(const QByteArray &name)
{
	return "___spisbackup_" + QCryptographicHash::hash(name + " @ " + QDateTime::currentDateTime().toString().toUtf8(), QCryptographicHash::Md5).toHex();
}

static QByteArray uniqueConstraintName(const QByteArray &tbl, const QByteArray &name)
{
	return "spis_sqlite_driver_constraint_unique_" + name + "_" + QCryptographicHash::hash(tbl + "." + name + " !unique"  + " @ " + QDateTime::currentDateTime().toString().toUtf8(), QCryptographicHash::Md5).toHex();
}

bool SQLiteDatabase::ensureTableImpl(SPISTable &tbl)
{
	Q_ASSERT(tbl.db());
	
	bool needsNewTable = !containsTable(tbl.name());
	const QByteArray backupSuffix = backupSuffixName(tbl.name());
	bool hasBackup = false;
	QList<SPISColumn> addedCols;
	
	if (!needsNewTable)
	{
		// if the table is empty there is no need to keep it
		QSqlQuery emptyq(db());
		if (emptyq.exec("SELECT * FROM \"" + tbl.name() + "\" LIMIT 1;"))
		{
			if (!emptyq.first())
			{
				emptyq.finish(); // otherwise table is locked
#ifdef CMAKE_DEBUG
				qDebug() << "SPIS[SQLite]: Dropping empty table" << tbl.name();
#endif
				QSqlQuery dropq(db());
				if (dropq.exec("DROP TABLE \"" + tbl.name() + "\";"))
					needsNewTable = true;
				else
					DUMP_ERROR(dropq);
			}
		}
		else
			DUMP_ERROR(emptyq);
	}
	
	if (!needsNewTable)
	{
		TableDiff diff(table(tbl.name()), tbl);
		
		/*
		 NOTE: Currently, SQLite only supports to add columns to a table when altering it. A
		 new unique constraint can be added with the create index query. SPIS doesn't enforce
		 to remove removed columns from the database, so this will be fine as long as the
		 column is nullable.
		 => We need to create a new table if and only if:
		    1. A constraint other than unique changed
		    2. The type of a column changed
		    3. A column with a not-null constraint was removed
			4. A column with a primary-key constraint was added
			5. A unique-constraint was removed that was not created by a CREATE INDEX statement
			6. A default value changed
		 => We need to fail if and only if:
		    A. A new column without a default value but not nullable was added and the table is
			   not empty
		*/
		
		// check A.
		//  (note that table is not empty if reaching this code)
		//  (note that currently default values are not supported by SPIS)
		for (auto col : diff.addedCols())
		{
			if ((col.constraints() & SPIS::notnull) == SPIS::notnull
					&& (!col.def().isValid() || col.def().isNull()))
			{
				qCritical() << "SPIS[SQLite]: In Table" << tbl.name() << ": ERROR: Column" << col.name() << "was added with !notnull but without a default value";
				return false;
			}
		}
		
		// check 2.
		if (!diff.typeChanged().isEmpty())
		{
			needsNewTable = true;
#ifdef CMAKE_DEBUG
			qDebug() << "SPIS[SQLite]: The types of the following columns changed, need to re-create table" << tbl.name();
			for (auto col : diff.typeChanged())
				qDebug() << "             -" << col.name();
#endif
		}
		
		// check 6.
		if (!diff.defChanged().isEmpty())
		{
			needsNewTable = true;
#ifdef CMAKE_DEBUG
			qDebug() << "SPIS[SQLite]: The default values of the following columns changed, need to re-create table" << tbl.name();
			for (auto col : diff.typeChanged())
				qDebug() << "             -" << col.name();
#endif
		}
		
		// check 1.
		if (!needsNewTable) // othewise we can skip
		{
			for (auto constraintDiff : diff.constraintsChanged())
			{
				if (constraintDiff.constraintsAdded() != SPIS::none && constraintDiff.constraintsAdded() != SPIS::unique)
				{
					needsNewTable = true;
					break;
				}
				if (constraintDiff.constraintsRemoved() != SPIS::none && constraintDiff.constraintsRemoved() != SPIS::unique)
				{
					needsNewTable = true;
					break;
				}
			}
			
#ifdef CMAKE_DEBUG
			if (needsNewTable)
				qDebug() << "SPIS[SQLite]: At least one constraint other than unique was added or removed, need to re-create table" << tbl.name();
#endif
		}
		
		// check 3.
		if (!needsNewTable) // otherwise we can skip
		{
			for (auto col : diff.removedCols())
				if ((col.constraints() & SPIS::notnull) == SPIS::notnull)
				{
#ifdef CMAKE_DEBUG
					qDebug() << "SPIS[SQLite]: At least one column with a notnull-constraint was removed, need to re-create table" << tbl.name();
#endif
					needsNewTable = true;
					break;
				}
		}
		
		// check 4.
		if (!needsNewTable) // otherwise we can skip
		{
			for (auto col : diff.addedCols())
				if ((col.constraints() & SPIS::primarykey) == SPIS::primarykey)
				{
#ifdef CMAKE_DEBUG
					qDebug() << "SPIS[SQLite]: The primary key changed, need to re-create table" << tbl.name();
#endif
					needsNewTable = true;
					break;
				}
		}
		
		// check 5.
		if (!needsNewTable) // otherwise we can skip
		{
			for (auto constraintDiff : diff.constraintsChanged())
			{
				if ((constraintDiff.constraintsRemoved() & SPIS::unique) == SPIS::unique)
				{
					if (!uniqueIndexNames.contains(tbl.name() + "." + constraintDiff.colName()))
					{
#ifdef CMAKE_DEBUG
						qDebug() << "SPIS[SQLite]: A unique-constraint not created by an CREATE INDEX statement was removed, need to re-create table" << tbl.name();
#endif
						needsNewTable = true;
						break;
					}
				}
			}
		}
		
		// if we need a new table, rename the old one
		if (needsNewTable)
		{
#ifdef CMAKE_DEBUG
			qDebug() << "SPIS[SQLite]: Creating backup of table" << tbl.name() << "-" << tbl.name() + backupSuffix;
#endif
			QSqlQuery renameq(db());
			if (!renameq.exec("ALTER TABLE \"" + tbl.name() + "\" RENAME TO \"" + tbl.name() + backupSuffix + "\";"))
			{
				DUMP_ERROR(renameq);
				return false;
			}
			hasBackup = true;
			addedCols = diff.addedCols();
		}
		
		// else, we need to alter the table according to the diff
		else
		{
			// first, add all columns
			for (auto col : diff.addedCols())
			{
#ifdef CMAKE_DEBUG
				qDebug() << "SPIS[SQLite]: Adding column" << col.name() << "to table" << tbl.name();
#endif
				QSqlQuery alterq(db());
				if (!alterq.exec("ALTER TABLE \"" + tbl.name() + "\" ADD COLUMN \"" + col.name() + "\" " + SQLiteTypes::fromSPIS(tbl.db(), col.type(), col.minsize(), usevar()) + ";"))
				{
					DUMP_ERROR(alterq);
					return false;
				}
				if ((col.constraints() & SPIS::unique) == SPIS::unique)
				{
#ifdef CMAKE_DEBUG
					qDebug() << "SPIS[SQLite]: Adding unique constraint to column" << col.name() << "of table" << tbl.name();
#endif
					QSqlQuery ciq(db());
					if (!ciq.exec("CREATE UNIQUE INDEX \"" + uniqueConstraintName(tbl.name(), col.name()) + "\" ON \"" + tbl.name() + "\"(\"" + col.name() + "\");"))
					{
						DUMP_ERROR(ciq);
						return false;
					}
				}
			}
			
			// now, add or remove all unique constraints on existing columns
			for (auto constraintDiff : diff.constraintsChanged())
			{
				if ((constraintDiff.constraintsAdded() & SPIS::unique) == SPIS::unique)
				{
					QSqlQuery ciq(db());
					if (!ciq.exec("CREATE UNIQUE INDEX \"" + uniqueConstraintName(tbl.name(), constraintDiff.colName()) + "\" ON \"" + tbl.name() + "\"(\"" + constraintDiff.colName() + "\");"))
					{
						DUMP_ERROR(ciq);
						return false;
					}
				}
				
				if ((constraintDiff.constraintsRemoved() & SPIS::unique) == SPIS::unique)
				{
					QByteArray indexName = uniqueIndexNames.value(tbl.name() + "." + constraintDiff.colName());
					if (indexName.isEmpty())
					{
						qCritical() << "SPIS[SQLite]: Attempt to remove unique constraint from" << tbl.name() + "." + constraintDiff.colName() + "but unable to find index name";
						return false;
					}
					QSqlQuery diq(db());
					if (!diq.exec("DROP INDEX \"" + indexName + "\";"))
					{
						DUMP_ERROR(diq);
						return false;
					}
				}
			}
			
			return true;
		}
	}
	
	Q_ASSERT(needsNewTable);
	
#ifdef CMAKE_DEBUG
	qDebug() << "SPIS[SQLite]: Creating table" << tbl.name();
#endif
	QSqlQuery createq(db());
	QList<QByteArray> createUniqueIndex;
	QString query = "CREATE TABLE \"" + tbl.name() + "\"(";
	for (int i = 0; i < tbl.columns().size(); i++)
	{
		if (i!=0)
			query += ",";
		SPISColumn col = tbl.columns()[i];
		query += "\"" + col.name() + "\" " + SQLiteTypes::fromSPIS(tbl.db(), col.type(), col.minsize(), usevar());
		if ((col.constraints() & SPIS::notnull) == SPIS::notnull)
			query += " NOT NULL";
		if ((col.constraints() & SPIS::unique) == SPIS::unique)
		{
			if ((col.constraints() & SPIS::primarykey) == SPIS::primarykey)
				query += " UNIQUE";
			else
				createUniqueIndex.push_back(col.name());
		}
		if (col.def().isValid() && !col.def().isNull())
		{
			query += " DEFAULT ";
			if (needsEnquote(col.type()))
				query += "'" + col.def().toString().replace("'", "''") + "'";
			else if (col.type() == "date")
				query += driver->fromQDate(col.def().toDate()).toString();
			else if (col.type() == "time")
				query += driver->fromQTime(col.def().toTime()).toString();
			else if (col.type() == "datetime")
				query += driver->fromQDateTime(col.def().toDateTime()).toString();
			else
				query += col.def().toString().replace(QRegularExpression("[^0-9a-zA-Z\\.,\\-+]"), "");
		}
	}
	if (!tbl.primaryKey().isEmpty())
		query += ",PRIMARY KEY(\"" + tbl.primaryKey() + "\")";
	query += ")";
	if (!tbl.primaryKey().isEmpty())
		query += " WITHOUT ROWID";
	query += ";";
	if (!createq.exec(query))
	{
		DUMP_ERROR(createq);
		createq.finish();
		
		// if we had a backup, lets restore it
		if (hasBackup)
		{
			QSqlQuery restoreq(db());
			if (!restoreq.exec("ALTER TABLE \"" + tbl.name() + backupSuffix + "\" RENAME TO \"" + tbl.name() + "\";"))
				DUMP_ERROR(restoreq);	
		}
		
		return false;
	}
	createq.finish();
	
	for (auto name : createUniqueIndex)
	{
		QSqlQuery ciq(db());
		if (!ciq.exec("CREATE UNIQUE INDEX \"" + uniqueConstraintName(tbl.name(), name) + "\" ON \"" + tbl.name() + "\"(\"" + name + "\");"))
		{
			DUMP_ERROR(ciq);
			ciq.finish();
			
			// if we had a backup, lets restore it
			if (hasBackup)
			{
				QSqlQuery restoreq(db());
				if (!restoreq.exec("ALTER TABLE \"" + tbl.name() + backupSuffix + "\" RENAME TO \"" + tbl.name() + "\";"))
					DUMP_ERROR(restoreq);	
			}
			
			return false;
		}
	}
	
	if (hasBackup)
	{
#ifdef CMAKE_DEBUG
		qDebug() << "SPIS[SQLite]: Trying to apply backup of table" << tbl.name();
#endif
		QSqlQuery restoreq(db());
		QString cols;
		QSet<QByteArray> commmonCols;
		for (auto col : tbl.columns())
			commmonCols.insert(col.name());
		for (auto col : addedCols)
			commmonCols.remove(col.name());
		for (auto name : commmonCols)
			cols += name + ",";
		if (commmonCols.size() > 0)
			cols = cols.mid(0, cols.size()-1);
		if (!restoreq.exec("INSERT INTO \"" + tbl.name() + "\" (" + cols + ") SELECT " + cols + " FROM \"" + tbl.name() + backupSuffix + "\";"))
		{
			DUMP_ERROR(restoreq);
			restoreq.finish();
			
			// to avoid data loss, lets restore the old table if possible
			QSqlQuery dropq(db());
			if (dropq.exec("DROP TABLE \"" + tbl.name() + "\";"))
			{
				dropq.finish();
				QSqlQuery renameq(db());
				if (renameq.exec("ALTER TABLE \"" + tbl.name() + backupSuffix + "\" RENAME TO \"" + tbl.name() + "\";"))
				{
#ifdef CMAKE_DEBUG
					qDebug() << "SPIS[SQLite]: Restored table" << tbl.name() << "after failure applying backup";
#endif
					return false;
				}
				else
					DUMP_ERROR(renameq);
			}
			else
				DUMP_ERROR(dropq);
			qWarning() << "SPIS[SQLite]: Unable to restore backup of table" << tbl.name() << "but the data is still available in a table called" << tbl.name() + backupSuffix;
			// return true because I were able to ensure the table (with data loss)
			return true;
		}
		restoreq.finish();
		
		// we no longer need the backup table
		QSqlQuery dropq(db());
		if (!dropq.exec("DROP TABLE \"" + tbl.name() + backupSuffix + "\";"))
			DUMP_ERROR(dropq);
	}
	
	return true;
}

bool SQLiteDatabase::needsEnquote(const QByteArray &type)
{
	QByteArray t = type.trimmed().toLower();
	return (t=="char" || t=="text" || t=="password" || t=="byte" || t=="blob" || t=="variant");
}

QString SQLiteDatabase::filterSQL(const SPISFilter &filter)
{
	if (filter.op() == SPISFilter::noop)
		return QString();
	QString sql;
	
	if (filter.op() < 0x20)
	{
		sql += "(\"" + filter.arg(0) + "\"";
		switch (filter.op())
		{
		case SPISFilter::eq: sql += " == "; break;
		case SPISFilter::ne: sql += " <> "; break;
		case SPISFilter::lt: sql += " <  "; break;
		case SPISFilter::le: sql += " <= "; break;
		case SPISFilter::gt: sql += " >  "; break;
		case SPISFilter::ge: sql += " >= "; break;
		case SPISFilter::like: sql += " LIKE "; break;
		default:
			fprintf(stderr, "SPIS[SQLite]: Unknown filter operator value 0x%02x\n", filter.op());
			return QString();
		}
		
		QString arg = filter.arg(1);
		if (arg.startsWith("int:") || arg.startsWith("double:"))
			sql += arg.mid(arg.indexOf(':') + 1);
		else if (arg.startsWith("'") && arg.endsWith("'"))
			sql += "'" + arg.mid(1, arg.size() - 2).replace("'", "''") + "'";
		else
			sql += "\"" + arg.replace("\"", "\"\"") + "\"";
		
		sql += ")";
	}
	
	else if (filter.op() < 0x30)
	{
		sql += "(\"" + filter.arg(0) + "\"";
		switch (filter.op())
		{
		case SPISFilter::isnull: sql += " ISNULL "; break;
		case SPISFilter::notnull: sql += " NOT NULL "; break;
		default:
			fprintf(stderr, "SPIS[SQLite]: Unknown filter operator value 0x%02x\n", filter.op());
			return QString();
		}
	}
	
	else if (filter.op() < 0x40)
	{
		sql += "(" + filterSQL(filter.filter(0));
		switch (filter.op())
		{
		case SPISFilter::op_and: sql += " AND "; break;
		case SPISFilter::op_or: sql += " OR "; break;
		default:
			fprintf(stderr, "SPIS[SQLite]: Unknown filter operator value 0x%02x\n", filter.op());
			return QString();
		}
		sql += filterSQL(filter.filter(1)) + ")";
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
		sql += filterSQL(filter.filter(0));
	}
	
	return sql;
}

SelectResult* SQLiteDatabase::selectTable(const SPISTable &tbl, const QList<SPISColumn> &cols,
										  const SPISFilter &filter, const QList<SPISJoinTable> &join,
										  int limit, bool asc, const QByteArray &orderBy)
{
	QSqlQuery q(db());
	QString qq = "SELECT ";
	for (int i = 0; i < cols.size(); i++)
	{
		if (i != 0)
			qq += ", ";
		qq += "\"" + cols[i].name() + "\"";
	}
	for (auto j : join)
		for (auto col : j.cols)
			qq += ", \"" + j.prefix + col.name() + "\"";
	qq += " FROM \"" + tbl.name() + "\"";
	
	for (auto j : join)
	{
		qq += " INNER JOIN (SELECT ";
		for (int i = 0; i < j.cols.size(); i++)
		{
			if (i != 0)
				qq += ", ";
			qq += "\"" + j.cols[i].name() + "\" AS \"" + j.prefix + j.cols[i].name() + "\"";
		}
		qq += " FROM \"" + j.tbl.name() + "\")"
			  " ON \"" + tbl.name() + "\".\"" + j.on.name() + "\"=\"" + j.prefix + j.onTbl.name() + "\"";
	}
	
	QString fsql = filterSQL(filter);
	if (!fsql.isEmpty())
		qq += " WHERE " + fsql;
	if (!orderBy.isEmpty())
		qq += " ORDER BY \"" + orderBy + "\" " + (asc ? "ASC" : "DESC");
	else if (!tbl.primaryKey().isEmpty())
		qq += " ORDER BY \"" + tbl.primaryKey() + "\" " + (asc ? "ASC" : "DESC");
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

bool SQLiteDatabase::insertIntoTable(const SPISTable &tbl, const QList<SPISColumn> &cols,
									 const QVector<QVector<QVariant>> &rows)
{
	Q_ASSERT(!cols.empty());
	
	QSqlQuery q(db());
	QString qq = "INSERT INTO \"" + tbl.name() + "\" (";
	for (int i = 0; i < cols.size(); i++)
	{
		if (i != 0)
			qq += ",";
		qq += "\"" + cols[i].name() + "\"";
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

bool SQLiteDatabase::updateTable(const SPISTable &tbl, const QMap<SPISColumn, QVariant> &values,
								 const QVector<QVariant> &pks)
{
	if (pks.empty() || values.empty())
		return true;
	
	QSqlQuery q(db());
	
	QString qq = "UPDATE \"" + tbl.name() + "\" SET ";
	for (auto key : values.keys())
		qq += "\"" + key.name() + "\"=" + (needsEnquote(key.type()) ? "\"" + values[key].toString() + "\"" : values[key].toString()) + ",";
	qq = qq.mid(0, qq.size()-1);
	
	qq += " WHERE ";
	for (auto pk : pks)
		qq += "(\"" + tbl.primaryKey() + "\"==" + QByteArray::number(pk.toInt()) + ") OR "; // note that currently only int and uint as pks are allowed
	qq = qq.mid(0, qq.size()-4);
	
	qq += ";";
	if (!q.exec(qq))
	{
		DUMP_ERROR(q);
		return false;
	}
	return true;
}

bool SQLiteDatabase::deleteFromTable(const SPISTable &tbl, const SPISFilter &filter)
{
	QSqlQuery q(db());
	QString qq = "DELETE FROM \"" + tbl.name() + "\"";
	QString fsql = filterSQL(filter);
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

bool SQLiteDatabase::deleteFromTable(const SPISTable &tbl, const QVector<QVariant> &pks)
{
	Q_ASSERT(!tbl.primaryKey().isEmpty());
	if (pks.empty())
		return true;
	
	QSqlQuery q(db());
	QString qq = "DELETE FROM \"" + tbl.name() + "\" WHERE ";
	for (int i = 0; i < pks.size(); i++)
	{
		if (i != 0)
			qq += " OR ";
		qq += "(\"" + tbl.primaryKey() + "\"==" + QByteArray::number(pks[i].toInt()) + ")"; // note that currently only int and uint as pks are allowed
	}
	qq += ";";
	if (!q.exec(qq))
	{
		DUMP_ERROR(q);
		return false;
	}
	return true;
}
