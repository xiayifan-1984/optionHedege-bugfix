#ifndef TRADING_TRADINGSTRATEGYUTIL_H
#define TRADING_TRADINGSTRATEGYUTIL_H

#include "../base/TE_DECLEAR.h"
#include "../../memoryManage/journal/StrategyUtil.h"
#include "../base/TradingConstant.h"
#include <boost/function.hpp>

TE_NAMESPACE_START
using optionHedge::memoryManage::StrategyUtil;

#define STRATEGY_TD_FAILED       0
#define STRATEGY_TD_ACK          1
#define STRATEGY_TD_READY        2

class TradingStrategyUtil :public StrategyUtil
{
private:
	long md_nano;
	long cur_nano;
	string strategy_name;

public:
	TradingStrategyUtil(const string& strategyName);
	/** take actions */
	int insert_market_order(short source, string instrument_id, string exchange_id, int volume, DirectionType direction, OffsetFlagType offset,string orderRef="");
	int insert_limit_order(short source, string instrument_id, string exchange_id, double price, int volume, DirectionType direction, OffsetFlagType offset, string orderRef = "");
	int insert_fok_order(short source, string instrument_id, string exchange_id, double price, int volume, DirectionType direction, OffsetFlagType offset, string orderRef = "");
	int insert_fak_order(short source, string instrument_id, string exchange_id, double price, int volume, DirectionType direction, OffsetFlagType offset, string orderRef = "");
	int req_position(short source);
	int req_tradingAccount(short source);
	int req_qry_order(short source, string order_sysid="");
	int req_qry_trade(short source, string investmentId = "", string tradeId = "");
	int cancel_order(short source,string orderRef,string instrumentID, string exchangeID, string orderSysID="");

	/** set md nano time */
	void set_md_nano(long cur_time) { md_nano = cur_time; }

	/** basic utilities */
	/** nano time */
	long   get_nano();
	/** get string time with format "%Y-%m-%d %H:%M:%S" */
	string get_time();
	/** parse time */
	long   parse_time(string time_str);
	/** parse nano */
	string parse_nano(long nano);
};

DECLARE_PTR(TradingStrategyUtil)

TE_NAMESPACE_END
#endif
