#pragma once

#include "qsl_global.h"

#include <QByteArray>

namespace qsl {

/**
 * This class is used to represent a column of a `QSLTable`.
 */
class QSLColumn
{
public:
	/// Creates a new column with the given name, type, type's minsize and constraints
	/// (see `QSL::ColumnConstraint`).
	constexpr QSLColumn(const char* name, const char* type, uint32_t minsize, uint8_t constraints)
		: _constraints(constraints)
		, _name(name)
		, _type(type)
		, _minsize(minsize)
	{
	}
	
	/// Returns the name of the column.
	QByteArray name() const { return _name; }
	/// Returns the type of the column.
	const char* type() const { return _type; }
	/// Returns the type's minsize.
	uint32_t minsize() const { return _minsize; }
	/// Returns the constraints of the column.
	uint8_t constraints() const { return _constraints; }
	
	// these functions are present to enable the use of QSLColumn inside a map
	bool operator< (const QSLColumn &other) const { return name() < other.name(); }
	
protected:
	uint8_t _constraints;
	
private:
	const char* _name;
	const char* _type;
	uint32_t _minsize;
};

}
