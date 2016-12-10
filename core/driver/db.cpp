#include "db.h"

#include "spistable.h"

#ifdef CMAKE_DEBUG
#  include <QDebug>
#endif

using namespace spis;
using namespace spis::driver;

CopySelectResult::CopySelectResult(const QVector<QString> &cols)
{
	for (int i = 0; i < cols.size(); i++)
		_cols.insert(cols[i], i);
}

bool CopySelectResult::addRow(const QVector<QVariant> &row)
{
	if (pos != -1 || row.size() != _cols.size())
		return false;
	_cells.push_back(row);
	return true;
}

bool CopySelectResult::first()
{
	return gotoIndex(0);
}
bool CopySelectResult::last()
{
	return gotoIndex(_cells.size()-1);
}
bool CopySelectResult::next()
{
	return isValid() && gotoIndex(pos+1);
}
bool CopySelectResult::prev()
{
	return isValid() && gotoIndex(pos-1);
}
bool CopySelectResult::isValid()
{
	return isValid(pos);
}

bool CopySelectResult::gotoIndex(int i)
{
	bool valid = isValid(i);
	if (valid)
		pos = i;
	return valid;
}
bool CopySelectResult::isValid(int i)
{
	return (!_cells.empty() && i>=0 && i<_cells.size());
}

QVariant CopySelectResult::value(int col) const
{
	return _cells[pos][col];
}
QVariant CopySelectResult::value(const QString &col) const
{
	if (!_cols.contains(col))
		return QVariant();
	return value(_cols.value(col));
}

// ##################################################

SelectResult* Database::selectTable(const SPISTable &tbl,
									const SPISFilter &filter,
									const QList<SPISJoinTable> &join,
									int limit, bool asc, const QByteArray &orderBy)
{
	return selectTable(tbl, tbl.columns(), filter, join, limit, asc, orderBy);
}

bool Database::insertIntoTable(const SPISTable &tbl, const QList<SPISColumn> &cols, const QVector<QVariant> &values)
{
	return insertIntoTable(tbl, cols, QVector<QVector<QVariant>>({values}));
}

bool Database::insertIntoTable(const SPISTable &tbl, const QMap<SPISColumn, QVariant> &values)
{
	return insertIntoTable(tbl, values.keys(), values.values().toVector());
}

bool Database::updateTable(const SPISTable &tbl, const QMap<SPISColumn, QVariant> &values,
						   const QVariant &pk)
{
	return updateTable(tbl, values, QVector<QVariant>() << pk);
}

bool Database::updateTable(const SPISTable &tbl, const SPISColumn &col, const QVariant &value,
						   const QVector<QVariant> &pks)
{
	return updateTable(tbl, QMap<SPISColumn, QVariant>({{col,value}}), pks);
}

bool Database::updateTable(const SPISTable &tbl, const SPISColumn &col, const QVariant &value,
						   const QVariant &pk)
{
	return updateTable(tbl, QMap<SPISColumn, QVariant>({{col,value}}), QVector<QVariant>() << pk);
}

bool Database::deleteFromTable(const SPISTable &tbl, const QVariant &pk)
{
	return deleteFromTable(tbl, QVector<QVariant>({pk}));
}

// ##################################################

QtSelectResult::QtSelectResult(const QSqlQuery &query)
	: q(query)
{
}

bool QtSelectResult::first()
{
	return q.first();
}
bool QtSelectResult::last()
{
	return q.first();
}
bool QtSelectResult::next()
{
	return q.next();
}
bool QtSelectResult::prev()
{
	return q.previous();
}
bool QtSelectResult::isValid()
{
	return q.isValid();
}

QVariant QtSelectResult::value(int col) const
{
	return q.value(col);
}
QVariant QtSelectResult::value(const QString &col) const
{
	return q.value(col);
}

// ##################################################

QtDatabase::QtDatabase(const char *charset, bool usevar, const QString &qtDriverName)
	: Database(charset, usevar)
{
	init(QSqlDatabase::addDatabase(qtDriverName));
}
QtDatabase::QtDatabase(const char *charset, bool usevar, QSqlDriver *driver)
	: Database(charset, usevar)
{
	init(QSqlDatabase::addDatabase(driver));
}
QtDatabase::QtDatabase(const char *charset, bool usevar, const QSqlDatabase &db)
	: Database(charset, usevar)
{
	init(db);
}

void QtDatabase::init(const QSqlDatabase &db)
{
	_db = db;
}

void QtDatabase::setName(const QString &name)
{
	db().setDatabaseName(name);
}
void QtDatabase::setHost(const QString &host)
{
	db().setHostName(host);
}
void QtDatabase::setPort(int port)
{
	db().setPort(port);
}
void QtDatabase::setUser(const QString &user)
{
	db().setUserName(user);
}
void QtDatabase::setPassword(const QString &password)
{
	db().setPassword(password);
}

bool QtDatabase::connect()
{
	if (!db().open())
		return false;
	loadTableInfo();
#ifdef CMAKE_DEBUG
	qDebug() << "SPIS[QtDatabase]: Loaded" << _tables.size() << "tables";
#endif
	return true;
}
bool QtDatabase::disconnect()
{
	db().close();
	return true;
}
bool QtDatabase::isConnected() const
{
	return db().isOpen();
}

QList<SPISTable> QtDatabase::tables() const
{
	return _tables.values();
}
bool QtDatabase::containsTable(const QByteArray &name) const
{
	return _tables.contains(name);
}
SPISTable QtDatabase::table(const QByteArray &name) const
{
	return _tables.value(name, SPISTable{"invalid", "invalid", 0});
}
void QtDatabase::addTable(const SPISTable &tbl)
{
#ifdef CMAKE_DEBUG
	qDebug() << "SPIS[QtDatabase]: Going to add Table" << tbl.name();
#endif
	_tables.insert(tbl.name(), tbl);
}
