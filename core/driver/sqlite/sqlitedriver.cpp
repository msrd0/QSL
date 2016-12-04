#include "sqlitedb.h"
#include "sqlitedriver.h"

using namespace std::chrono;
using namespace spis;
using namespace spis::driver;

#define time_point SQLiteDriver::time_point

Database* SQLiteDriver::newDatabase(const char *charset, bool usevar)
{
	return new SQLiteDatabase(this, charset, usevar);
}

QDate SQLiteDriver::toQDate(const QVariant &date)
{
	return toQDateTime(date).date();
}
QTime SQLiteDriver::toQTime(const QVariant &time)
{
	return toQDateTime(time).time();
}
QDateTime SQLiteDriver::toQDateTime(const QVariant &datetime)
{
	return QDateTime::fromMSecsSinceEpoch(datetime.toULongLong() * 1000L);
}

time_point SQLiteDriver::toChronoDate(const QVariant &date)
{
	return toChronoDateTime(date);
}
time_point SQLiteDriver::toChronoTime(const QVariant &time)
{
	return toChronoDateTime(time);
}
time_point SQLiteDriver::toChronoDateTime(const QVariant &datetime)
{
	return system_clock::from_time_t(datetime.toLongLong());
}

QVariant SQLiteDriver::fromQDate(const QDate &date)
{
	return fromQDateTime(QDateTime(date));
}
QVariant SQLiteDriver::fromQTime(const QTime &time)
{
	return fromQDateTime(QDateTime(QDate(1970,1,1), time));
}
QVariant SQLiteDriver::fromQDateTime(const QDateTime &datetime)
{
	return spisvariant((qulonglong)(datetime.toMSecsSinceEpoch() / 1000L));
}

QVariant SQLiteDriver::fromChronoDate(const time_point &date)
{
	return fromChronoDateTime(date);
}
QVariant SQLiteDriver::fromChronoTime(const time_point &time)
{
	return fromChronoDateTime(time);
}
QVariant SQLiteDriver::fromChronoDateTime(const time_point &datetime)
{
	return spisvariant(system_clock::to_time_t(datetime));
}
