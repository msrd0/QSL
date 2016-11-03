#pragma once

/// @file qslvariant.h This file contains a lot of methods called `qslvariant` to convert
/// every supported type into a `QVariant` even if there is no way provided by Qt to store
/// that type.

#include "qsl_global.h"
#include "qslpassword.h"

#include <QVariant>

namespace qsl {

/** Converts a `std::string` into a `QVariant`. */
QVariant qslvariant(const std::string &v);
/** Converts a `qsl::Password` into a `QVariant`. */
QVariant qslvariant(const qsl::Password &p);

// in this case (u)int64_t is a (unsigned) long while qt requires a (unsigned) long long
#if LONG_MAX > INT_MAX

/** Converts a `int64_t` into a `QVariant`. */
QVariant qslvariant (int64_t v);
/** Converts a `uint64_t` into a `QVariant`. */
QVariant qslvariant (uint64_t v);

#endif

/** Converts every compatible type into a `QVariant`. */
template<typename T>
QVariant qslvariant(const T &v)
{
	return v;
}

}
