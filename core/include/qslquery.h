#pragma once

#include "qsl_global.h"
#include "qslfilter.h"
#include "qslnamespace.h"

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
	static QHash<QSL::Driver, QJsonObject> _driverJson;
public:
	static QJsonObject driverJson(QSL::Driver driver);
	QByteArray driverType(QSL::Driver driver, const QByteArray &type, uint32_t minsize);
	
	QSLQuery(QSLTable *tbl, QSL::QueryType type);
	
	/// Overwrite the filter used by `SELECT` queries.
	QSLQuery& filter(const QSLFilter &filter) { _filter = filter; return *this; }
	/// Overwrite the limit of resulting columns.
	void limit(int l) { _limit = l; }
	
	/// Returns an SQL expression for the given driver.
	QString sql(QSL::Driver driver);
	
protected:
	QSLTable *_tbl;
	QSL::QueryType _type;
	
private:
	QSLFilter _filter;
	int _limit = -1;
};

template<typename T>
class QSLSelectQuery : public QSLQuery
{
public:
	QSLSelectQuery(QSLTable *tbl)
		: QSLQuery(tbl, QSL::SelectTable)
	{
	}
	
	virtual QList<T> query() = 0;
};

}
