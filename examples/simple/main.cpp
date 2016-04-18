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
  
  return 0;
}
