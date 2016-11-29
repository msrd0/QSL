#include "driver.h"

#include <dlfcn.h>

#ifdef CMAKE_DEBUG
#  include <QDebug>
#endif

using namespace spis;
using namespace spis::driver;

QHash<QString, Driver*> Driver::exportedDrivers;

bool Driver::exportDriver(const QString &name, Driver *driver)
{
	if (!driver || exportedDrivers.contains(name))
		return false;
	exportedDrivers.insert(name, driver);
	return true;
}

Driver* Driver::driver(const QString &name)
{
	if (exportedDrivers.contains(name))
		return exportedDrivers[name];
	
	QString libname = "libspisd" + name + ".so";
#ifdef CMAKE_DEBUG
	qDebug() << "SPIS: Trying to load driver for" << name << "from" << libname;
#endif
	void *handle = dlopen(qPrintable(libname), RTLD_NOW);
	if (!handle)
	{
		fprintf(stderr, "SPIS: Failed to load driver \"%s\": %s\n", qPrintable(name), dlerror());
		return 0;
	}
	
	void (*init)();
	*(void **) (&init) = dlsym(handle, "init_spis_driver");
	init();
	
	if (exportedDrivers.contains(name))
		return exportedDrivers[name];
	fprintf(stderr, "SPIS: Loaded %s but still unable to find driver \"%s\"\n", qPrintable(libname), qPrintable(name));
	return 0;
}
