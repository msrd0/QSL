#include "parser.h"
#include "table.h"

#include <QFile>

using namespace qsl::qslc;

#define error(...) \
	{ \
	if (!filename.isEmpty()) \
		fprintf(stderr, "%s:%d: ", qPrintable(filename), ll); \
	fprintf(stderr, __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	if (db) delete db; \
	return 0; \
	}

Database* qsl::qslc::parse(const QString &filename, bool qtype)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
	{
		fprintf(stderr, "Failed to open file %s: %s\n", qPrintable(filename), qPrintable(file.errorString()));
		return 0;
	}
	return parse(&file, filename, qtype);
}

Database* qsl::qslc::parse(QIODevice *in, const QString &filename, bool qtype)
{
	if (!in || !in->isOpen() || !in->isReadable())
		return 0;
	
	Database *db = 0;
	Table *tbl = 0;
	QByteArray line;
	for (uint ll = 1; !(line = in->readLine()).isEmpty(); ll++)
	{
		line = line.trimmed();
		if (line.isEmpty() || line.startsWith('#'))
			continue;
		
		if (line.startsWith("database"))
		{
			if (db)
				error("Each database has to be in its own file.")
			QByteArray name = line.mid(8).trimmed();
			if (name.startsWith('"') && name.endsWith('"'))
				name = name.mid(1, name.length()-2);
			db = new Database(name);
		}
		
		else if (line.startsWith("charset"))
		{
			if (tbl)
				error("The charset has to be defined before the tables")
			QByteArray charset = line.mid(7).trimmed();
			if (charset.startsWith('"') && charset.endsWith('"'))
				charset = charset.mid(1, charset.length() - 2);
			db->setCharset(charset);
		}
		
		else if (line == "usevar")
		{
			if (tbl)
				error("usevar has to be set before the tables")
			db->setUsevar(true);
		}
		
		else if (line.startsWith("table"))
		{
			QByteArray name = line.mid(5).trimmed();
			if (name.startsWith('"') && name.endsWith('"'))
				name = name.mid(1, name.length()-2);
			if (db->containsTable(name))
				error("The database already contains a table called %s", name.data())
			tbl = new Table(db, name);
			db->addTable(tbl);
		}
		
		else if (line.startsWith("-"))
		{
			if (!tbl)
				error("Defining a field without a table")
			line = line.mid(1).trimmed();
			QByteArray type = line.mid(0, line.indexOf(' '));
			line = line.mid(type.length()).trimmed();
			QByteArray name;
			if (line.startsWith('"'))
			{
				name = line.mid(1, line.indexOf('"', 1) - 1);
				line = line.mid(2 + name.length()).trimmed();
			}
			else
			{
				name = line.mid(0, line.indexOf(' '));
				line = line.mid(name.length()).trimmed();
			}
			Column f(name, type, qtype);
			while (!line.isEmpty())
			{
				if (!line.startsWith('!'))
					error("Syntax error near %s", line.mid(0,10).data())
				line = line.mid(1);
				QByteArray constraint = line.mid(0, line.indexOf(' '));
				if (f.setConstraint(constraint) == QSL::primarykey)
				{
					if (!tbl->primaryKey().isEmpty())
						error("A table may only have one primary key, but two columns with primary key found: %s and %s", tbl->primaryKey().data(), f.name().data());
					tbl->setPrimaryKey(f.name());
				}
				line = line.mid(constraint.length()).trimmed();
			}
			tbl->addField(f);
		}
	}
	return db;
}
