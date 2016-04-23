#include "qslquery.h"
#include "qsltable.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaEnum>

using namespace qsl;

QHash<QSL::Driver, QJsonObject> QSLQuery::_driverJson;

QJsonObject QSLQuery::driverJson(QSL::Driver driver)
{
	if (_driverJson.contains(driver))
		return _driverJson[driver];
	static QMetaEnum e = QSL::staticMetaObject.enumerator(QSL::staticMetaObject.indexOfEnumerator("Driver"));
	QFile file(QString(":/drivers/") + e.valueToKey(driver) + ".json");
	file.open(QIODevice::ReadOnly);
	QByteArray a = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(a);
	file.close();
	_driverJson.insert(driver, doc.object());
	return doc.object();
}

QByteArray QSLQuery::driverType(QSL::Driver driver, const QByteArray &type, uint32_t minsize)
{
	QJsonObject json = driverJson(driver);
	QJsonObject types = json["types"].toObject();
	QJsonValue val = types[type];
	QByteArray dtype;
	if (val.isObject())
	{
		QJsonObject o = val.toObject();
		QStringList keys = o.keys();
		if (keys.empty())
			fprintf(stderr, "QSLQuery: WARNING: Empty Object for Type '%s'\n", type.data());
		std::sort(keys.begin(), keys.end(), [](const QString &a, const QString &b) {
			return a.toInt() < b.toInt();
		});
		for (QString s : keys)
			if (s.toInt() > minsize)
			{
				dtype = o[s].toVariant().toByteArray();
				break;
			}
		if (dtype.isEmpty() && keys.size() > 0)
			dtype = o[keys.last()].toVariant().toByteArray();
	}
	else
		dtype = val.toVariant().toByteArray();
	dtype.replace('$', QByteArray::number(minsize));
	if (dtype.contains('[') && dtype.contains(']'))
	{
		if (_tbl->db()->usevar())
		{
			dtype.remove(dtype.indexOf('['), 1);
			dtype.remove(dtype.indexOf(']'), 1);
		}
		else
			dtype.remove(dtype.indexOf('['), dtype.indexOf(']') - dtype.indexOf('['));
	}
	return dtype;
}

QSLQuery::QSLQuery(QSLTable *tbl, QSL::QueryType type)
	: _tbl(tbl)
	, _type(type)
{
	Q_ASSERT(type != QSL::UnknownQueryType);
}

QSLQuery::QSLQuery(QSLTable *tbl)
	: _tbl(tbl)
	, _type(QSL::UnknownQueryType)
{
}

QString QSLQuery::sql(QSL::Driver driver)
{
	Q_ASSERT(_type != QSL::UnknownQueryType);
	switch (_type)
	{
	case QSL::CreateTableQuery:
		switch (driver)
		{
		case QSL::PostgreSQL: {
				QString sql = QString("CREATE SEQUENCE ") + _tbl->db()->name() + "_" + _tbl->name() + "_pkey_seq; CREATE TABLE " + _tbl->name() + " (";
				int i = 0;
				for (QSLColumn c : _tbl->columns())
				{
					if (i > 0)
						sql += ", ";
					sql += c.name() + " " + driverType(driver, c.type(), c.minsize());
					if (c.constraints() & QSL::primarykey != 0)
						sql += QString(" PRIMARY KEY DEFAULT nextval('") + _tbl->db()->name() + "_" + _tbl->name() + "_pkey_seq')";
					if (c.constraints() & QSL::unique != 0)
						sql += " UNIQUE";
					if (c.constraints() & QSL::notnull != 0)
						sql += " NOT NULL";
					i++;
				}
				sql += ");";
				return sql;
			}
		}
		return __FILE__ + QString::number(__LINE__);
	case QSL::SelectQuery:
		switch (driver)
		{
		case QSL::PostgreSQL: {
				QString filter = _filter.sql(driver);
				QString sql = QString("SELECT * FROM ") + _tbl->name();
				if (!filter.isEmpty())
					sql += " " + filter;
				if (_limit > 0)
					sql += " LIMIT BY " + _limit;
				return sql + ";";
			}
		}
		return __FILE__ + QString::number(__LINE__);
	case QSL::InsertQuery:
		switch (driver)
		{
		case QSL::PostgreSQL: {
				if (_rows.empty())
					return QString();
				QString sql = QString("INSERT INTO ") + _tbl->name() + " (";
				int i = 0;
				for (QSLColumn c : _tbl->columns())
				{
					if (c.constraints() & QSL::primarykey != 0)
						continue;
					if (i > 0)
						sql += ", ";
					sql += c.name();
					i++;
				}
				sql += ") VALUES ";
				for (auto r : _rows)
				{
					sql += "(";
					i = 0;
					for (auto c : r)
					{
						if (i > 0)
							sql += ", ";
						sql += "'" + c.toString().replace("'", "''") + "'";
					}
					sql += ")";
				}
				return sql + ";";
			}
		}

	default:
		return __FILE__ + QString::number(__LINE__);
	}
}
