#pragma once

/// @file spisdb.h This file contains the `spis::SPISDB` class to store meta-data about
/// a database and to connect to a database. This class is the class every generated
/// database class extends.

#include "spisnamespace.h"

#include <QMap>
#include <QSqlDatabase>

namespace spis {
class SPISTable;

namespace driver {
class Database;
class Driver;
}

/**
 * This class is the superclass of every generated SPIS Database. It defines methods
 * to specify the database connection and to establish the connection. To connect
 * to a PostgreSQL Database called "example", write something like this:
 * ```
 * #include "db_example.h"
 * using namespace spis;
 * // ...
 * db::example e(SPISDB::PostgreSQL);
 * e.setHost("localhost");
 * e.setUser("user");
 * e.setPassword("secret");
 * if (e.connect())
 *     // successfully connected
 * ```
 */
class SPIS_PUBLIC SPISDB
{
	Q_DISABLE_COPY(SPISDB)
	
public:
	virtual ~SPISDB();
	
	/// Returns the name of the database.
	const char* name() const { return _name; }
	/// Returns the driver of the database.
	driver::Driver *driver() const { return _driver; }
	
	/// Returns the charset used by this database.
	virtual const char* charset() const = 0;
	/// Returns if the database uses variable data types.
	virtual bool usevar() const = 0;
	
	/// Set the name of the Database. When using SQLite this is the filename.
	void setName(const QString &name);
	/// Set the host of the Database Server.
	void setHost(const QString &host);
	/// Set the port of the Database Server.
	void setPort(int port);
	/// Set the user to access the Database.
	void setUser(const QString &user);
	/// Set the password for the user.
	void setPassword(const QString &password);
	/// Establish a connection to the server, or return false.
	virtual bool connect();
	/// Disconnect from the database again. If not connected, does nothing.
	virtual void disconnect();
	/// Returns true if a connection to the database exists.
	bool isConnected();
	
	/// Returns the table with the given name, or 0 if no such table exists.
	SPISTable* table(const QByteArray &name);
	
	/// The underlying database.
	driver::Database *db();
	
protected:
	/// Creates a new database with the given name and the given driver.
	SPISDB(const char* name, const QString &driver);
	/// Creates a new database with the given name and the given driver.
	SPISDB(const char* name, driver::Driver *driver);
	
	/// Register a new table. If it doesn't exist it will be created as soon as the connection
	/// to the database is established.
	void registerTable(SPISTable *tbl);
	
private:
	const char* _name;
	driver::Driver *_driver;
	QMap<QByteArray, SPISTable*> _tables;
	
	driver::Database *_db;
	
};

}
