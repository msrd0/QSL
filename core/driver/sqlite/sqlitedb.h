#pragma once

#include "driver/db.h"

namespace qsl {
namespace driver {

class SQLiteDatabase : public QtDatabase
{
public:
	SQLiteDatabase(const char *charset, bool usevar);
	
	virtual bool connect() override;
	
	virtual bool ensureTable(const QSLTable &tbl) override;
	
	virtual SelectResult* selectTable(const QSLTable &tbl, const QList<QSLColumn> &cols, const QSharedPointer<QSLFilter> &filter, int limit, bool asc) override;
	
	virtual bool insertIntoTable(const QSLTable &tbl, const QList<QSLColumn> &cols, const QVector<QVector<QVariant>> &rows) override;
	
	virtual bool updateTable(const QSLTable &tbl, const QMap<QSLColumn, QVariant> &values, const QVector<QVariant> &pks) override;
	
protected:
	virtual void loadTableInfo() override;
	
	virtual bool ensureTableImpl(const QSLTable &tbl);
	
private:
	bool needsEnquote(const QByteArray &type);
};

}
}
