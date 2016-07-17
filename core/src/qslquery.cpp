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

QByteArray QSLQuery::driverType(QSL::Driver driver, const QByteArray &type, uint32_t minsize) const
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

void QSLQuery::updateq(const QString &col, const QVariant &val, const QVariant &pk)
{
	_ucol = col;
	_uval = val;
	_upk  = pk;
}

#ifdef CMAKE_DEBUG
#  define ret(x) \
	{ \
	QString __ret = (x); \
	qDebug() << "QSLQuery: DEBUG: sql(" << driver << _type << "):" << __ret; \
	return __ret; \
	}
#else
#  define ret(x) return (x);
#endif

QString QSLQuery::sql(QSL::Driver driver) const
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
				ret(sql);
			}
		case QSL::SQLite: {
				QString sql = QString("CREATE TABLE ") + _tbl->name() + " (";
				int i = 0;
				for (QSLColumn c : _tbl->columns())
				{
					if (c.constraints() & QSL::primarykey != 0)
						continue; // pkey is added as 'rowid' automatically
					if (i > 0)
						sql += ", ";
					sql += c.name() + " " + driverType(driver, c.type(), c.minsize());
					if (c.constraints() & QSL::unique != 0)
						sql += " UNIQUE";
					if (c.constraints() & QSL::notnull != 0)
						sql += " NOT NULL";
					i++;
				}
				sql += ");";
				ret(sql);
			}
		}
		ret(__FILE__ + QString::number(__LINE__));
	case QSL::SelectQuery:
		switch (driver)
		{
		case QSL::PostgreSQL: {
				QString filter = _filter->sql(driver);
				QString sql = QString("SELECT * FROM ") + _tbl->name();
				if (!filter.isEmpty())
				{
					sql += " WHERE " + filter;
				}
				if (_limit > 0)
					sql += " LIMIT BY " + _limit;
				ret(sql + ";");
			}
		case QSL::SQLite: {
				QString filter = _filter->sql(driver);
				QString sql = QString("SELECT *, rowid AS ") + _tbl->primaryKey() + " FROM " + _tbl->name();
				if (!filter.isEmpty())
					sql += " WHERE " + filter;
				if (_limit > 0)
					sql += " LIMIT BY " + _limit;
				ret(sql + ";");
			}
		}
		ret(__FILE__ + QString::number(__LINE__));
	case QSL::InsertQuery:
		switch (driver)
		{
		case QSL::PostgreSQL:
		case QSL::SQLite: {
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
						i++;
					}
					sql += ")";
				}
				ret(sql + ";");
			}
		}
		ret(__FILE__ + QString::number(__LINE__));
	case QSL::UpdateQuery:
		switch (driver)
		{
		case QSL::PostgreSQL: {
				if (_ucol.isEmpty() || _uval.isNull() || _upk.isNull())
					return QString();
				ret(QString("UPDATE ") + _tbl->name() + " SET " + _ucol + "='" + _uval.toString().replace("'", "''")
						+ "' WHERE " + _tbl->primaryKey() + "='" + _upk.toString().replace("'", "''") + "';");
			}
		case QSL::SQLite: {
				if (_ucol.isEmpty() || _uval.isNull() || _upk.isNull())
					return QString();
				ret(QString("UPDATE ") + _tbl->name() + " SET " + _ucol + "='" + _uval.toString().replace("'", "''")
						+ "' WHERE rowid='" + _upk.toString().replace("'", "''") + "';");
			}
		}
		ret(__FILE__ + QString::number(__LINE__));

	default:
		ret(__FILE__ + QString::number(__LINE__));
	}
}
