#pragma once

#include "qsl_global.h"

#include <QDir>

namespace qsl {
namespace qslc {
class Database;

bool generate(Database *db, const QDir &dir, bool qtype = false);

}
}
