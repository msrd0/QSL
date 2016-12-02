#include "driver/driver.h"
#include "spisdb.h"
#include "spisquery.h"
#include "spistable.h"

#include <QMetaEnum>
#include <QSqlError>
#include <QSqlQuery>

using namespace spis;
using namespace spis::driver;

SPISDB::SPISDB(const char* name, const QString& driver)
	: SPISDB(name, Driver::driver(driver))
{
}

SPISDB::SPISDB(const char *name, Driver *driver)
	: _name(name)
	, _driver(driver)
	, _db(0)
{
	Q_ASSERT(driver);
}

SPISDB::~SPISDB()
{
	if (isConnected())
		disconnect();
}

Database *SPISDB::db()
{
	if (!_db)
	{
		_db = driver()->newDatabase(charset(), usevar());
		Q_ASSERT(_db);
		_db->setName(name());
	}
	return _db;
}

void SPISDB::setName(const QString &name)
{
	db()->setName(name);
}
void SPISDB::setHost(const QString &host)
{
	db()->setHost(host);
}
void SPISDB::setPort(int port)
{
	db()->setPort(port);
}
void SPISDB::setUser(const QString &user)
{
	db()->setUser(user);
}
void SPISDB::setPassword(const QString &password)
{
	db()->setPassword(password);
}

bool SPISDB::connect()
{
	if (!db()->connect())
		return false;
	for (SPISTable *tbl : _tables.values())
		if (!db()->ensureTable(*tbl))
		{
			fprintf(stderr, "SPISDB: Unable to ensure table %s.%s\n", name(), tbl->name().data());
			db()->disconnect();
			return false;
		}
	return true;
}

void SPISDB::disconnect()
{
	db()->disconnect();
}

bool SPISDB::isConnected()
{
	return db()->isConnected();
}

void SPISDB::registerTable(SPISTable *tbl)
{
	Q_ASSERT(tbl);
	if (db()->isConnected())
	{
		fprintf(stderr, "SPISDB: Attempt to add table %s after database was opened\n", tbl->name().data());
		return;
	}
	_tables.insert(tbl->name(), tbl);
}

SPISTable* SPISDB::table(const QByteArray &name)
{
	return _tables.value(name, 0);
}

const SPISTable* SPISDB::table(const QByteArray &name) const
{
	return _tables.value(name, 0);
}
