#include "db_qsl_example.h"

using namespace qsl;

int main()
{
  db::qsl_example e(QSLDB::PostgreSQL);
  e.setHost("localhost");
  e.setUser("postgres");
  e.setPassword("postgres");
  if (!e.connect())
    return 1;
  printf("connected :)\n");
  
  return 0;
}
