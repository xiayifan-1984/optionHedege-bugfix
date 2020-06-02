#ifndef MEMORYMANAGE_STRATEGYUTIL_H
#define MEMORYMANAGE_STRATEGYUTIL_H

#include "IStrategyUtil.h"
#include "JournalWriter.h"

MM_NAMESPACE_START

#define STRATEGY_BASE_FOLDER OPTIONHEDGE_JOURNAL_FOLDER "strategy/"

FORWARD_DECLARE_PTR(StrategySocketHandler);
FORWARD_DECLARE_PTR(JournalWriter);
FORWARD_DECLARE_PTR(StrategyUtil);

class StrategyUtil :public IStrategyUtil
{
protected:
	int rid_start;
	int rid_end;
	int cur_rid;

private:
	StrategySocketHandlerPtr handler;
	JournalWriterPtr writer;

protected:
	void init();

public:
	bool td_connect(short source);
	bool md_subscribe(const vector<string>& tickers, short source, short msg_type);
	bool register_strategy(int &rquestIdStart, int& requestIdEnd);
	long writeStr(const string& str);

	long write_frame(const void* data, FH_TYPE_LENGTH length,
		FH_TYPE_SOURCE source, FH_TYPE_MSG_TP msgType,
		FH_TYPE_LASTFG lastFlag, FH_TYPE_REQ_ID requestId);
	/** writer feature may used by strategy */
	long write_frame_extra(const void* data, FH_TYPE_LENGTH length,
		FH_TYPE_SOURCE source, FH_TYPE_MSG_TP msgType,
		FH_TYPE_LASTFG lastFlag, FH_TYPE_REQ_ID requestId,
		FH_TYPE_NANOTM extraNano);

	/** subscribe md with MARKET_DATA flag */
	bool subscribeMarketData(const vector<string>& tickers, short source);
	/** subscribe md with L2_MD flag */
	bool subscribeL2MD(const vector<string>& tickers, short source);
	/** subscribe md with INDEX flag */
	bool subscribeIndex(const vector<string>& tickers, short source);
	/** subscribe md with ORDER_TRADE flag */
	bool subscribeOrderTrade(const vector<string>& tickers, short source);

	int get_rid();
	IntPair getRequestIds() const;

public:
	~StrategyUtil();
	StrategyUtil(const string& strategyName, bool isReplay = false);
	/** create method, only strategy name is enough
	 *  folder and journal name is determined */
	static StrategyUtilPtr create(const string& strategyName, bool isReplay = false);
};
MM_NAMESPACE_END
#endif
