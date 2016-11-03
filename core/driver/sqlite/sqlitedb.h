#pragma once

#include "driver/db.h"

#include <QHash>

namespace qsl {
namespace driver {

class QSL_PRIVATE SQLiteDatabase : public QtDatabase
{
public:
	SQLiteDatabase(const char *charset, bool usevar);
	
	virtual bool connect() override;
	
	virtual bool ensureTable(const QSLTable &tbl) override;
	
	virtual SelectResult* selectTable(const QSLTable &tbl, const QList<QSLColumn> &cols, const QSLFilter &filter, const QList<QSLJoinTable> &join, int limit, bool asc) override;
	
	virtual bool insertIntoTable(const QSLTable &tbl, const QList<QSLColumn> &cols, const QVector<QVector<QVariant>> &rows) override;
	
	virtual bool updateTable(const QSLTable &tbl, const QMap<QSLColumn, QVariant> &values, const QVector<QVariant> &pks) override;
	
	virtual bool deleteFromTable(const QSLTable &tbl, const QSLFilter &filter) override;
	virtual bool deleteFromTable(const QSLTable &tbl, const QVector<QVariant> &pks) override;
	
protected:
	virtual void loadTableInfo() override;
	
	virtual bool ensureTableImpl(const QSLTable &tbl);
	
	static QString filterSQL(const QSLFilter &filter);
	
	// key is <tbl-name>.<col-name>
	QHash<QByteArray, QByteArray> uniqueIndexNames;
	
private:
	bool needsEnquote(const QByteArray &type);
};

}
}
