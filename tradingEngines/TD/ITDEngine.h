#ifndef TRADINGENGINE_ITDENGINE_H
#define TRADINGENGINE_ITDENGINE_H

#include "../base/IEngine.h"
#include "../../memoryManage/utils/Timer.h"
#include "../../memoryManage/paged/PageCommStruct.h"
#include "../base/DataStruct.h"
#include "../base/TDUserStruct.h"
#include "../base/TDPosManage.h"
#include <dlfcn.h>
#include <boost/regex.hpp>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <mutex>

TE_NAMESPACE_START

struct ClientInfoUnit
{
	bool is_alive;
	int  journal_index;
	int  account_index;
	int  rid_start;
	int  rid_end;
	//	PosHandlerPtr pos_handler;
};

/**
 * Trade account information,
 * used when connect or login
 */
struct TradeAccount
{
	char BrokerID[19];
	char UserID[16];
	char InvestorID[19];
	char BusinessUnit[21];
	char Password[21];
	//	FeeHandlerPtr fee_handler;
};

class ITDEngine :public IEngine
{
protected:
	JournalWriterPtr send_writer;
	/** information about all existing clients. */
	map<string, ClientInfoUnit > clients; // clientName: (exist, idx)
	/** trade account */
	vector<TradeAccount> accounts;
	/** -1 if do not accept unregistered account, else return index of accounts */
	int default_account_index;
	/** default handler, will be utilized if user do not assign fee structure in account info */
//	FeeHandlerPtr default_fee_handler;
	/** trade engine user info helper */
	TDUserInfoHelperPtr user_helper;
	/** trade engine user info helper */
	TDEngineInfoHelperPtr td_helper;
	/** pos Manage for trade engine */
	TDPosManagePtr pos_manage;
	/** request_id, incremental*/
	int request_id;
	/** local id, incremental */
	int local_id;
	/** current nano time */
	long cur_time;
	/**CTP trader API handler */
	void* traderapiHandler;
	/**KingNew trader API handler */
	void* kntraderapiHandler;

	std::mutex mtx;

	std::unordered_map<std::string, int> directStratgeRid;
	std::unordered_map<std::string, int> parkedStratgeRid;

	// add strategy, return true if added successfully.
	bool add_client(const string& name, const json& j_request);
	// remove strategy
	bool remove_client(const string& name, const json& j_request);

	void initDirectstrategyRid(const std::string& configFile, short source);

	bool isDirectStrategy(string username, int rid);
protected:
	/** default constructor */
	ITDEngine(short source);
	/** default destructor */
	virtual ~ITDEngine();
	/** setup reader thread */
	virtual void set_reader_thread();
	/** keep listening to system journal */
	void listening();
	/** load config information */
	virtual void load(const json& j_config);
	/** init reader and writer */
	virtual void init();
	/** load TradeAccount from json */
	virtual TradeAccount load_account(int idx, const json& j_account);

public:
	// config and account related
	/** resize account number */
	virtual void resize_accounts(int account_num) = 0;
	/** connect each account and api with timeout as wait interval*/
	virtual void connect(long timeout_nsec) = 0;
	/** login all account and api with timeout as wait interval */
//	virtual void login(long timeout_nsec) = 0;
	/** send logout request for each api */
//	virtual void logout() = 0;
	/** release api items */
	virtual void release_api() = 0;
	/** is every accounts connected? */
	virtual bool is_connected() const = 0;
	/** is every accounts logged in? */
	virtual bool is_logged_in() const = 0;


	//public:
		/** some clean up when day switch */
	//	virtual void on_switch_day() {};
		/** on market data */
	//	virtual void on_market_data(const LFMarketDataField* data, long rcv_time) {};

		// virtual interfaces for all trade engines
		/** request investor's existing position */
	virtual void req_investor_position(const QryPositionField* data, int account_index, int requestId) = 0;
	/** request order position */
	virtual void req_qry_order(const QryOrderField* data, int account_index, int requestId) = 0;
	/** request trade position */
	virtual void req_qry_trade(const QryTradeField* data, int account_index, int requestId) = 0;
	/** request account info */
	virtual void req_qry_account(const QryAccountField* data, int account_index, int requestId) = 0;
	/** insert order */
	virtual void req_order_insert(const InputOrderField* data, int account_index, int requestId, long rcv_time) = 0;
	/** request order action (only cancel is accomplished) */
	virtual void req_order_action(const OrderActionField* data, int account_index, int requestId, long rcv_time) = 0;
	/** on investor position, engine (on_data) */
	void on_rsp_position(const RspPositionField* pos, bool isLast, int requestId, int errorId = 0, const char* errorMsg = nullptr);
	/** on qry order, engine (on_data) */
	void on_rsp_qry_order(const RtnOrderField* order, bool isLast, int requestId, int errorId = 0, const char* errorMsg = nullptr);
	/** on qry trade, engine (on_data) */
	void on_rsp_qry_trade(const RtnTradeField* trade, bool isLast, int requestId, int errorId = 0, const char* errorMsg = nullptr);
	/** on rsp order insert, engine (on_data) */
	void on_rsp_order_insert(const InputOrderField* order, int requestId, int errorId = 0, const char* errorMsg = nullptr);
	/** on rsp order action, engine (on_data) */
	void on_rsp_order_action(const RspActionField* action, int requestId, int errorId = 0, const char* errorMsg = nullptr);
	/** on rsp account info, engine (on_data) */
	void on_rsp_account(const RspAccountField* account, bool isLast, int requestId, int errorId = 0, const char* errorMsg = nullptr);
	/** on rtn order, engine (on_data) */
	void on_rtn_order(const RtnOrderField* rtn_order);
	/** on rtn trade, engine (on_data) */
	void on_rtn_trade(const RtnTradeField* rtn_trade);
	/** on rtn Instrument Status, engine (on_data) */
	void on_rtn_instrument_status(const RspInstrumentStatus* rtn_instatus, string username);
};

TE_NAMESPACE_END
#endif
