#pragma once

#include "spis_global.h"
#include "database.h"

#include <QIODevice>
#include <QString>

namespace spis {
namespace spisc {

extern Database* parse(const QString &filename, bool qtype = false);
extern Database* parse(QIODevice *in, const QString &filename = QString(), bool qtype = false);

}
}
