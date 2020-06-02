#include "MCounter.h"

MCounter::MCounter()
{
	restart();
}

MCounter::~MCounter()
{

}

void				MCounter::restart()
{
	t_start = std::chrono::system_clock::now();
}
	
double		MCounter::elapsed()
{
	std::chrono::system_clock::time_point t_end = std::chrono::system_clock::now();

	double ms = std::chrono::duration<double, std::ratio<1,1> >(t_end - t_start).count();

	return ms;
}

//(milli second)
double		MCounter::elapsed_ms()
{
	std::chrono::system_clock::time_point t_end = std::chrono::system_clock::now();

	double ms = std::chrono::duration<double, std::milli>(t_end - t_start).count();

	return ms;
}

//(micro second)
double		MCounter::elapsed_us()
{
	std::chrono::system_clock::time_point t_end = std::chrono::system_clock::now();

	double ms = std::chrono::duration<double, std::micro>(t_end - t_start).count();

	return ms;
}

//(nano second)
double		MCounter::elapsed_ns()
{
	std::chrono::system_clock::time_point t_end = std::chrono::system_clock::now();

	double ms = std::chrono::duration<double, std::nano>(t_end - t_start).count();

	return ms;
}

//...............................................................................................................................................................................................
//...............................................................................................................................................................................................
//...............................................................................................................................................................................................
//...............................................................................................................................................................................................

MExactCounter::MExactCounter()
{
	restart();
}

MExactCounter::~MExactCounter()
{

}

void				MExactCounter::restart()
{
	t_start = std::chrono::high_resolution_clock::now();
}


double				MExactCounter::elapsed()
{
	std::chrono::high_resolution_clock::time_point t_end = std::chrono::high_resolution_clock::now();

	double ms = std::chrono::duration<double, std::ratio<1,1> >(t_end - t_start).count();

	return ms;
}

//(milli second)
double				MExactCounter::elapsed_ms()
{
	std::chrono::high_resolution_clock::time_point t_end = std::chrono::high_resolution_clock::now();

	double ms = std::chrono::duration<double, std::milli>(t_end - t_start).count();

	return ms;
}

//(micro second)
double				MExactCounter::elapsed_us()
{
	std::chrono::high_resolution_clock::time_point t_end = std::chrono::high_resolution_clock::now();

	double ms = std::chrono::duration<double, std::micro>(t_end - t_start).count();

	return ms;
}

//(nano second)
double				MExactCounter::elapsed_ns()
{
	std::chrono::high_resolution_clock::time_point t_end = std::chrono::high_resolution_clock::now();

	double ms = std::chrono::duration<double, std::nano>(t_end - t_start).count();

	return ms;
}






