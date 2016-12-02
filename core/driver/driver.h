#pragma once

#include "spisnamespace.h"
#include "db.h"
#include "spisfilter.h"
#include "spistable.h"

#include <chrono>

#include <QDate>
#include <QDateTime>
#include <QHash>
#include <QSharedPointer>
#include <QTime>

namespace spis {

/**
 * This namespace contains driver-related classes. You should not need to use this
 * namespace directly, however if you want to write your own driver take a look at
 * `spis::driver::Driver`.
 */
namespace driver {

/**
 * This is the base class of every driver. It is used to execute commands on the
 * database. To create your own driver, subclass this class and provide a method
 * called init_spis_driver, like this:
 * ```
 * extern "C"
 * void init_spis_driver()
 * {
 *     bool success = Driver::exportDriver("YourDriverName", new YourDriver);
 *     if (!success)
 *         fprintf(stderr, "Some error occured while exporting driver\n");
 * }
 * ```
 * Then, package everything into a library called `libspisdYourDriverName.so`.
 */
class SPIS_PUBLIC Driver
{
public:
	/**
	 * Exports a new Driver with the given name. If driver is 0 or a driver
	 * with the same name is already present, false is returned.
	 */
	static bool exportDriver(const QString &name, Driver* driver);
	/**
	 * Tries to return the driver with the given name. If there is currently no
	 * driver with that name registered, it tries to load `libspisd<name>.so`. If
	 * this fails as well, 0 is returned.
	 */
	static Driver* driver(const QString &name);
private:
	static QHash<QString, Driver*> exportedDrivers;
	
public:
	/** Creates a new Database instance with this driver. */
	virtual Database *newDatabase(const char *charset, bool usevar) = 0;
	
	/** Converts the Database-specific date format into a QDate. */
	virtual QDate toQDate(const QVariant &date) = 0;
	/** Converts the Database-specific date format into a stl's chrono time_point. */
	virtual std::chrono::system_clock::time_point toChronoDate(const QVariant &date) = 0;
	/** Converts the Database-specific time format into a QTime. */
	virtual QTime toQTime(const QVariant &time) = 0;
	/** Converts the Database-specific time format into a stl's chrono time_point. */
	virtual std::chrono::system_clock::time_point toChronoTime(const QVariant &time) = 0;
	/** Converts the Database-specific datetime format into a QDateTime. */
	virtual QDateTime toQDateTime(const QVariant &datetime) = 0;
	/** Converts the Database-specific datetime format into a stl's chrono time_point. */
	virtual std::chrono::system_clock::time_point toChronoDateTime(const QVariant &datetime) = 0;
	/** Converts the QDate to the Database-specific date format. */
	virtual QVariant fromQDate(const QDate &date) = 0;
	/** Converts the QTime to the Database-specific time format. */
	virtual QVariant fromQTime(const QTime &time) = 0;
	/** Converts the QDateTime to the Database-specific datetime format. */
	virtual QVariant fromQDateTime(const QDateTime &datetime) = 0;
	/** Converts the stl's chrono time_point to the Database-specific date format. */
	virtual QVariant fromChronoDate(const std::chrono::system_clock::time_point &date) = 0;
	/** Converts the stl's chrono time_point to the Database-specific time format. */
	virtual QVariant fromChronoTime(const std::chrono::system_clock::time_point &time) = 0;
	/** Converts the stl's chrono time_point to the Database-specific datetime format. */
	virtual QVariant fromChronoDateTime(const std::chrono::system_clock::time_point &datetime) = 0;
};

}

}
