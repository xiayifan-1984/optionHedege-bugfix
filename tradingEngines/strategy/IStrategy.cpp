#include "IStrategy.h"
#include <csignal>
#include "../base/EngineUtil.hpp"
#include <fstream>

USING_TE_NAMESPACE

#define CONFIGURATION_JSON_FILE OPTIONHEDGE_ROOT_DIR "configFiles/tradingConfig.json"
#define SOURCE_INDEX SOURCE_CTP

void setup_signal_callback()
{
	std::signal(SIGTERM, IDataProcessor::signal_handler);
	std::signal(SIGINT, IDataProcessor::signal_handler);
	std::signal(SIGHUP, IDataProcessor::signal_handler);
	std::signal(SIGQUIT, IDataProcessor::signal_handler);
	std::signal(SIGKILL, IDataProcessor::signal_handler);
}

IStrategy::IStrategy(const string& strategyname):name(strategyname), userId(""), investorId(""), brokerId(""), tradedate("")
{
	logger = optionHedge::memoryManage::Log::getLogger("strategy");
	util = TradingStrategyUtilPtr(new TradingStrategyUtil(strategyname));
	data = TSDataWrapperPtr(new TSDataWrapper(this, util.get()));
	parsingAccountInfo(CONFIGURATION_JSON_FILE, SOURCE_INDEX);

	std::cout << "userID is " << userId << std::endl;
	std::cout << "investorId is " << investorId << std::endl;
	std::cout << "brokerId is " << brokerId << std::endl;
	setup_signal_callback();
}

void IStrategy::start()
{
	data_thread = ThreadPtr(new std::thread(&TSDataWrapper::run, data.get()));
	OPTIONHEDGE_LOG_INFO(logger, "[start] data started");
}

void IStrategy::parsingAccountInfo(const std::string& configFile, short source)
{
	std::ifstream ifs(configFile);
	json j_config = json::parse(ifs);
	if (source == SOURCE_CTP)
	{
		auto iter = j_config[TE_CONFIG_KEY_TD][TE_CONFIG_KEY_SOURCECTP].find("accounts");
		bool bfound = false;
		std::string default_broker_id = "";
		std::string default_user_id = "";
		std::string default_investor_id = "";
		

		if (iter != j_config[TE_CONFIG_KEY_TD][TE_CONFIG_KEY_SOURCECTP].end())
		{
			int account_num = iter.value().size();
			OPTIONHEDGE_LOG_INFO(logger, "[account] number" << account_num);

			for (auto& j_account : iter.value())
			{
				const json& j_info = j_account["info"];
				const json& j_clients = j_account["clients"];
				const json& j_isdefault = j_account["is_default"];

				if (j_isdefault.get<bool>())
				{
					default_broker_id = j_info[TE_CONFIG_KEY_BROKER_ID];
					default_user_id = j_info[TE_CONFIG_KEY_USER_ID];
					default_investor_id = j_info[TE_CONFIG_KEY_INVESTOR_ID];
				}
				for (auto& j_client : j_clients)
				{
					if (name == j_client.get<string>())
					{
						userId = j_info[TE_CONFIG_KEY_USER_ID];
						investorId = j_info[TE_CONFIG_KEY_INVESTOR_ID];
						brokerId = j_info[TE_CONFIG_KEY_BROKER_ID];
						bfound = true;
					}
				}
			}
		}

		if (!bfound)
		{
			userId = default_user_id;
			investorId = default_investor_id;
			brokerId = default_broker_id;
		}
	}	
}

IStrategy::~IStrategy()
{
	OPTIONHEDGE_LOG_INFO(logger, "[~IWCStrategy]");
	data.reset();
	data_thread.reset();
	logger.reset();
	util.reset();
}

void IStrategy::terminate()
{
	stop();
	if (data_thread.get() != nullptr)
	{
		data_thread->join();
		data_thread.reset();
	}
	OPTIONHEDGE_LOG_INFO(logger, "[terminate] terminated!");
}

void IStrategy::stop()
{
	if (data.get() != nullptr)
	{
		data->stop();
	}
}

void IStrategy::run()
{
	data->run();
}

void IStrategy::block()
{
	if (data_thread.get() != nullptr)
	{
		data_thread->join();
	}
}

void IStrategy::on_market_data(const MarketDataField* data, short source, long rcv_time)
{
	OPTIONHEDGE_LOG_DEBUG(logger, "[market_data] (source)" << source << " (ticker)" << data->InstrumentID << " (lp)" << data->LastPrice);
}

void IStrategy::on_rtn_order(const RtnOrderField* data, int request_id, short source, long rcv_time)
{
	OPTIONHEDGE_LOG_DEBUG(logger, "[rtn_order] (source)" << source << " (rid)" << request_id << " (ticker)" << data->InstrumentID << " (status)" << data->OrderStatus);
}

void IStrategy::on_rtn_trade(const RtnTradeField* data, int request_id, short source, long rcv_time)
{
	OPTIONHEDGE_LOG_DEBUG(logger, "[rtn_trade] (source)" << source << " (rid)" << request_id << " (ticker)" << data->InstrumentID << " (p)" << data->Price << " (v)" << data->Volume);
}

void IStrategy::on_rtn_instrument_status(const RspInstrumentStatus* data, int request_id, short source, long rcv_time)
{
	OPTIONHEDGE_LOG_DEBUG(logger, "[rtn_instrument_status] (source)" << source << " (rid)" << request_id <<"(ExchangeID)"<<data->ExchangeID<<" (ticker)" << data->InstrumentID << " (status) "<< data->InstrumentStatus);
}

void IStrategy::on_rsp_order(const InputOrderField* data, int request_id, short source, long rcv_time, short errorId, const char* errorMsg)
{
	if (errorId == 0)
		OPTIONHEDGE_LOG_DEBUG(logger, "[rsp_order] (source)" << source << " (rid)" << request_id << " (ticker)" << data->InstrumentID << " (p)" << data->LimitPrice << " (v)" << data->Volume);
	else
		OPTIONHEDGE_LOG_ERROR(logger, "[rsp_order] (source)" << source << " (rid)" << request_id << " (ticker)" << data->InstrumentID << " (p)" << data->LimitPrice << " (v)" << data->Volume << " (errId)" << errorId << " (errMsg)" << errorMsg);
}

void IStrategy::on_rsp_account(const RspAccountField * data, int request_id, short source, long rcv_time,bool isLast,short errorId, const char* errorMsg)
{
	OPTIONHEDGE_LOG_DEBUG(logger, "[rsp_account] (source)" << source << " (rid)" << request_id << " (Available)" << data->Available);
}

void IStrategy::on_rsp_qry_order(const RtnOrderField* data, int request_id, short source, long rcv_time, bool isLast, short errorId, const char* errorMsg)
{
	if (errorId == 0)
		OPTIONHEDGE_LOG_DEBUG(logger, "[rsp_qry_order] (source)" << source << " (rid)" << request_id << " (ticker)" << data->InstrumentID);
	else
		OPTIONHEDGE_LOG_ERROR(logger, "[rsp_qry_order] (source)" << source << " (rid)" << request_id << " (ticker)" << data->InstrumentID << " (errId)" << errorId << " (errMsg)" << errorMsg);
}

void IStrategy::on_rsp_qry_trade(const RtnTradeField* data, int request_id, short source, long rcv_time, bool isLast, short errorId, const char* errorMsg)
{
	if (errorId == 0)
		OPTIONHEDGE_LOG_DEBUG(logger, "[rsp_qry_trade] (source)" << source << " (rid)" << request_id << " (ticker)" << data->InstrumentID);
	else
		OPTIONHEDGE_LOG_DEBUG(logger, "[rsp_qry_trade] (source)" << source << " (rid)" << request_id << " (ticker)" << data->InstrumentID << " (errId)" << errorId << " (errMsg)" << errorMsg);
}

void IStrategy::on_rsp_position(const RspPositionField* pos, int request_id, short source, long rcv_time, bool isLast, short errorId, const char* errorMsg)
{
	OPTIONHEDGE_LOG_DEBUG(logger, "[rsp_position] (source)" << source << " (rid)" << request_id << " (ticker)" << pos->InstrumentID);
}

void IStrategy::on_rsp_action(const RspActionField* data, int request_id, short source, long rcv_time)
{
	OPTIONHEDGE_LOG_DEBUG(logger, "[rsp_position] (source)" << source << " (rid)" << request_id << " (statusMsg)" << data->StatusMsg);
}

void IStrategy::on_td_login(bool ready, const json& js, short source)
{
	tradedate = js["tradedate"].get<string>();
	OPTIONHEDGE_LOG_DEBUG(logger, "[td_login] (source)" << source << " (ready)" << ready << " (with json content)" << js.dump());
}

bool IStrategy::td_is_ready(short source) const
{
	if (data.get() != nullptr)
	{
		byte status = data->get_td_status(source);
		if (status == CONNECT_TD_STATUS_ACK_POS|| status == CONNECT_TD_STATUS_SET_BACK)
			return true;
	}
	return false;
}

bool IStrategy::td_is_connected(short source) const
{
	if (data.get() != nullptr)
	{
		byte status = data->get_td_status(source);
		if (status == CONNECT_TD_STATUS_ACK_POS
			|| status == CONNECT_TD_STATUS_SET_BACK
			|| status == CONNECT_TD_STATUS_ACK_NONE)
			return true;
	}
	return false;
}

#define CHECK_TD_READY(source) \
    if (!td_is_ready(source)) \
    {\
        if (!td_is_connected(source))\
            OPTIONHEDGE_LOG_ERROR(logger, "td (" << source << ") is not connected. please check TD or yjj status"); \
        else \
            OPTIONHEDGE_LOG_ERROR(logger, "td (" << source << ") holds no position here. please init current strategy's position before insert an order"); \
        return -1;\
    }

#define CHECK_EXCHANGE_AND_OFFSET(exchange_name, offset) \
    {\
        short eid = getExchangeId(exchange_name);\
        if (eid < 0){\
            OPTIONHEDGE_LOG_ERROR(logger, "unrecognized exchange name: " << exchange_name); \
            return -1;\
        }\
        if (eid == EXCHANGE_ID_CFFEX || eid == EXCHANGE_ID_DCE || eid == EXCHANGE_ID_CZCE){\
            if (offset == CHAR_CloseToday || offset == CHAR_CloseYesterday){\
                OPTIONHEDGE_LOG_DEBUG(logger, "CFFEX/DCE/CZCE don't support CloseToday or CloseYesterday, auto revised to default Close");\
                offset = CHAR_Close;\
            }\
        }\
        if (eid == EXCHANGE_ID_SSE || eid == EXCHANGE_ID_SZE) {\
            if (offset != CHAR_Open) {\
                OPTIONHEDGE_LOG_DEBUG(logger, "stock don't need to specify offset, default is open");\
                offset = CHAR_Open;\
            }\
        }\
    }

int IStrategy::insert_market_order(short source, string instrument_id, string exchange_id, int volume, DirectionType direction, OffsetFlagType offset)
{
	CHECK_TD_READY(source);
	CHECK_EXCHANGE_AND_OFFSET(exchange_id, offset);
	util->insert_market_order(source, instrument_id, exchange_id, volume, direction, offset);
}

int IStrategy::insert_limit_order(short source, string instrument_id, string exchange_id, double price, int volume, DirectionType direction, OffsetFlagType offset)
{
	CHECK_TD_READY(source);
	CHECK_EXCHANGE_AND_OFFSET(exchange_id, offset);
	return util->insert_limit_order(source, instrument_id, exchange_id, price, volume, direction, offset);
}

int IStrategy::insert_fok_order(short source, string instrument_id, string exchange_id, double price, int volume, DirectionType direction, OffsetFlagType offset)
{
	CHECK_TD_READY(source);
	CHECK_EXCHANGE_AND_OFFSET(exchange_id, offset);
	return util->insert_fok_order(source, instrument_id, exchange_id, price, volume, direction, offset);
}

int IStrategy::insert_fak_order(short source, string instrument_id, string exchange_id, double price, int volume, DirectionType direction, OffsetFlagType offset)
{
	CHECK_TD_READY(source);
	CHECK_EXCHANGE_AND_OFFSET(exchange_id, offset);
	return util->insert_fak_order(source, instrument_id, exchange_id, price, volume, direction, offset);
}

int IStrategy::req_position(short source)
{
	if (!td_is_connected(source))
	{
		OPTIONHEDGE_LOG_ERROR(logger, "td (" << source << ") connection is failed. please check TD or yjj status");
		return -1;
	}
	return util->req_position(source);
}

int IStrategy::cancel_order(short source, string orderRef, string instrumentID, string exchangeID, string orderSysID)
{
	CHECK_TD_READY(source);
	return util->cancel_order(source, orderRef, instrumentID,exchangeID, orderSysID);
}
