#if defined TEST_COMPILE
#  include "db_qsldumptest.h"
#elif defined TEST_CREATE
#  include <QSqlDatabase>
#  include <QSqlQuery>
#else
#  error Either TEST_COMPILE or TEST_CREATE needs to be defined
#endif

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

#if defined TEST_COMPILE
using namespace qsl;
#endif

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addPositionalArgument("file", "The filename of the sqlite database", "<db-file>");
	parser.process(app);
	QStringList args = parser.positionalArguments();
	if (args.size() != 1)
	{
		parser.showHelp(1);
		return 1;
	}
	
#if defined TEST_CREATE
	QFileInfo info(args[0]);
	if (info.exists())
		info.dir().remove(info.fileName());
#endif
	
#if defined TEST_COMPILE
	db::qsldumptest test("sqlite");
	test.setName(args[0]);
	if (!test.connect())
#elif defined TEST_CREATE
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(args[0]);
	if (!db.open())
#endif
	{
		fprintf(stderr, "Failed to connect to database %s\n", qPrintable(args[0]));
		return 1;
	}
	
#if defined TEST_COMPILE
	test.disconnect();
#elif defined TEST_CREATE
	db.exec("PRAGMA foreign_keys = ON;");
	
	db.exec("CREATE TABLE tbl1(boring);");
	
	db.exec("CREATE TABLE tbl2(key TEXT NOT NULL, value NOT NULL);");
	db.exec("CREATE UNIQUE INDEX \"tbl2-key-index\" ON tbl2(key);");
	
	db.exec("CREATE TABLE tbl3(id UNSIGNED BIGINT NOT NULL UNIQUE, fkey TEXT, time DATETIME, PRIMARY KEY(id), FOREIGN KEY(fkey) REFERENCES tbl2(key));");
	
	db.exec("CREATE TABLE tbl4(large varying character(1500000000));");
	
	db.close();
#endif
	
	return 0;
}
