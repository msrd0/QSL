#pragma once

#include "spis_global.h"
#include "spisdb.h"

#include <utility>

#include <QByteArray>

namespace spis {
namespace driver {

class SPIS_PRIVATE MySQLTypes
{
public:
	static std::pair<QByteArray, int> fromSQL(const QByteArray &type);
	static QByteArray fromSPIS(const SPISDB *db, const QByteArray &type, int minsize, bool usevar);
	
	// no ctors
private:
	MySQLTypes() {}
	Q_DISABLE_COPY(MySQLTypes)
};

}
}
