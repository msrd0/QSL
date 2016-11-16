#include "db_qsl_example.h"

using namespace qsl;
using namespace std;

#include "../exampleglobal.h"

#ifdef DB_QSL_EXAMPLE_QTYPE
#  define string QString
#  define to_string(x) QString::number(x)
#  define data(x) qPrintable(x)
#else
#  define data(x) x.data()
#endif

int main(int argc, char **argv)
{
	db::qsl_example *e = connect<db::qsl_example>(argc, argv);
	if (!e)
	{
		fprintf(stderr, "Failed to connect or database driver unknown\n");
		return 1;
	}
	printf("connected :)\n");
	
	auto r = e->foo().query();
	printf("result size: %d\n", r.size());
	
	int64_t t = time(NULL);
	string text = "example run at " + to_string(t);
	if (!e->foo().insert({text}))
	{
		printf("Unable to insert into table\n");
		return 1;
	}
	printf("inserted: %s\n", data(text));
	
	r = e->foo().query();
	printf("result size: %d\n", r.size());
	
	r = e->foo().filter("bar" LIKE "example run at%" AND "bar" LIKE "%" + QString::number(t) + "%").query();
	printf("result size for insert: %d\n", r.size());
	
	if (r.size() != 1)
		return 1;
	if (!r[0].setBar(text + " (updated)"))
		return 1;
	printf("updated text\n");
	
	text = "this should be deleted asap";
	if (!e->foo().insert({text}))
	{
		printf("Unable to insert into table #2\n");
		return 1;
	}
	printf("insert again successfull\n");
	if (!e->foo().filter("bar" LIKE "%delete%").remove())
	{
		printf("Unable to delete from table\n");
		return 1;
	}
	printf("delete successfull\n");
	
	printf(" --- DONE\n");
	return 0;
}
