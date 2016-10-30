#pragma once

#include "qslcolumn.h"
#include "qslnamespace.h"
#include "qslvariant.h"

#include <QByteArray>
#include <QMap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVector>

namespace qsl {
class QSLFilter;
class QSLTable;

namespace driver {

/**
 * This is a simple subclass of `QSLColumn` that provides mutable constraints. This can be usefull
 * to retrieve the column information from the existing database, create the columns, and later
 * match the constraints to the columns.
 */
class MutableColumn : public QSLColumn
{
public:
	/// Creates a new column with the given name, type, type's minsize and, if given, constraints
	/// (see `QSL::ColumnConstraint`).
	constexpr MutableColumn(const char* name, const char* type, uint32_t minsize, uint8_t constraints = 0)
		: QSLColumn(name, type, minsize, constraints)
	{
	}
	
	/// Creates a new column with the given name, type, type's minsize and, if given, constraints
	/// (see `QSL::ColumnConstraint`).
	MutableColumn(const QByteArray &name, const QByteArray &type, uint32_t minsize, uint8_t constraints = 0)
		: QSLColumn(name, type, minsize, constraints)
	{
	}
	
	/** Adds the given constraint to the constraints. This method has no effect if the constraint is already present. */
	void addConstraint(uint8_t constraint)
	{
		_constraints |= constraint;
	}
	/** Adds the given constraint to the constraints. This method has no effect if the constraint is already present. */
	void addConstraint(QSL::ColumnConstraint constraint)
	{
		addConstraint((uint8_t)constraint);
	}
};

/**
 * This class is the result of an SQL `SELECT` query. It is used to get all records
 * of a select query.
 */
class SelectResult
{
public:
	/** Retrieves the first record in the select result. */
	virtual bool first() = 0;
	/** Retrieves the last record in the select result. */
	virtual bool last() = 0;
	/** Retrieves the next record in the select result. */
	virtual bool next() = 0;
	/** Retrieves the previous record in the select result. */
	virtual bool prev() = 0;
	/** Returns `true` if the currently selected record is valid. */
	virtual bool isValid() = 0;
	
	/** Returns the value of the current record at the given column index. */
	virtual QVariant value(int col) const = 0;
	/** Returns the value of the current record at the given column name. */
	virtual QVariant value(const QString &col) const = 0;
};

/**
 * This class is an implementation of `SelectResult` that copies all the records
 * first and then later uses the copy to provide the necessary information. Do
 * not use this class if your driver gives you a result similar to `SelectResult`
 * to prevent unneccessary copies.
 */
class CopySelectResult : public SelectResult
{
public:
	/** Creates a new empty `SelectResult` with the give selected column names. */
	CopySelectResult(const QVector<QString> &cols);
	
	/** Adds a new row to the result. Note that the size of row should match the column count. */
	virtual bool addRow(const QVector<QVariant> &row);
	
	bool first() override;
	bool last() override;
	bool next() override;
	bool prev() override;
	bool isValid() override;
	
	/** Returns the value of the current record at the given column index. */
	QVariant value(int col) const override;
	/** Returns the value of the current record at the given column name. */
	QVariant value(const QString &col) const override;
	
protected:
	bool gotoIndex(int i);
	bool isValid(int i);
	
private:
	QHash<QString, int> _cols;
	QList<QVector<QVariant>> _cells;
	
	int pos = -1;
};

/**
 * This class needs to be overridden by every driver. Take a look at
 * the `qsl::driver::QtDatabase` class that uses a `QSqlDatabase` behind
 * the scenes to provide a database connection.
 */
class Database
{
	
public:
	/** Creates a database with the given encoding and the given usevar value. */
	constexpr Database(const char *charset, bool usevar)
		: _charset(charset)
		, _usevar(usevar)
	{
	}

	/** Returns the charset that should be used by this database. Note that there is no
	 * guarantee that the charset is changed if the database was created with another
	 * charset. */
	const char *charset() const { return _charset; }
	bool usevar() const { return _usevar; }
private:
	const char *_charset;
	bool _usevar;
	
public:
	/** Set the name of the database. */
	virtual void setName(const QString &name) = 0;
	/** Set the host of the database server. */
	virtual void setHost(const QString &host) = 0;
	/** Set the port of the database server. */
	virtual void setPort(int port) = 0;
	/** Set the username used to connect to the database. */
	virtual void setUser(const QString &user) = 0;
	/** Set the password used to connect to the database. */
	virtual void setPassword(const QString &password) = 0;
	/** Connect to the database. */
	virtual bool connect() = 0;
	/** Disconnect from the database. */
	virtual bool disconnect() = 0;
	/** Returns true if connected to the database. */
	virtual bool isConnected() const = 0;
	
	/** Returns all tables of the database including their schema. */
	virtual QList<QSLTable> tables() const = 0;
	/**
	 * Ensures that the given table exists in the database with at least
	 * the columns given. If the database is type-safe (not like SQLite)
	 * this method also ensures that the datatypes match. Also, the indexes
	 * (unique etc) and other constraints (like foreign keys & not null)
	 * are present if needed.
	 * 
	 * After the successfull call to this method the table must exist with
	 * the requirements above, but may contain any other fields that were
	 * present before. This method must not create any extra fields that
	 * were not present in the table before. Also, this method must not
	 * delete existing rows in the table, while this method may delete
	 * columns that were existing before but are not mentioned in `tbl`.
	 */
	virtual bool ensureTable(const QSLTable &tbl) = 0;
	
	/**
	 * Selects the given columns from the table that apply to the given filter. If
	 * `limit` is greater than 0, a maximum of `limit` rows are retrieved.
	 */
	virtual SelectResult* selectTable(const QSLTable &tbl,
									  const QList<QSLColumn> &cols,
									  const QSharedPointer<QSLFilter> &filter,
									  int limit = -1) = 0;
	/**
	 * Selects all columns from the table that apply to the given filter. If
	 * `limit` is greater than 0, a maximum of `limit` rows are retrieved.
	 */
	virtual SelectResult* selectTable(const QSLTable &tbl,
									  const QSharedPointer<QSLFilter> &filter,
									  int limit = -1);
	
	/** Updates the values of the given table at the given indexes to the new values. */
	virtual bool updateTable(const QSLTable &tbl, const QMap<QSLColumn, QVariant> &values,
							 const QVector<QVariant> &pks) = 0;
	/** Updates the values of the given table at the given index to the new values. */
	virtual bool updateTable(const QSLTable &tbl, const QMap<QSLColumn, QVariant> &values,
							 const QVariant &pk);
	/** Updates the values of the given table at the given indexes to the new value. */
	virtual bool updateTable(const QSLTable &tbl, const QSLColumn &col, const QVariant &value,
							 const QVector<QVariant> &pks);
	/** Updates the values of the given table at the given index to the new value. */
	virtual bool updateTable(const QSLTable &tbl, const QSLColumn &col, const QVariant &value,
							 const QVariant &pk);
};

/**
 * A `SelectResult` implementation that mirrors the result of a `QSqlQuery`.
 */
class QtSelectResult : public SelectResult
{
public:
	/** Constructs a new `QtSelectResult` that mirrors `query`. */
	explicit QtSelectResult(const QSqlQuery &query);
	
	virtual bool first() override;
	virtual bool last() override;
	virtual bool next() override;
	virtual bool prev() override;
	virtual bool isValid() override;
	
	virtual QVariant value(int col) const override;
	virtual QVariant value(const QString &col) const override;
	
protected:
	/** The underlying `QSqlQuery`. */
	QSqlQuery q;
};

/**
 * A database implementation that can be used by every driver with
 * a corresponding QtSql driver.
 */
class QtDatabase : public Database
{
	
protected:
	QtDatabase(const char *charset, bool usevar, const QString& qtDriverName);
	QtDatabase(const char *charset, bool usevar, QSqlDriver *driver);
	QtDatabase(const char *charset, bool usevar, const QSqlDatabase &db);
	
public:
	virtual void setName(const QString &name) override;
	virtual void setHost(const QString &host) override;
	virtual void setPort(int port) override;
	virtual void setUser(const QString &user) override;
	virtual void setPassword(const QString &password) override;
	virtual bool connect() override;
	virtual bool disconnect() override;
	virtual bool isConnected() const override;
	
	QList<QSLTable> tables() const override;
	bool containsTable(const QByteArray &name) const;
	QSLTable table(const QByteArray &name) const;
	
protected:
	virtual void loadTableInfo() = 0;
	virtual void addTable(const QSLTable &tbl);
	
	QSqlDatabase& db() { return _db; }
	const QSqlDatabase& db() const { return _db; }
	
private:
	void init(const QSqlDatabase &db);
	
	QSqlDatabase _db;
	
	QMap<QByteArray, QSLTable> _tables;
	
};

}
}
