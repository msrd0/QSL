#pragma once

template<typename db_t>
db_t* connect (int argc, char **argv)
{
    db_t *db = 0;
    if (argc<=1 || strcmp(argv[1], "PostgreSQL")==0)
    {
	db = new db_t(QSL::PostgreSQL);
	db->setHost("localhost");
	db->setUser("postgres");
	db->setPassword("postgres");
    }
    else if (strcmp(argv[1], "SQLite")==0)
    {
	db = new db_t(QSL::SQLite);
	db->setName("/tmp/qslexample.db");
    }
    if (db && db->connect())
	return db;
    if (db)
	delete db;
    return 0;
}
