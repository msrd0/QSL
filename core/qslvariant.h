#pragma once

#include "qsl_global.h"
#include "qslpassword.h"

#include <QVariant>

namespace qsl {

QVariant qslvariant(const std::string &v);
QVariant qslvariant(const qsl::Password &p);

// in this case (u)int64_t is a (unsigned) long while qt requires a (unsigned) long long
#if LONG_MAX > INT_MAX
QVariant qslvariant (int64_t v);
QVariant qslvariant (uint64_t v);
#endif

template<typename T>
QVariant qslvariant(const T &v)
{
	return v;
}

}
