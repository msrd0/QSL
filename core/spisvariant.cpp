#include "spisvariant.h"

QVariant spis::spisvariant(const std::string &v)
{
	return v.data();
}

QVariant spis::spisvariant(const spis::Password &p)
{
	return p.entry();
}

#if LONG_MAX > INT_MAX

QVariant spis::spisvariant (int64_t v)
{
	return (qlonglong)v;
}

QVariant spis::spisvariant (uint64_t v)
{
	return (qulonglong)v;
}

#endif
