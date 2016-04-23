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
	QSLColumn(const char* name, const char* type, uint32_t minsize, uint8_t constraints);
	
	/// Returns the name of the column.
	QByteArray name() const { return _name; }
	/// Returns the type of the column.
	const char* type() const { return _type; }
	/// Returns the type's minsize.
	uint32_t minsize() const { return _minsize; }
	/// Returns the constraints of the column.
	uint8_t constraints() const { return _constraints; }
	
private:
	QByteArray _name;
	const char* _type;
	uint32_t _minsize;
	uint8_t _constraints;
};

}
