#pragma once

#include "qsltable.h"

namespace qsl {
namespace driver {

class ConstraintDifference
{
public:
	ConstraintDifference(const QSLColumn &a, const QSLColumn &b);
	
	QByteArray colName() const { return _colName; }
	uint8_t constraintsAdded() const { return _constraintsAdded; }
	uint8_t constraintsRemoved() const { return _constraintsRemoved; }
	
private:
	QByteArray _colName;
	uint8_t _constraintsAdded, _constraintsRemoved;
};

/**
 * This class is used to compute the difference of two tables. This is usefull to
 * update existing tables in a database in the `Database::ensureTable` method. For
 * more information, see `TableDiff::TableDiff`.
 */
class TableDiff
{
public:
	/**
	 * Computes the difference of table `a` and table `b`. There are four different
	 * types of differences, returned by the following functions:
	 * - `addedCols()`
	 * - `removedCols()`
	 * - `constraintsChanged()`
	 * - `typeChanged()`
	 */
	TableDiff(const QSLTable &a, const QSLTable &b);
	
	QSLTable a() const { return _a; }
	QSLTable b() const { return _b; }
	
	/**
	 * Returns the columns that were not present in table `a` but are present in table
	 * `b` and such were added.
	 */
	QList<QSLColumn> addedCols() const { return _addedCols; }
	/**
	 * Returns the columns that were present in table `a` but are no longer present in
	 * table `b` and such were removed.
	 */
	QList<QSLColumn> removedCols() const { return _removedCols; }
	/**
	 * Returns the columns that were present in both tables `a` and `b` but whose constraints
	 * were changed. For more information about constraints see `QSL::ColumnConstraint`.
	 * Note that these columns may also appear in `typeChanged()`.
	 */
	QList<ConstraintDifference> constraintsChanged() const { return _constraintsChanged; }
	/**
	 * Returns the columns that were present in both tables `a` and `b` but whose type (or
	 * the minsize of the type) has changed. Note that these columns may also appear in
	 * `constraintsChanged()`.
	 */
	QList<QSLColumn> typeChanged() const { return _typeChanged; }
	
protected:
	virtual void computeDiff();
	
	QList<QSLColumn> _addedCols;
	QList<QSLColumn> _removedCols;
	QList<ConstraintDifference> _constraintsChanged;
	QList<QSLColumn> _typeChanged;
	
private:
	QSLTable _a, _b;
};

}
}
