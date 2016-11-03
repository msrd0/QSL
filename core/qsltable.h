#pragma once

/// @file qsltable.h This file contains the `qsl::QSLTable` class to store meta-data about
/// a table at runtime.

#include "qsl_global.h"
#include "qslcolumn.h"

#include <QList>

namespace qsl {
class QSLDB;

/**
 * This class is used by generated database classes to represent the tables in the database.
 * After the table is completely constructed it needs to be registered via `QSLDB::registerTable()`.
 */
class QSLTable
{
public:
	/// Creates a new table with the given name for the given database. Please note that the
	/// database won't be notified of this table; you have to do so explicitly via `QSLDB::registerTable()`.
	QSLTable(const QByteArray &name, const QByteArray &pk, QSLDB *db = 0)
		: _name(name)
		, _pk(pk)
		, _db(db)
	{
	}
	
	/// Adds a new column to the table. This should be called before the table is added
	/// to the database using `QSLDB::registerTable()`.
	void addColumn(const QSLColumn &column)
	{
		_columns.append(column);
	}
	
	/// Returns the name of the table.
	QByteArray name() const { return _name; }
	/// Returns the primary key of the table, or an empty string if the table has no primary key.
	QByteArray primaryKey() const { return _pk; }
	/// Returns the parent `QSLDB`.
	QSLDB* db() { return _db; }
	/// Returns all columns in this table.
	QList<QSLColumn> columns() const { return _columns; }
	
private:
	QByteArray _name;
	QByteArray _pk;
	QSLDB *_db;
	QList<QSLColumn> _columns;
};

}
