#ifndef TRADINGENGINE_TDUSERSTRUCT_H
#define TRADINGENGINE_TDUSERSTRUCT_H

#include "TE_DECLEAR.h"
#include "TradingConstant.h"
#include "../../memoryManage/utils/constants.h"
#include "../../memoryManage/utils/memoryManage_declare.h"
#include "../../memoryManage/utils/json.hpp"
#include "../../memoryManage/utils/redistool.hpp"

using json = nlohmann::json;

TE_NAMESPACE_START

#define ORDER_INFO_RAW  '\0'
#define ORDER_INFO_TICKER_LIMIT 30

struct TDOrderInfo
{
	/** rid of the order */
	int order_id;
	/** rid of by the TDEngine */
	char local_id[13];
	char status;
	/** Instrument ID */
	char ticker[ORDER_INFO_TICKER_LIMIT];
};

#define AVAILABLE_ORDER_LIMIT  10000
#define POS_JSON_STR_LENGTH  100000

#define TD_INFO_NORMAL        'n'
#define TD_INFO_WRITING       'w'
#define TD_INFO_FORCE_QUIT    'e'

struct TDUserInfo
{
	char status;
	long start_time;
	long end_time;
	int last_order_index;
	/** strategy name */
	char name[JOURNAL_SHORT_NAME_MAX_LENGTH];
	/** folder name */
	char folder[JOURNAL_FOLDER_MAX_LENGTH];
	/** strategy-wise position */
	char pos_str[POS_JSON_STR_LENGTH];
	/** order info hash-map base */
	TDOrderInfo orders[AVAILABLE_ORDER_LIMIT];
}__attribute__((packed));

#define TD_USER_INFO_FOLDER OPTIONHEDGE_JOURNAL_FOLDER "user/" /**< where we put user info files */
/** user info file length is determined */
const int USER_INFO_SIZE = sizeof(TDUserInfo) + 1024;

class TDUserInfoHelper
{
	friend class ITDEngine;
	friend bool set_pos_tool(const string& user_name, short source, const string& file_name, bool is_csv);

private:
	map<string, TDUserInfo*> address_book;
	short source;

protected:
	/** internal locate at readable order with order_id, return nullptr if no available */
	TDOrderInfo* locate_readable(const string& user_name, int order_id) const;
	/** internal locate at writable order position with order_id, return nullptr if no available */
	TDOrderInfo* locate_writable(const string& user_name, int order_id);
	/** constructor with "write" authority */
	TDUserInfoHelper(short source);
	/** load user info of user_name */
	void load(const string& user_name, bool need_write = true);
	/** remove user from list, detach memory */
	void remove(const string& user_name);
	/** necessary cleanup when to a trading day is over */
	void switch_day();
	/** record order info */
	void record_order(const string& user_name,char* local_id, int order_id, const char* ticker);
	/** set position json string */
//	void set_pos(const string& user_name, const json& pos);
	/** clean up user info */
	void clean_up(TDUserInfo* info);
	/** get info block address for internal usage only */
	inline TDUserInfo* get_user_info(const string& user_name) const
	{
		auto iter = address_book.find(user_name);
		if (iter == address_book.end())
			return nullptr;
		return iter->second;
	}

public:
	/** constructor with read-ability only */
	TDUserInfoHelper(short source, const string& user_name);
	/** get const user info */
	const TDUserInfo* get_const_user_info(const string& user_name) const;
	/** default destructor */
	~TDUserInfoHelper();
	/** get order info, return true if local_id and ticker are set properly */
	bool get_order(const string& user_name, int order_id, string &local_id, char* ticker) const;
	/** set order status when updated by return-order */
	void set_order_status(const string& user_name, int order_id, char status);
	/** get position json */
	json get_pos(const string& user_name) const;
	/** get existing orders of this user */
	vector<int> get_existing_orders(const string& user_name) const;
};

DECLARE_PTR(TDUserInfoHelper);

/** for whole td engine,
 * how many orders can be supported.
 * available order */
#define TD_AVAILABLE_ORDER_LIMIT  100000

#define OrderIDtoRID 1
#define OrderIDtoOrderSysID 2
#define OrderSysIDtoOrderID 3

#define OrderID 1
#define OrderSysId 3

struct TDBasicOrderInfo
{
	TDBasicOrderInfo()
	{
		order_id = -2;
		memset(local_id, '\0', 128);
		memset(ordersys_id, '\0', 128);
		status = ORDER_INFO_RAW;
	}
	/** request id of the order */
	int  order_id;
	/** assigned by TradeEngine */
	char  local_id[128];
	/** assigned when order get accepted */
	char ordersys_id[128];
	/** OrderStatusType, ORDER_INFO_RAW means nothing */
	char status;
} __attribute__((packed));

/** VERSION */
#define TD_ENGINE_INFO_VERSION 1

#define TD_ENGINE_INFO_LOCK        0
#define TD_ENGINE_INFO_UNLOCK      1


struct TDEngineInfo
{
	/** TD_INFO_NORMAL / TD_INFO_WRITING / TD_INFO_FORCE_QUIT, others raw */
	char status;
	/** version */
	short version;
	/** start time */
	long start_time;
	/** last end time */
	long end_time;
	/** last order local id */
	int  last_local_index;
	/** source id name */
	short source;
	/** orders by local_id */
	TDBasicOrderInfo orders[TD_AVAILABLE_ORDER_LIMIT];
	
} __attribute__((packed));

const int TD_ENGINE_INFO_SIZE = sizeof(TDEngineInfo) + 1024;


class TDEngineInfoHelper
{
	/** all method that may modify data must be private
	 * to make it modifiable by ITDEngine only */
	friend class ITDEngine;

private:
	TDEngineInfo* info;
	string tradeDate;
protected:
	TDBasicOrderInfo* locate_readable(const char* local_id,int key=OrderID) const;
	TDBasicOrderInfo* locate_writable();
	void cleanup();

protected:
	TDEngineInfoHelper(short source, const string& name);

public:
	/** default destructor */
	~TDEngineInfoHelper();
	/** necessary cleanup when to a trading day is over */
	void switch_day();
	/** set order status when updated by return-order */
	void set_order_status(const char* local_id, char status,int keytype= OrderID);
	/** set order unlocked when updated by return-order */
//	void set_order_unlocked(const char* local_id);
	/** get order_id */
	int get_order_id(const char* local_id,int type=OrderIDtoRID);
	/** insert order info */
	void record_order(const char* local_id, int order_id,size_t length);
	void record_order(const char* local_id, int order_id, OrderStatusType status, size_t length);

	/*using orderRef to record orderSysid*/
	void record_order_orderSysID(const char* local_id,const char* orderSys_id, size_t length);
	
	TDBasicOrderInfo* get_order_info(const char* local_id,int keytype=OrderID);
	void set_trade_date(char* tradedate);
	string get_trade_date() { return tradeDate; }
	void serilizeOrderInfo(TDBasicOrderInfo* order);
	void deserilizeOrderInfo();
};

DECLARE_PTR(TDEngineInfoHelper);
TE_NAMESPACE_END
#endif
