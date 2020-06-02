#include "ITDEngine.h"
#include "../../memoryManage/paged/PageCommStruct.h"
#include "../../memoryManage/utils/Timer.h"
#include "../base/EngineUtil.hpp"
#include "../base/TradingConstant.h"
#include "../base/TDUserStruct.h"
#include "../../system/systemConfig/sys_messages.h"
#include <algorithm>
#include <boost/bind.hpp>

USING_TE_NAMESPACE

#define TRADER_API_PATH OPTIONHEDGE_ROOT_DIR "lib64/api/thosttraderapi_se.so"
#define KNTRADER_API_PATH OPTIONHEDGE_ROOT_DIR "lib64/api/knthosttraderapi_se.so"

class RidClientManager
{
private:
	/** rid, strategy_name map */
	map<int, string> rid2client;
public:
	inline void set(int rid_start, string name)
	{
		rid2client[rid_start / REQUEST_ID_RANGE] = name;
	}
	inline string get(int rid)
	{
		auto iter = rid2client.find(rid / REQUEST_ID_RANGE);
		if (iter == rid2client.end())
			return "";
		else
			return iter->second;
	}
};

/** manage rid and client_name matching */
RidClientManager rid_manager;

ITDEngine::ITDEngine(short source) :IEngine(source), default_account_index(-1), request_id(1), local_id(1)
{
	traderapiHandler = dlopen(TRADER_API_PATH, RTLD_LAZY | RTLD_GLOBAL);

	if (!traderapiHandler)
	{
		std::runtime_error("Can not create traderapiHandler");
	}
	kntraderapiHandler = dlopen(KNTRADER_API_PATH, RTLD_LAZY | RTLD_GLOBAL);

	if (!kntraderapiHandler)
	{
		std::runtime_error("Can not create kntraderapiHandler");
	}

	initDirectstrategyRid(CONFIGURATION_JSON_FILE, SOURCE_INDEX);
}

void ITDEngine::initDirectstrategyRid(const std::string& configFile, short source)
{
	std::ifstream ifs(configFile);
	json j_config = json::parse(ifs);
	unordered_map<string, int> strategyRid;
	unordered_map<string, int> parkedRid;

	string directStrategyPattern = "^[Dd]irect";
	boost::regex pattern(directStrategyPattern);

	string parkedStrategyPattern = "^[Pp]arked";
	boost::regex parkedpattern(parkedStrategyPattern);

	if (source == SOURCE_CTP)
	{
		auto iter = j_config[TE_CONFIG_KEY_TD][TE_CONFIG_KEY_SOURCECTP].find("StrategyRange");

		if (iter != j_config[TE_CONFIG_KEY_TD][TE_CONFIG_KEY_SOURCECTP].end())
		{
			int strategy_num = iter.value().size();
			cout << "Strategy num is " << strategy_num << endl;

			for (auto& j_strategyRange : iter.value())
			{
				const json& j_name = j_strategyRange["name"];
				const json& j_rid_start = j_strategyRange["rid_start"];
				string strategyname = j_name.get<string>();

				if (boost::regex_search(strategyname.begin(), strategyname.end(), pattern))
				{
					strategyRid[strategyname] = j_rid_start.get<int>();
				}
				else if (boost::regex_search(strategyname.begin(), strategyname.end(), parkedpattern))
				{
					parkedRid[strategyname]= j_rid_start.get<int>();
				}
			}
		}

		iter = j_config[TE_CONFIG_KEY_TD][TE_CONFIG_KEY_SOURCECTP].find("accounts");

		int accountnum;
		if (iter != j_config[TE_CONFIG_KEY_TD][TE_CONFIG_KEY_SOURCECTP].end())
		{
			accountnum = iter.value().size();
			for (auto& j_account : iter.value())
			{
				const json& j_info = j_account["info"];
				const json& j_clients = j_account["clients"];

				string userId = j_info[TE_CONFIG_KEY_USER_ID];
				for (auto& j_client : j_clients)
				{
					string strategyName = j_client.get<string>();
					auto it = strategyRid.find(strategyName);
					auto pit = parkedRid.find(strategyName);
					if (it != strategyRid.end())
					{
						directStratgeRid[userId] = it->second;
					}
					
					if (pit != parkedRid.end())
					{
						parkedStratgeRid[userId] = pit->second;
					}

				}
			}
		}

		if (accountnum != directStratgeRid.size())
		{
			string errorMsg = "Do not find direct strategy for all the users, currently find direct strategy for below users: ";
			for (auto d : directStratgeRid)
			{
				errorMsg += d.first + " ";
			}
			throw std::runtime_error(errorMsg.c_str());
		}
	}
}

ITDEngine::~ITDEngine()
{
	dlclose(traderapiHandler);
	dlclose(kntraderapiHandler);
}

void ITDEngine::set_reader_thread()
{
	reader_thread = ThreadPtr(new std::thread(boost::bind(&ITDEngine::listening, this)));
	OPTIONHEDGE_LOG_INFO(logger, "setup reader ITDEngine thread with address " << &reader_thread);
}

void ITDEngine::init()
{
	reader = optionHedge::memoryManage::JournalReader::createRevisableReader(name());
	JournalPair tdPair = getTDJournalPair(source_id);
	writer = optionHedge::memoryManage::JournalWriter::create(tdPair.first, tdPair.second, name());
	user_helper = TDUserInfoHelperPtr(new TDUserInfoHelper(source_id));
	td_helper = TDEngineInfoHelperPtr(new TDEngineInfoHelper(source_id, name()));
	pos_manage = TDPosManagePtr(new TDPosManage(source_id));
}

void ITDEngine::listening()
{
	optionHedge::memoryManage::FramePtr frame;
	while (isRunning && signal_received < 0)
	{
		frame = reader->getNextFrame();
		if (frame.get() != nullptr)
		{
			short msg_type = frame->getMsgType();
			short msg_source = frame->getSource();

			std::cout << "msg_type is " << msg_type << std::endl;
			cur_time = frame->getNano();

			if (msg_type == MSG_TYPE_MD)
			{
				void* fdata = frame->getData();
				MarketDataField* md = (MarketDataField*)fdata;
				//				on_market_data(md, cur_time);
				continue;
			}
			else if (msg_type < 200)
			{
				if (msg_type == MSG_TYPE_TRADE_ENGINE_LOGIN && msg_source == source_id)
				{
					try
					{
						string content((char*)frame->getData());
						json j_request = json::parse(content);
						string client_name = j_request["name"].get<string>();
						if (add_client(client_name, j_request))
							OPTIONHEDGE_LOG_INFO(logger, "[user] Accepted: " << client_name);
						else
							OPTIONHEDGE_LOG_INFO(logger, "[user] Rejected: " << client_name);
					}
					catch (...)
					{
						OPTIONHEDGE_LOG_ERROR(logger, "error in parsing TRADE_ENGINE_LOGIN: " << (char*)frame->getData());
					}
				}
				else if (msg_type == MSG_TYPE_STRATEGY_END)
				{
					try
					{
						string content((char*)frame->getData());
						json j_request = json::parse(content);
						string client_name = j_request["name"].get<string>();
						if (remove_client(client_name, j_request))
							OPTIONHEDGE_LOG_INFO(logger, "[user] Removed: " << client_name);

					}
					catch (...)
					{
						OPTIONHEDGE_LOG_ERROR(logger, "error in parsing STRATEGY_END: " << (char*)frame->getData());
					}
				}
				else if (msg_type == MSG_TYPE_STRATEGY_POS_SET && msg_source == source_id)
				{
					try
					{
						string content((char*)frame->getData());
						json j_request = json::parse(content);
						string client_name = j_request["name"].get<string>();
					}
					catch (...)
					{
						OPTIONHEDGE_LOG_ERROR(logger, "error in parsing STRATEGY_POS_SET: " << (char*)frame->getData());
					}
				}
				else if (msg_type == MSG_TYPE_TRADE_ENGINE_OPEN && (msg_source <= 0 || msg_source == source_id))
				{
					on_engine_open();
				}
				else if (msg_type == MSG_TYPE_TRADE_ENGINE_CLOSE && (msg_source <= 0 || msg_source == source_id))
				{
					on_engine_close();
				}
				else if (msg_type == MSG_TYPE_STRING_COMMAND)
				{
					string cmd((char*)frame->getData());
					on_command(cmd);
				}
				/*
				else if (msg_type == MSG_TYPE_SWITCH_TRADING_DAY)
				{
					user_helper->switch_day();
					for (auto iter : clients)
					{
						if (iter.second.pos_handler.get() != nullptr)
							iter.second.pos_handler->switch_day();
					}
					local_id = 1;
					on_switch_day();
				}
				*/
			}
			else if (msg_source == source_id && is_logged_in())
			{
				string name = reader->getFrameName();
				auto iter = clients.find(name);
				if (iter == clients.end())
					continue;

				void* fdata = frame->getData();
				int requestId = frame->getRequestId();
				int idx = iter->second.account_index;
				switch (msg_type)
				{
				case MSG_TYPE_QRY_POS:
				{
					QryPositionField* pos = (QryPositionField*)fdata;
					strcpy(pos->BrokerID, accounts[idx].BrokerID);
					strcpy(pos->InvestorID, accounts[idx].InvestorID);
					req_investor_position(pos, idx, requestId);
					break;
				}
				case MSG_TYPE_QRY_ORDER:
				{
					QryOrderField* qryOrder = (QryOrderField*)fdata;
					strcpy(qryOrder->BrokerID, accounts[idx].BrokerID);
					strcpy(qryOrder->InvestorID, accounts[idx].InvestorID);
					req_qry_order(qryOrder, idx, requestId);
					break;
				}
				case MSG_TYPE_QRY_TRADE:
				{
					QryTradeField* qryTrade = (QryTradeField*)fdata;
					strcpy(qryTrade->BrokerID, accounts[idx].BrokerID);
					strcpy(qryTrade->InvestorID, accounts[idx].InvestorID);
					req_qry_trade(qryTrade, idx, requestId);
					break;
				}
				case MSG_TYPE_ORDER:
				{
					InputOrderField* order = (InputOrderField*)fdata;
					strcpy(order->BrokerID, accounts[idx].BrokerID);
					strcpy(order->InvestorID, accounts[idx].InvestorID);
					strcpy(order->UserID, accounts[idx].UserID);
					strcpy(order->BusinessUnit, accounts[idx].BusinessUnit);
					char* order_ref;
					order_ref = order->OrderRef;
					string username(order->UserID);

					if (order->OffsetFlag == '\0')
					{
						order->OffsetFlag = pos_manage->autoOffset(string(order->UserID), order->InstrumentID, order->ExchangeID, order->HedgeFlag, order->Direction, order->Volume);
					}
					else if (order->OffsetFlag == THOST_FTDC_OF_Close || order->OffsetFlag == THOST_FTDC_OF_CloseToday || order->OffsetFlag == THOST_FTDC_OF_CloseYesterday)
					{
						string exchangeID(order->ExchangeID);
						string searchDir;
						(order->Direction == THOST_FTDC_D_Buy) ? searchDir = THOST_FTDC_PD_Short : searchDir = THOST_FTDC_PD_Long;
						string sinstrumentKey = string(order->InstrumentID) + "_" + order->HedgeFlag + "_" + searchDir;

						if (order->OffsetFlag == THOST_FTDC_OF_Close || order->OffsetFlag == THOST_FTDC_OF_CloseYesterday)
						{
							if (exchangeID == string("CZCE") || exchangeID == string("DCE") || exchangeID == string("CFFEX"))
							{
								pos_manage->setValue(username, sinstrumentKey, lockClose, Add, order->Volume);
							}
							else if (exchangeID == string("SHFE") || exchangeID == string("INE"))
							{
								pos_manage->setValue(username, sinstrumentKey, lockYesterdayClose, Add, order->Volume);
							}

						}
						else if (order->OffsetFlag == THOST_FTDC_OF_CloseToday)
						{
							pos_manage->setValue(username, sinstrumentKey, lockTodayClose, Add, order->Volume);
						}
						pos_manage->display();
					}

				
					memcpy(order->OrderRef, order_ref, 13);
					long before_nano = optionHedge::memoryManage::getNanoTime();
					req_order_insert(order, idx, requestId, cur_time);
					// insert order, we need to track in send
					OPTIONHEDGE_LOG_DEBUG(logger, "[insert_order] (rid)" << requestId << " (ticker)" << order->InstrumentID << " (ref)" << order_ref);
					break;
				}
				case MSG_TYPE_ORDER_ACTION:
				{
					OrderActionField* order = (OrderActionField*)fdata;
					strcpy(order->BrokerID, accounts[idx].BrokerID);
					strcpy(order->InvestorID, accounts[idx].InvestorID);
					strcpy(order->UserID, accounts[idx].UserID);
					OPTIONHEDGE_LOG_DEBUG(logger, "[cancel_order] (rid)" << order->InternalOrderID << " (ticker)" << order->InstrumentID << " (ref) " << order->OrderRef << " orderSysID " << order->OrderSysID);
					req_order_action(order, idx, requestId, cur_time);
					
					break;
				}
				case MSG_TYPE_QRY_ACCOUNT:
				{
					QryAccountField* acc = (QryAccountField*)fdata;
					strcpy(acc->BrokerID, accounts[idx].BrokerID);
					strcpy(acc->InvestorID, accounts[idx].InvestorID);
					req_qry_account(acc, idx, requestId);
					break;
				}
				default:
					OPTIONHEDGE_LOG_DEBUG(logger, "[Unexpected] frame found: (msg_type)" << msg_type << ", (name)" << name);
				}
			}
		}
	}

	if (IEngine::signal_received >= 0)
	{
		OPTIONHEDGE_LOG_INFO(logger, "[IEngine] signal received: " << IEngine::signal_received);
	}

	if (!isRunning)
	{
		OPTIONHEDGE_LOG_INFO(logger, "[IEngine] forced to stop.");
	}
}

static const RspPositionField empty_pos = {};

void ITDEngine::on_rsp_position(const RspPositionField* pos, bool isLast, int requestId, int errorId, const char* errorMsg)
{
	string username(pos->InvestorID);

	if (errorId == 0)
	{
		writer->write_frame(pos, sizeof(RspPositionField), source_id, MSG_TYPE_RSP_POS, isLast, requestId);
		OPTIONHEDGE_LOG_DEBUG(logger, "[RspPosition]" << " (rid)" << requestId
			<< " (ticker)" << pos->InstrumentID
			<< " (dir)" << pos->PosiDirection
			<< " (pos)" << pos->Position
			<< " (posDate)" << pos->PositionDate
			<< " (yd)" << pos->YdPosition
			<< " (todayPosition)" << pos->TodayPosition);

		if (!isDirectStrategy(username, requestId))
		{
			int drid = directStratgeRid[username] + REQUEST_ID_RANGE - 200;
			writer->write_frame(pos, sizeof(RspPositionField), source_id, MSG_TYPE_RSP_POS, isLast, drid);
			OPTIONHEDGE_LOG_DEBUG(logger, "Send another [RspPosition] to direct strategy with " << " (rid)" << drid
				<< " (ticker)" << pos->InstrumentID
				<< " (dir)" << pos->PosiDirection
				<< " (pos)" << pos->Position
				<< " (posDate)" << pos->PositionDate
				<< " (yd)" << pos->YdPosition
				<< " (todayPosition)" << pos->TodayPosition);
		}
	}
	else
	{
		writer->write_error_frame(pos, sizeof(RspPositionField), source_id, MSG_TYPE_RSP_POS, isLast, requestId, errorId, errorMsg);
		OPTIONHEDGE_LOG_ERROR(logger, "[RspPosition] fail! " << " (rid)" << requestId << " (errorId)" << errorId << " (errorMsg)" << errorMsg);
		if (!isDirectStrategy(username, requestId))
		{
			int drid = directStratgeRid[username] + REQUEST_ID_RANGE - 200;
			writer->write_error_frame(pos, sizeof(RspPositionField), source_id, MSG_TYPE_RSP_POS, isLast, drid, errorId, errorMsg);
			OPTIONHEDGE_LOG_ERROR(logger, "send another [RspPosition] fail to direct strategy with " << " (rid)" << drid << " (errorId)" << errorId << " (errorMsg)" << errorMsg);
		}
	}
}

void ITDEngine::on_rsp_qry_order(const RtnOrderField* order, bool isLast, int requestId, int errorId, const char* errorMsg)
{
	string username(order->UserID);
	if (errorId == 0)
	{
		writer->write_frame(order, sizeof(RtnOrderField), source_id, MSG_TYPE_RTN_QRY_ORDER, isLast, requestId);
		OPTIONHEDGE_LOG_DEBUG(logger, "[RtnQryOrder]" << " (rid)" << requestId
			<< " (ticker)" << order->InstrumentID
			<< " (dir)" << order->Direction
			<< " (offset)"<<order->OffsetFlag
			<< " (order status)" << order->OrderStatus
			<< " (volumn traded)" << order->VolumeTraded);

		if (!isDirectStrategy(username, requestId))
		{
			int drid = directStratgeRid[username] + REQUEST_ID_RANGE - 200;
			writer->write_frame(order, sizeof(RtnOrderField), source_id, MSG_TYPE_RTN_QRY_ORDER, isLast, drid);
			OPTIONHEDGE_LOG_DEBUG(logger, "Send another [RtnQryOrder] to direct strategy with " << " (rid)" << drid
				<< " (ticker)" << order->InstrumentID
				<< " (dir)" << order->Direction
				<< " (offset)" << order->OffsetFlag
				<< " (order status)" << order->OrderStatus
				<< " (volumn traded)" << order->VolumeTraded);
		}
	}
	else
	{
		writer->write_error_frame(order, sizeof(RtnOrderField), source_id, MSG_TYPE_RTN_QRY_ORDER, isLast, requestId, errorId, errorMsg);
		OPTIONHEDGE_LOG_ERROR(logger, "[RtnQryOrder] fail! " << " (rid)" << requestId << " (errorId)" << errorId << " (errorMsg)" << errorMsg);

		if (!isDirectStrategy(username, requestId))
		{
			int drid = directStratgeRid[username] + REQUEST_ID_RANGE - 200;
			writer->write_error_frame(order, sizeof(RtnOrderField), source_id, MSG_TYPE_RTN_QRY_ORDER, isLast, drid, errorId, errorMsg);
			OPTIONHEDGE_LOG_ERROR(logger, "Send another [RtnQryOrder] fail to direct strategy with " << " (rid)" << drid << " (errorId)" << errorId << " (errorMsg)" << errorMsg);
		}
	}
}

void ITDEngine::on_rsp_qry_trade(const RtnTradeField* trade, bool isLast, int requestId, int errorId, const char* errorMsg)
{
	string username(trade->UserID);

	if (errorId == 0)
	{
		writer->write_frame(trade, sizeof(RtnTradeField), source_id, MSG_TYPE_RTN_QRY_TRADE, isLast, requestId);
		OPTIONHEDGE_LOG_DEBUG(logger, "[RtnQryTrade]" << " (rid)" << requestId
			<< " (ticker)" << trade->InstrumentID << " direction is " << trade->Direction << " offsetvalue " << trade->OffsetFlag << " volume is " << trade->Volume << " isLast " << isLast);

		if (!isDirectStrategy(username, requestId))
		{
			int drid = directStratgeRid[username] + REQUEST_ID_RANGE - 200;
			writer->write_frame(trade, sizeof(RtnTradeField), source_id, MSG_TYPE_RTN_QRY_TRADE, isLast, drid);
			OPTIONHEDGE_LOG_DEBUG(logger, "send another [RtnQryTrade] to direct strategy with " << " (rid)" << drid
				<< " (ticker)" << trade->InstrumentID << " direction is " << trade->Direction << " offsetvalue " << trade->OffsetFlag << " volume is " << trade->Volume << " isLast " << isLast);
		}
	}
	else
	{
		writer->write_error_frame(trade, sizeof(RtnTradeField), source_id, MSG_TYPE_RTN_QRY_TRADE, isLast, requestId, errorId, errorMsg);
		OPTIONHEDGE_LOG_ERROR(logger, "[RtnQryTrade] fail! " << " (rid)" << requestId << " (errorId)" << errorId << " (errorMsg)" << errorMsg);

		if (!isDirectStrategy(username, requestId))
		{
			int drid = directStratgeRid[username] + REQUEST_ID_RANGE - 200;
			writer->write_error_frame(trade, sizeof(RtnTradeField), source_id, MSG_TYPE_RTN_QRY_TRADE, isLast, drid, errorId, errorMsg);
			OPTIONHEDGE_LOG_ERROR(logger, "send another [RtnQryTrade] fail to direct strategy with " << " (rid)" << drid << " (errorId)" << errorId << " (errorMsg)" << errorMsg);
		}
	}
}

void ITDEngine::on_rsp_order_insert(const InputOrderField* order, int requestId, int errorId, const char* errorMsg)
{
	string username(order->UserID);

	if (errorId == 0)
	{
		writer->write_frame(order, sizeof(InputOrderField), source_id, MSG_TYPE_ORDER, true, requestId);
		OPTIONHEDGE_LOG_DEBUG(logger, "[Rsp_order_insert]" << " (rid)" << requestId << " (ticker)" << order->InstrumentID);

		if (!isDirectStrategy(username, requestId))
		{
			int drid = directStratgeRid[username] + REQUEST_ID_RANGE - 200;
			writer->write_frame(order, sizeof(InputOrderField), source_id, MSG_TYPE_ORDER, true, drid);
			OPTIONHEDGE_LOG_DEBUG(logger, "send another [Rsp_order_insert] to direct strategy with " << " (rid)" << drid << " (ticker)" << order->InstrumentID);
		}
	}
	else
	{
		writer->write_error_frame(order, sizeof(InputOrderField), source_id, MSG_TYPE_ORDER, true, requestId, errorId, errorMsg);
		OPTIONHEDGE_LOG_ERROR(logger, "[Rsp_order_insert] fail!" << " (rid)" << requestId << " (errorId)" << errorId << " (errorMsg)" << errorMsg);
		if (!isDirectStrategy(username, requestId))
		{
			int drid = directStratgeRid[username] + REQUEST_ID_RANGE - 200;
			writer->write_error_frame(order, sizeof(InputOrderField), source_id, MSG_TYPE_ORDER, true, drid, errorId, errorMsg);
			OPTIONHEDGE_LOG_ERROR(logger, "send another [Rsp_order_insert] fail to direct strategy with " << " (rid)" << drid << " (errorId)" << errorId << " (errorMsg)" << errorMsg);
		}

	}
}

void ITDEngine::on_rsp_order_action(const RspActionField* action, int requestId, int errorId, const char *errorMsg)
{
	string username(action->InvestorID);

	if (errorId == 0)
	{
		writer->write_frame(action, sizeof(RspActionField), source_id, MSG_TYPE_ORDER_ACTION_RSP, true, requestId);
		OPTIONHEDGE_LOG_DEBUG(logger, "[RspAction]" << " (rid)" << requestId << " (orderSysID)" << action->OrderSysID);

		if (!isDirectStrategy(username, requestId))
		{
			int drid = directStratgeRid[username] + REQUEST_ID_RANGE - 200;
			writer->write_frame(action, sizeof(RspActionField), source_id, MSG_TYPE_ORDER_ACTION_RSP, true, drid);
			OPTIONHEDGE_LOG_DEBUG(logger, "send another [RspAction] to direct strategy with " << " (rid)" << drid << " (orderSysID)" << action->OrderSysID);
		}
	}
	else
	{
		writer->write_error_frame(action, sizeof(RspActionField), source_id, MSG_TYPE_ORDER_ACTION_RSP, true, requestId, errorId, errorMsg);
		OPTIONHEDGE_LOG_ERROR(logger, "[RspAction] fail!" << " (rid)" << requestId << " (errorId)" << errorId << " (errorMsg)" << errorMsg);

		if (!isDirectStrategy(username, requestId))
		{
			int drid = directStratgeRid[username] + REQUEST_ID_RANGE - 200;
			writer->write_error_frame(action, sizeof(RspActionField), source_id, MSG_TYPE_ORDER_ACTION_RSP, true, drid, errorId, errorMsg);
			OPTIONHEDGE_LOG_ERROR(logger, "send another [RspAction] fail to direct strategy with " << " (rid)" << drid << " (errorId)" << errorId << " (errorMsg)" << errorMsg);
		}
	}
}

void ITDEngine::on_rsp_account(const RspAccountField* account, bool isLast, int requestId, int errorId, const char* errorMsg)
{
	string username(account->InvestorID);

	if (errorId == 0)
	{
		writer->write_frame(account, sizeof(RspAccountField), source_id, MSG_TYPE_RSP_ACCOUNT, isLast, requestId);
		OPTIONHEDGE_LOG_DEBUG(logger, "[RspAccount]" << " (rid)" << requestId
			<< " (investor)" << account->InvestorID
			<< " (balance)" << account->Balance
			<< " (value)" << account->MarketValue);

		if (!isDirectStrategy(username, requestId))
		{
			int drid = directStratgeRid[username] + REQUEST_ID_RANGE - 200;
			writer->write_frame(account, sizeof(RspAccountField), source_id, MSG_TYPE_RSP_ACCOUNT, isLast, drid);
			OPTIONHEDGE_LOG_DEBUG(logger, "send another [RspAccount] to direct strategy with " << " (rid)" << drid
				<< " (investor)" << account->InvestorID
				<< " (balance)" << account->Balance
				<< " (value)" << account->MarketValue);
		}
	}
	else
	{
		writer->write_error_frame(account, sizeof(RspAccountField), source_id, MSG_TYPE_RSP_ACCOUNT, isLast, requestId, errorId, errorMsg);
		OPTIONHEDGE_LOG_ERROR(logger, "[RspAccount] fail!" << " (rid)" << requestId << " (errorId)" << errorId << " (errorMsg)" << errorMsg);

		if (!isDirectStrategy(username, requestId))
		{
			int drid = directStratgeRid[username] + REQUEST_ID_RANGE - 200;
			writer->write_error_frame(account, sizeof(RspAccountField), source_id, MSG_TYPE_RSP_ACCOUNT, isLast, drid, errorId, errorMsg);
			OPTIONHEDGE_LOG_ERROR(logger, "send another [RspAccount] fail to direct strategy with " << " (rid)" << drid << " (errorId)" << errorId << " (errorMsg)" << errorMsg);
		}
	}
}

void ITDEngine::on_rtn_order(const RtnOrderField* rtn_order)
{
	string orderKey(rtn_order->OrderRef);
	orderKey = orderKey + "_" + to_string(rtn_order->FrontID) + "_" + to_string(rtn_order->SessionID);
	int rid = td_helper->get_order_id(orderKey.c_str());

	string username(rtn_order->UserID);

	writer->write_frame(rtn_order, sizeof(RtnOrderField), source_id, MSG_TYPE_RTN_ORDER, 1/*islast*/, rid);
	OPTIONHEDGE_LOG_DEBUG_FMT(logger, "[o] (id)%d (ref)%s (dir)%c (offset)%c (ticker)%s (VolumeTotalOriginal)%d (VolumeTraded)%d (VolumeTotal)%d (St)%c (BrokerSeqNo)%d",
		rid,
		rtn_order->OrderRef,
		rtn_order->Direction,
		rtn_order->OffsetFlag,
		rtn_order->InstrumentID,
		rtn_order->VolumeTotalOriginal,
		rtn_order->VolumeTraded,
		rtn_order->VolumeTotal,
		rtn_order->OrderStatus,
		rtn_order->BrokerOrderSeq);

	if (!isDirectStrategy(username, rid))
	{
		int drid = directStratgeRid[username]+ REQUEST_ID_RANGE - 200;
		writer->write_frame(rtn_order, sizeof(RtnOrderField), source_id, MSG_TYPE_RTN_ORDER, 1/*islast*/, drid);
		OPTIONHEDGE_LOG_DEBUG_FMT(logger, "send another [o] to direct strategy (id)%d (ref)%s (dir)%c (offset)%c (ticker)%s (VolumeTotalOriginal)%d (VolumeTraded)%d (VolumeTotal)%d (St)%c (BrokerSeqNo)%d",
			drid,
			rtn_order->OrderRef,
			rtn_order->Direction,
			rtn_order->OffsetFlag,
			rtn_order->InstrumentID,
			rtn_order->VolumeTotalOriginal,
			rtn_order->VolumeTraded,
			rtn_order->VolumeTotal,
			rtn_order->OrderStatus,
			rtn_order->BrokerOrderSeq);
	}
	string name = rid_manager.get(rid);
	user_helper->set_order_status(name, rid, rtn_order->OrderStatus);
	td_helper->set_order_status(rtn_order->OrderRef, rtn_order->OrderStatus);
}

void ITDEngine::on_rtn_trade(const RtnTradeField* rtn_trade)
{
	int rid = td_helper->get_order_id(rtn_trade->OrderSysID, OrderSysId);
	string username(rtn_trade->UserID);

	writer->write_frame(rtn_trade, sizeof(RtnTradeField), source_id, MSG_TYPE_RTN_TRADE, 1/*islast*/, rid);
	OPTIONHEDGE_LOG_DEBUG_FMT(logger, "[t] (id)%d (ref)%s (dir)%c (offset)%c (ticker)%s (V)%d (P)%f ",
		rid,
		rtn_trade->OrderRef,
		rtn_trade->Direction,
		rtn_trade->OffsetFlag,
		rtn_trade->InstrumentID,
		rtn_trade->Volume,
		rtn_trade->Price);

	if (!isDirectStrategy(username, rid))
	{
		int drid = directStratgeRid[username] + REQUEST_ID_RANGE - 200;
		writer->write_frame(rtn_trade, sizeof(RtnTradeField), source_id, MSG_TYPE_RTN_TRADE, 1/*islast*/, drid);
		OPTIONHEDGE_LOG_DEBUG_FMT(logger, "send another [t] to direct strategy (id)%d (ref)%s (dir)%c (offset)%c (ticker)%s (V)%d (P)%f ",
			drid,
			rtn_trade->OrderRef,
			rtn_trade->Direction,
			rtn_trade->OffsetFlag,
			rtn_trade->InstrumentID,
			rtn_trade->Volume,
			rtn_trade->Price);
	}
}

void ITDEngine::on_rtn_instrument_status(const RspInstrumentStatus* rtn_instatus, string username)
{
	auto it = parkedStratgeRid.find(username);
	if (it != parkedStratgeRid.end())
	{
		int prid=parkedStratgeRid[username] + REQUEST_ID_RANGE - 200;
		writer->write_frame(rtn_instatus, sizeof(RspInstrumentStatus), source_id, MSG_TYPE_RTN_INS_STATUS, 1/*islast*/, prid);
		OPTIONHEDGE_LOG_DEBUG_FMT(logger, "send [on_rtn_instrument_status] (id)%d (ExchangeID)%s (InstrumentID)%s (InstrumentStatus)%c ",
			prid,
			rtn_instatus->ExchangeID,
			rtn_instatus->InstrumentID,
			rtn_instatus->InstrumentStatus);
	}

	int drid = directStratgeRid[username];
	writer->write_frame(rtn_instatus, sizeof(RspInstrumentStatus), source_id, MSG_TYPE_RTN_INS_STATUS, 1/*islast*/, drid);
	OPTIONHEDGE_LOG_DEBUG_FMT(logger, "send [on_rtn_instrument_status] (id)%d (ExchangeID)%s (InstrumentID)%s (InstrumentStatus)%c ",
		drid,
		rtn_instatus->ExchangeID,
		rtn_instatus->InstrumentID,
		rtn_instatus->InstrumentStatus);
}

bool ITDEngine::add_client(const string& client_name, const json& j_request)
{
	string folder = j_request["folder"].get<string>();
	int rid_s = j_request["rid_s"].get<int>();
	int rid_e = j_request["rid_e"].get<int>();

	rid_manager.set(rid_s, client_name);
	/*
	long last_switch_day = j_request["last_switch_nano"].get<long>();
	*/
	user_helper->load(client_name);

	auto iter = clients.find(client_name);

	if (iter == clients.end())
	{
		if (default_account_index < 0)
		{
			return false;
		}
		else
		{
			int idx = reader->addJournal(folder, client_name);
			reader->seekTimeJournal(idx, cur_time);
			ClientInfoUnit& status = clients[client_name];
			status.is_alive = true;
			status.journal_index = idx;
			status.account_index = default_account_index;
			status.rid_start = rid_s;
			status.rid_end = rid_e;
		}
	}
	else if (iter->second.is_alive)
	{
		OPTIONHEDGE_LOG_ERROR(logger, "login already exists... (info)" << client_name);
	}
	else
	{
		if (iter->second.journal_index < 0)
			iter->second.journal_index = reader->addJournal(folder, client_name);

		reader->seekTimeJournal(iter->second.journal_index, cur_time);
		iter->second.rid_start = rid_s;
		iter->second.rid_end = rid_e;
		iter->second.is_alive = true;
		json json_rspstr;

		json_rspstr["name"] = client_name;
		json_rspstr["ok"] = true;
		json_rspstr["tradedate"] = td_helper->get_trade_date();

		string json_content = json_rspstr.dump();
		writer->write_frame(json_content.c_str(), json_content.length() + 1, source_id, MSG_TYPE_TRADE_ENGINE_ACK, 1, -1);
	}

	return true;
}


bool ITDEngine::remove_client(const string &client_name, const json &j_request)
{
	auto iter = clients.find(client_name);
	if (iter == clients.end())
		return false;
	reader->expireJournal(iter->second.journal_index);
	iter->second.is_alive = false;
	/*
	if (iter->second.pos_handler.get() != nullptr)
	{
		json j_pos = iter->second.pos_handler->to_json();
		user_helper->set_pos(client_name, j_pos);
		iter->second.pos_handler.reset();
	}
	*/
	if (is_logged_in())
	{
		// cancel all pending orders, and clear the memory
		auto orders = user_helper->get_existing_orders(client_name);
		int idx = iter->second.account_index;
		for (int order_id : orders)
		{
			OrderActionField action = {};
			action.ActionFlag = CHAR_Delete;
			action.InternalOrderID = order_id;
			action.LimitPrice = 0;
			action.VolumeChange = 0;
			strcpy(action.BrokerID, accounts[idx].BrokerID);
			strcpy(action.InvestorID, accounts[idx].InvestorID);
			int local_id;
			/*
			if (user_helper->get_order(client_name, order_id, local_id, action.InstrumentID))
			{
				string order_ref = std::to_string(local_id);
				strcpy(action.OrderRef, order_ref.c_str());
				OPTIONHEDGE_LOG_DEBUG(logger, "[cancel_remain_order] (rid)" << order_id << " (ticker)" << action.InstrumentID << " (ref)" << order_ref);
				req_order_action(&action, iter->second.account_index, order_id, cur_time);
			}*/
		}
	}
	//	user_helper->remove(client_name);
	return true;
}

void ITDEngine::load(const json& j_config)
{
	/*
	if (j_config.find(PH_FEE_SETUP_KEY) != j_config.end())
	{
		default_fee_handler = FeeHandlerPtr(new FeeHandler(j_config[PH_FEE_SETUP_KEY]));
	}
	*/
	auto iter = j_config[TE_CONFIG_KEY_TD][TE_CONFIG_KEY_SOURCECTP].find("accounts");
	if (iter != j_config[TE_CONFIG_KEY_TD][TE_CONFIG_KEY_SOURCECTP].end())
	{
		int account_num = iter.value().size();
		OPTIONHEDGE_LOG_INFO(logger, "[account] number" << account_num);

		accounts.resize(account_num);
		resize_accounts(account_num);

		int account_idx = 0;

		for (auto& j_account : iter.value())
		{
			const json& j_info = j_account["info"];
			accounts[account_idx] = load_account(account_idx, j_info);
			/*
			auto &fee_handler = accounts[account_idx].fee_handler;
			if (fee_handler.get() == nullptr)
			{
				if (j_info.find(PH_FEE_SETUP_KEY) != j_info.end())
				{
					fee_handler = FeeHandlerPtr(new FeeHandler(j_info[PH_FEE_SETUP_KEY]));
				}
				else
				{
					if (default_fee_handler.get() == nullptr)
					{
						KF_LOG_ERROR(logger, "[client] no fee_handler (idx)" << account_idx);
						throw std::runtime_error("cannot find fee_handler for account!");
					}
					else
					{
						fee_handler = default_fee_handler;
					}
				}
			}
			*/

			const json& j_clients = j_account["clients"];
			for (auto& j_client : j_clients)
			{
				string client_name = j_client.get<string>();
				ClientInfoUnit& client_status = clients[client_name];
				client_status.account_index = account_idx;
				client_status.is_alive = false;
				client_status.journal_index = -1;
				OPTIONHEDGE_LOG_INFO(logger, "[client] (name)" << client_name << " (account)" << accounts[client_status.account_index].InvestorID);
			}

			/** set default */
			if (j_account["is_default"].get<bool>())
				default_account_index = account_idx;
			// add
			account_idx++;
		}
	}
}

TradeAccount ITDEngine::load_account(int idx, const json& j_account)
{
	OPTIONHEDGE_LOG_ERROR(logger, "[account] NOT IMPLEMENTED! (content)" << j_account);
	throw std::runtime_error("load_account not implemented yet!");
}

bool ITDEngine::isDirectStrategy(string username, int rid)
{
	auto it = directStratgeRid.find(username);

	if (it == directStratgeRid.end())
	{
		return true;
	}
	else
	{
		if (directStratgeRid[username] <= rid && rid < (directStratgeRid[username] + REQUEST_ID_RANGE))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
