#include "qsltable.h"

using namespace qsl;

QSLTable::QSLTable(const char *name, QSLDB *db)
	: _name(name)
	, _db(db)
{
}
