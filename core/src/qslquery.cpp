#include "qslquery.h"
#include "qsltable.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaEnum>

using namespace qsl;

QHash<QSLDB::Driver, QJsonObject> QSLQuery::_driverJson;

QJsonObject QSLQuery::driverJson(QSLDB::Driver driver)
{
	if (_driverJson.contains(driver))
		return _driverJson[driver];
	static QMetaEnum e = QSLDB::staticMetaObject.enumerator(QSLDB::staticMetaObject.indexOfEnumerator("Driver"));
	QFile file(QString(":/drivers/") + e.valueToKey(driver) + ".json");
	file.open(QIODevice::ReadOnly);
	QByteArray a = file.readAll();
//	qDebug() << a;
//	QJsonParseError err;
	QJsonDocument doc = QJsonDocument::fromJson(a);
//	qDebug() << err.errorString();
	file.close();
//	qDebug() << doc.toJson();
	_driverJson.insert(driver, doc.object());
	return doc.object();
}

QByteArray QSLQuery::driverType(QSLDB::Driver driver, QByteArray type)
{
	QJsonObject json = driverJson(driver);
//	qDebug() << json;
	QJsonObject types = json["types"].toObject();
//	qDebug() << types;
	QJsonValue val = types[type];
	int minsize = -1;
	if (type.contains('('))
	{
		minsize = type.mid(type.indexOf('(')+1, type.indexOf(')')-type.indexOf('(')-1).toInt();
		type = type.mid(0, type.indexOf('('));
	}
	QByteArray dtype;
	qDebug() << type << minsize;
	if (val.isString())
		dtype = val.toVariant().toByteArray();
	else
	{
		QJsonObject o = val.toObject();
		QStringList keys = o.keys();
//		qDebug() << keys;
		std::sort(keys.begin(), keys.end(), [](const QString &a, const QString &b) {
			return a.toInt() < b.toInt();
		});
//		qDebug() << keys;
		for (QString s : keys)
			if (s.toInt() > minsize)
			{
				dtype = o[s].toVariant().toByteArray();
				break;
			}
		if (dtype.isEmpty() && keys.size() > 0)
			dtype = o[keys.last()].toVariant().toByteArray();
	}
	return dtype;
}

QSLQuery::QSLQuery(QSLTable *tbl, Type type)
	: _tbl(tbl)
	, _type(type)
{
}

QString QSLQuery::sql(QSLDB::Driver driver)
{
	switch (_type)
	{
	case CreateTable:
		switch (driver)
		{
		case QSLDB::PostgreSQL: {
				QString sql = QString("CREATE TABLE ") + _tbl->name() + " (";
				int i = 0;
				for (QSLColumn c : _tbl->columns())
				{
					if (i > 0)
						sql += ", ";
					printf("sql += '%s' + ' ' + '%s'\n", c.name().data(), driverType(driver, c.type()).data());
					sql += c.name() + " " + driverType(driver, c.type());
					if (c.constraints() & QSLColumn::primarykey != 0)
						sql += " PRIMARY KEY";
					if (c.constraints() & QSLColumn::unique != 0)
						sql += " UNIQUE";
					if (c.constraints() & QSLColumn::notnull != 0)
						sql += " NOT NULL";
					i++;
				}
				sql += ");";
				return sql;
			}
		}
	}
}
