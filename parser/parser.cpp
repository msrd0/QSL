#include "parser.h"
#include "table.h"

#include <QDateTime>
#include <QFile>
#include <QRegularExpression>
#include <QSet>

using namespace spis::spisc;

#define error(...) \
	{ \
	if (!filename.isEmpty()) \
		fprintf(stderr, "%s:%d: ERROR: ", qPrintable(filename), ll); \
	fprintf(stderr, __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	if (db) delete db; \
	return 0; \
	}

#define warning(...) \
	{ \
	if (!filename.isEmpty()) \
		fprintf(stderr, "%s:%d: WARNING: ", qPrintable(filename), ll); \
	fprintf(stderr, __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	}

// from http://en.cppreference.com/w/cpp/keyword
static const QSet<QByteArray> keywords = {
	"alignas",
	"alignof",
	"and",
	"and_eq",
	"asm",
	"atomic_cancel",
	"atomic_commit",
	"atomic_noexcept",
	"auto",
	"bitand",
	"bitor",
	"bool",
	"break",
	"case",
	"catch",
	"char",
	"char16_t",
	"char32_t",
	"class",
	"compl",
	"concept",
	"const",
	"constexpr",
	"const_cast",
	"continue",
	"decltype",
	"default",
	"delete",
	"do",
	"double",
	"dynamic_cast",
	"else",
	"enum",
	"explicit",
	"export",
	"extern",
	"false",
	"final",
	"float",
	"for",
	"friend",
	"goto",
	"if",
	"inline",
	"int",
	"import",
	"long",
	"module",
	"mutable",
	"namespace",
	"new",
	"noexcept",
	"not",
	"not_eq",
	"nullptr",
	"operator",
	"or",
	"or_eq",
	"override",
	"private",
	"protected",
	"public",
	"register",
	"reinterpret_cast",
	"requires",
	"return",
	"short",
	"signed",
	"short",
	"signed",
	"sizeof",
	"static",
	"static_assert",
	"static_cast",
	"struct",
	"switch",
	"synchronized",
	"template",
	"this",
	"thread_local",
	"throw",
	"transaction_safe",
	"transaction_safe_dynamic",
	"true",
	"try",
	"typedef",
	"typeid",
	"typename",
	"union",
	"unsigned",
	"using",
	"virtual",
	"void",
	"volatile",
	"wchar_t",
	"while",
	"xor",
	"xor_eq"
};

static const QSet<QByteArray> forbidden = {
	"db"
};

static bool checkName(const QByteArray &name)
{
	// everything with a double underscore or starting with an underscore followed
	// by an uppercase letter is reserved
	if (name.contains("__") || (name.startsWith("_") && (name.size()<2 || isupper(name[1]))))
		return false;
	// every C++ keyword is (of course) reserved
	if (keywords.contains(name))
		return false;
	// the forbidden names are as the name says forbidden
	if (forbidden.contains(name))
		return false;
	// of course normal rules apply: start with an underscore or a letter and only
	// consists of only digits, underscores and letters
	static const QRegularExpression identifierRegex("^[a-zA-Z_][a-zA-Z_0-9]*$");
	return identifierRegex.match(name).hasMatch();
}

static const QSet<QByteArray> legalTypes = {
	"blob",
	"byte",
	"char",
	"date",
	"datetime",
	"double",
	"int",
	"password",
	"text",
	"time",
	"uint",
	"variant"
};

static bool checkType(const QByteArray &type)
{
	if (type.startsWith('&'))
	{
		int in = type.indexOf('.');
		if (in == -1)
			return false;
		return checkName(type.mid(1, in - 1)) && checkName(type.mid(in + 1));
	}
	
	static const QRegularExpression typeRegex("(?P<type>[a-z]+)(?:\\([0-9]+\\))?");
	QRegularExpressionMatch match = typeRegex.match(type);
	if (!match.hasMatch())
		return false;
	return legalTypes.contains(match.captured("type").toUtf8());
}

Database* spis::spisc::parse(const QString &filename, bool qtype)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
	{
		fprintf(stderr, "Failed to open file %s: %s\n", qPrintable(filename), qPrintable(file.errorString()));
		return 0;
	}
	return parse(&file, filename, qtype);
}

Database* spis::spisc::parse(QIODevice *in, const QString &filename, bool qtype)
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
		
		int i = 0; // i can start at 0 because the first char won't be a #
		while ((i = line.indexOf('#', i+1)) >= 0)
		{
			// lets check we are not enquoted
			if (line.mid(0,i).count('"') % 2 == 0)
			{
				line = line.mid(0, i-1);
				break;
			}
		}
		
		if (line.startsWith("database"))
		{
			if (db)
				error("Each database has to be in its own file.")
			QByteArray name = line.mid(8).trimmed();
			if (name.startsWith('"') && name.endsWith('"'))
				name = name.mid(1, name.length()-2);
			if (!checkName(name))
				error("The name of the database does not follow the identifier rules")
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
			if (!checkName(name))
				error("The name of the table (`%s') does not follow the identifier rules", name.data())
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
			
			// parse type
			if (line.indexOf(' ') < 0)
				error("Syntax error")
			QByteArray type = line.mid(0, line.indexOf(' '));
			if (!checkType(type))
				error("Illegal type `%s'", type.data())
			if (type.startsWith('&'))
			{
				QByteArray table = type.mid(1, type.indexOf('.') - 1);
				if (!db->containsTable(table))
					error("Reference to unknown table `%s'", table.data());
				// TODO check that the table has the references column
			}
			line = line.mid(type.length()).trimmed();
			
			// parse name
			QByteArray name;
			if (line.startsWith('"'))
			{
				if (line.indexOf('"', 1) == -1)
					error("Missing closing \"")
				name = line.mid(1, line.indexOf('"', 1) - 1);
				line = line.mid(2 + name.length()).trimmed();
			}
			else
			{
				name = line.mid(0, QString::fromUtf8(line).indexOf(QRegularExpression("[ \(\[]"))); // indexOf can return -1 but this will match the entire string so this is fine
				line = line.mid(name.length()).trimmed();
			}
			if (!checkName(name))
				error("The name of the column (`%s') does not follow the identifier rules", name.data());
			if (tbl->takenNames.contains(name))
				error("The name of the column (`%s') was already assigned to another column", name.data());
			tbl->takenNames.insert(name);
			
			Column f(name, type, qtype);
			
			// parse alternate names
			if (line.startsWith('('))
			{
				f.enableRename(false);
				
				// TODO: if the next ')' is enquoted than this will lead to an error
				int in = line.indexOf(')');
				if (in < 0)
					error("Missing closing `)'");
				QByteArray p = line.mid(1, in-1);
				line = line.mid(in+1).trimmed();
				
				// TODO: if the name contains a "," (which is not valid but may happen) and is enquoted this doesnt work
				QList<QByteArray> names = p.split(',');
				for (QByteArray &n : names)
				{
					n = n.trimmed();
					
					if (n.startsWith('"'))
					{
						if (!n.endsWith('"'))
							error("Enquoted names may not contain a ',' (not implemented), fix it at " __FILE__ ":%d", __LINE__);
						n = n.mid(1, n.size()-2);
					}
					
					// TODO: do I need to check name here?
					if (!checkName(n))
						error("The name `%s' does not follow the identifier rules", n.data());
					
					if (tbl->takenNames.contains(n))
						error("The name of the column (`%s') was already assigned to another column", n.data());
					tbl->takenNames.insert(n);
					f.addAlternateName(n);
				}
			}
			else if (line.startsWith('['))
			{
				f.enableRename();
				
				// TODO: if the next ']' is enquoted than this will lead to an error
				int in = line.indexOf(']');
				if (in < 0)
					error("Missing closing `]'");
				QByteArray p = line.mid(1, in-1);
				line = line.mid(in+1).trimmed();
				
				// TODO: if the name contains a "," (which is not valid but may happen) and is enquoted this doesnt work
				QList<QByteArray> names = p.split(',');
				for (QByteArray &n : names)
				{
					n = n.trimmed();
					
					if (n.startsWith('"'))
					{
						if (!n.endsWith('"'))
							error("Enquoted names may not contain a ',' (not implemented), fix it at " __FILE__ ":%d", __LINE__);
						n = n.mid(1, n.size()-2);
					}
					bool rename = n.startsWith('+');
					if (rename)
						n = n.mid(1);
					// TODO: do I need to check name here?
					if (!checkName(n))
						error("The name `%s' does not follow the identifier rules", n.data());
					
					if (tbl->takenNames.contains(n))
						error("The name of the column (`%s') was already assigned to another column", n.data());
					tbl->takenNames.insert(n);
					if (rename && f.nameInDb() != f.name())
						error("This column already has another database name `%s'", n.data());
					
					f.addAlternateName(n);
					if (rename)
						f.setNameInDb(n);
				}
			}
			
			// parse default value
			QVariant def;
			if (line.startsWith('='))
			{
				QByteArray defVal;
				line = line.mid(1).trimmed();
				if (line.startsWith('"'))
				{
					int in = line.indexOf('"', 1);
					if (in == -1)
						error("Missing closing \"")
					defVal = line.mid(1, in-1);
					line = line.mid(2 + in).trimmed();
				}
				else
				{
					defVal = line.mid(0, line.indexOf(' '));
					line = line.mid(defVal.length()).trimmed();
				}
				
				if (type == "date")
					def = QDate::fromString(defVal, "yyyy-MM-dd");
				else if (type == "time")
					def = QTime::fromString(defVal, "hh:mm:ss");
				else if (type == "datetime")
					def = QDateTime::fromString(defVal, "yyyy-MM-dd hh:mm:ss");
				else if (type == "int")
					def = defVal.toLongLong();
				else if (type == "uint")
					def = defVal.toULongLong();
				else if (type == "double")
					def = defVal.toDouble();
				else
					def = defVal;
				if (def.isNull())
					error("Default value provided in wrong format")
			}
			f.setDefault(def);
			
			// parse constraints
			while (!line.isEmpty())
			{
				if (!line.startsWith('!'))
					error("Syntax error near %s", line.mid(0,10).data())
				line = line.mid(1);
				QByteArray constraint = line.mid(0, line.indexOf(' '));
				int con = f.setConstraint(constraint);
				if (con == SPIS::primarykey)
				{
					if (!tbl->primaryKey().isEmpty())
						error("A table may only have one primary key, but two columns with primary key found: %s and %s", tbl->primaryKey().data(), f.name().data());
					if (f.type() != "int" && f.type() != "uint")
						error("Only int and uint are allowed as type of a primary key, but %s was specified", f.type().data());
					tbl->setPrimaryKey(f.name());
					// pkey implies unique
					f.setConstraint(SPIS::unique);
				}
				if ((f.type() == "text" || f.type() == "blob") && con == SPIS::unique)
					warning("Not all drivers support unique indexes on text/blob fields; consider using char/byte instead");
				line = line.mid(constraint.length()).trimmed();
			}
			tbl->addField(f);
		}
	}
	return db;
}
