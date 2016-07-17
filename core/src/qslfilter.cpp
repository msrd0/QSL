#include "qslfilter.h"

#include <QDebug>

using namespace qsl;
using namespace qsl::filters;

QString QSLFilter::sql(QSL::Driver driver) const
{
	Q_UNUSED(driver);
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
	default:
		return QString();
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
	default:
		return QString();
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
	default:
		return QString();
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
	default:
		return QString();
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
	default:
		return QString();
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
	default:
		return QString();
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
	default:
		return QString();
	}
}
