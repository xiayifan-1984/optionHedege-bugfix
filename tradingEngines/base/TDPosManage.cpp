#include "TDPosManage.h"
#include <iostream>

USING_TE_NAMESPACE

TDPosManage::TDPosManage(short source):source(source)
{
	tdPos = {};
	logger = optionHedge::memoryManage::Log::getLogger("tradingEngine");
}

void TDPosManage::display(string username)
{
	if (username == "")
	{
		for (auto pb : tdPos)
		{
			string username(pb.first);
			for (auto ip : pb.second)
			{
				string instrument(ip.first);
				InstrumentPosDetail ipd = ip.second;
				OPTIONHEDGE_LOG_INFO(logger, "user name is " << username << " instrument is " << instrument << " YPOS is " << ipd.YesterdayPos
					<< " TodayOpen " << ipd.TodayOpen << " Close " << ipd.Close << " TodayClose " << ipd.TodayClose
					<< " YesterdayClose " << ipd.YesterdayClose<<" LockClose "<<ipd.LockClose<<" LockYesterdayClose "<<ipd.LockYesterdayClose
					<<" LockTodayClose "<<ipd.LockTodayClose);
			}
		}
	}
	else
	{
		auto userIt = tdPos.find(username);
		if (userIt != tdPos.end())
		{
			InstrumentPos ips = userIt->second;
			for (auto ip : ips)
			{
				string instrument(ip.first);
				InstrumentPosDetail ipd = ip.second;
				OPTIONHEDGE_LOG_INFO(logger, "user name is " << username << " instrument is " << instrument << " YPOS is " << ipd.YesterdayPos
					<< " TodayOpen " << ipd.TodayOpen << " Close " << ipd.Close << " TodayClose " << ipd.TodayClose
					<< " YesterdayClose " << ipd.YesterdayClose << " LockClose " << ipd.LockClose << " LockYesterdayClose " << ipd.LockYesterdayClose
					<< " LockTodayClose " << ipd.LockTodayClose);
			}
		}
	}
}

void TDPosManage::setYesterdayPos(string userID, string instrumentKey, int yesterdayPos)
{
	auto userIt = tdPos.find(userID);

	if (userIt == tdPos.end())
	{
		InstrumentPosDetail insDetail;
		insDetail.YesterdayPos = yesterdayPos;
		InstrumentPos ip;
		ip.insert(std::make_pair(instrumentKey, insDetail));
		tdPos.insert(std::make_pair(userID,ip));
	}

	else
	{
		InstrumentPos& ip = userIt->second;
		auto insIt = ip.find(instrumentKey);
		if (insIt == ip.end())
		{
			InstrumentPosDetail insDetail;
			insDetail.YesterdayPos = yesterdayPos;
			ip.insert(std::make_pair(instrumentKey,insDetail));
		}
		else
		{
			insIt->second.YesterdayPos += yesterdayPos;
		}
	}
}

void TDPosManage::setTodayPos(string userID, string InstrumentKey, TThostFtdcOffsetFlagType offset, TThostFtdcVolumeType volume)
{
	auto userIt = tdPos.find(userID);

	if (userIt == tdPos.end())
	{
		InstrumentPosDetail insDetail;
		switch (offset)
		{
			case '0':
				insDetail.TodayOpen = volume;
				break;
			case '1':
				insDetail.Close = volume;
				break;
			case '3':
				insDetail.TodayClose = volume;
				break;
			case '4':
				insDetail.YesterdayClose = volume;
				break;
			default:
				OPTIONHEDGE_LOG_DEBUG(logger, "unknow offset type in setTodayPos");
		}

		InstrumentPos ip;
		ip.insert(std::make_pair(InstrumentKey, insDetail));
		tdPos.insert(std::make_pair(userID, ip));
	}
	else
	{
		InstrumentPos& ip = userIt->second;
		auto insIt = ip.find(InstrumentKey);
		if (insIt == ip.end())
		{
			InstrumentPosDetail insDetail;
			switch (offset)
			{
				case '0':
					insDetail.TodayOpen = volume;
					break;
				case '1':
					insDetail.Close = volume;
					break;
				case '3':
					insDetail.TodayClose = volume;
					break;
				case '4':
					insDetail.YesterdayClose = volume;
					break;
				default:
					OPTIONHEDGE_LOG_DEBUG(logger, "unknow offset type in setTodayPos");
			}
			ip.insert(std::make_pair(InstrumentKey, insDetail));
		}
		else
		{
			switch (offset)
			{
				case '0':
					insIt->second.TodayOpen += volume;
					break;
				case '1':
					insIt->second.Close += volume;
					break;
				case '3':
					insIt->second.TodayClose += volume;
					break;
				case '4':
					insIt->second.YesterdayClose += volume;
					break;
				default:
					OPTIONHEDGE_LOG_DEBUG(logger, "unknow offset type in setTodayPos");
			}
		}
	}
}

TThostFtdcPosiDirectionType TDPosManage::calPositionDirection(TThostFtdcDirectionType Direction, TThostFtdcOffsetFlagType OffsetFlag)
{
	if (Direction == THOST_FTDC_D_Buy)
	{
		if (OffsetFlag == THOST_FTDC_OF_Open)
		{
			return THOST_FTDC_PD_Long;
		}
		else
		{
			return THOST_FTDC_PD_Short;
		}
	}

	else
	{
		if (OffsetFlag == THOST_FTDC_OF_Open)
		{
			return THOST_FTDC_PD_Short;
		}
		else
		{
			return THOST_FTDC_PD_Long;
		}
	}
}

void TDPosManage::setValue(string username, string InstrumentKey, ValueType type, OperationType op, TThostFtdcVolumeType volume)
{
	std::lock_guard<std::mutex> lck(mtx);

	auto userIt = tdPos.find(username);

	if (userIt == tdPos.end())
	{
		InstrumentPosDetail insDetail;
		setDetailValue(insDetail, type, op, volume);
		InstrumentPos ip;
		ip.insert(std::make_pair(InstrumentKey,insDetail));
		tdPos.insert(std::make_pair(username,ip));
	}
	else
	{
		InstrumentPos& ip = userIt->second;
		auto insIt = ip.find(InstrumentKey);

		if (insIt == ip.end())
		{
			InstrumentPosDetail insDetail;
			setDetailValue(insDetail, type, op, volume);
			ip.insert(std::make_pair(InstrumentKey, insDetail));
		}
		else
		{
			setDetailValue(insIt->second, type, op, volume);
		}
	}
}

void TDPosManage::setDetailValue(InstrumentPosDetail& ipd, ValueType type, OperationType op, TThostFtdcVolumeType volume)
{
	if (type == lockClose)
	{
		if (op == Add)
		{
			ipd.LockClose += volume;
		}
		else
		{
			ipd.LockClose -= volume;
		}
	}

	else if (type == lockYesterdayClose)
	{
		if (op == Add)
		{
			ipd.LockYesterdayClose += volume;
		}
		else
		{
			ipd.LockYesterdayClose -= volume;
		}
	}

	else if (type == lockTodayClose)
	{
		if (op == Add)
		{
			ipd.LockTodayClose += volume;
		}
		else
		{
			ipd.LockTodayClose -= volume;
		}
	}

	else if (type == yesPos)
	{
		if (op == Add)
		{
			ipd.YesterdayPos += volume;
		}
		else
		{
			ipd.YesterdayPos -= volume;
		}
	}

	else if (type == todayOpen)
	{
		if (op == Add)
		{
			ipd.TodayOpen += volume;
		}
		else
		{
			ipd.TodayOpen -= volume;
		}
	}

	else if (type == closE)
	{
		if (op == Add)
		{
			ipd.Close += volume;
		}
		else
		{
			ipd.Close -= volume;
		}
	}

	else if (type == todayClose)
	{
		if (op == Add)
		{
			ipd.TodayClose += volume;
		}
		else
		{
			ipd.TodayClose -= volume;
		}
	}

	else if (type == yesClose)
	{
		if (op == Add)
		{
			ipd.YesterdayClose += volume;
		}
		else
		{
			ipd.YesterdayClose -= volume;
		}
	}
}


TThostFtdcOffsetFlagType TDPosManage::autoOffset(string username, TThostFtdcInstrumentIDType instrument, TThostFtdcExchangeIDType exchangeID, TThostFtdcHedgeFlagType hedgeFlag, TThostFtdcDirectionType direction, TThostFtdcVolumeType volume)
{
	//postion not right, should use pos direction in qry pos, not in send orders
	string searchDir,posDir;
	(direction == THOST_FTDC_D_Buy) ? searchDir = THOST_FTDC_PD_Short : searchDir = THOST_FTDC_PD_Long;
	(direction == THOST_FTDC_D_Buy) ? posDir = THOST_FTDC_PD_Long : posDir = THOST_FTDC_PD_Short;
	string sinstrumentKey = string(instrument) +"_"+ hedgeFlag + "_"+ searchDir;
	string instrumentKey = string(instrument) + "_" + hedgeFlag + "_" + posDir;

	display(username);
	auto userIt = tdPos.find(username);

	if (userIt == tdPos.end())
	{
		setValue(username, instrumentKey, todayOpen, Add,volume);
		return THOST_FTDC_OF_Open;
	}

	else
	{
		InstrumentPos& ip = userIt->second;
		auto insIt = ip.find(sinstrumentKey);

		if (insIt == ip.end())
		{
			return THOST_FTDC_OF_Open;
		}
		else
		{
			InstrumentPosDetail& insDetail=insIt->second;
			if (exchangeID == string("CZCE") || exchangeID == string("DCE") || exchangeID == string("CFFEX"))
			{
				if (insDetail.YesterdayPos + insDetail.TodayOpen-insDetail.LockClose >= volume)
				{
					setValue(username, sinstrumentKey, lockClose, Add, volume);
					return THOST_FTDC_OF_Close;
				}
				else
				{
					return THOST_FTDC_OF_Open;
				}
			}
			else if(exchangeID== string("SHFE")||exchangeID== string("INE"))
			{
				//SHFE INE 1 close today 2 close yesterday 3 open
				if (insDetail.TodayOpen -insDetail.LockTodayClose >= volume)
				{
					setValue(username, sinstrumentKey, lockTodayClose, Add, volume);
					OPTIONHEDGE_LOG_DEBUG(logger, "pos detail in autoOffset");
					display(username);
					return THOST_FTDC_OF_CloseToday;
				}
				else if (insDetail.YesterdayPos-insDetail.LockYesterdayClose>=volume)
				{
					setValue(username, sinstrumentKey, lockYesterdayClose, Add, volume);
					OPTIONHEDGE_LOG_DEBUG(logger, "pos detail in autoOffset");
					display(username);
					return THOST_FTDC_OF_CloseYesterday;
				}
				else
				{
					return THOST_FTDC_OF_Open;
				}
			}
		}
	}
}

void TDPosManage::setLockInfo(string key, bool lock, bool unlock)
{
	OPTIONHEDGE_LOG_DEBUG(logger, "set orderLockInfo " << key);
	lockInfo lockinfo;

	lockinfo.blocked = lock;
	lockinfo.bunlocked = unlock;

	auto it = orderLockInfo.find(key);
	if (it == orderLockInfo.end())
	{
		orderLockInfo[key]=lockinfo;
	}
	else
	{
		lockInfo& li = it->second;
		li.blocked = lock;
		li.bunlocked = unlock;
	}
}

bool TDPosManage::checkLockInfo(string key)
{
	auto it = orderLockInfo.find(key);
	if (it == orderLockInfo.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

lockInfo TDPosManage::getLockInfo(string key)
{
	auto it = orderLockInfo.find(key);
	return it->second;
}

void TDPosManage::clearPos(string username)
{
	if (username == "")
	{
		tdPos.clear();
		return ;
	}

	tdPos.erase(username);
	boost::regex pattern(username);
	for (auto it= orderLockInfo.begin();it!=orderLockInfo.end();)
	{
		if (boost::regex_search(it->first.begin(), it->first.end(), pattern))
		{
			OPTIONHEDGE_LOG_DEBUG(logger, "remove orderLockInfo " << it->first);
			it = orderLockInfo.erase(it);
		}
		else
		{
			it++;
		}
	}
}
