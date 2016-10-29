#include "sqlitetypes.h"

#include <QDebug>

using namespace qsl::driver;

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
		// TODO
	}
	
	// real
	if (t == "real" || t.contains("double") || t == "float")
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
