#include "database.h"
#include "table.h"

using namespace qsl::qslc;

Database::Database(const QByteArray &name)
	: _name(name)
{
}

Database::~Database()
{
	qDeleteAll(_tables);
}

bool Database::containsTable(const QByteArray &name)
{
	return _tables.contains(name);
}

void Database::addTable(Table *table)
{
	_tables.insert(table->name(), table);
}
