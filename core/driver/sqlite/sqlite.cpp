#include "sqlitedriver.h"

using namespace spis::driver;

extern "C"
void init_spis_driver()
{
	bool success = Driver::exportDriver("sqlite", new SQLiteDriver);
	if (!success)
		fprintf(stderr, "SPIS: Some error occured while exporting SQLite driver\n");
}
