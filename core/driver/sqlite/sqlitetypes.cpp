#include "sqlitetypes.h"

#include <QDebug>

using namespace std;
using namespace qsl::driver;

// for more information, visit https://www.sqlite.org/datatype3.html

pair<QByteArray, int> SQLiteTypes::fromSQL(const QByteArray &type)
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
		return {(nosign ? "uint" : "int"), size};
	}
	
	// text & blob
	if (t == "text" || t == "clob")
		return {"text", -1};
	if (t == "blob")
		return {"blob", -1};
	if (t.contains("char"))
	{
		if (t.contains("var"))
			t.replace("var", "");
		bool byte = t.contains("nchar") || t.contains("native");
		int in0 = t.indexOf('(');
		int in1 = t.indexOf(')');
		if (in0 >= 0 && in1 > in0)
		{
			int size = t.mid(in0 + 1, in1 - in0 - 1).toInt();
			return {(byte ? "byte" : "char"), size};
		}
		else
			return {(byte ? "text" : "blob"), -1};
	}
	
	// real
	if (t == "real" || t.contains("doub"))
		return {"double", 8};
	if (t.contains("floa"))
		return {"double", 4};
	
	// numeric
	if (t == "numeric")
		return {"int", -1};
	if (t == "decimal")
		return {"double", -1};
	if (t == "boolean")
		return {"bool", -1};
	if (t == "date")
		return {"date", -1};
	if (t == "datetime")
		return {"datetime", -1};
	
	// and there are columns without a type
	if (t.isEmpty())
		return {"variant", -1};
	
	// rest
	qCritical() << "QSL[SQLite]: Critical: Unable to parse SQL type" << type << "(in " __FILE__ " line" << __LINE__ << ")";
	return {type, -1};
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
