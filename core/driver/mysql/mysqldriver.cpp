#include "mysqldb.h"
#include "mysqldriver.h"

using namespace spis::driver;
using namespace std::chrono;

#define time_point MySQLDriver::time_point

Database* MySQLDriver::newDatabase(const char *charset, bool usevar)
{
	return new MySQLDatabase(charset, usevar);
}


QDate MySQLDriver::toQDate(const QVariant &date)
{
	return date.toDate();
}

QTime MySQLDriver::toQTime(const QVariant &time)
{
	return time.toTime();
}

QDateTime MySQLDriver::toQDateTime(const QVariant &datetime)
{
	return datetime.toDateTime();
}


time_point MySQLDriver::toChronoDate(const QVariant &date)
{
	return system_clock::from_time_t(QDateTime(toQDate(date)).toMSecsSinceEpoch() / 1000L);
}

time_point MySQLDriver::toChronoTime(const QVariant &time)
{
	return system_clock::from_time_t(QDateTime(QDate(1970,1,1), toQTime(time)).toMSecsSinceEpoch() / 1000L);
}

time_point MySQLDriver::toChronoDateTime(const QVariant &datetime)
{
	return system_clock::from_time_t(toQDateTime(datetime).toMSecsSinceEpoch() / 1000L);
}



QVariant MySQLDriver::fromQDate(const QDate &date)
{
	return date.toString("yyyy-MM-dd");
}

QVariant MySQLDriver::fromQTime(const QTime &time)
{
	return time.toString("hh:mm:ss");
}

QVariant MySQLDriver::fromQDateTime(const QDateTime &datetime)
{
	return datetime.toString("yyyy-MM-dd hh:mm:ss");
}


QVariant MySQLDriver::fromChronoDate(const time_point &date)
{
	return fromQDate(QDateTime::fromMSecsSinceEpoch(system_clock::to_time_t(date) * 1000L).date());
}

QVariant MySQLDriver::fromChronoTime(const time_point &time)
{
	return fromQTime(QDateTime::fromMSecsSinceEpoch(system_clock::to_time_t(time) * 1000L).time());
}

QVariant MySQLDriver::fromChronoDateTime(const time_point &datetime)
{
	return fromQDateTime(QDateTime::fromMSecsSinceEpoch(system_clock::to_time_t(datetime) * 1000L));
}
