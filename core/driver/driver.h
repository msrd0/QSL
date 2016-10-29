#pragma once

#include "db.h"
#include "qslfilter.h"
#include "qsltable.h"

#include <QHash>
#include <QSharedPointer>

namespace qsl {

/**
 * This namespace contains driver-related classes. You should not need to use this
 * namespace directly, however if you want to write your own driver take a look at
 * `qsl::driver::Driver`.
 */
namespace driver {

/**
 * This is the base class of every driver. It is used to execute commands on the
 * database. To create your own driver, subclass this class and provide a method
 * called init_qsl_driver, like this:
 * ```
 * extern "C"
 * void init_qsl_driver()
 * {
 *     bool success = Driver::exportDriver("YourDriverName", new YourDriver);
 *     if (!success)
 *         fprintf(stderr, "Some error occured while exporting driver\n");
 * }
 * ```
 * Then, package everything into a library called `libqsldYourDriverName.so`.
 */
class Driver
{
public:
	/**
	 * Exports a new Driver with the given name. If driver is 0 or a driver
	 * with the same name is already present, false is returned.
	 */
	static bool exportDriver(const QString &name, Driver* driver);
	/**
	 * Tries to return the driver with the given name. If there is currently no
	 * driver with that name registered, it tries to load `libqsld<name>.so`. If
	 * this fails as well, 0 is returned.
	 */
	static Driver* driver(const QString &name);
private:
	static QHash<QString, Driver*> exportedDrivers;
	
public:	
	virtual Database *newDatabase() = 0;
	
};

}

}
