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

QByteArray QSLQuery::driverType(QSL::Driver driver, QByteArray type)
{
	QJsonObject json = driverJson(driver);
	QJsonObject types = json["types"].toObject();
	uint minsize = std::numeric_limits<uint>::max();
	if (type.contains('('))
	{
		minsize = type.mid(type.indexOf('(')+1, type.indexOf(')')-type.indexOf('(')-1).toUInt();
		type = type.mid(0, type.indexOf('('));
	}
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
	dtype.replace('$', minsize);
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
}

QString QSLQuery::sql(QSL::Driver driver)
{
	switch (_type)
	{
	case QSL::CreateTable:
		switch (driver)
		{
		case QSL::PostgreSQL: {
				QString sql = QString("CREATE TABLE ") + _tbl->name() + " (";
				int i = 0;
				for (QSLColumn c : _tbl->columns())
				{
					if (i > 0)
						sql += ", ";
					printf("sql += '%s' + ' ' + '%s'\n", c.name().data(), driverType(driver, c.type()).data());
					sql += c.name() + " " + driverType(driver, c.type());
					if (c.constraints() & QSL::primarykey != 0)
						sql += " PRIMARY KEY";
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
	}
}
