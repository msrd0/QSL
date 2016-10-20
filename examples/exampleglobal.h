#pragma once

template<typename db_t>
db_t* connect (int argc, char **argv)
{
    db_t *db = 0;
    if (argc<=1 || strcmp(argv[1], "PostgreSQL")==0)
    {
	db = new db_t("psql");
	db->setHost("localhost");
	db->setUser("postgres");
	db->setPassword("postgres");
    }
    else
    {
	if (strcmp(argv[1], "SQLite")==0)
	    fprintf(stderr, "Unknown driver %s, falling back to SQLite\n", argv[1]);
	db = new db_t("sqlite");
	db->setName("/tmp/qslexample.db");
    }
    if (db && db->connect())
	return db;
    if (db)
	delete db;
    return 0;
}
