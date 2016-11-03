#pragma once

/// @file qslcolumn.h This file contains the `qsl::QSLColumn` class to store meta-data
/// about a column at runtime.

#include "qslnamespace.h"

namespace qsl {

/**
 * This class is used to represent a column of a `QSLTable`.
 */
class QSLColumn
{
public:
	/// Creates a new column with the given name, type, type's minsize and constraints
	/// (see `QSL::ColumnConstraint`).
	constexpr QSLColumn(const char* name, const char* type, int minsize, uint8_t constraints)
		: _constraints(constraints)
		, _name(name)
		, _type(type)
		, _minsize(minsize)
		, malloced(false)
	{
	}
	
	/// Creates a new column with the given name, type, type's minsize and constraints
	/// (see `QSL::ColumnConstraint`).
	QSLColumn(const QByteArray &name, const QByteArray &type, int minsize, uint8_t constraints)
		: _constraints(constraints)
		, _minsize(minsize)
		, malloced(true)
	{
		char *nname = new char[name.size()+1];
		strcpy(nname, name);
		_name = nname;
		
		char *ntype = new char[type.size()+1];
		strcpy(ntype, type);
		_type = ntype;
	}
	
	/// Returns the name of the column.
	QByteArray name() const { return _name; }
	/// Returns the type of the column.
	const char* type() const { return _type; }
	/// Returns the type's minsize.
	int minsize() const { return _minsize; }
	/// Returns the constraints of the column.
	uint8_t constraints() const { return _constraints; }
	
	// these functions are present to enable the use of QSLColumn inside a map
	/** Compares the name of this column to the name of the other column. See `QByteArray::operator<`. */
	bool operator< (const QSLColumn &other) const { return name() < other.name(); }
	
protected:
	/** The constraints of this column. */
	uint8_t _constraints;
	
private:
	const char* _name;
	const char* _type;
	int _minsize;
	
	bool malloced;
};

}
