#pragma once

#include "qsl_global.h"

#include <QVariant>

namespace qsl {

QVariant qslvariant(const std::string &v)
{
	return v.data();
}

// in this case (u)int64_t is a (unsigned) long while qt requires a (unsigned) long long
#if LONG_MAX > INT_MAX

QVariant qslvariant (int64_t v)
{
	return (qlonglong)v;
}

QVariant qslvariant (uint64_t v)
{
	return (qulonglong)v;
}

#endif

template<typename T>
QVariant qslvariant(const T &v)
{
	return v;
}

}
