#pragma once

#include "spis_global.h"
#include "database.h"
#include <spiscolumn.h>
#include <spisnamespace.h>

#include <QString>

namespace spis {
namespace spisc {
class Database;

class Column : public SPISColumn
{	
public:
	Column(const QByteArray &name, const QByteArray &type, bool qtype = false);
	
	QByteArray cppType() const { return _ctype; }
	bool cppReference() const { return _cref; }
	QByteArray cppArgType() const { return (_cref ? "const " + cppType() + "&" : cppType()); }
	
	void setConstraint(SPIS::ColumnConstraint constraint) { d->constraints |= constraint; }
	int setConstraint(const QByteArray &constraint);
	
	void setDefault(const QVariant &def) { d->def = def; }
	
	void enableRename(bool flag = true) { d->rename = flag; }
	void setNameInDb(const QByteArray &name) { d->nameInDb = name; }
	void addAlternateName(const QByteArray &name) { d->alternateNames.insert(name); }
	
private:
	/// The type of the field as a C++ typename.
	QByteArray _ctype;
	/// Whether the type should be passed via const reference.
	bool _cref;
};

class Table
{
public:
	Table(Database *db, const QByteArray &name);
	
	Database* db() const { return _db; }
	QByteArray name() const { return _name; }
	QList<Column> fields() const { return _fields; }
	QByteArray primaryKey() const { return _pk; }
	
	void setPrimaryKey(const QByteArray &pk) { _pk = pk; }
	
	void addField(const Column &field);
	
	QSet<QByteArray> takenNames;
	
private:
	/// The database containing this table.
	Database *_db;
	/// The name of the table.
	QByteArray _name;
	/// The primary key of the table.
	QByteArray _pk;
	/// All fields of the table.
	QList<Column> _fields;
};

}
}
