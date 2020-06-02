#include "Timer.h"
#include "json.hpp"
//#include "PageSocketStruct.h"
#include <chrono>
#include <boost/asio.hpp>
#include <boost/array.hpp>

USING_MM_NAMESPACE

using json = nlohmann::json;

boost::shared_ptr<NanoTimer> NanoTimer::m_ptr = boost::shared_ptr<NanoTimer>(nullptr);

NanoTimer* NanoTimer::getInstance()
{
	if (m_ptr.get() == nullptr)
	{
		m_ptr = boost::shared_ptr<NanoTimer>(new NanoTimer());
	}

	return m_ptr.get();
}

inline std::chrono::steady_clock::time_point get_time_now()
{
	timespec tp;
	clock_gettime(CLOCK_MONOTONIC, &tp);
	return std::chrono::steady_clock::time_point(std::chrono::steady_clock::duration(
		std::chrono::seconds(tp.tv_sec) + std::chrono::nanoseconds(tp.tv_nsec)
	));
}

inline long get_socket_diff()
{
	return 0;
}

inline long get_local_diff()
{
	int unix_second_num = std::chrono::seconds(std::time(NULL)).count();
	int tick_second_num = std::chrono::duration_cast<std::chrono::seconds>(
		get_time_now().time_since_epoch()
		).count();
	return (unix_second_num - tick_second_num) * NANOSECONDS_PER_SECOND;
}

NanoTimer::NanoTimer()
{
	try
	{
		//need to update here after page service implemented
		//secDiff = get_socket_diff();
		secDiff = get_local_diff();
	}
	catch (...)
	{
		secDiff = get_local_diff();
	}
}

long NanoTimer::getNano() const
{
	long _nano = std::chrono::duration_cast<std::chrono::nanoseconds>(
		get_time_now().time_since_epoch()
		).count();
	return _nano + secDiff;
}
