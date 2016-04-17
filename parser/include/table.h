#pragma once

#include "qsl_global.h"
#include "database.h"

#include <QObject>
#include <QString>

namespace qsl {
namespace qslc {
class Database;

class Field : public QObject
{
	Q_OBJECT
	
public:
	enum Constraint : uint8_t
	{
		none       = 0x0,
		primarykey = 0x1,
		unique     = 0x2
	};
	Q_ENUM(Constraint)
	
	Field(const QByteArray &name, const QByteArray &type);
	Field(const Field &other);
	Field& operator= (const Field &other);
	
	QByteArray name() const { return _name; }
	QByteArray type() const { return _type; }
	uint8_t constraints() const { return _constraints; }
	
	void setConstraint(Field::Constraint constraint) { _constraints |= constraint; }
	void setConstraint(const QByteArray &constraint);
	
private:
	/// The name of the field.
	QByteArray _name;
	/// The type of the field.
	QByteArray _type;
	/// The constraints for the field.
	uint8_t _constraints = none;
};

class Table
{
public:
	Table(Database *db, const QByteArray &name);
	
	Database* db() const { return _db; }
	QByteArray name() const { return _name; }
	QList<Field> fields() const { return _fields; }
	
	void addField(const Field &field);
	
private:
	/// The database containing this table.
	Database *_db;
	/// The name of the table.
	QByteArray _name;
	/// The primary key of the table.
	QByteArray _pk;
	/// All fields of the table.
	QList<Field> _fields;
};

}
}
