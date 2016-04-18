#include "qslcolumn.h"

using namespace qsl;

QSLColumn::QSLColumn(const char *name, const char *type, uint8_t constraints)
	: _name(name)
	, _type(type)
	, _constraints(constraints)
{
}
