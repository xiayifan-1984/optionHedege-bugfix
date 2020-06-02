#include "Platform.h"
#include "MDateTime.h"


MDateTime::MDateTime()
{
	m_curTime = 0;
}

MDateTime::~MDateTime()
{

}

MDateTime::MDateTime(time_t stTime)
{
	m_curTime = stTime;
}

MDateTime::MDateTime(unsigned short sYear, unsigned short sMonth, unsigned short sDay)
{
	struct tm						sttime;

	sttime.tm_year = sYear - 1900;
	sttime.tm_mon = sMonth - 1;
	sttime.tm_mday = sDay;
	sttime.tm_hour = 0;
	sttime.tm_min = 0;
	sttime.tm_sec = 0;
	m_curTime = mktime(&sttime);
}

MDateTime::MDateTime(unsigned short sHour, unsigned short sMin, unsigned short sSec, unsigned short sMSec)
{
	struct tm						sttime;

	sttime.tm_year = 70;
	sttime.tm_mon = 0;
	sttime.tm_mday = 1;
	sttime.tm_hour = sHour;
	sttime.tm_min = sMin;
	sttime.tm_sec = sSec;
	m_curTime = mktime(&sttime);
}

MDateTime::MDateTime(unsigned short sYear, unsigned short sMonth, unsigned short sDay, unsigned short sHour, unsigned short sMin, unsigned short sSec)
{
	struct tm						sttime;

	sttime.tm_year = sYear - 1900;
	sttime.tm_mon = sMonth - 1;
	sttime.tm_mday = sDay;
	sttime.tm_hour = sHour;
	sttime.tm_min = sMin;
	sttime.tm_sec = sSec;
	m_curTime = mktime(&sttime);
}

MDateTime::MDateTime(const MDateTime&  mTime)
{
	m_curTime = mTime.m_curTime;
}

MDateTime & MDateTime::operator =(const MDateTime&  mTime)
{
	m_curTime = mTime.m_curTime;
	return(*this);
}

MDateTime & MDateTime::operator =(time_t tt)
{
	m_curTime = tt;
	return (*this);
}

time_t		MDateTime::to_time_t() const
{
	return m_curTime;
}

MDateTime &			MDateTime::assign(time_t tt)
{
	m_curTime = tt;
	return (*this);
}

MDateTime &			MDateTime::assign(unsigned short sYear, unsigned short sMonth, unsigned short sDay)
{
	struct tm						sttime;

	sttime.tm_year = sYear - 1900;
	sttime.tm_mon = sMonth - 1;
	sttime.tm_mday = sDay;
	sttime.tm_hour = 0;
	sttime.tm_min = 0;
	sttime.tm_sec = 0;
	m_curTime = mktime(&sttime);
	return (*this);
}

MDateTime &			MDateTime::assign(unsigned short sHour, unsigned short sMin, unsigned short sSec, unsigned short sMSec)
{
	struct tm						sttime;

	sttime.tm_year = 70;
	sttime.tm_mon = 0;
	sttime.tm_mday = 1;
	sttime.tm_hour = sHour;
	sttime.tm_min = sMin;
	sttime.tm_sec = sSec;
	m_curTime = mktime(&sttime);
	return (*this);
}

MDateTime &			MDateTime::assign(unsigned short sYear, unsigned short sMonth, unsigned short sDay, unsigned short sHour, unsigned short sMin, unsigned short sSec)
{
	struct tm						sttime;

	sttime.tm_year = sYear - 1900;
	sttime.tm_mon = sMonth - 1;
	sttime.tm_mday = sDay;
	sttime.tm_hour = sHour;
	sttime.tm_min = sMin;
	sttime.tm_sec = sSec;
	m_curTime = mktime(&sttime);
	return (*this);
}

void MDateTime::from_now()
{
	m_curTime = time(NULL);
}

MDateTime	MDateTime::now()
{
	MDateTime o;
	o.from_now();
	return o;
}

int	MDateTime::year() const
{
	struct tm 	temptime;
	localtime_s(&temptime, &m_curTime);

	return (temptime.tm_year + 1900);
}

int		MDateTime::month() const
{
	struct tm 	temptime;
	localtime_s(&temptime, &m_curTime);

	return(temptime.tm_mon + 1);
}

int	MDateTime::day() const
{
	struct tm 	temptime;
	localtime_s(&temptime, &m_curTime);

	return(temptime.tm_mday);
}

int	MDateTime::hour() const
{
	struct tm 	temptime;
	localtime_s(&temptime, &m_curTime);

	return(temptime.tm_hour);
}

int		MDateTime::minute() const
{
	struct tm 	temptime;
	localtime_s(&temptime, &m_curTime);

	return(temptime.tm_min);
}

int		MDateTime::second() const
{
	struct tm 	temptime;
	localtime_s(&temptime, &m_curTime);

	return(temptime.tm_sec);
}

int	MDateTime::dateToInt()	//YYYYMMDD
{
	struct tm 	temptime;
	localtime_s(&temptime, &m_curTime);

	return((temptime.tm_year + 1900) * 10000 + (temptime.tm_mon + 1) * 100 + temptime.tm_mday);
}

int		MDateTime::timeToInt()	//HHMMSS
{
	struct tm 	temptime;
	localtime_s(&temptime, &m_curTime);

	return(temptime.tm_hour * 10000 + temptime.tm_min * 100 + temptime.tm_sec);
	
}

bool 		MDateTime::isEmpty()
{
	if (m_curTime == 0)
	{
		return(true);
	}
	else
	{
		return(false);
	}
}

void 		MDateTime::clear(void)
{
	m_curTime = 0;
}

bool MDateTime::operator == (const MDateTime & mIn)
{
	if (m_curTime == mIn.m_curTime)
	{
		return(true);
	}
	else
	{
		return(false);
	}
}

bool MDateTime::operator != (const MDateTime & mIn)
{
	if (m_curTime != mIn.m_curTime)
	{
		return(true);
	}
	else
	{
		return(false);
	}
}

bool MDateTime::operator >  (const MDateTime & mIn)
{
	if (m_curTime > mIn.m_curTime)
	{
		return(true);
	}
	else
	{
		return(false);
	}
}

bool MDateTime::operator >= (const MDateTime & mIn)
{
	if (m_curTime >= mIn.m_curTime)
	{
		return(true);
	}
	else
	{
		return(false);
	}
}

bool MDateTime::operator <  (const MDateTime & mIn)
{
	if (m_curTime < mIn.m_curTime)
	{
		return(true);
	}
	else
	{
		return(false);
	}
}

bool MDateTime::operator <= (const MDateTime & mIn)
{
	if (m_curTime <= mIn.m_curTime)
	{
		return(true);
	}
	else
	{
		return(false);
	}
}

int MDateTime::operator - (MDateTime & mIn)
{
	return((int)(m_curTime - mIn.m_curTime));
}

MDateTime MDateTime::operator - (int iIn)
{
	return(MDateTime(m_curTime - iIn));
}

MDateTime MDateTime::operator + (int iIn)
{
	return(MDateTime(m_curTime + iIn));
}

MDateTime & MDateTime::operator += (int iIn)
{
	m_curTime += iIn;

	return(*this);
}

MDateTime & MDateTime::operator -= (int iIn)
{
	m_curTime -= iIn;

	return(*this);
}





