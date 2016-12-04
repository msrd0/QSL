#pragma once

/// @file spiscolumn.h This file contains the `spis::SPISColumn` class to store meta-data
/// about a column at runtime.

#include "spisnamespace.h"

#include <QSharedDataPointer>
#include <QVariant>

namespace spis {

/**
 * THIS CLASS IS NOT PART OF THE API AND MIGHT CHANGE AT ANY TIME!!!
 * 
 * This class holds all data that a `SPISColumn` stores.
 */
class SPIS_PRIVATE SPISColumnData : public QSharedData
{
public:
	SPISColumnData(const QByteArray &name, const QByteArray &type, int minsize, uint8_t constraints, const QVariant &def)
		: name(name)
		, type(type)
		, minsize(minsize)
		, constraints(constraints)
		, def(def)
	{
	}
	
	QByteArray name;
	QByteArray type;
	int minsize;
	uint8_t constraints;
	
	QVariant def;
};

/**
 * This class is used to represent a column of a `SPISTable`.
 */
class SPIS_PUBLIC SPISColumn
{
public:
	
	/// Creates a new column with the given name, type, type's minsize and constraints
	/// (see `SPIS::ColumnConstraint`).
	SPISColumn(const QByteArray &name, const QByteArray &type, int minsize, uint8_t constraints, const QVariant &def)
		: d(new SPISColumnData(name, type, minsize, constraints, def))
	{
	}
	
	/// Returns the name of the column.
	QByteArray name() const { return d->name; }
	/// Returns the type of the column.
	QByteArray type() const { return d->type; }
	/// Returns the type's minsize.
	int minsize() const { return d->minsize; }
	/// Returns the constraints of the column.
	uint8_t constraints() const { return d->constraints; }
	
	/// Returns the default value of the column.
	QVariant def() const { return d->def; }
	
	// these functions are present to enable the use of SPISColumn inside a map
	/** Compares the name of this column to the name of the other column. See `QByteArray::operator<`. */
	bool operator< (const SPISColumn &other) const { return name() < other.name(); }
	
protected:
	QSharedDataPointer<SPISColumnData> d;
};

}
