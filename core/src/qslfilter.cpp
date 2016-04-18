#include "qslfilter.h"

using namespace qsl;

eq::eq(const char *column, const QVariant &val)
	: _column(column)
	, _val(val)
{
}

QString eq::sql(QSL::Driver driver)
{
	switch (driver)
	{
	case QSL::PostgreSQL:
	case QSL::MySQL:
	case QSL::SQLite:
		return _column + "=='" + _val.toString().replace("'", "''") + "'";
	}
}

ne::ne(const char *column, const QVariant &val)
	: eq(column, val)
{
}

QString ne::sql(QSL::Driver driver)
{
	switch (driver)
	{
	case QSL::PostgreSQL:
	case QSL::MySQL:
	case QSL::SQLite:
		return _column + "!='" + _val.toString().replace("'", "''") + "'";
	}
}
