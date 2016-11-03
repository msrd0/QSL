#include "table.h"

#include <QDebug>
#include <QMetaEnum>

using namespace qsl::qslc;

static std::pair<QByteArray, bool> toCppType(const QByteArray &t, uint32_t minsize, bool qtype)
{
	if (t == "int")
	{
		if (minsize <= 8)
			return {qtype ? "qint8" : "int8_t", false};
		else if (minsize <= 16)
			return {qtype ? "qint16" : "int16_t", false};
		else if (minsize <= 32)
			return {qtype ? "qint32" : "int32_t", false};
		else
			return {qtype ? "qint64" : "int64_t", false};
	}
	else if (t == "uint")
	{
		if (minsize <= 8)
			return {qtype ? "quint8" : "uint8_t", false};
		else if (minsize <= 16)
			return {qtype ? "quint16" : "uint16_t", false};
		else if (minsize <= 32)
			return {qtype ? "quint32" : "uint32_t", false};
		else
			return {qtype ? "quint64" : "uint64_t", false};
	}
	else if (t == "double")
	{
		if (minsize <= 4)
			return {"float", false};
		else
			return {qtype ? "qreal" : "double", false};
	}
	else if (t == "bool")
		return {"bool", false};
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
	else if (t == "password")
		return {"qsl::Password", true};
	else if (!qtype && (t == "date" || t == "time" || t == "datetime"))
		return {"std::chrono::system_clock::time_point", true};
	else if (t == "date")
		return {"QDate", true};
	else if (t == "time")
		return {"QTime", true};
	else if (t == "datetime")
		return {"QDateTime", true};
	else if (t == "variant")
		return {"QVariant", true};
	else if (t.startsWith('&'))
		return {t.mid(1, t.indexOf('.')-1) + "_t", true};
	else
	{
		qCritical() << "QSL[Parser]: Unknown type" << t << "(in " __FILE__ " line" << __LINE__ << ")";
		return {"void*", false};
	}
}

Column::Column(const QByteArray &name, const QByteArray &type, bool qtype) 
	: _name(name)
{
	_type = type;
	if (_type.contains('('))
	{
		QByteArray ms = _type.mid(_type.indexOf('(')+1, _type.indexOf(')')-_type.indexOf('(')-1);
		uint factor = 1;
		if (ms.endsWith("K"))
			factor = 1e3;
		else if (ms.endsWith("M"))
			factor = 1e6;
		else if (ms.endsWith("G"))
			factor = 1e9;
		if (factor != 1)
			ms = ms.mid(0, ms.size()-1);
		_minsize = ms.toUInt() * factor;
		
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
