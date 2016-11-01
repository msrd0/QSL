#include "qslquery.h"
#include "qsltable.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaEnum>

using namespace qsl;
using namespace qsl::driver;

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
