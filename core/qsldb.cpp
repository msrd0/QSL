#include "driver/driver.h"
#include "qsldb.h"
#include "qslquery.h"
#include "qsltable.h"

#include <QMetaEnum>
#include <QSqlError>
#include <QSqlQuery>

using namespace qsl;
using namespace qsl::driver;

QSLDB::QSLDB(const char* name, const QString& driver)
	: QSLDB(name, Driver::driver(driver))
{
}

QSLDB::QSLDB(const char *name, Driver *driver)
	: _name(name)
	, _driver(driver)
{
	Q_ASSERT(driver);
	db = _driver->newDatabase();
	db->setName(name);
}

void QSLDB::setName(const QString &name)
{
	db->setName(name);
}
void QSLDB::setHost(const QString &host)
{
	db->setHost(host);
}
void QSLDB::setPort(int port)
{
	db->setPort(port);
}
void QSLDB::setUser(const QString &user)
{
	db->setUser(user);
}
void QSLDB::setPassword(const QString &password)
{
	db->setPassword(password);
}

bool QSLDB::connect()
{
	if (!db->connect())
		return false;
	for (QSLTable *tbl : _tables)
		if (!db->ensureTable(*tbl))
		{
			fprintf(stderr, "QSLDB: Unable to ensure table %s.%s\n", name(), tbl->name().data());
			return false;
		}
	return true;
}

void QSLDB::registerTable(QSLTable *tbl)
{
	Q_ASSERT(tbl);
	if (db->isConnected())
	{
		fprintf(stderr, "QSLDB: Attempt to add table %s after database was opened\n", tbl->name().data());
		return;
	}
	_tables.append(tbl);
}
