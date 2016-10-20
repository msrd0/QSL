#include "qslfilter.h"

#include <QDebug>

using namespace qsl;
using namespace qsl::driver;
using namespace qsl::filters;

QString QSLFilter::sql(Driver*) const
{
	return QString();
}

QString eq::sql(Driver*) const
{
	return _column + "=='" + _val.toString().replace("'", "''") + "'";
}

QString ne::sql(Driver*) const
{
	return _column + "!='" + _val.toString().replace("'", "''") + "'";
}

QString sw::sql(Driver*) const
{
	return _column + " LIKE '" + _val.toString().replace("'", "''") + "%'";
}

QString ew::sql(Driver*) const
{
	return _column + " LIKE '%" + _val.toString().replace("'", "''") + "'";
}

QString co::sql(Driver*) const
{
	return _column + " LIKE '%" + _val.toString().replace("'", "''") + "%'";
}

QString a::sql(Driver *driver) const
{
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

QString o::sql(Driver *driver) const
{
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
