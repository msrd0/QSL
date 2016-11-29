#pragma once

#include "spis_global.h"

#include <QDir>

namespace spis {
namespace spisc {
class Database;

bool generate(Database *db, const QString &filename, const QDir &dir, bool qtype = false);

}
}
