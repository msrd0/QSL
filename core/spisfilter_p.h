#include "spisfilter.h"

namespace spis {

/**
 * THIS CLASS IS NOT PART OF THE API AND MIGHT CHANGE AT ANY TIME!!!
 * 
 * This class holds all data that a `SPISFilter` stores.
 */
class SPIS_PRIVATE SPISFilterData : public QSharedData
{
public:
	SPISFilterData(SPISFilter::Operator oper)
		: op(oper)
	{
	}
	
	SPISFilterData(const SPISFilterData &other)
		: QSharedData(other)
		, op(other.op)
		, filters(other.filters)
		, args(other.args)
	{
	}
	
	SPISFilter::Operator op;
	QVector<SPISFilter> filters;
	QVector<QString> args;
};

}
