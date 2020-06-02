#ifndef TRADINGENGINE_TDPOSMANAGE_H
#define TRADINGENGINE_TDPOSMANAGE_H

#include <unordered_map>
#include "TE_DECLEAR.h"
#include "TradingConstant.h"
#include <boost/regex.hpp>
#include "../../system/api/ctp/v6.3.15/include/ThostFtdcUserApiDataType.h"
#include "../../memoryManage/log/MM_Log.h"
#include <mutex>

TE_NAMESPACE_START

using optionHedge::memoryManage::LogPtr;

typedef struct InstrumentPosDetail
{
	InstrumentPosDetail()
	{
		YesterdayPos = 0;
		Close = 0;
		TodayOpen = 0;
		TodayClose = 0;
		YesterdayClose = 0;

		LockTodayClose = 0;
		LockYesterdayClose = 0;
		LockClose = 0;
	}

	int YesterdayPos;
	int Close;
	int TodayOpen;
	int TodayClose;
	int YesterdayClose;

	int LockTodayClose;
	int LockYesterdayClose;
	int LockClose;
};

typedef struct lockInfo
{
	lockInfo()
	{
		blocked = false;
		bunlocked = false;
	}
	bool blocked;
	bool bunlocked;
};


typedef std::unordered_map<std::string, InstrumentPosDetail> InstrumentPos;
typedef std::unordered_map<std::string, InstrumentPos> PosBook;

class TDPosManage
{
public:
	TDPosManage(short source);
	TThostFtdcPosiDirectionType calPositionDirection(TThostFtdcDirectionType Direction, TThostFtdcOffsetFlagType OffsetFlag);
	void setYesterdayPos(string userID,string instrumentKey,int yesterdayPos);
	void setTodayPos(string userID, string InstrumentKey, TThostFtdcOffsetFlagType offset, TThostFtdcVolumeType volume);
	void display(string username="");
	void setValue(string username,string InstrumentKey,ValueType type, OperationType op, TThostFtdcVolumeType volume);
	TThostFtdcOffsetFlagType autoOffset(string username, TThostFtdcInstrumentIDType instrument, TThostFtdcExchangeIDType exchangeID, TThostFtdcHedgeFlagType hedgeFlag,TThostFtdcDirectionType direction, TThostFtdcVolumeType volume);
	void setLockInfo(string key,bool lock=true,bool unlock=false);
	bool checkLockInfo(string key);
	lockInfo getLockInfo(string key);
	void clearPos(string username);
private:
	void setDetailValue(InstrumentPosDetail& ipd, ValueType type, OperationType op, TThostFtdcVolumeType volume);
	short source;
	PosBook tdPos;
	LogPtr logger;
	std::unordered_map<string, lockInfo> orderLockInfo;
	std::mutex mtx;
};

DECLARE_PTR(TDPosManage);
TE_NAMESPACE_END
#endif
