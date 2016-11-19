#pragma once

template<typename db_t>
db_t* connect (int argc, char **argv)
{
	db_t *db = 0;
	const char *driver = "sqlite";
	if (argc > 1)
		driver = argv[1];
	if (strcmp(driver, "sqlite") == 0)
	{
		db = new db_t(driver);
		db->setName("/tmp/qslexample.db");
	}
	else
	{
		db = new db_t(driver);
		db->setHost("localhost");
		if (strcmp(driver, "psql") == 0)
		{
			db->setUser("postgres");
			db->setPassword("postgres");
		}
		else
		{
			db->setUser("root");
		}
	}
	
	if (db && db->connect())
		return db;
	if (db)
		delete db;
	return 0;
}
