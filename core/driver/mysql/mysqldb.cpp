#include "mysqldb.h"
#include "mysqltypes.h"

#include <unistd.h>

#include <QDebug>
#include <QSqlError>

using namespace spis::driver;

static void dumpError(const QSqlQuery &q, const char* file, int line)
{
	if (isatty(STDERR_FILENO))
	{
		fprintf(stderr, "\033[1mSPIS[MySQL]: \033[31mError\033[0m in \033[32m%s:%d\033[0m:\n", file, line);
		fprintf(stderr, "    \033[1;31mQuery:\033[0m %s\n", qPrintable(q.lastQuery()));
		fprintf(stderr, "    \033[1;31mError:\033[0m %s\n", qPrintable(q.lastError().text()));
	}
	else
	{
		fprintf(stderr, "SPIS[MySQL]: Error in %s:%d:\n", file, line);
		fprintf(stderr, "    Query: %s\n", qPrintable(q.lastQuery()));
		fprintf(stderr, "    Error: %s\n", qPrintable(q.lastError().text()));
	}
}
#define DUMP_ERROR(q) dumpError((q), __FILE__, __LINE__);

MySQLDatabase::MySQLDatabase(const char *charset, bool usevar)
	: QtDatabase(charset, usevar, "QMYSQL")
{
}

void MySQLDatabase::loadTableInfo()
{
#ifdef CMAKE_DEBUG
	qDebug() << "SPIS[MySQL]: Starting loadTableInfo()";
#endif
	QSqlQuery tables(db());
	if (!tables.exec("SHOW TABLES;"))
	{
		DUMP_ERROR(tables);
		return;
	}
	if (!tables.first())
		return;
	do
	{
		QByteArray tblName = tables.value(0).toByteArray();
		QList<SPISColumn> cols;
		QByteArray pk;
		
		QSqlQuery columns(db());
		if (!columns.exec("SHOW COLUMNS FROM " + tblName + ";"))
		{
			DUMP_ERROR(columns);
			continue;
		}
		if (!columns.first())
			continue;
		do
		{
			uint8_t constraints = SPIS::none;
			if (!columns.value("Null").toBool())
				constraints |= SPIS::notnull;
			if (columns.value("Key").toString() == "PRI")
			{
				constraints |= SPIS::primarykey;
				pk = columns.value("Field").toByteArray();
			}
			else if (columns.value("Key").toString() == "UNI")
				constraints |= SPIS::unique;
			auto type = MySQLTypes::fromSQL(columns.value("Type").toByteArray());
			SPISColumn col(columns.value("Field").toByteArray(), type.first, type.second, constraints); // TODO: types
			cols << col;
		}
		while (columns.next());
		
		SPISTable tbl(tblName.data(), pk, 0);
		for (auto c : cols)
			tbl.addColumn(c);
		addTable(tbl);
	}
	while (tables.next());
}

bool MySQLDatabase::ensureTable(const SPISTable &tbl)
{
	bool success = ensureTableImpl(tbl);
	if (success)
		addTable(tbl); // otherwise subsequent calls to ensureTable will repeat the changes
	return success;
}

bool MySQLDatabase::ensureTableImpl(const SPISTable &tbl)
{
	qCritical() << "SPIS[MySQL]: Implement " << __PRETTY_FUNCTION__;
	return false;
}

SelectResult* MySQLDatabase::selectTable(const SPISTable &tbl, const QList<SPISColumn> &cols, const SPISFilter &filter,
										 const QList<SPISJoinTable> &join, int limit, bool asc)
{
	qCritical() << "SPIS[MySQL]: Implement " << __PRETTY_FUNCTION__;
	return 0;
}

bool MySQLDatabase::insertIntoTable(const SPISTable &tbl, const QList<SPISColumn> &cols, const QVector<QVector<QVariant> > &rows)
{
	qCritical() << "SPIS[MySQL]: Implement " << __PRETTY_FUNCTION__;
	return false;
}

bool MySQLDatabase::updateTable(const SPISTable &tbl, const QMap<SPISColumn, QVariant> &values, const QVector<QVariant> &pks)
{
	qCritical() << "SPIS[MySQL]: Implement " << __PRETTY_FUNCTION__;
	return false;
}

bool MySQLDatabase::deleteFromTable(const SPISTable &tbl, const QVector<QVariant> &pks)
{
	qCritical() << "SPIS[MySQL]: Implement " << __PRETTY_FUNCTION__;
	return false;
}

bool MySQLDatabase::deleteFromTable(const SPISTable &tbl, const SPISFilter &filter)
{
	qCritical() << "SPIS[MySQL]: Implement " << __PRETTY_FUNCTION__;
	return false;
}
