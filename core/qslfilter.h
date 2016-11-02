#pragma once

#include "qsl_global.h"
#include "qsldb.h"
#include "qslnamespace.h"
#include "qslvariant.h"

#include <QSharedDataPointer>
#include <QVariant>
#include <QVector>

namespace qsl {
class QSLFilterData;
class QSLFilterExprType;

class QSLFilter : public QObject
{
	Q_OBJECT
	
public:
	enum Operator : uint8_t
	{
		noop           = 0x0,
		
		// operators with 2 args
		
		eq             = 0x1,
		equals         = eq,
		
		ne             = 0x2,
		not_equals     = ne,
		
		lt             = 0x3,
		lesser_than    = lt,
		
		le             = 0x4,
		lesser_equal   = le,
		
		gt             = 0x5,
		greater_than   = gt,
		
		ge             = 0x6,
		greater_equal  = ge,
		
		like           = 0x7,
		
		// operators with 1 arg
		
		isnull         = 0x20,
		
		notnull        = 0x21,
		
		// operators that connect expressions
		
		op_and         = 0x30,
		
		op_or          = 0x31,
		
		// operators that change one expression
		
		op_not         = 0x40,
		
	};
	Q_ENUM(Operator)
	
	static const char* opName(Operator op);
	
	/**
	 * Constructs an empty filter.
	 */
	QSLFilter();
	
	/**
	 * Constructs a filter that compares the column `col` with either the column or the
	 * value `val` by using the operator `op`. It should be one of:
	 * 
	 * - `Operator::eq` or `Operator::equals`
	 * - `Operator::ne` or `Operator::not_equals`
	 * - `Operator::lt` or `Operator::lesser_than`
	 * - `Operator::le` or `Operator::lesser_equal`
	 * - `Operator::gt` or `Operator::greater_than`
	 * - `Operator::ge` or `Operator::greater_equal`
	 * - `Operator::like`
	 * 
	 * If `val` is the name of a column, it will be treated as the corresponding value of the
	 * column in that row. Otherwise it will be treated as a value. To treat the name of a
	 * column as a value, enquote it with single quotes (`'`).
	 * 
	 * In the case that `val` is treated as a value, it defaults to be treated as a string. To
	 * change this, prefix the value with either `int:` or `double:`. This needs to happen before
	 * the quote if present, otherwise it will belong to the value itself. Everything prefixed
	 * will of course not be treated as a column.
	 */
	QSLFilter(const QString &col, Operator op, const QString &val);
	
	/**
	 * Constructs a filter that emposes a condition on the column `col`. The operator should
	 * be one of:
	 * 
	 * - `Operator::isnull`
	 * - `Operator::notnull`
	 */
	QSLFilter(const QString &col, Operator op);
	
	/**
	 * Constructs a new filter by comparing the two filters using the operator `op`. It should
	 * be one of:
	 * 
	 * - `Operator::op_and`
	 * - `Operator::op_or`
	 */
	QSLFilter(const QSLFilter &filter0, Operator op, const QSLFilter &filter1);
	
	/**
	 * Constructs a new filter that changes the output of the filter `filter` using the operator
	 * `op`. It should be one of:
	 * 
	 * - `Operator::op_not`
	 */
	QSLFilter(Operator op, const QSLFilter &filter);
	
	/** Returns the operator of this filter. */
	Operator op() const;
	/** Returns the column or value argument at index i. */
	QString arg(int i) const;
	/** Returns the filter argument at index i. */
	QSLFilter filter(int i) const;
	
private:
	QSharedDataPointer<QSLFilterData> d;
	
	
	// all those methods that were deleted by QObject
public:
	QSLFilter(const QSLFilter &other)
		: QObject()
		, d(other.d)
	{
	}
	QSLFilter(QSLFilter &&other)
		: QObject()
		, d(other.d)
	{
	}
	
	QSLFilter& operator= (const QSLFilter &other)
	{
		d = other.d;
		return *this;
	}
	QSLFilter& operator= (QSLFilter &&other)
	{
		d = other.d;
		return *this;
	}
	
};

/**
 * THIS CLASS IS NOT PART OF THE API AND MIGHT CHANGE AT ANY TIME!!!
 * 
 * This class holds all data that a `QSLFilter` stores.
 */
class QSLFilterData : public QSharedData
{
public:
	QSLFilterData(QSLFilter::Operator oper)
		: op(oper)
	{
	}
	
	QSLFilterData(const QSLFilterData &other)
		: QSharedData(other)
		, op(other.op)
		, filters(other.filters)
		, args(other.args)
	{
	}
	
	QSLFilter::Operator op;
	QVector<QSLFilter> filters;
	QVector<QString> args;
};

/**
 * This is a convenient class to store values of either string, integer,
 * double or `QSLFilter::Operator`. Note that integer and double types are
 * converted to strings but prefixed with either `int:` or `double:`.
 */
class QSLFilterExprType
{
public:
	QSLFilterExprType(const QString &str)
		: _isOperator(false)
		, _val(str)
	{
	}
	QSLFilterExprType(const char *str)
		: _isOperator(false)
		, _val(str)
	{
	}
	QSLFilterExprType(const std::string &str)
		: _isOperator(false)
		, _val(str.data())
	{
	}
	QSLFilterExprType(int64_t i)
		: _isOperator(false)
		, _val("int:" + QString::number(i))
	{
	}
	QSLFilterExprType(double d)
		: _isOperator(false)
		, _val("double:" + QString::number(d, 'f', std::numeric_limits<double>::max_digits10))
	{
	}
	QSLFilterExprType(QSLFilter::Operator op)
		: _isOperator(true)
		, _val((char)op)
	{
	}
	
	bool isOperator() const { return _isOperator; }
	
	QString str() const { return _val; }
	QSLFilter::Operator op() const { return (QSLFilter::Operator)qPrintable(_val)[0]; }
	
private:
	bool _isOperator;
	QString _val;
};

/**
 * Constructs a QSLFilter by parsing the given arguments. These arguments
 * must be of type `QSLFilterExprType`.
 */
extern QSLFilter qsl_filter(const QList<QSLFilterExprType> &args);

// and for convenience, a lot of definitions

// all operators with 2 args

#define EQ ,qsl::QSLFilter::eq,
#define EQUALS EQ

#define NE ,qsl::QSLFilter::ne,
#define NOTEQUALS NE

#define LT ,qsl::QSLFilter::lt,
#define LESSER_THAN LT

#define LE ,qsl::QSLFilter::le,
#define LESSER_EQUAL LE

#define GT ,qsl::QSLFilter::gt,
#define GREATER_THAN GT

#define GE ,qsl::QSLFilter::ge,
#define GREATER_EQUAL GE

#define LIKE ,qsl::QSLFilter::like,

// all operators with 1 arg

#define ISNULL ,qsl::QSLFilter::isnull

#define NOTNULL ,qsl::QSLFilter::notnull

// operators that connect expressions

#define AND ,qsl::QSLFilter::op_and,

#define OR ,qsl::QSLFilter::op_or,

// operators that change one expression

#define NOT qsl::QSLFilter::op_not,

}
