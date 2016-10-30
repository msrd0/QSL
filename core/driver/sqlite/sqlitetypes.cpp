#include "sqlitetypes.h"

#include <QDebug>

using namespace qsl::driver;

// for more information, visit https://www.sqlite.org/datatype3.html

QByteArray SQLiteTypes::fromSQL(const QByteArray &type)
{
	QByteArray t = type.trimmed().toLower();
	
	// integers
	if (t.contains("int"))
	{
		bool nosign = t.contains("unsigned");
		int size = 32;
		if (t.contains("tiny"))
			size = 8;
		else if (t.contains("small") || t.contains("medium") || t.contains("2"))
			size = 16;
		else if (t.contains("big") || t.contains("8"))
			size = 64;
		return (nosign ? "uint(" : "int(") + QByteArray::number(size) + ")";
	}
	
	// text & blob
	if (t == "text" || t == "clob")
		return "text";
	if (t == "blob")
		return "blob";
	if (t.contains("char"))
	{
#warning "TODO: SQLite SQL type parse for char"
	}
	
	// real
	if (t == "real" || t.contains("doub") || t.contains("floa"))
		return "double";
	
	// numeric
	if (t == "numeric")
		return "int";
	if (t == "decimal")
		return "double";
	if (t == "boolean")
		return "bool";
	if (t == "date")
		return "date";
	if (t == "datetime")
		return "datetime";
	
	// and there are columns without a type
	if (t.isEmpty())
		return "variant";
	
	// rest
	qCritical() << "QSL[SQLite]: Critical: Unable to parse SQL type" << type << "(in " __FILE__ " line" << __LINE__ << ")";
	return type;
}

QByteArray SQLiteTypes::fromQSL(const QByteArray &type, int minsize, bool usevar)
{
	QByteArray t = type.trimmed().toLower();
	QByteArray v = usevar ? "var" : "";
	
	if (t == "int" || t == "uint")
	{
		QByteArray u = t=="uint" ? "unsigned " : "";
		if (minsize <= 8)
			return u + "tinyint";
		if (minsize <= 16)
			return u + "int2";
		if (minsize <= 32)
			return u + "int";
		if (minsize <= 64)
			return u + "bigint";
	}
	
	if (t == "double")
		return "double";
	
	if (t == "char")
		return v + "char(" + QByteArray::number(minsize) + ")";
	if (t == "byte")
		return "n" + v + "char(" + QByteArray::number(minsize) + ")";
	if (t == "text" || t == "blob")
		return t;
	if (t == "password")
		return "text";
	
	if (t == "date" || t == "datetime")
		return t;
	if (t == "time")
		return "numeric";
	
	if (t == "variant")
		return "";
	
	qCritical() << "QSL[SQLite]: Critical: Unable to parse QSL type" << type << "(in " __FILE__ " line" << __LINE__ << ")";
	return "";
}
