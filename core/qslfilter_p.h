#include "qslfilter.h"

namespace qsl {

/**
 * THIS CLASS IS NOT PART OF THE API AND MIGHT CHANGE AT ANY TIME!!!
 * 
 * This class holds all data that a `QSLFilter` stores.
 */
class QSL_PRIVATE QSLFilterData : public QSharedData
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

}
