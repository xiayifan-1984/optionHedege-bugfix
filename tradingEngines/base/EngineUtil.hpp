#ifndef TRADINGENGINE_ENGINEUTIL_H
#define TRADINGENGINE_ENGINEUTIL_H

#include "TE_DECLEAR.h"
#include "DataStruct.h"
#include <string>
#include "../../memoryManage/utils/json.hpp"


#define TE_ENGINE_STATUS_IDLE           "Idle"
#define TE_ENGINE_STATUS_CONNECTING     "Connecting"
#define TE_ENGINE_STATUS_CONNECTED      "Connected"
#define TE_ENGINE_STATUS_CONNECT_FAIL   "ConnectFail"
#define TE_ENGINE_STATUS_LOGIN_SUCCESS  "LoginSuc"
#define TE_ENGINE_STATUS_LOGIN_FAIL     "LoginFail"
#define TE_ENGINE_STATUS_STOPPED        "Stopped"
#define TE_ENGINE_STATUS_STOP_FAIL      "StopFail"

//////////////////////////////////////////
/// (string) ConfigKey level 1
//////////////////////////////////////////
#define TE_CONFIG_KEY_MD "md"
#define TE_CONFIG_KEY_TD "td"
//////////////////////////////////////////
/// (string) ConfigKey level 2
//////////////////////////////////////////
#define TE_CONFIG_KEY_SOURCECTP "ctp"
//////////////////////////////////////////
/// (string) ConfigKey level 3
//////////////////////////////////////////
#define TE_CONFIG_KEY_PASSWORD          "Password"
#define TE_CONFIG_KEY_AUTH_CODE         "AuthCode"
#define TE_CONFIG_KEY_NEED_AUTH         "NeedAuth"
#define TE_CONFIG_KEY_NEED_SETTLE_CONFIRM "NeedSettleConfirm"
#define TE_CONFIG_KEY_BROKER_ID         "BrokerId"
#define TE_CONFIG_KEY_USER_ID           "UserId"
#define TE_CONFIG_KEY_EXCHANGE_ID       "ExchangeId"
#define TE_CONFIG_KEY_INVESTOR_ID       "InvestorId"
#define TE_CONFIG_KEY_APP_ID			"appID"
#define TE_CONFIG_KEY_FRONT_URI         "FrontUri"
#define TE_CONFIG_KEY_FRONT_URI_Q       "FrontUriQ"
#define TE_CONFIG_KEY_FRONT_URI_KN      "FrontUriKN"
#define TE_CONFIG_KEY_USER_PRODUCT_INFO "UserProductInfo"
#define TE_CONFIG_KEY_MCAST_ADDRESS     "McastAddress"
#define TE_CONFIG_KEY_NIC               "Nic"
#define TE_CONFIG_KEY_PROTOCOL_INFO     "ProtocolInfo"
#define TE_CONFIG_KEY_ACCOUNT_TYPE		"AccountType"

using json = nlohmann::json;

class EngineUtil
{
public:
//	static inline void splitTicker(const char* ticker, char* commodityNo, char* contractNo);
	static const char* gbkErrorMsg2utf8(const char* errorMsg);
//	static short getExchangeIdFromStockTicker(const char* ticker);
//	static string getExchangeNameFromStockTicker(const char* ticker);
};

inline std::string getExchangeName(short exchange_id)
{
	switch (exchange_id)
	{
	case 2: // 深圳证券交易所
		return "SZE";
	case 11: // 中国金融期货交易所
		return "CFFEX";
	case 1: // 上海证券交易所
		return "SSE";
	case 13: // 大连商品交易所
		return "DCE";
	case 12: // 上海期货交易所
		return "SHFE";
	case 14: // 郑州商品交易所
		return "CZCE";
	}
}

inline int getExchangeId(std::string exchange_name)
{
	if (exchange_name.compare("SZE") == 0 || exchange_name.compare("sze") == 0) //深圳证券交易所
		return 2;
	if (exchange_name.compare("CFFEX") == 0 || exchange_name.compare("cffex") == 0) //中国金融期货交易所
		return 11;
	if (exchange_name.compare("SSE") == 0 || exchange_name.compare("sse") == 0) //上海证券交易所
		return 1;
	if (exchange_name.compare("DCE") == 0 || exchange_name.compare("dce") == 0) //大连商品交易所
		return 13;
	if (exchange_name.compare("SHFE") == 0 || exchange_name.compare("shfe") == 0) //上海期货交易所
		return 12;
	if (exchange_name.compare("CZCE") == 0 || exchange_name.compare("czce") == 0) //郑州商品交易所
		return 14;
	return -1;
}

typedef std::pair<std::string, std::string> JournalPair;

inline JournalPair getMdJournalPair(short source)
{
	switch (source)
	{
	case 1:
		return { OPTIONHEDGE_MARKETDATA_FOLDER, "MD_CTP" };
	default:
		return { "", "" };
	}
}

inline JournalPair getTDJournalPair(short source)
{
	switch (source)
	{
	case 1:
		return { OPTIONHEDGE_TRADINGDATA_FOLDER,"TD_CTP" };
	default:
		return {"",""};
	}
}

inline JournalPair getTdRawJournalPair(short source)
{
	switch (source)
	{
	case 1:
		return { OPTIONHEDGE_TRADINGDATA_FOLDER, "TD_RAW_CTP" };
	default:
		return { "", "" };
	}
}

#endif
