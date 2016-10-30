#pragma once

#include <utility>

#include <QByteArray>

namespace qsl {
namespace driver {

class SQLiteTypes
{
public:
	static std::pair<QByteArray, int> fromSQL(const QByteArray &type);
	static QByteArray fromQSL(const QByteArray &type, int minsize, bool usevar);
	
	// no ctors
private:
	SQLiteTypes() {}
	Q_DISABLE_COPY(SQLiteTypes)
};

}
}
