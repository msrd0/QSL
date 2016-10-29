#include "parser.h"
#include "table.h"

#include <QFile>
#include <QRegularExpression>
#include <QSet>

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

static bool checkName(const QByteArray &name)
{
	// everything with a double underscore or starting with an underscore followed
	// by an uppercase letter is reserved
	if (name.contains("__") || (name.startsWith("_") && (name.size()<2 || isupper(name[1]))))
		return false;
	// every C++ keyword is (of course) reserved
	if (keywords.contains(name))
		return false;
	// of course normal rules apply: start with an underscore or a letter and only
	// consists of only digits, underscores and letters
	static const QRegularExpression identifierRegex("[a-zA-Z_][a-zA-Z_0-9]?");
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
	static const QRegularExpression typeRegex("(?P<type>[a-z]+)(?:\\([0-9]+\\))?");
	QRegularExpressionMatch match = typeRegex.match(type);
	if (!match.hasMatch())
		return false;
	return legalTypes.contains(match.captured("type").toUtf8());
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
				error("The name of the table does not follow the identifier rules")
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
			if (line.indexOf(' ') < 0)
				error("Syntax error")
			QByteArray type = line.mid(0, line.indexOf(' '));
			if (!checkType(type))
				error("Illegal type `%s'", type.data())
			line = line.mid(type.length()).trimmed();
			QByteArray name;
			if (line.startsWith('"'))
			{
				if (line.indexOf('"') == -1)
					error("Missing closing \"")
				name = line.mid(1, line.indexOf('"', 1) - 1);
				line = line.mid(2 + name.length()).trimmed();
			}
			else
			{
				name = line.mid(0, line.indexOf(' ')); // indexOf can return -1 but this will match the entire string so this is fine
				line = line.mid(name.length()).trimmed();
			}
			if (!checkName(name))
				error("The name of the column does not follow the identifier rules")
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
