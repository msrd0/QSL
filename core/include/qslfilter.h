#pragma once

#include "qsl_global.h"
#include "qsldb.h"
#include "qslnamespace.h"

#include <QSharedPointer>
#include <QVariant>
#include <QVector>

namespace qsl {

/**
 * This class is used to filter the results of a `SELECT` query. Subclasses should overwrite the `sql(QSLDB::Driver)`
 * method. By default, it returns an empty string.
 */
class QSLFilter
{
public:
	/// Returns an SQL expression for the given driver.
	virtual QString sql(QSL::Driver driver) const;
};

/**
 * This namespace contains a lot of filters that can be used in conjunction with `QSLQuery`. Note that
 * you can only pass one filter to the `QSLQuery` class, subsequent calls to `QSLQuery::filter` will
 * overwrite the old one.
 */
namespace filters {

/// Used to create a class that matches some value against a column.
#define _QSL_FILTER_VAL_CLASS(name, tname) \
	class name : public QSLFilter \
	{ \
	public: \
		/** Constructs the filter for the given column and the given value to match against. */ \
		name (const char *column, const QVariant &val) \
			: _column(column), _val(val) \
		{ \
		} \
		virtual QString sql(QSL::Driver driver) const; \
	private: \
		QString _column; \
		QVariant _val; \
	}; \
	/** Typedef for the `tname` filter (short: `name##`) */ \
	typedef name tname

/// Used to create a filter from a variable number of other filters.
#define _QSL_FILTER_COMBINE_CLASS(name) \
	class name : public QSLFilter \
	{ \
	public: \
		/** Constructs the filter from the given other filters. */ \
		template<typename F, typename ... Types> \
		name (const F &filter, Types ... rest) \
		{ \
			addfilter(filter, rest...); \
		} \
		virtual QString sql(QSL::Driver driver) const; \
	private: \
		void addfilter() {} \
		template<typename F, typename ... Types> \
		void addfilter(const F &filter, Types ... rest) \
		{ \
			_filters.push_back(QSharedPointer<QSLFilter>(new F(filter))); \
			addfilter(rest...); \
		} \
		QVector<QSharedPointer<QSLFilter>> _filters; \
	}

/**
 * This class is a `QSLFilter` that compares a column with a value and returns true if the value matches.
 */
_QSL_FILTER_VAL_CLASS(eq, equals);

/**
 * This class is a `QSLFilter` that compares a column with a value and returns true if the values don't match.
 */
_QSL_FILTER_VAL_CLASS(ne, unequals);

/**
 * This class is a `QSLFilter` that checks whether the given column starts with the given value.
 */
_QSL_FILTER_VAL_CLASS(sw, startsWith);

/**
 * This class is a `QSLFilter` that checks whether the given column ends with the given value.
 */
_QSL_FILTER_VAL_CLASS(ew, endsWith);

/**
 * This class is a `QSLFilter` that checks whether the given column contains the given value.
 */
_QSL_FILTER_VAL_CLASS(co, contains);

/**
 * This class combines two `QSLFilter` into one and returns true if both filters are true.
 */
_QSL_FILTER_COMBINE_CLASS(a);

/**
 * This class combines two `QSLFilter` into one and returns true if at least one filter is true.
 */
_QSL_FILTER_COMBINE_CLASS(o);

}
}
