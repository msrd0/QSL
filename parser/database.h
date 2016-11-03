#pragma once

#include "qsl_global.h"

#include <QMap>
#include <QString>

namespace qsl {
namespace qslc {
class Table;

class Database
{
public:
	Database(const QByteArray &name);
	~Database();
	
	QByteArray name() const { return _name; }
	QByteArray charset() const { return _charset; }
	bool usevar() const { return _usevar; }
	
	void setCharset(const QByteArray &charset) { _charset = charset; }
	void setUsevar(bool usevar) { _usevar = usevar; }
	
	bool containsTable(const QByteArray &name);
	Table* table(const QByteArray &name);
	void addTable(Table *table);
	QList<Table*> tables() { return _orderedTables; }
	
private:
	/// The name of the database.
	QByteArray _name;
	/// The charset of the database. Default is utf-8.
	QByteArray _charset = "utf-8";
	/// Whether to use `varchar` instead of `char`.
	bool _usevar = false;
	
	/// All tables in this database, sorted by their name.
	QMap<QByteArray, Table*> _tables;
	/// All tables in this database, sorted as they appeared in the qsl file.
	QList<Table*> _orderedTables;
};

}
}
