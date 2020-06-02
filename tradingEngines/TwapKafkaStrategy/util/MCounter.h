#ifndef _SPIDER_MCOUNTER_DEFINE_H
#define _SPIDER_MCOUNTER_DEFINE_H

#include <chrono>			

class MCounter
{
public:
	MCounter();
	~MCounter();

public:
	void				restart();
	
	//second
	double				elapsed();

	//(milli second)
	double				elapsed_ms();

	//(micro second)
	double				elapsed_us();

	//(nano second)
	double				elapsed_ns();

protected:
	std::chrono::system_clock::time_point		t_start;

private:

};


//high resolution clock
class MExactCounter
{
public:
	MExactCounter();
	~MExactCounter();

	void				restart();

	//second
	double				elapsed();

	//(milli second)
	double				elapsed_ms();

	//(micro second)
	double				elapsed_us();

	//(nano second)
	double				elapsed_ns();

protected:
	std::chrono::high_resolution_clock::time_point  t_start;

private:

};



#endif