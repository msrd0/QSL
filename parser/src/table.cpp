#include "table.h"

#include <QMetaEnum>

using namespace qsl::qslc;

static QByteArray toCppType(const QByteArray &t, uint32_t minsize)
{
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
		return "QVariant";
}

Column::Column(const QByteArray &name, const QByteArray &type)
	: _name(name)
{
	_type = type;
	if (_type.contains('('))
	{
		_minsize = _type.mid(_type.indexOf('(')+1, _type.indexOf(')')-_type.indexOf('(')-1).toUInt();
		_type = _type.mid(0, _type.indexOf('('));
	}
	_ctype = toCppType(_type, _minsize);
}

void Column::setConstraint(const QByteArray &constraint)
{
	static QMetaEnum e = QSL::staticMetaObject.enumerator(QSL::staticMetaObject.indexOfEnumerator("ColumnConstraint"));
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
