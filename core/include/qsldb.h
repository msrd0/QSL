#pragma once

#include "qsl_global.h"

#include <QObject>
#include <QSqlDatabase>

namespace qsl {
class QSLTable;

/**
 * This class is the superclass of every generated QSL Database. It defines methods
 * to specify the database connection and to establish the connection. To connect
 * to a PostgreSQL Database called "example", write something like this:
 * ```
 * #include "db_example.h"
 * using namespace qsl;
 * // ...
 * db::example e(QSLDB::PostgreSQL);
 * e.setHost("localhost");
 * e.setUser("user");
 * e.setPassword("secret");
 * if (e.connect())
 *     // successfully connected
 * ```
 */
class QSLDB : public QObject
{
	Q_OBJECT
	
public:
	/// This enum contains all supported drivers.
	enum Driver
	{
		PostgreSQL,
		MySQL,
		SQLite
	};
	Q_ENUM(Driver)
	/// Returns the qt driver name for the driver.
	static QString qDriverName(Driver driver);
	
	/// Returns the name of the database.
	const char* name() const { return _name; }
	/// Returns the driver of the database.
	Driver driver() const { return _driver; }
	
	/// Set the name of the Database. When using SQLite this is the filename.
	void setName(const QString &name) { db.setDatabaseName(name); }
	/// Set the host of the Database Server.
	void setHost(const QString &host) { db.setHostName(host); }
	/// Set the port of the Database Server.
	void setPort(int port) { db.setPort(port); }
	/// Set the user to access the Database.
	void setUser(const QString &user) { db.setUserName(user); }
	/// Set the password for the user.
	void setPassword(const QString &password) { db.setPassword(password); }
	/// Establish a connection to the server, or return false.
	bool connect();
	
protected:
	QSLDB(const char* name, Driver driver);
	
	/// Register a new table. If it doesn't exist it will be created as soon as the connection
	/// to the database is established.
	void registerTable(QSLTable *tbl);
	
	QSqlDatabase db;
	
private:
	const char* _name;
	Driver _driver;
	QList<QSLTable*> _tables;
	
};

}
