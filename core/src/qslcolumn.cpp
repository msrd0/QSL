#include "qslcolumn.h"

using namespace qsl;

QSLColumn::QSLColumn(const char *name, const char *type, uint32_t minsize, uint8_t constraints)
	: _name(name)
	, _type(type)
	, _minsize(minsize)
	, _constraints(constraints)
{
}
