#pragma once

#include "driver/driver.h"

namespace qsl {
namespace driver {

class SQLiteDriver : public Driver
{
public:
	virtual Database *newDatabase() override;
	
	virtual QString sql_select(QSLTable *tbl, const QSharedPointer<QSLFilter> &filter, uint limit);
};

}
}
