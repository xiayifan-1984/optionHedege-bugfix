
#ifndef _SPIDER_MDATETIME_DEFINE_H
#define _SPIDER_MDATETIME_DEFINE_H

#include <time.h>

class MDateTime
{
public:
	MDateTime();
	~MDateTime();

public:
	void				from_now();
	static MDateTime	now();

public:
	int					dateToInt();		//YYYYMMDD
	int					timeToInt();		//HHMMSS

	int					year() const;
	int					month() const;
	int					day() const;
	int					hour() const;
	int					minute() const;
	int					second() const;

public:
	//构造函数，入参time_t
	MDateTime(time_t stTime);
	//构造函数， (YYYY MM DD)
	MDateTime(unsigned short sYear, unsigned short sMonth, unsigned short sDay);
	//构造函数,  (HH  MM  SS)
	MDateTime(unsigned short sHour, unsigned short sMin, unsigned short sSec, unsigned short sMSec);
	//构造函数， (YYYY MM DD) (HH MM SS)
	MDateTime(unsigned short sYear, unsigned short sMonth, unsigned short sDay, unsigned short sHour, unsigned short sMin, unsigned short sSec);

	MDateTime(const MDateTime&  mTime);
	MDateTime & operator =(const MDateTime&  mTime);

public:
	MDateTime & operator =(time_t tt);
	time_t				to_time_t() const;

	MDateTime &			assign(time_t tt);
	MDateTime &			assign(unsigned short sYear, unsigned short sMonth, unsigned short sDay);
	MDateTime &			assign(unsigned short sHour, unsigned short sMin, unsigned short sSec, unsigned short sMSec);
	MDateTime &			assign(unsigned short sYear, unsigned short sMonth, unsigned short sDay, unsigned short sHour, unsigned short sMin, unsigned short sSec);

public:
	//接口二  比较
	bool operator == (const MDateTime & mIn);
	bool operator != (const MDateTime & mIn);
	bool operator >  (const MDateTime & mIn);
	bool operator >= (const MDateTime & mIn);
	bool operator <  (const MDateTime & mIn);
	bool operator <= (const MDateTime & mIn);

public:
	//接口三 重载运算符
	//重载运算符，减去一个时间，得到两个时间之差，单位为秒
	int operator - (MDateTime & mIn);
	//重载运算符，减去一个时间（单位为秒），得到另外一个时间
	MDateTime operator - (int iIn);
	//重载运算符，加上一个时间（单位为秒），得到另外一个时间
	MDateTime operator + (int iIn);
	//重载运算符，加上一个时间（单位为秒），得到另外一个时间
	MDateTime & operator += (int iIn);
	//重载运算符，减去一个时间（单位为秒），得到另外一个时间
	MDateTime & operator -= (int iIn);

public:
	bool 				isEmpty(void);
	void 				clear(void);

protected:
	time_t				m_curTime;

private:

};





#endif
