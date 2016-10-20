#include "qsltable.h"

#include <QList>

using namespace qsl;

QSLTable::QSLTable(const char *name, const char *pk, QSLDB *db)
	: _name(name)
	, _pk(pk)
	, _db(db)
{
}

void QSLTable::addColumn(const QSLColumn &column)
{
	_columns.append(column);
}
