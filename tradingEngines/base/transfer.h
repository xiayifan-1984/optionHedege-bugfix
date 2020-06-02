#ifndef TRADINGENGINE_TRANSFER_H
#define TRADINGENGINE_TRANSFER_H

#include "TradingConstant.h"

inline void UpdateTimeFromMilSecInt(char* time_str, int time_int)
{
	time_int /= 1000;
	sprintf(time_str, "%02d:%02d:%02d", time_int / 10000, time_int / 100 % 100, time_int % 100);
}

/*
 * 150006320 -> 320
 */
inline void UpdateMillisecFromMilSecInt(int& mil_sec, int time_int)
{
	mil_sec = time_int % 1000;
}

/*
 * YYYYMMDDHHMMSSsss -> 'YYYYMMDD'
 */
inline void TradingDayFromLong(char* trading_day, long datetime)
{
	sprintf(trading_day, "%d", (int)(datetime / 1000000000));
}

/*
 * YYYYMMDDHHMMSSsss -> 'DD:HH:MM'
 */
inline void UpdateTimeFromLong(char* update_time, long datetime)
{
	int time_int = datetime % 1000000000;
	time_int /= 1000;
	sprintf(update_time, "%02d:%02d:%02d", time_int / 10000, time_int / 100 % 100, time_int % 100);
}

/*
 * YYYYMMDDHHMMSSsss -> sss
 */
inline void MillisecFromLong(int& mil_sec, long datetime)
{
	mil_sec = datetime % 1000;
}

#endif
