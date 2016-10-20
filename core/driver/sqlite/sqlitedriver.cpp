#include "sqlitedb.h"
#include "sqlitedriver.h"

using namespace qsl;
using namespace qsl::driver;

Database* SQLiteDriver::newDatabase()
{
	return new SQLiteDatabase;
}

QString SQLiteDriver::sql_select(QSLTable *tbl, const QSharedPointer<QSLFilter> &filter, uint limit)
{
	QString f = filter->sql(this);
	QString sql = QString("SELECT *, rowid AS ") + tbl->primaryKey() + " FROM " + tbl->name();
	if (!f.isEmpty())
		sql += " WHERE " + f;
	if (limit > 0)
		sql += " LIMIT BY " + QString::number(limit);
	return (sql + ";");
}
