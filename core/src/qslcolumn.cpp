#include "qslcolumn.h"

using namespace qsl;

QSLColumn::QSLColumn(const char *name, const char *type, uint8_t constraints)
	: _name(name)
	, _type(type)
	, _constraints(constraints)
{
}

QSLColumn::QSLColumn(const QSLColumn &other)
	: _name(other._name)
	, _type(other._type)
	, _constraints(other._constraints)
{
}

QSLColumn& QSLColumn::operator= (const QSLColumn &other)
{
	_name = other._name;
	_type = other._type;
	_constraints = other._constraints;
	return *this;
}
