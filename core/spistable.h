#pragma once

/// @file spistable.h This file contains the `spis::SPISTable` class to store meta-data about
/// a table at runtime.

#include "spis_global.h"
#include "spiscolumn.h"

#include <QMap>

namespace spis {
class SPISDB;

/**
 * This class is used by generated database classes to represent the tables in the database.
 * After the table is completely constructed it needs to be registered via `SPISDB::registerTable()`.
 */
class SPIS_PUBLIC SPISTable
{
public:
	/// Creates a new table with the given name for the given database. Please note that the
	/// database won't be notified of this table; you have to do so explicitly via `SPISDB::registerTable()`.
	SPISTable(const QByteArray &name, const QByteArray &pk, SPISDB *db = 0)
		: _name(name)
		, _pk(pk)
		, _db(db)
	{
	}
	
	/// Adds a new column to the table. This should be called before the table is added
	/// to the database using `SPISDB::registerTable()`.
	void addColumn(const SPISColumn &column)
	{
		_columns.insert(column.name(), column);
	}
	
	/// Returns the name of the table.
	QByteArray name() const { return _name; }
	/// Returns the primary key of the table, or an empty string if the table has no primary key.
	QByteArray primaryKey() const { return _pk; }
	/// Returns the parent `SPISDB`.
	SPISDB* db() { return _db; }
	/// Returns all columns in this table.
	QList<SPISColumn> columns() const { return _columns.values(); }
	/// Returns the column with the given name.
	SPISColumn column(const QByteArray &name) const { return _columns.value(name, SPISColumn("invalid", "invalid", -1, SPIS::none)); }
	
private:
	QByteArray _name;
	QByteArray _pk;
	SPISDB *_db;
	QMap<QByteArray, SPISColumn> _columns;
};

}
