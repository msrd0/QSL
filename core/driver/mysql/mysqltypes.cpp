#include "mysqltypes.h"

#include <QDebug>

using namespace std;
using namespace spis::driver;

// for more information, visit http://dev.mysql.com/doc/refman/5.7/en/data-types.html

pair<QByteArray, int> MySQLTypes::fromSQL(const QByteArray &type)
{
	QByteArray t = type.trimmed().toLower();
	
	// integers
	if (t.contains("int"))
	{
		bool nosign = t.contains("unsigned");
		int size = 32;
		if (t.contains("tiny"))
			size = 8;
		else if (t.contains("small"))
			size = 16;
		else if (t.contains("medium"))
			size = 24;
		else if (t.contains("big"))
			size = 64;
		return {(nosign ? "uint" : "int"), size};
	}
	
	// decimal,numeric - need to store them as double since c++ doesn't support it
	if (t.contains("decimal") || t.contains("numeric"))
		return {"double", -1};
	
	// float,double
	if (t.contains("float"))
		return {"double", 4};
	if (t.contains("real") || t.contains("double"))
		return {"double", 8};
	
	// bit
	if (t.contains("bit") || t.contains("binary"))
	{
		int in0 = t.indexOf('(');
		int in1 = t.indexOf(')');
		if (in0 >= 0 && in1 > in0)
		{
			int size = t.mid(in0 + 1, in1 - in0 - 1).toInt();
			return {"byte", size};
		}
	}
	
	// char
	if (t.contains("char"))
	{
		int in0 = t.indexOf('(');
		int in1 = t.indexOf(')');
		if (in0 >= 0 && in1 > in0)
		{
			int size = t.mid(in0 + 1, in1 - in0 - 1).toInt();
			return {"char", size};
		}
	}
	
	// blob, text
	if (t.contains("blob") || t.contains("text"))
	{
		bool blob = t.contains("blob");
		int size = -1;
		
		int in0 = t.indexOf('(');
		int in1 = t.indexOf(')');
		if (in0 >= 0 && in1 > in0)
			size = t.mid(in0 + 1, in1 - in0 - 1).toInt();
		
		if (size < 0)
		{
			if (t.contains("tiny"))
				size = 1<<8;
			else if (t.contains("medium"))
				size = 1<<24;
			else if (t.contains("long"))
				size = -1; // 1<<32 is more than the maximum of int
			else
				size = 1<<16;
		}
		
		return {(blob ? "blob" : "text"), size};
	}
	
	// temporal
	if (t.contains("datetime") || t.contains("timestamp"))
		return {"datetime", -1};
	if (t.contains("date"))
		return {"date", -1};
	if (t.contains("time"))
		return {"time", -1};
	if (t.contains("year"))
		return {"uint", 14}; // first 2^n ge 1000
	
	// rest
	qCritical() << "SPIS[MySQL]: Critical: Unable to parse SQL type" << type << "(in " __FILE__ " line" << __LINE__ << ")";
	return {type, -1};
}
