#include "sqlitedb.h"
#include "sqlitetypes.h"
#include "qsltable.h"

#include <unistd.h>

#include <QSqlError>

#ifdef CMAKE_DEBUG
#  include <QDebug>
#endif

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
		qWarning() << "the encoding is listet at http://www.sqlite.org/pragma.html#pragma_encoding";
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
			MutableColumn col(tblInfo.value("name").toByteArray(), SQLiteTypes::fromSQL(tblInfo.value("type").toByteArray()), -1);
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
	if (containsTable(tbl.name()))
	{
		
	}
	else
	{
		QSqlQuery q(db());
		QString query = "CREATE TABLE " + tbl.name() + "(";
		for (int i = 0; i < tbl.columns().size(); i++)
		{
			if (i!=0)
				query += ",";
			QSLColumn col = tbl.columns()[i];
			query += col.name() + " " + SQLiteTypes::fromQSL(col.type(), col.minsize(), usevar());
		}
		query += ");";
		if (!q.exec(query))
		{
			DUMP_ERROR(q);
			return false;
		}
		return true;
	}
}

SelectResult* SQLiteDatabase::selectTable(const QSLTable &tbl, const QList<QSLColumn> &cols,
										  const QSharedPointer<QSLFilter> &filter, int limit)
{
	// TODO
	return 0;
}

bool SQLiteDatabase::updateTable(const QSLTable &tbl, const QMap<QSLColumn, QVariant> &values,
								 const QVector<QVariant> &pks)
{
	// TODO
	return 0;
}
