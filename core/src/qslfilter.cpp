#include "qslfilter.h"

using namespace qsl;

eq::eq(const char *column, const QVariant &val)
	: _column(column)
	, _val(val)
{
}

QString eq::sql(QSLDB::Driver driver)
{
	switch (driver)
	{
	case QSLDB::PostgreSQL:
	case QSLDB::MySQL:
	case QSLDB::SQLite:
		return _column + "=='" + _val.toString().replace("'", "''") + "'";
	}
}

ne::ne(const char *column, const QVariant &val)
	: eq(column, val)
{
}

QString ne::sql(QSLDB::Driver driver)
{
	switch (driver)
	{
	case QSLDB::PostgreSQL:
	case QSLDB::MySQL:
	case QSLDB::SQLite:
		return _column + "!='" + _val.toString().replace("'", "''") + "'";
	}
}
