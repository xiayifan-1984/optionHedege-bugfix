#ifndef TRADINGENGINE_IDATAPROCESSOR_H
#define TRADINGENGINE_IDATAPROCESSOR_H

#include "../base/TE_DECLEAR.h"
#include "../base/DataStruct.h"
#include "../../memoryManage/utils/json.hpp"


TE_NAMESPACE_START

using json = nlohmann::json;

class IDataProcessor
{
public:
	/* market data */
	virtual void on_market_data(const MarketDataField* data, short source, long rcv_time) = 0;
//	virtual void on_market_data_level2(const L2MarketDataField* data, short source, long rcv_time) = 0;
//	virtual void on_l2_index(const L2IndexField* data, short source, long rcv_time) = 0;
//	virtual void on_l2_order(const L2OrderField* data, short source, long rcv_time) = 0;
//	virtual void on_l2_trade(const L2TradeField* data, short source, long rcv_time) = 0;

	/* market bar data */
//	virtual void on_market_bar(const BarMdMap& data, int min_interval, short source, long rcv_time) = 0;
	/* trade data */
	virtual void on_rtn_order(const RtnOrderField* data, int request_id, short source, long rcv_time) = 0;
	virtual void on_rtn_trade(const RtnTradeField* data, int request_id, short source, long rcv_time) = 0;
	virtual void on_rtn_instrument_status(const RspInstrumentStatus* data, int request_id, short source, long rcv_time) = 0;
	virtual void on_rsp_order(const InputOrderField* data, int request_id, short source, long rcv_time, short errorId = 0, const char* errorMsg = nullptr) = 0;
	virtual void on_rsp_account(const RspAccountField * data, int request_id, short source, long rcv_time, bool isLast, short errorId = 0, const char* errorMsg = nullptr)=0;
	virtual void on_rsp_position(const RspPositionField* pos, int request_id, short source, long rcv_time, bool isLast, short errorId = 0, const char* errorMsg = nullptr)=0;
	virtual void on_rsp_qry_order(const RtnOrderField* data, int request_id, short source, long rcv_time, bool isLast, short errorId = 0, const char* errorMsg = nullptr) = 0;
	virtual void on_rsp_qry_trade(const RtnTradeField* data, int request_id, short source, long rcv_time, bool isLast, short errorId = 0, const char* errorMsg = nullptr) = 0;
	virtual void on_rsp_action(const RspActionField * data, int request_id, short source, long rcv_time)=0;
	/* system utilities */
//	virtual void on_switch_day(long rcv_time) = 0;
//	virtual void on_time(long cur_time) = 0;
	virtual void on_td_login(bool ready, const json& js, short source) = 0;
	/* on log */
//	virtual void debug(const char* msg) = 0;
	/* get name */
	virtual string get_name() const = 0;
	/* signal received */
	static volatile int signal_received;
	/* signal handler */
	static void signal_handler(int signum) { signal_received = signum; exit(signum); }
};

TE_NAMESPACE_END
#endif

