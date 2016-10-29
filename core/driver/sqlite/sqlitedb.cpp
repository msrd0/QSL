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


SQLiteDatabase::SQLiteDatabase()
	: QtDatabase("QSQLITE")
{
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
			columns.insert(col.name(), col);
		}
		while (tblInfo.next());
		
		// create and add the table
		QSLTable tbl(tblName.data(), 0);
		for (auto col : columns.values())
			tbl.addColumn(col);
		addTable(tbl);
	}
	while (tables.next());
}

bool SQLiteDatabase::ensureTable(const QSLTable &tbl)
{
	// TODO
	return false;
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
