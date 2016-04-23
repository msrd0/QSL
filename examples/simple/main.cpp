#include "db_qsl_example.h"

using namespace qsl;

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
  
  std::string text = "example run at " + std::to_string(time(NULL));
  if (!e.foo().insert({text}))
    return 1;
  printf("inserted: %s\n", text.data());
  
  return 0;
}
