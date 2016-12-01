#include "mysqldriver.h"

using namespace spis::driver;

extern "C"
void init_spis_driver()
{
	bool success = Driver::exportDriver("mysql", new MySQLDriver);
	if (!success)
		fprintf(stderr, "SPIS: Some error occured while exporting MySQL driver\n");
}
