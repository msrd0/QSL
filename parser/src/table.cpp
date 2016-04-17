#include "table.h"

#include <QMetaEnum>

using namespace qsl::qslc;

Field::Field(const QByteArray &name, const QByteArray &type)
	: _name(name)
	, _type(type)
{
}

Field::Field(const Field &other)
	: _name(other.name())
	, _type(other.type())
	, _constraints(other.constraints())
{
}

Field& Field::operator= (const Field &other)
{
	_name = other.name();
	_type = other.type();
	_constraints = other.constraints();
	return *this;
}

void Field::setConstraint(const QByteArray &constraint)
{
	QMetaEnum e = metaObject()->enumerator(metaObject()->indexOfEnumerator(constraint.data()));
	_constraints |= e.keyToValue(constraint);
}

Table::Table(Database *db, const QByteArray &name)
	: _db(db)
	, _name(name)
{
	Q_ASSERT(db);
}

void Table::addField(const Field &field)
{
	_fields.append(field);
}
