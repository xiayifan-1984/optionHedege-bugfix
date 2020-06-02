#ifndef TRADINGENGINE_ISTRATEGY_H
#define TRADINGENGINE_ISTRATEGY_H

#include "../base/TE_DECLEAR.h"
#include "TradingStrategyUtil.h"
#include "TSDataWrapper.h"
#include "../../memoryManage/log/MM_Log.h"
#include "IDataProcessor.h"

TE_NAMESPACE_START

using optionHedge::memoryManage::LogPtr;

class IStrategy :public IDataProcessor
{
protected:
	bool td_is_ready(short source) const;
	bool td_is_connected(short source) const;
	void parsingAccountInfo(const std::string& configFile,short source);
public:
	/** IWCDataProcessor functions *//* market data */
	virtual void on_market_data(const MarketDataField* data, short source, long rcv_time);
//	virtual void on_market_data_level2(const L2MarketDataField* data, short source, long rcv_time);
//	virtual void on_l2_index(const L2IndexField* data, short source, long rcv_time);
//	virtual void on_l2_order(const L2OrderField* data, short source, long rcv_time);
//	virtual void on_l2_trade(const L2TradeField* data, short source, long rcv_time);
//	virtual void on_market_bar(const BarMdMap& data, int min_interval, short source, long rcv_time);
	/* trade data */
	virtual void on_rtn_order(const RtnOrderField* data, int request_id, short source, long rcv_time);
	virtual void on_rtn_trade(const RtnTradeField* data, int request_id, short source, long rcv_time);
	virtual void on_rtn_instrument_status(const RspInstrumentStatus* data, int request_id, short source, long rcv_time);
	virtual void on_rsp_order(const InputOrderField* data, int request_id, short source, long rcv_time, short errorId = 0, const char* errorMsg = nullptr);
	virtual void on_rsp_qry_order(const RtnOrderField* data, int request_id, short source, long rcv_time, bool isLast,short errorId = 0, const char* errorMsg = nullptr);
	virtual void on_rsp_qry_trade(const RtnTradeField* data, int request_id, short source, long rcv_time, bool isLast, short errorId = 0, const char* errorMsg = nullptr);
	virtual void on_rsp_position(const RspPositionField* pos, int request_id, short source, long rcv_time, bool isLast, short errorId = 0, const char* errorMsg = nullptr);
	virtual void on_rsp_account(const RspAccountField * data, int request_id, short source, long rcv_time, bool isLast, short errorId = 0, const char* errorMsg = nullptr);
	virtual void on_rsp_action(const RspActionField * data, int request_id, short source, long rcv_time);
	//	virtual void on_rsp_position(const PosHandlerPtr posMap, int request_id, short source, long rcv_time);
	/* system utilities */
//	virtual void on_switch_day(long rcv_time);
//	virtual void on_time(long cur_time);
	virtual void on_td_login(bool ready, const json& js, short source);
	/* on log */
	virtual void debug(const char* msg) { OPTIONHEDGE_LOG_DEBUG(logger, msg); }
	/* get name */
	virtual string get_name() const { return name; }

	virtual string get_tradeDate() const { return tradedate; }

	/**
	 * setup data wrapper
	 * by calling add_* functions in WCDataWrapper
	 * also subscribe tickers
	 */
	virtual void init() = 0;

public: // util functions, wrap upon WCStrategyUtil
/** insert order, check status before calling WCStrategyUtil */
	int insert_market_order(short source, string instrument_id, string exchange_id, int volume, DirectionType direction, OffsetFlagType offset);
	int insert_limit_order(short source, string instrument_id, string exchange_id, double price, int volume, DirectionType direction, OffsetFlagType offset);
	int insert_fok_order(short source, string instrument_id, string exchange_id, double price, int volume, DirectionType direction, OffsetFlagType offset);
	int insert_fak_order(short source, string instrument_id, string exchange_id, double price, int volume, DirectionType direction, OffsetFlagType offset);
	int req_position(short source);
	int cancel_order(short source, string orderRef, string instrumentID, string exchangeID, string orderSysID = "");

public:
	virtual ~IStrategy();
	IStrategy(const string& name);
	virtual void start();
	void run();
	void terminate();
	void stop();
	void block();

protected:
	LogPtr logger;
	const string name;
	TradingStrategyUtilPtr util;
	TSDataWrapperPtr data;
	ThreadPtr data_thread;
	std::string userId;
	std::string investorId;
	std::string brokerId;
	std::string tradedate;
};
TE_NAMESPACE_END
#endif
