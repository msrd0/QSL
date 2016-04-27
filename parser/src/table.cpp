#include "table.h"

#include <QMetaEnum>

using namespace qsl::qslc;

static std::pair<QByteArray, bool> toCppType(const QByteArray &t, uint32_t minsize, bool qtype)
{
	if (t == "int")
	{
		if (minsize <= 8)
			return {"int8_t", false};
		else if (minsize <= 16)
			return {"int16_t", false};
		else if (minsize <= 32)
			return {"int32_t", false};
		else
			return {"int64_t", false};
	}
	else if (t == "uint")
	{
		if (minsize <= 8)
			return {"uint8_t", false};
		else if (minsize <= 16)
			return {"uint16_t", false};
		else if (minsize <= 32)
			return {"uint32_t", false};
		else
			return {"uint64_t", false};
	}
	else if (t == "double")
	{
		if (minsize <= 4)
			return {"float", false};
		else
			return {"double", false};
	}
	else if (t == "char" || t == "byte" || t == "text" || t == "blob")
	{
		if (qtype)
		{
			if (t == "char" || t == "text")
				return {"QString", true};
			else
				return {"QByteArray", true};
		}
		return {"std::string", true};
	}
	else
		return {"QVariant", true};
}

Column::Column(const QByteArray &name, const QByteArray &type, bool qtype) 
	: _name(name)
{
	_type = type;
	if (_type.contains('('))
	{
		_minsize = _type.mid(_type.indexOf('(')+1, _type.indexOf(')')-_type.indexOf('(')-1).toUInt();
		_type = _type.mid(0, _type.indexOf('('));
	}
	auto ct = toCppType(_type, _minsize, qtype);
	_ctype = ct.first;
	_cref  = ct.second;
}

int Column::setConstraint(const QByteArray &constraint)
{
	static QMetaEnum e = QSL::staticMetaObject.enumerator(QSL::staticMetaObject.indexOfEnumerator("ColumnConstraint"));
	int val = e.keyToValue(constraint);
	_constraints |= val;
	return val;
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
