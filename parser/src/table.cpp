#include "table.h"

#include <QMetaEnum>

using namespace qsl::qslc;

static QByteArray toCppType(QByteArray t)
{
	uint minsize = std::numeric_limits<uint>::max();
	if (t.contains('('))
	{
		minsize = t.mid(t.indexOf('(')+1, t.indexOf(')')-t.indexOf('(')-1).toUInt();
		t = t.mid(0, t.indexOf('('));
	}
	if (t == "int")
	{
		if (minsize <= 8)
			return "int8_t";
		else if (minsize <= 16)
			return "int16_t";
		else if (minsize <= 32)
			return "int32_t";
		else
			return "int64_t";
	}
	else if (t == "uint")
	{
		if (minsize <= 8)
			return "uint8_t";
		else if (minsize <= 16)
			return "uint16_t";
		else if (minsize <= 32)
			return "uint32_t";
		else
			return "uint64_t";
	}
	else if (t == "double")
	{
		if (minsize <= 4)
			return "float";
		else
			return "double";
	}
	else if (t == "char" || t == "byte" || t == "text" || t == "blob")
		return "std::string";
	else
		return "void*";
}

Column::Column(const QByteArray &name, const QByteArray &type)
	: _name(name)
	, _type(type)
{
	_ctype = toCppType(type);
}

Column::Column(const Column &other)
	: _name(other.name())
	, _type(other.type())
	, _ctype(other.cppType())
	, _constraints(other.constraints())
{
}

Column& Column::operator= (const Column &other)
{
	_name = other.name();
	_type = other.type();
	_ctype = other.cppType();
	_constraints = other.constraints();
	return *this;
}

void Column::setConstraint(const QByteArray &constraint)
{
	static QMetaEnum e = QSLColumn::staticMetaObject.enumerator(QSLColumn::staticMetaObject.indexOfEnumerator("Constraint"));
	_constraints |= e.keyToValue(constraint);
}

Table::Table(Database *db, const QByteArray &name)
	: _db(db)
	, _name(name)
{
	Q_ASSERT(db);
}

void Table::addField(const Column &field)
{
	_fields.append(field);
}
