#pragma once

#include <QByteArray>

namespace qsl {
namespace driver {

class SQLiteTypes
{
public:
	static QByteArray fromSQL(const QByteArray &type);
	static QByteArray fromQSL(const QByteArray &type);
	
	// no ctors
private:
	SQLiteTypes() {}
	Q_DISABLE_COPY(SQLiteTypes)
};

}
}
