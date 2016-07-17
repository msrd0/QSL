#include "qslvariant.h"

QVariant qsl::qslvariant(const std::string &v)
{
	return v.data();
}

QVariant qsl::qslvariant(const qsl::Password &p)
{
	return p.entry();
}

#if LONG_MAX > INT_MAX

QVariant qsl::qslvariant (int64_t v)
{
	return (qlonglong)v;
}

QVariant qsl::qslvariant (uint64_t v)
{
	return (qulonglong)v;
}

#endif
