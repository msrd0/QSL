#pragma once

/// @file qslnamespace.h This file contains a class called `QSL` that holds some usefull
/// enums and the documentation of the namespaces of the core library.

#include "qsl_global.h"

#include <QObject>

/**
 * This class simply contains a lot of enums. The reason for this being a class
 * and not a namespace is that all enums are registered via [moc](http://doc.qt.io/qt-5/moc.html).
 */
class QSL_PUBLIC QSL : public QObject
{
	Q_OBJECT
	
public:
	
	/// A list of constraints that can be used to describe a column.
	enum ColumnConstraint : uint8_t
	{
		none       = 0x0,
		primarykey = 0x1,
		unique     = 0x2,
		notnull    = 0x4
	};
	Q_ENUM(ColumnConstraint)
};

// just for doxygen

/**
 * This is the namespace containing every class by the QSL Core library except
 * for `QSL`.
 */
namespace qsl {

/**
 * This namespace contains all generated databases. No QSL library will declare
 * types inside this namespace.
 */
namespace db {
}

}
