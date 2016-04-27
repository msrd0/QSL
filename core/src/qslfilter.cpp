#include "qslfilter.h"

#include <QDebug>

using namespace qsl;
using namespace qsl::filters;

QString QSLFilter::sql(QSL::Driver driver) const
{
//	fprintf(stderr, "QSLFilter: WARNING: Called sql() on an empty filter.\n");
	return QString();
}

QString eq::sql(QSL::Driver driver) const
{
	switch (driver)
	{
	case QSL::PostgreSQL:
	case QSL::MySQL:
	case QSL::SQLite:
		return _column + "=='" + _val.toString().replace("'", "''") + "'";
	}
}

QString ne::sql(QSL::Driver driver) const
{
	switch (driver)
	{
	case QSL::PostgreSQL:
	case QSL::MySQL:
	case QSL::SQLite:
		return _column + "!='" + _val.toString().replace("'", "''") + "'";
	}
}

QString sw::sql(QSL::Driver driver) const
{
	switch (driver)
	{
	case QSL::PostgreSQL:
	case QSL::MySQL:
	case QSL::SQLite:
		return _column + " LIKE '" + _val.toString().replace("'", "''") + "%'";
	}
}

QString ew::sql(QSL::Driver driver) const
{
	switch (driver)
	{
	case QSL::PostgreSQL:
	case QSL::MySQL:
	case QSL::SQLite:
		return _column + " LIKE '%" + _val.toString().replace("'", "''") + "'";
	}
}

QString co::sql(QSL::Driver driver) const
{
	switch (driver)
	{
	case QSL::PostgreSQL:
	case QSL::MySQL:
	case QSL::SQLite:
		return _column + " LIKE '%" + _val.toString().replace("'", "''") + "%'";
	}
}

QString a::sql(QSL::Driver driver) const
{
	switch (driver)
	{
	case QSL::PostgreSQL:
	case QSL::MySQL:
	case QSL::SQLite: {
			QString sql;
			for (auto &filter : _filters)
			{
				if (!sql.isEmpty())
					sql += " AND ";
				QString fs = filter->sql(driver);
				sql += "(" + fs + ")";
			}
			return sql;
		}
	}
}

QString o::sql(QSL::Driver driver) const
{
	switch (driver)
	{
	case QSL::PostgreSQL:
	case QSL::MySQL:
	case QSL::SQLite: {
			QString sql;
			for (auto &filter : _filters)
			{
				if (!sql.isEmpty())
					sql += " OR ";
				QString fs = filter->sql(driver);
				sql += "(" + fs + ")";
			}
			return sql;
		}
	}
}
