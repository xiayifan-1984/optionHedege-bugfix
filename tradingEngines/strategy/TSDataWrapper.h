#ifndef TRADINGENGINE_TSDATAWRAPPER_H
#define TRADINGENGINE_TSDATAWRAPPER_H

#include "../base/TE_DECLEAR.h"
#include "../../memoryManage/journal/JournalReader.h"
#include "IDataProcessor.h"

TE_NAMESPACE_START

using optionHedge::memoryManage::JournalReaderPtr;


class TradingStrategyUtil;

/** unknown, as default value */
#define CONNECT_TD_STATUS_UNKNOWN   0
/** td connect just added */
#define CONNECT_TD_STATUS_ADDED     1
/** td connect requested */
#define CONNECT_TD_STATUS_REQUESTED 2
/** got td ack msg, with valid pos */
#define CONNECT_TD_STATUS_ACK_POS   3
/** got td ack msg, no valid pos */
#define CONNECT_TD_STATUS_ACK_NONE  4
/** got td ack msg, set back */
#define CONNECT_TD_STATUS_SET_BACK  5

class TSDataWrapper
{
protected:
	/** setup reader and request td connect */
	void pre_run();
	/** process position */
	void process_rsp_position(const RspPositionField* pos, bool is_last, int request_id, short source, long rcv_time, short errorId = 0, const char* errorMsg = nullptr);
	/** process ack of td */
	void process_td_ack(const string& content, short source, long rcv_time);

public:
	TSDataWrapper(IDataProcessor* processor, TradingStrategyUtil* util);
	void disable_auto_sub_mode() { auto_sub_mode_enabled = false; };
	/** add market data */
	void add_market_data(short source);
	/** register td */
	void add_register_td(short source);

	void run();
	/** stop by changing force_stop flag */
	void stop();
	/** set current pos */
	byte get_td_status(short source) const;
	/** get effective orders */
	vector<int> get_effective_orders() const;

protected:
//	map<int, PosHandlerPtr> pos_handlers;
//	map<short, PosHandlerPtr> internal_pos;
	umap<string, double> last_prices;
	umap<int, char> order_statuses;
	vector<string>      folders;
	vector<string>      names;
	map<short, byte>    tds;
	JournalReaderPtr    reader;
	IDataProcessor*   processor;
	TradingStrategyUtil*     util;
	int rid_start;
	int rid_end;
	bool force_stop;
	long cur_time;
	bool auto_sub_mode_enabled;
};

DECLARE_PTR(TSDataWrapper);

TE_NAMESPACE_END
#endif
