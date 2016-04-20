#pragma once

#include "qsl_global.h"
#include "database.h"

#include <QIODevice>
#include <QString>

namespace qsl {
namespace qslc {

extern Database* parse(const QString &filename, bool qtype = false);
extern Database* parse(QIODevice *in, const QString &filename = QString(), bool qtype = false);

}
}
