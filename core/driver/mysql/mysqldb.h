#pragma once

#include "driver/db.h"

#include <QHash>

namespace spis {
namespace driver {

class SPIS_PRIVATE MySQLDatabase : public QtDatabase
{
public:
	MySQLDatabase(const char *charset, bool usevar);
	
	virtual bool ensureTable(const SPISTable &tbl) override;
	
	virtual SelectResult* selectTable(const SPISTable &tbl, const QList<SPISColumn> &cols, const SPISFilter &filter, const QList<SPISJoinTable> &join, int limit, bool asc) override;
	
	virtual bool insertIntoTable(const SPISTable &tbl, const QList<SPISColumn> &cols, const QVector<QVector<QVariant>> &rows) override;
	
	virtual bool updateTable(const SPISTable &tbl, const QMap<SPISColumn, QVariant> &values, const QVector<QVariant> &pks) override;
	
	virtual bool deleteFromTable(const SPISTable &tbl, const SPISFilter &filter) override;
	virtual bool deleteFromTable(const SPISTable &tbl, const QVector<QVariant> &pks) override;
	
protected:
	virtual void loadTableInfo() override;
	
	virtual bool ensureTableImpl(const SPISTable &tbl);
	
	static QString filterSQL(const SPISTable &tbl, const SPISFilter &filter);
	
	// key is <tbl-name>.<col-name>
	QHash<QByteArray, QByteArray> uniqueIndexNames;
	
private:
	bool needsEnquote(const QByteArray &type);
};

}
}
