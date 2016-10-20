#pragma once

#include "driver/db.h"

namespace qsl {
namespace driver {

class SQLiteDatabase : public QtDatabase
{
public:
	SQLiteDatabase();
	
	virtual bool ensureTable(const QSLTable &tbl) override;
	
	virtual SelectResult* selectTable(const QSLTable &tbl, const QList<QSLColumn> &cols, const QSharedPointer<QSLFilter> &filter, int limit) override;
	
	virtual bool updateTable(const QSLTable &tbl, const QMap<QSLColumn, QVariant> &values, const QVector<QVariant> &pks) override;
	
protected:
	virtual void loadTableInfo() override;
};

}
}
