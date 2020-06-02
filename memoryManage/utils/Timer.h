#ifndef MEMORYMANAGE_TIMER_H
#define MEMORYMANAGE_TIMER_H

#include "memoryManage_declare.h"

MM_NAMESPACE_START

const long MILLISECONDS_PER_SECOND = 1000;
const long MICROSECONDS_PER_MILLISECOND = 1000;
const long NANOSECONDS_PER_MICROSECOND = 1000;

const long MICROSECONDS_PER_SECOND = MICROSECONDS_PER_MILLISECOND * MILLISECONDS_PER_SECOND;
const long NANOSECONDS_PER_MILLISECOND = NANOSECONDS_PER_MICROSECOND * MICROSECONDS_PER_MILLISECOND;
const long NANOSECONDS_PER_SECOND = NANOSECONDS_PER_MILLISECOND * MILLISECONDS_PER_SECOND;

const int  SECONDS_PER_MINUTE = 60;
const int  MINUTES_PER_HOUR = 60;
const int  HOURS_PER_DAY = 24;
const int  SECONDS_PER_HOUR = SECONDS_PER_MINUTE * MINUTES_PER_HOUR;

const long MILLISECONDS_PER_MINUTE = MILLISECONDS_PER_SECOND * SECONDS_PER_MINUTE;
const long NANOSECONDS_PER_MINUTE = NANOSECONDS_PER_SECOND * SECONDS_PER_MINUTE;
const long NANOSECONDS_PER_HOUR = NANOSECONDS_PER_SECOND * SECONDS_PER_HOUR;
const long NANOSECONDS_PER_DAY = NANOSECONDS_PER_HOUR * HOURS_PER_DAY;

class NanoTimer
{
public:
	/** return current nano time: unix-timestamp * 1e9 + nano-part */
	long getNano() const;
	/** return secDiff */
	inline long getSecDiff() const { return secDiff; }
	/** singleton */
	static NanoTimer* getInstance();

private:
	NanoTimer();
	static boost::shared_ptr<NanoTimer> m_ptr;
	long secDiff;
};

inline long getNanoTime()
{
	return NanoTimer::getInstance()->getNano();
}

inline long getSecDiff()
{
	return NanoTimer::getInstance()->getSecDiff();
}

inline long parseTm(struct tm _tm)
{
	return timelocal(&_tm)*NANOSECONDS_PER_SECOND;
}

inline long parseTime(const char* timeStr, const char* format)
{
	struct tm _tm;
	strptime(timeStr, format, &_tm);
	return parseTm(_tm);
}

inline string parseNano(long nano, const char* format)
{
	if (nano <= 0)
	{
		return string("NULL");
	}
	nano /= NANOSECONDS_PER_SECOND;

	struct tm *dt;
	char buffer[40];
	dt = localtime(&nano);

	strftime(buffer, sizeof(buffer), format, dt);
	return string(buffer);
}

inline struct tm parseNano(long nano)
{
	time_t sec_num = nano / NANOSECONDS_PER_SECOND;
	return *localtime(&sec_num);
}

MM_NAMESPACE_END
#endif
