#pragma once

/// @file spiscolumn.h This file contains the `spis::SPISColumn` class to store meta-data
/// about a column at runtime.

#include "spisnamespace.h"

#include <QSet>
#include <QSharedDataPointer>
#include <QVariant>

namespace spis {

/**
 * This class holds all data that a `SPISColumn` stores.
 */
class SPIS_PRIVATE SPISColumnData : public QSharedData
{
public:
	SPISColumnData() = default;
	
	SPISColumnData(const QByteArray &name, const QByteArray &type, int minsize, uint8_t constraints, const QVariant &def)
		: name(name)
		, nameInDb(name)
		, type(type)
		, minsize(minsize)
		, constraints(constraints)
		, def(def)
	{
	}
	
	SPISColumnData(const QByteArray &name, const QByteArray &nameInDb, const QSet<QByteArray> &alternateNames, bool rename,
				   const QByteArray &type, int minsize, uint8_t constraints, const QVariant &def)
		: name(name)
		, nameInDb(nameInDb)
		, alternateNames(alternateNames)
		, rename(rename)
		, type(type)
		, minsize(minsize)
		, constraints(constraints)
		, def(def)
	{
	}
	
	/// The name of the column.
	QByteArray name;
	/// The name of the column that should be used in the database.
	QByteArray nameInDb;
	/// A set of alternative names that may appear in the database.
	QSet<QByteArray> alternateNames;
	/// When an alternative name is found in the db and this flag is set to true, the column
	/// is renamed to `nameInDb`.
	bool rename = false;
	/// The name that is used in the current database. Required by the driver.
	QByteArray usedName;
	
	/// The SPIS type of the column.
	QByteArray type;
	/// The minsize of the column, or -1 to use the largest available.
	int minsize = -1;
	/// The constraints of this column.
	uint8_t constraints = SPIS::none;
	
	/// The default value of this column or an invalid or null variant.
	QVariant def;
	
};

/**
 * This class is used to represent a column of a `SPISTable`.
 */
class SPIS_PUBLIC SPISColumn
{
public:
	
	/// Creates a new column with the given name, names to be used in the db including their behaviour,
	/// type, type's minsize, constraints (see `SPIS::ColumnConstraint`) and default value.
	/// 
	/// For a detailed explanation of these parameters take a look at the public fields of `SPISColumnData`.
	SPISColumn(const QByteArray &name, const QByteArray &nameInDb, const QSet<QByteArray> &alternateNames, bool rename,
			   const QByteArray &type, int minsize, uint8_t constraints, const QVariant &def)
		: d(new SPISColumnData(name, nameInDb, alternateNames, rename, type, minsize, constraints, def))
	{
	}
	
	/// Creates a new column with the given name, type, type's minsize, constraints
	/// (see `SPIS::ColumnConstraint`) and default value.
	/// 
	/// For a detailed explanation of these parameters take a look at the public fields of `SPISColumnData`.
	SPISColumn(const QByteArray &name, const QByteArray &type, int minsize, uint8_t constraints, const QVariant &def)
		: d(new SPISColumnData(name, type, minsize, constraints, def))
	{
	}
	
protected:
	
	SPISColumn() : d(new SPISColumnData)
	{
	}
	
public:
	
	/// Returns the name of the column.
	QByteArray name() const { return d->name; }
	/// Returns the name of the column that should be used in the database.
	QByteArray nameInDb() const { return d->nameInDb; }
	/// Returns the alternative names of the column that can be appear in the database.
	QSet<QByteArray> alternateNames() const { return d->alternateNames; }
	/// When an alternative name is found in the db and this method returns true, the column
	/// should be renamed to `nameInDb`.
	bool rename() const { return d->rename; }
	/// The name that is used in the current db. This value is automatically updated by the
	/// driver and should never be changed manually.
	QByteArray usedName() const { return d->usedName; }
	/// The driver calls this method to remember the name that is currently used in the database.
	/// Never call this from outside the driver.
	void setUsedName(const QByteArray &name) { d->usedName = name; }
	
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
