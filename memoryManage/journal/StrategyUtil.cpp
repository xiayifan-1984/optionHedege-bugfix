#include "StrategyUtil.h"
#include "StrategySocketHandler.h"
#include "../utils/util.hpp"
#include "../paged/PageCommStruct.h"
#include "../../system/systemConfig/sys_messages.h"

USING_MM_NAMESPACE

StrategyUtil::StrategyUtil(const string& strategyName, bool isReplay)
{
	if (isReplay)
	{
		throw std::runtime_error("replay mode is not fully supported yet!");
	}
	else
	{
		handler = StrategySocketHandlerPtr(new StrategySocketHandler(strategyName));
		writer = JournalWriter::create(STRATEGY_BASE_FOLDER, strategyName, handler);
		init();
	}
}

StrategyUtilPtr StrategyUtil::create(const string& strategyName, bool isReplay)
{
#ifndef USE_PAGED_SERVICE
	throw std::runtime_error("Strategy utils must use paged services");
#endif
	StrategyUtilPtr util = StrategyUtilPtr(new StrategyUtil(strategyName, isReplay));
	return util;
}

StrategyUtil::~StrategyUtil()
{
	writer.reset();
	handler.reset();
}

void StrategyUtil::init()
{
	register_strategy(rid_start, rid_end);
	cur_rid = 0;
}

bool StrategyUtil::td_connect(short source)
{
	if (handler.get() != nullptr)
	{
		return handler->td_connect(source);
	}

	else
		return false;
}

IntPair StrategyUtil::getRequestIds() const
{
	return std::make_pair(rid_start, rid_end);
}

bool StrategyUtil::md_subscribe(const vector<string>& tickers, short source, short msg_type)
{
	if (handler.get() != nullptr)
	{
		return handler->md_subscribe(tickers, source, msg_type);
	}
	else
		return false;
}

bool StrategyUtil::register_strategy(int& requestIdStart, int& requestIdEnd)
{
	if (handler.get() != nullptr)
	{
		return handler->register_strategy(requestIdStart, requestIdEnd);
	}
	else
		return false;
}

long StrategyUtil::writeStr(const string& str)
{
	return writer->writeStr(str);
}

long StrategyUtil::write_frame(const void* data, FH_TYPE_LENGTH length, FH_TYPE_SOURCE source,
	FH_TYPE_MSG_TP msgType, FH_TYPE_LASTFG lastFlag, FH_TYPE_REQ_ID requestId)
{
	return writer->write_frame(data, length, source, msgType, lastFlag, requestId);
}

long StrategyUtil::write_frame_extra(const void* data, FH_TYPE_LENGTH length, FH_TYPE_SOURCE source, FH_TYPE_MSG_TP msgType,
	FH_TYPE_LASTFG lastFlag, FH_TYPE_REQ_ID requestId, FH_TYPE_NANOTM extraNano)
{
	return writer->write_frame_extra(data, length, source, msgType, lastFlag, requestId, extraNano);
}

bool StrategyUtil::subscribeMarketData(const vector<string>& tickers, short source)
{
	return md_subscribe(tickers, source, MSG_TYPE_SUBSCRIBE_MARKET_DATA);
}

bool StrategyUtil::subscribeL2MD(const vector<string>& tickers, short source)
{
	return md_subscribe(tickers, source, MSG_TYPE_SUBSCRIBE_L2_MD);
}

bool StrategyUtil::subscribeIndex(const vector<string>& tickers, short source)
{
	return md_subscribe(tickers, source, MSG_TYPE_SUBSCRIBE_INDEX);
}

bool StrategyUtil::subscribeOrderTrade(const vector<string>& tickers, short source)
{
	return md_subscribe(tickers, source, MSG_TYPE_SUBSCRIBE_ORDER_TRADE);
}

int StrategyUtil::get_rid()
{
	return (cur_rid++ % REQUEST_ID_RANGE) + rid_start;
}