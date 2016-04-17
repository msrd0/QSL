#include "qsldb.h"
#include "qslquery.h"
#include "qsltable.h"

#include <QSqlError>
#include <QSqlQuery>

using namespace qsl;

QString QSLDB::qDriverName(Driver driver)
{
	switch (driver)
	{
	case PostgreSQL:
		return "QPSQL";
	case MySQL:
		return "QMYSQL";
	case SQLite:
		return "QSQLITE";
	}
}

QSLDB::QSLDB(const char *name, Driver driver)
	: db(QSqlDatabase::addDatabase(qDriverName(driver)))
	, _name(name)
	, _driver(driver)
{
	db.setDatabaseName(name);
}

bool QSLDB::connect()
{
	if (!db.open())
		return false;
	QStringList dbtables = db.tables();
	for (QSLTable *tbl : _tables)
		if (!dbtables.contains(tbl->name()))
		{
			QSLQuery qq(tbl, QSLQuery::CreateTable);
			QString sql = qq.sql(driver());
#ifdef CMAKE_DEBUG
			fprintf(stderr, "QSLDB: Executing SQL '%s'\n", qPrintable(sql));
#endif
			QSqlQuery q(sql);
			if (q.lastError().isValid())
			{
				fprintf(stderr, "QSLDB: Failed to create table %s: %s\n", tbl->name(), qPrintable(q.lastError().text()));
				db.close();
				return false;
			}
		}
	return true;
}
