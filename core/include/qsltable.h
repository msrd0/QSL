#pragma once

#include "qsl_global.h"
#include "qslcolumn.h"

namespace qsl {
class QSLDB;

/**
 * This class is used by generated database classes to represent the tables in the database.
 */
class QSLTable
{
public:
	QSLTable(const char *name, QSLDB *db);
	
	void addColumn(const QSLColumn &column);
	
	const char* name() const { return _name; }
	QSLDB* db() { return _db; }
	QList<QSLColumn> columns() const { return _columns; }
	
private:
	const char* _name;
	QSLDB *_db;
	QList<QSLColumn> _columns;
};

}
