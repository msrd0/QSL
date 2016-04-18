#include "qsltable.h"

#include <QList>

using namespace qsl;

QSLTable::QSLTable(const char *name, QSLDB *db)
	: _name(name)
	, _db(db)
{
}

void QSLTable::addColumn(const QSLColumn &column)
{
	_columns.append(column);
}
