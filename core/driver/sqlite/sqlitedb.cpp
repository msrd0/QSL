#include "sqlitedb.h"
#include "sqlitetypes.h"
#include "driver/diff.h"

#include <unistd.h>

#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QRegularExpression>
#include <QSqlError>


using namespace qsl;
using namespace qsl::driver;

static void dumpError(const QSqlQuery &q, const char* file, int line)
{
	if (isatty(STDERR_FILENO))
	{
		fprintf(stderr, "\033[1mQSL[SQLite]: \033[31mError\033[0m in \033[32m%s:%d\033[0m:\n", file, line);
		fprintf(stderr, "    \033[1;31mQuery:\033[0m %s\n", qPrintable(q.lastQuery()));
		fprintf(stderr, "    \033[1;31mError:\033[0m %s\n", qPrintable(q.lastError().text()));
	}
	else
	{
		fprintf(stderr, "QSL[SQLite]: Error in %s:%d:\n", file, line);
		fprintf(stderr, "    Query: %s\n", qPrintable(q.lastQuery()));
		fprintf(stderr, "    Error: %s\n", qPrintable(q.lastError().text()));
	}
}
#define DUMP_ERROR(q) dumpError((q), __FILE__, __LINE__);


SQLiteDatabase::SQLiteDatabase(const char *charset, bool usevar)
	: QtDatabase(charset, usevar, "QSQLITE")
{
}

bool SQLiteDatabase::connect()
{
	if (!QtDatabase::connect())
		return false;
#ifdef CMAKE_DEBUG
	qDebug() << "QSL[SQLite]: Conncted to" << db().databaseName();
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
	if (enc == "utf8")
		enc = "UTF-8";
	else if (enc == "utf16")
		enc = "UTF-16";
	else if (enc == "utf16le")
		enc = "UTF-16le";
	else if (enc == "utf16be")
		enc = "UTF-16be";
	else
	{
		qWarning() << "QSL[SQLite]: Encoding" << enc << "is currently not supported. Please only report if";
		qWarning() << "             the encoding is listet at http://www.sqlite.org/pragma.html#pragma_encoding";
		enc = "";
	}
	if (!enc.isEmpty())
	{
		QSqlQuery qenc(db());
		if (!qenc.exec("PRAGMA encoding = \"" + enc + "\";"))
		{
			DUMP_ERROR(qenc);
			qWarning() << "QSL[SQLite]: Failed to set encoding. Will use the default provided by SQLite.";
		}
	}
	
	return true;
}

void SQLiteDatabase::loadTableInfo()
{
#ifdef CMAKE_DEBUG
	qDebug() << "QSL[SQLite]: Starting loadTableInfo()";
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
		QByteArray pk;
#ifdef CMAKE_DEBUG
		qDebug() << "QSL[SQLite]: Detecting table" << tblName;
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
			MutableColumn col(tblInfo.value("name").toByteArray(), type.first, type.second);
#ifdef CMAKE_DEBUG
			qDebug() << "QSL[SQLite]: Adding column" << col.name();
#endif
			if (tblInfo.value("notnull") == "1")
				col.addConstraint(QSL::notnull);
			if (tblInfo.value("pk") == "1")
			{
				col.addConstraint(QSL::primarykey);
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
				qDebug() << "QSL[SQLite]: Looking at index" << indexList.value("name").toString();
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
					
					auto it = columns.find(q.value("name").toByteArray());
					if (it != columns.end())
						it->addConstraint(QSL::unique);
				}
			}
			while (indexList.next());
		}
		
		// create and add the table
		QSLTable tbl(tblName.data(), pk, 0);
		for (auto col : columns.values())
			tbl.addColumn(col);
		addTable(tbl);
	}
	while (tables.next());
}

bool SQLiteDatabase::ensureTable(const QSLTable &tbl)
{
	bool success = ensureTableImpl(tbl);
	if (success)
		addTable(tbl); // otherwise subsequent calls to ensureTable will repeat the changes
	return success;
}

static QByteArray backupSuffixName(const QByteArray &name)
{
	return "___qslbackup_" + QCryptographicHash::hash(name + " @ " + QDateTime::currentDateTime().toString().toUtf8(), QCryptographicHash::Md5).toHex();
}

static QByteArray uniqueConstraintName(const QByteArray &tbl, const QByteArray &name)
{
	return "qsl_sqlite_driver_constraint_unique_" + name + "_" + QCryptographicHash::hash(tbl + "." + name + " !unique", QCryptographicHash::Md5).toHex();
}

bool SQLiteDatabase::ensureTableImpl(const QSLTable &tbl)
{
	bool needsNewTable = !containsTable(tbl.name());
	const QByteArray backupSuffix = backupSuffixName(tbl.name());
	bool hasBackup = false;
	QList<QSLColumn> addedCols;
	
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
				qDebug() << "QSL[SQLite]: Dropping empty table" << tbl.name();
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
		 new unique constraint can be added with the create index query. QSL doesn't enforce
		 to remove removed columns from the database, so this will be fine as long as the
		 column is nullable.
		 => We need to create a new table if and only if:
		    1. A constraint other than unique changed
		    2. The type of a column changed
		    3. A column with a not-null constraint was removed
			4. If a column with a primary-key constraint was added
		 => We need to fail if and only if:
		    A. A new column without a default value but not nullable was added and the table is
			   not empty
		*/
		
		// check A.
		//  (note that table is not empty if reaching this code)
		//  (note that currently default values are not supported by QSL)
		for (auto col : diff.addedCols())
		{
			if ((col.constraints() & QSL::notnull) == QSL::notnull)
			{
				qCritical() << "QSL[SQLite]: In Table" << tbl.name() << ": ERROR: Column" << col.name() << "was added with !notnull but without a default value";
				return false;
			}
		}
		
		// check 2.
		if (!diff.typeChanged().isEmpty())
		{
			needsNewTable = true;
#ifdef CMAKE_DEBUG
			qDebug() << "QSL[SQLite]: The types of the following columns changed, need to re-create table" << tbl.name();
			for (auto col : diff.typeChanged())
				qDebug() << "             -" << col.name();
#endif
		}
		
		// check 1.
		if (!needsNewTable) // othewise we can skip
		{
			for (auto constraintDiff : diff.constraintsChanged())
			{
				if (constraintDiff.constraintsAdded() != QSL::none && constraintDiff.constraintsAdded() != QSL::unique)
				{
					needsNewTable = true;
					break;
				}
				if (constraintDiff.constraintsRemoved() != QSL::none && constraintDiff.constraintsRemoved() != QSL::unique)
				{
					needsNewTable = true;
					break;
				}
			}
			
#ifdef CMAKE_DEBUG
			if (needsNewTable)
				qDebug() << "QSL[SQLite]: At least one constraint other than unique was added or removed, need to re-create table" << tbl.name();
#endif
		}
		
		// check 3.
		if (!needsNewTable) // otherwise we can skip
		{
			for (auto col : diff.removedCols())
				if ((col.constraints() & QSL::notnull) == QSL::notnull)
				{
#ifdef CMAKE_DEBUG
					qDebug() << "QSL[SQLite]: At least one column with a notnull-constraint was removed, need to re-create table" << tbl.name();
#endif
					needsNewTable = true;
					break;
				}
		}
		
		// check 4.
		if (!needsNewTable) // otherwise we can skip
		{
			for (auto col : diff.addedCols())
				if ((col.constraints() & QSL::primarykey) == QSL::primarykey)
				{
#ifdef CMAKE_DEBUG
					qDebug() << "QSL[SQLite]: The primary key changed, need to re-create table" << tbl.name();
#endif
					needsNewTable = true;
					break;
				}
		}
		
		// if we need a new table, rename the old one
		if (needsNewTable)
		{
#ifdef CMAKE_DEBUG
			qDebug() << "QSL[SQLite]: Creating backup of table" << tbl.name() << "-" << tbl.name() + backupSuffix;
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
				qDebug() << "QSL[SQLite]: Adding column" << col.name() << "to table" << tbl.name();
#endif
				QSqlQuery alterq(db());
				if (!alterq.exec("ALTER TABLE \"" + tbl.name() + "\" ADD COLUMN \"" + col.name() + "\" " + SQLiteTypes::fromQSL(col.type(), col.minsize(), usevar()) + ";"))
				{
					DUMP_ERROR(alterq);
					return false;
				}
				if ((col.constraints() & QSL::unique) == QSL::unique)
				{
#ifdef CMAKE_DEBUG
					qDebug() << "QSL[SQLite]: Adding unique constraint to column" << col.name() << "of table" << tbl.name();
#endif
					QSqlQuery ciq(db());
					if (!ciq.exec("CREATE UNIQUE INDEX \"" + uniqueConstraintName(tbl.name(), col.name()) + "\" ON \"" + tbl.name() + "\"(\"" + col.name() + "\");"))
					{
						DUMP_ERROR(ciq);
						return false;
					}
				}
			}
			
			// now, add all unique constraints on existing columns
			for (auto constraintDiff : diff.constraintsChanged())
			{
				if ((constraintDiff.constraintsAdded() & QSL::unique) == QSL::unique)
				{
					QSqlQuery ciq(db());
					if (!ciq.exec("CREATE UNIQUE INDEX \"" + uniqueConstraintName(tbl.name(), constraintDiff.colName()) + "\" ON \"" + tbl.name() + "\"(\"" + constraintDiff.colName() + "\");"))
					{
						DUMP_ERROR(ciq);
						return false;
					}
				}
			}
			
			// TODO: remove unique constraints
			
			return true;
		}
	}
	
	if (needsNewTable)
	{
#ifdef CMAKE_DEBUG
		qDebug() << "QSL[SQLite]: Creating table" << tbl.name();
#endif
		QSqlQuery createq(db());
		QString query = "CREATE TABLE \"" + tbl.name() + "\"(";
		for (int i = 0; i < tbl.columns().size(); i++)
		{
			if (i!=0)
				query += ",";
			QSLColumn col = tbl.columns()[i];
			query += "\"" + col.name() + "\" " + SQLiteTypes::fromQSL(col.type(), col.minsize(), usevar());
			if ((col.constraints() & QSL::notnull) == QSL::notnull)
				query += " NOT NULL";
			if ((col.constraints() & QSL::unique) == QSL::unique)
				query += " UNIQUE";
		}
		if (!tbl.primaryKey().isEmpty())
			query += ",PRIMARY KEY(\"" + tbl.primaryKey() + "\")";
		query += ") WITHOUT ROWID;";
		if (!createq.exec(query))
		{
			DUMP_ERROR(createq);
			
			// if we had a backup, lets restore it
			if (hasBackup)
			{
				QSqlQuery restoreq(db());
				if (!restoreq.exec("ALTER TABLE \"" + tbl.name() + backupSuffix + "\" RENAME TO \"" + tbl.name() + "\";"))
					DUMP_ERROR(restoreq);	
			}
			
			return false;
		}
		return true;
	}
}

bool SQLiteDatabase::needsEnquote(const QByteArray &type)
{
	QByteArray t = type.trimmed().toLower();
	return (t=="char" || t=="text" || t=="password" || t=="byte" || t=="blob" || t=="variant");
}

SelectResult* SQLiteDatabase::selectTable(const QSLTable &tbl, const QList<QSLColumn> &cols,
										  const QSharedPointer<QSLFilter> &filter, int limit, bool asc)
{
	qWarning() << "TODO: Handle filters in " << __PRETTY_FUNCTION__;
	QSqlQuery q(db());
	QString qq = "SELECT ";
	for (int i = 0; i < cols.size(); i++)
	{
		if (i != 0)
			qq += ", ";
		qq += "\"" + cols[i].name() + "\"";
	}
	qq += " FROM \"" + tbl.name() + "\"";
	if (!tbl.primaryKey().isEmpty())
	{
		qq += " ORDER BY \"" + tbl.primaryKey() + "\"";
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

bool SQLiteDatabase::insertIntoTable(const QSLTable &tbl, const QList<QSLColumn> &cols,
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

bool SQLiteDatabase::updateTable(const QSLTable &tbl, const QMap<QSLColumn, QVariant> &values,
								 const QVector<QVariant> &pks)
{
	// TODO
	return 0;
}
