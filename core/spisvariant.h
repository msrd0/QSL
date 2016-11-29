#pragma once

/// @file spisvariant.h This file contains a lot of methods called `spisvariant` to convert
/// every supported type into a `QVariant` even if there is no way provided by Qt to store
/// that type.

#include "spis_global.h"
#include "spispassword.h"

#include <QVariant>

namespace spis {

/** Converts a `std::string` into a `QVariant`. */
QVariant spisvariant(const std::string &v);
/** Converts a `spis::Password` into a `QVariant`. */
QVariant spisvariant(const spis::Password &p);

// in this case (u)int64_t is a (unsigned) long while qt requires a (unsigned) long long
#if LONG_MAX > INT_MAX

/** Converts a `int64_t` into a `QVariant`. */
QVariant spisvariant (int64_t v);
/** Converts a `uint64_t` into a `QVariant`. */
QVariant spisvariant (uint64_t v);

#endif

/** Converts every compatible type into a `QVariant`. */
template<typename T>
QVariant spisvariant(const T &v)
{
	return v;
}

}
