#include "db_qsl_example.h"

using namespace qsl;
using namespace qsl::filters;

#include "../exampleglobal.h"

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
	std::string text = "example run at " + std::to_string(t);
	if (!e->foo().insert({text}))
	{
		printf("Unable to insert into table\n");
		return 1;
	}
	printf("inserted: %s\n", text.data());
	
	r = e->foo().query();
	printf("result size: %d\n", r.size());
	
	r = e->foo().filter(a(sw("bar", "example run at"), co("bar", (qlonglong)t))).query();
	printf("result size for insert: %d\n", r.size());
	
	if (r.size() != 1)
		return 1;
	if (!r[0].setBar(text + " (updated)"))
		return 1;
	printf("updated text\n");
	
	printf(" --- DONE\n");
	return 0;
}
