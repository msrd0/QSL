#include "db_qsl_example.h"

using namespace qsl;
using namespace qsl::filters;

int main()
{
	db::qsl_example e(QSL::PostgreSQL);
	e.setHost("localhost");
	e.setUser("postgres");
	e.setPassword("postgres");
	if (!e.connect())
	{
		printf("failed to connect :(\n");
		return 1;
	}
	printf("connected :)\n");

	auto r = e.foo().query();
	printf("result size: %d\n", r.size());
	
	int64_t t = time(NULL);
	std::string text = "example run at " + std::to_string(t);
	if (!e.foo().insert({text}))
		return 1;
	printf("inserted: %s\n", text.data());
	
	r = e.foo().query();
	printf("result size: %d\n", r.size());
	
	r = e.foo().filter(a(sw("bar", "example run at"), co("bar", (qlonglong)t))).query();
	printf("result size for insert: %d\n", r.size());
	
	return 0;
}
