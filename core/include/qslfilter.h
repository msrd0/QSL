#pragma once

#include "qsl_global.h"
#include "qsldb.h"
#include "qslnamespace.h"

#include <QVariant>

namespace qsl {

/**
 * This class is used to filter the results of a `SELECT` query. Subclasses should overwrite the `sql(QSLDB::Driver)`
 * method. By default, it returns an empty string.
 */
class QSLFilter
{
public:
	/// Returns an SQL expression for the given driver.
	virtual QString sql(QSL::Driver driver) { return QString(); }
};

/**
 * This namespace contains a lot of filters that can be used in conjunction with `QSLQuery`. Note that
 * you can only pass one filter to the `QSLQuery` class, subsequent calls to `QSLQuery::filter` will
 * overwrite the old one.
 */
namespace filters {

/**
 * This class is a QSLFilter that compares a column with a value and returns true if the value matches.
 */
class eq : public QSLFilter
{
public:
	eq(const char* column, const QVariant &val);
	
	virtual QString sql(QSL::Driver driver);
	
protected:
	QString _column;
	QVariant _val;
};

/**
 * This class is a QSLFilter that compares a column with a value and returns true if the values don't match.
 */
class ne : public eq
{
public:
	ne(const char* column, const QVariant &val);
	
	virtual QString sql(QSL::Driver driver);
};

}
}
