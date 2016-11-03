#include "qslquery.h"
#include "qsltable.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaEnum>

using namespace qsl;
using namespace qsl::driver;

QSLQuery::QSLQuery(QSLTable *tbl)
	: _tbl(tbl)
{
	Q_ASSERT(tbl);
}
