#include "qslquery.h"
#include "qsltable.h"

using namespace qsl;

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
			}
		}
	}
}
