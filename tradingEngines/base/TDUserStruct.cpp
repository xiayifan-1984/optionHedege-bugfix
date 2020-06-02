#include "TDUserStruct.h"
#include "../../memoryManage/journal/PageUtil.h"
#include "../../memoryManage/utils/Timer.h"
#include <sstream>
#include <iostream>

USING_TE_NAMESPACE
USING_MM_NAMESPACE


TDUserInfoHelper::TDUserInfoHelper(short source) :source(source)
{
	address_book.clear();
}

TDUserInfoHelper::TDUserInfoHelper(short source, const string&user_name) : TDUserInfoHelper(source)
{
	load(user_name, false);
}

void TDUserInfoHelper::load(const string& user_name, bool need_write)
{
	if (address_book.find(user_name) == address_book.end())
	{
		std::stringstream ss;
		ss << TD_USER_INFO_FOLDER << user_name << "." << source;
		TDUserInfo* address=(TDUserInfo*)PageUtil::LoadPageBuffer(ss.str(), USER_INFO_SIZE, need_write, true);
		if (need_write)
		{
			address->status = TD_INFO_WRITING;
			address->start_time = getNanoTime();
			strncpy(address->name, user_name.c_str(), JOURNAL_SHORT_NAME_MAX_LENGTH);
		}
		address_book[user_name] = address;
	}
}

void TDUserInfoHelper::remove(const string&user_name)
{
	if (address_book.find(user_name) != address_book.end())
	{
		TDUserInfo* address = address_book[user_name];
		clean_up(address);
		address->end_time = getNanoTime();
		address->status = TD_INFO_NORMAL;
		address_book.erase(user_name);
		PageUtil::ReleasePageBuffer(address, USER_INFO_SIZE, true);
	}
}

TDUserInfoHelper::~TDUserInfoHelper()
{
	for (auto iter : address_book)
	{
		TDUserInfo* address = iter.second;
		address->end_time = getNanoTime();
		address->status = TD_INFO_FORCE_QUIT;
		PageUtil::ReleasePageBuffer(address, USER_INFO_SIZE, true);
	}
}

void TDUserInfoHelper::switch_day()
{
	for (auto iter : address_book)
	{
		clean_up(iter.second);
	}
}

void TDUserInfoHelper::clean_up(TDUserInfo* info)
{
	info->last_order_index = -1;
	memset(info->orders, '\0', AVAILABLE_ORDER_LIMIT * sizeof(TDOrderInfo));
}

TDOrderInfo* TDUserInfoHelper::locate_readable(const string& user_name, int order_id) const
{
	TDUserInfo* info = get_user_info(user_name);
	if (info == nullptr)
	{
		return nullptr;
	}

	int count = 0;
	//still search the whole order array, just start from idx have better chance to hit
	while (info->orders[count].order_id != order_id && count < AVAILABLE_ORDER_LIMIT)
	{
		count++;
	}

	if (count == AVAILABLE_ORDER_LIMIT)
	{
		return nullptr;
	}
	else
	{
		return &(info->orders[count]);
	}
}

inline bool order_is_existing(char status)
{
	return status != ORDER_INFO_RAW
		&& status != CHAR_Error
		&& status != CHAR_AllTraded
		&& status != CHAR_Canceled
		&& status != CHAR_PartTradedNotQueueing;
}

TDOrderInfo* TDUserInfoHelper::locate_writable(const string& user_name, int order_id)
{
	TDUserInfo* info = get_user_info(user_name);
	if (info == nullptr)
	{
		return nullptr;
	}

	int idx = order_id % AVAILABLE_ORDER_LIMIT;
	int count = 0;
	char status = info->orders[idx].status;

	while (order_is_existing(status) && count < AVAILABLE_ORDER_LIMIT)
	{
		idx = (idx + 1) % AVAILABLE_ORDER_LIMIT;
		status = info->orders[idx].status;
		count++;
	}

	if (count == AVAILABLE_ORDER_LIMIT)
	{
		return nullptr;
	}

	else
	{
		if (idx > info->last_order_index)
		{
			info->last_order_index = idx;
		}
		return &(info->orders[idx]);
	}
}

void TDUserInfoHelper::record_order(const string&user_name,char* local_id, int order_id, const char* ticker)
{
	TDOrderInfo* order_info = locate_writable(user_name, order_id);
	if (order_info != nullptr)
	{
		memcpy(order_info->local_id,local_id,13);
		order_info->order_id = order_id;
		order_info->status = CHAR_OrderInserted;
		strncpy(order_info->ticker, ticker, ORDER_INFO_TICKER_LIMIT);
	}
}

/*
bool TDUserInfoHelper::get_order(const string&user_name, string order_id, string& local_id, char* ticker) const
{
	TDOrderInfo* order_info = locate_readable(user_name, order_id);
	if (order_info != nullptr)
	{
		local_id = order_info->local_id;
		strncpy(ticker, order_info->ticker, ORDER_INFO_TICKER_LIMIT);
		return true;
	}
	else
	{
		return false;
	}
}
*/
/*
json TDUserInfoHelper::get_pos(const string&user_name) const
{
	json pos;
	auto info = get_user_info(user_name);

	if (info == nullptr || info->pos_str[0] != '{')
	{
		pos["ok"] = false;
	}
	else
	{
		pos = json::parse(info->pos_str);
	}
	pos["name"] = user_name;
	return pos;
}

void TDUserInfoHelper::set_pos(const string& user_name, const json& pos)
{
	json j_pos = pos;
	j_pos["nano"] = optionHedge::memoryManage::getNanoTime();
	j_pos["name"] = user_name;
	load(user_name);
	TDUserInfo* info = address_book[user_name];
	strcpy(info->pos_str, j_pos.dump().c_str());
}
*/

void TDUserInfoHelper::set_order_status(const string& user_name, int order_id, char status)
{
	TDOrderInfo* order_info = locate_readable(user_name, order_id);
	if (order_info != nullptr)
	{
		order_info->status = status;
	}
}

vector<int> TDUserInfoHelper::get_existing_orders(const string&user_name) const
{
	vector<int> order_ids;
	auto info = get_user_info(user_name);
	if (info != nullptr)
	{
		for (int idx = 0; idx < info->last_order_index; ++idx)
		{
			if(order_is_existing(info->orders[idx].status))
			{
				order_ids.push_back(info->orders[idx].order_id);
			}
		}
	}
	return order_ids;
}

const TDUserInfo* TDUserInfoHelper::get_const_user_info(const string& user_name) const
{
	return get_user_info(user_name);
}

// TDEngineInfoHelper 


TDEngineInfoHelper::TDEngineInfoHelper(short source, const string&name)
{
	std::stringstream ss;
	ss << TD_USER_INFO_FOLDER << name;
	info = (TDEngineInfo*)PageUtil::LoadPageBuffer(ss.str(), TD_ENGINE_INFO_SIZE, true, true);
	if (info->status != TD_INFO_NORMAL)
	{
		memset(info, '\0', TD_ENGINE_INFO_SIZE);
		info->version = TD_ENGINE_INFO_VERSION;
		info->source = source;
	}
	info->status = TD_INFO_WRITING;
	info->start_time = getNanoTime();
	info->end_time = -1;
	cleanup();
}

TDEngineInfoHelper::~TDEngineInfoHelper()
{
	info->end_time = getNanoTime();
	info->status = TD_INFO_NORMAL;
	PageUtil::ReleasePageBuffer(info, USER_INFO_SIZE, true);
}

TDBasicOrderInfo* TDEngineInfoHelper::locate_readable(const char* local_id,int type) const
{
	if (type == OrderID)
	{
		int count = 0;
		while (std::strcmp(info->orders[count].local_id, local_id) != 0 && count < TD_AVAILABLE_ORDER_LIMIT)
		{
			count++;
		}
		if (count == TD_AVAILABLE_ORDER_LIMIT)
		{
			return nullptr;
		}
		else
		{
			return &(info->orders[count]);
		}
	}

	if (type == OrderSysId)
	{
		int count = 0;
		while (std::strcmp(info->orders[count].ordersys_id, local_id) != 0 && count < TD_AVAILABLE_ORDER_LIMIT)
		{
			count++;
		}
		if (count == TD_AVAILABLE_ORDER_LIMIT)
		{
			return nullptr;
		}
		else
		{
			return &(info->orders[count]);
		}
	}
	
}

TDBasicOrderInfo* TDEngineInfoHelper::locate_writable()
{
	int count = 0;
	char status = info->orders[count].status;
	while (order_is_existing(status) && count < TD_AVAILABLE_ORDER_LIMIT)
	{
		count++;
		status = info->orders[count].status;
	}
	if (count == TD_AVAILABLE_ORDER_LIMIT)
		return nullptr;
	else
	{
		if (count > info->last_local_index)
			info->last_local_index = count;
		return &(info->orders[count]);
	}
}

void TDEngineInfoHelper::switch_day()
{
	cleanup();
}

void TDEngineInfoHelper::cleanup()
{
	info->last_local_index = -1;
	memset(info->orders, '\0', TD_AVAILABLE_ORDER_LIMIT * sizeof(TDBasicOrderInfo));
}

void TDEngineInfoHelper::set_order_status(const char* local_id, char status,int keytype)
{
	TDBasicOrderInfo* order_info = locate_readable(local_id, keytype);
	if (order_info != nullptr)
	{
		order_info->status = status;
		serilizeOrderInfo(order_info);
	}
	
}

/*
void TDEngineInfoHelper::set_order_unlocked(const char* local_id)
{
	TDBasicOrderInfo* order_info = locate_readable(local_id);
	if (order_info != nullptr)
	{
		order_info->bunlocked = true;
		serilizeOrderInfo(order_info);
	}

}
*/
//get rid by the strategy
int TDEngineInfoHelper::get_order_id(const char* local_id,int type)
{
	TDBasicOrderInfo* order = locate_readable(local_id,type);
	if (order != nullptr)
	{
		return order->order_id;
	}
	else
	{
		return -1;
	}
}

void TDEngineInfoHelper::record_order(const char* local_id, int order_id, size_t length)
{
	TDBasicOrderInfo* order = locate_writable();
	if (order != nullptr)
	{
		order->order_id = order_id;
		memcpy(order->local_id,local_id,length);
		order->status = CHAR_OrderInserted;
		serilizeOrderInfo(order);
	}

}

void TDEngineInfoHelper::record_order(const char* local_id, int order_id, OrderStatusType status,size_t length)
{
	TDBasicOrderInfo* order = locate_writable();
	if (order != nullptr)
	{
		order->order_id = order_id;
		memcpy(order->local_id, local_id, length);
		order->status = status;
		serilizeOrderInfo(order);
	}
}

//only record ordersysID when the orderRef+frontID+sessionID can be find 
void TDEngineInfoHelper::record_order_orderSysID(const char* local_id, const char* orderSys_id, size_t length)
{
	TDBasicOrderInfo* order = locate_readable(local_id, OrderID);
	if (order != nullptr)
	{
		memcpy(order->ordersys_id, orderSys_id, length);
		serilizeOrderInfo(order);
	}
}

TDBasicOrderInfo* TDEngineInfoHelper::get_order_info(const char* local_id,int keytype)
{
	TDBasicOrderInfo* order = locate_readable(local_id, keytype);
	return order;
}

void TDEngineInfoHelper::set_trade_date(char* tradedate)
{
	tradeDate = string(tradedate);
}

void TDEngineInfoHelper::serilizeOrderInfo(TDBasicOrderInfo* order)
{
	if (order->local_id != "")
	{
		string key = "TDEngine" + tradeDate + "_" + order->local_id;
		RedisTool rls;
		auto res = rls.setBits(key, (char*)order, sizeof(*order));
	}
}

void TDEngineInfoHelper::deserilizeOrderInfo()
{
	RedisTool rls;
	string keypattern = "TDEngine" + tradeDate + "_*";
	auto rkey = rls.getKeysWithPattern(keypattern);

	for (auto key : rkey)
	{

		TDBasicOrderInfo tmp;
		auto rres = rls.getBitsAsStruct(key, tmp);
		if (get_order_id(tmp.local_id) == -1)
		{
			TDBasicOrderInfo* order = locate_writable();
			memcpy(order->local_id, &tmp.local_id, 13);
			order->order_id = tmp.order_id;
			order->status = tmp.status;
			std::cout << "Deserilize order info with localId " << order->local_id << " order id " << order->order_id << " status " << order->status <<  std::endl;
		}
	}
}
