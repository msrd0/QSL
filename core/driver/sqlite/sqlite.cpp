#include "sqlitedriver.h"

using namespace qsl::driver;

extern "C"
void init_qsl_driver()
{
	bool success = Driver::exportDriver("sqlite", new SQLiteDriver);
	if (!success)
		fprintf(stderr, "QSL: Some error occured while exporting SQLite driver\n");
}
