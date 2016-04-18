#pragma once

#include "qsl_global.h"
#include "qslfilter.h"

#include <QSqlDatabase>

namespace qsl {
class QSLTable;

/**
 * This class helps creating an SQL Query for a QSLTable. It provides methods
 * for creating SQL queries for different drivers, but it doesn't execute
 * anything.
 */
class QSLQuery
{
private:
	static QHash<QSLDB::Driver, QJsonObject> _driverJson;
public:
	static QJsonObject driverJson(QSLDB::Driver driver);
	QByteArray driverType(QSLDB::Driver driver, QByteArray type);
	
	/// The different types of queries that can be created.
	enum Type
	{
		CreateTable,
		SelectTable,
		InsertTable,
		UpdateTable
	};
	
	QSLQuery(QSLTable *tbl, Type type);
	
	/// Overwrite the filter used by `SELECT` queries.
	QSLQuery& filter(const QSLFilter &filter) { _filter = filter; return *this; }
	
	/// Returns an SQL expression for the given driver.
	QString sql(QSLDB::Driver driver);
	
private:
	QSLTable *_tbl;
	Type _type;
	
	QSLFilter _filter;
};

}
