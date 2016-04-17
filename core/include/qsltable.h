#pragma once

#include "qsl_global.h"

namespace qsl {
class QSLDB;

/**
 * This class is used by generated database classes to represent the tables in the database.
 */
class QSLTable
{
public:
	QSLTable(const char *name, QSLDB *db);
	
	const char* name() const { return _name; }
	
private:
	const char* _name;
	QSLDB *_db;
};

}
