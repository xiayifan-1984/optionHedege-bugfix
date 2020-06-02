#include "TDEngineCTP.h"
#include "../base/ctp.h"
#include "../base/EngineUtil.hpp"
#include "../../memoryManage/utils/util.hpp"
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <cstdio>

USING_TE_NAMESPACE

TDEngineCTP::TDEngineCTP() :ITDEngine(SOURCE_CTP), need_settleConfirm(true), need_authenticate(false), curAccountIdx(-1)
{
	logger = optionHedge::memoryManage::Log::getLogger("tradingEngine");
	OPTIONHEDGE_LOG_INFO(logger, "[ATTENTION] default to confirm settlement and no authentication!");
}

void TDEngineCTP::init()
{
	ITDEngine::init();
	//	JournalPair tdRawPair = getTdRawJournalPair(source_id);
	//	raw_writer = optionHedge::memoryManage::JournalWriter::create(tdRawPair.first, tdRawPair.second, "RAW_" + name());
	for (auto s : directStratgeRid)
	{
		OPTIONHEDGE_LOG_INFO(logger, "[DirectStrategyRid] mapping: " << s.first << " with Rid " << s.second);
	}

	for (auto s : parkedStratgeRid)
	{
		OPTIONHEDGE_LOG_INFO(logger, "[ParkedStrategyRid] mapping: " << s.first << " with Rid " << s.second);
	}
}

void TDEngineCTP::pre_load(const json& j_config)
{
	front_uri = j_config[TE_CONFIG_KEY_TD][TE_CONFIG_KEY_SOURCECTP][TE_CONFIG_KEY_FRONT_URI].get<string>();
	front_uri_kn = j_config[TE_CONFIG_KEY_TD][TE_CONFIG_KEY_SOURCECTP][TE_CONFIG_KEY_FRONT_URI_KN].get<string>();
	need_settleConfirm = j_config[TE_CONFIG_KEY_TD][TE_CONFIG_KEY_SOURCECTP][TE_CONFIG_KEY_NEED_SETTLE_CONFIRM].get<bool>();

	if (!need_settleConfirm)
	{
		OPTIONHEDGE_LOG_INFO(logger, "[ATTENTION] no need to confirm settlement!");
	}

	need_authenticate = j_config[TE_CONFIG_KEY_TD][TE_CONFIG_KEY_SOURCECTP][TE_CONFIG_KEY_NEED_AUTH].get<bool>();

	if (need_authenticate)
	{
		OPTIONHEDGE_LOG_INFO(logger, "[ATTENTION] need to authenticate code!");
	}
}


void TDEngineCTP::resize_accounts(int account_num)
{
	account_units.resize(account_num);
}

TradeAccount TDEngineCTP::load_account(int idx, const json& j_config)
{
	std::cout << j_config.dump() << std::endl;
	// internal load
	string broker_id = j_config[TE_CONFIG_KEY_BROKER_ID].get<string>();
	string user_id = j_config[TE_CONFIG_KEY_USER_ID].get<string>();
	string investor_id = j_config[TE_CONFIG_KEY_INVESTOR_ID].get<string>();
	string password = j_config[TE_CONFIG_KEY_PASSWORD].get<string>();

	AccountUnitCTP& unit = account_units[idx];
	unit.api = nullptr;
	unit.front_id = -1;
	unit.session_id = -1;
	unit.initialized = false;
	unit.connected = false;
	unit.authenticated = false;
	unit.logged_in = false;
	unit.settle_confirmed = false;
	unit.appID = "";
	unit.auth_code = "";
	unit.accountType = j_config[TE_CONFIG_KEY_ACCOUNT_TYPE].get<string>();
	unit.bYesterdayPosInitialized = false;
	unit.bTodayPosInitialized = false;
	unit.bOrderInitialized = false;

	if (need_authenticate)
	{
		unit.auth_code = j_config[TE_CONFIG_KEY_AUTH_CODE].get<string>();
		unit.appID = j_config[TE_CONFIG_KEY_APP_ID].get<string>();
	}

	TradeAccount account = {};
	strncpy(account.BrokerID, broker_id.c_str(), 19);
	strncpy(account.InvestorID, investor_id.c_str(), 19);
	strncpy(account.UserID, user_id.c_str(), 16);
	strncpy(account.Password, password.c_str(), 21);

	return account;
}

void TDEngineCTP::connect(long timeout_nsec)
{
	typedef CThostFtdcTraderApi *(*createapiFunctor)(const char *);
	createapiFunctor cf = nullptr;
	for (size_t idx = 0; idx < account_units.size(); ++idx)
	{
		AccountUnitCTP& unit = account_units[idx];

		if (unit.api == nullptr)
		{
			if (unit.accountType == "CTP")
			{
				cf = (createapiFunctor)dlsym(traderapiHandler, "_ZN19CThostFtdcTraderApi19CreateFtdcTraderApiEPKc");
			}

			else if (unit.accountType == "KINGNEW")
			{
				cf = (createapiFunctor)dlsym(kntraderapiHandler, "_ZN19CThostFtdcTraderApi19CreateFtdcTraderApiEPKc");
			}

			char* error;
			if ((error = dlerror()) != NULL)
			{
				std::cout << error << std::endl;
				OPTIONHEDGE_LOG_ERROR(logger, error);
				std::runtime_error("CTP_TD failed to load function for create api");
			}
			CThostFtdcTraderApi* api = cf("");
			if (!api)
			{
				throw std::runtime_error("CTP_TD failed to create api");
			}
			TDEngineSpi* spi = new TDEngineSpi(accounts[idx], unit.appID, unit.auth_code, api, this);
			api->RegisterSpi(spi);
			unit.api = api;
		}

		if (!unit.connected)
		{
			curAccountIdx = idx;
			if (unit.accountType == "CTP")
			{
				unit.api->RegisterFront((char*)front_uri.c_str());
			}
			else if (unit.accountType == "KINGNEW")
			{
				unit.api->RegisterFront((char*)front_uri_kn.c_str());
			}
			unit.api->SubscribePublicTopic(THOST_TERT_QUICK);
			unit.api->SubscribePrivateTopic(THOST_TERT_QUICK);

			if (!unit.initialized)
			{
				unit.api->Init();
				unit.initialized = true;
			}
			long start_time = optionHedge::memoryManage::getNanoTime();
			while (!unit.connected && optionHedge::memoryManage::getNanoTime() - start_time < timeout_nsec)
			{
			}
		}
	}
}

/*
void TDEngineCTP::login(long timeout_nsec)
{
	for (size_t idx = 0; idx < account_units.size(); ++idx)
	{
		AccountUnitCTP& unit = account_units[idx];
		TradeAccount& account = accounts[idx];

		if (need_authenticate && !unit.authenticated)
		{
			struct CThostFtdcReqAuthenticateField req = {};
			strcpy(req.BrokerID, account.BrokerID);
			strcpy(req.UserID, account.UserID);
			strcpy(req.AppID, unit.appID.c_str());
			strcpy(req.AuthCode, unit.auth_code.c_str());

			unit.auth_rid = request_id;
			if (unit.api->ReqAuthenticate(&req, request_id++))
			{
				OPTIONHEDGE_LOG_ERROR(logger, "[request] auth failed!" << " (Bid)" << req.BrokerID
					<< " (Uid)" << req.UserID
					<< " (Auth)" << req.AuthCode);
			}
			long start_time = optionHedge::memoryManage::getNanoTime();
			while (!unit.authenticated && optionHedge::memoryManage::getNanoTime() - start_time < timeout_nsec)
			{
			}
		}

		//login
		if (!unit.logged_in)
		{
			struct CThostFtdcReqUserLoginField req = {};
			strcpy(req.TradingDay, "");
			strcpy(req.UserID, account.UserID);
			strcpy(req.BrokerID, account.BrokerID);
			strcpy(req.Password, account.Password);
			unit.login_rid = request_id;

			int rCode = unit.api->ReqUserLogin(&req, request_id++);
			if (rCode != 0)
			{
				OPTIONHEDGE_LOG_ERROR(logger, "[request] login failed!" << " (Bid)" << req.BrokerID
					<< " (Uid)" << req.UserID);
			}
			long start_time = optionHedge::memoryManage::getNanoTime();
			while (!unit.logged_in && optionHedge::memoryManage::getNanoTime() - start_time < timeout_nsec)
			{
			}
		}

		// confirm settlement
		if (need_settleConfirm && !unit.settle_confirmed)
		{
			CThostFtdcSettlementInfoConfirmField  req;
			memset(&req, 0, sizeof(req));
			strcpy(req.BrokerID, account.BrokerID);
			strcpy(req.InvestorID, account.InvestorID);
			unit.settle_rid = request_id;
			int rCode = unit.api->ReqSettlementInfoConfirm(&req, request_id++);
			if (rCode != 0)
			{
				OPTIONHEDGE_LOG_ERROR(logger, "[request] QrySettlementInfoConfirm failed!" << " (Bid)" << req.BrokerID
					<< " (Iid)" << req.InvestorID);
			}

			long start_time = optionHedge::memoryManage::getNanoTime();
			while (optionHedge::memoryManage::getNanoTime() - start_time < timeout_nsec)
			{

			}
		}
	}
}
*/
void TDEngineCTP::logout()
{
	for (size_t idx = 0; idx < account_units.size(); idx++)
	{
		AccountUnitCTP& unit = account_units[idx];
		TradeAccount& account = accounts[idx];
		if (unit.logged_in)
		{
			CThostFtdcUserLogoutField req = {};
			strcpy(req.BrokerID, account.BrokerID);
			strcpy(req.UserID, account.UserID);
			unit.login_rid = request_id;
			if (unit.api->ReqUserLogout(&req, request_id++))
			{
				OPTIONHEDGE_LOG_ERROR(logger, "[request] logout failed!" << " (Bid)" << req.BrokerID
					<< " (Uid)" << req.UserID);
			}
		}

		unit.authenticated = false;
		unit.settle_confirmed = false;
		unit.logged_in = false;
	}
}

void TDEngineCTP::release_api()
{
	for (auto& unit : account_units)
	{
		if (unit.api != nullptr)
		{
			unit.api->Release();
			unit.api = nullptr;
		}
		unit.initialized = false;
		unit.connected = false;
		unit.authenticated = false;
		unit.settle_confirmed = false;
		unit.logged_in = false;
		unit.api = nullptr;
	}
}

bool TDEngineCTP::is_logged_in() const
{
	int i = 0;
	for (auto& unit : account_units)
	{
		TradeAccount account = accounts[i];
		if (!unit.logged_in || (need_settleConfirm && !unit.settle_confirmed))
		{

			OPTIONHEDGE_LOG_INFO(logger, "account ID "<<account.UserID<<" unit login " << unit.logged_in << " unit settle confirmed " << unit.settle_confirmed);
			std::cout << "account " << account.UserID << " loggin status " << unit.logged_in << " settlement confirmed " << unit.settle_confirmed << std::endl;
			return false;
		}
		i++;
	}
	std::cout << "All account logged in already" << std::endl;
	return true;
}

bool TDEngineCTP::is_connected() const
{
	for (auto& unit : account_units)
	{
		if (!unit.connected)
			return false;
	}
	return true;
}

/**
 * req functions
 */

void TDEngineCTP::req_investor_position(const QryPositionField* data, int account_index, int requestID)
{
	struct CThostFtdcQryInvestorPositionField req = parseTo(*data);
	OPTIONHEDGE_LOG_DEBUG(logger, "[req_pos]" << " (Bid)" << req.BrokerID
		<< " (Iid)" << req.InvestorID
		<< " (Tid)" << req.InstrumentID);

	int rCode = account_units[account_index].api->ReqQryInvestorPosition(&req, requestID);
	while (rCode != 0)
	{
		if (rCode == -3)
		{
			OPTIONHEDGE_LOG_INFO(logger, "[request] wait 1 sec before send investor position again!" << " (rid)" << requestID
				<< " (idx)" << account_index);
			sleep(1);
			rCode = account_units[account_index].api->ReqQryInvestorPosition(&req, requestID);
		}
		else
		{
			OPTIONHEDGE_LOG_ERROR(logger, "[request] investor position failed!" << " (rid)" << requestID
				<< " (idx)" << account_index << " with return code " << rCode);
			break;
		}
	}
}

void TDEngineCTP::req_qry_order(const QryOrderField* data, int account_index, int requestID)
{
	struct CThostFtdcQryOrderField req = parseTo(*data);
	OPTIONHEDGE_LOG_DEBUG(logger, "[req_qry_order]" << " (Bid)" << req.BrokerID
		<< " (Iid)" << req.InvestorID
		<< " (Tid)" << req.InstrumentID);

	int rCode = account_units[account_index].api->ReqQryOrder(&req, requestID);
	while (rCode != 0)
	{
		if (rCode == -3)
		{
			OPTIONHEDGE_LOG_INFO(logger, "[request] wait 1 sec before qry order again!" << " (rid)" << requestID
				<< " (idx)" << account_index);
			sleep(1);
			rCode = account_units[account_index].api->ReqQryOrder(&req, requestID);
		}
		else
		{
			OPTIONHEDGE_LOG_ERROR(logger, "[request] qry order failed!" << " (rid)" << requestID
				<< " (idx)" << account_index);
			break;
		}
	}
}

void TDEngineCTP::req_qry_trade(const QryTradeField* data, int account_index, int requestID)
{
	struct CThostFtdcQryTradeField req = parseTo(*data);
	OPTIONHEDGE_LOG_DEBUG(logger, "[req_qry_trade]" << " (Bid)" << req.BrokerID
		<< " (Iid)" << req.InvestorID
		<< " (Tid)" << req.InstrumentID);
	int rCode = account_units[account_index].api->ReqQryTrade(&req, requestID);
	while (rCode != 0)
	{
		if (rCode == -3)
		{
			OPTIONHEDGE_LOG_ERROR(logger, "[request] wait 1 sec before send qry trade again!" << " (rid)" << requestID
				<< " (idx)" << account_index);
			sleep(1);
			rCode = account_units[account_index].api->ReqQryTrade(&req, requestID);
		}
		else
		{
			OPTIONHEDGE_LOG_ERROR(logger, "[request] qry trade failed!" << " (rid)" << requestID
				<< " (idx)" << account_index);
			break;
		}
	}
}

void TDEngineCTP::req_qry_account(const QryAccountField *data, int account_index, int requestId)
{
	struct CThostFtdcQryTradingAccountField req = parseTo(*data);
	OPTIONHEDGE_LOG_DEBUG(logger, "[req_account]" << " (Bid)" << req.BrokerID
		<< " (Iid)" << req.InvestorID);

	int rCode = account_units[account_index].api->ReqQryTradingAccount(&req, requestId);
	while (rCode != 0)
	{
		if (rCode == -3)
		{
			OPTIONHEDGE_LOG_ERROR(logger, "[request] wait 1 sec before send account info again!" << " (rid)" << requestId
				<< " (idx)" << account_index);
			sleep(1);
			rCode = account_units[account_index].api->ReqQryTradingAccount(&req, requestId);
		}
		else
		{
			OPTIONHEDGE_LOG_ERROR(logger, "[request] account info failed!" << " (rid)" << requestId
				<< " (idx)" << account_index);
			break;
		}
	}
}

void TDEngineCTP::req_order_insert(const InputOrderField* data, int account_index, int requestId, long rcv_time)
{
	struct CThostFtdcInputOrderField req = parseTo(*data);
	req.RequestID = requestId;
	req.IsAutoSuspend = 0;
	req.UserForceClose = 0;
	OPTIONHEDGE_LOG_DEBUG(logger, "[req_order_insert]" << " (rid)" << requestId
		<< " (Iid)" << req.InvestorID
		<< " (Instrumentid)" << req.InstrumentID
		<< " (OrderRef)" << req.OrderRef
		<< " (VolumeTotalOriginal) " << req.VolumeTotalOriginal
		<< " (ExchangeID)" << req.ExchangeID
		<< " (CombOffsetFlag)" << req.CombOffsetFlag);

	//key for order record is orderRef+frontID+SessionID
	string orderRef(data->OrderRef);
	orderRef = orderRef + "_"+to_string(account_units[account_index].front_id)+"_"+ to_string(account_units[account_index].session_id);

	td_helper->record_order(orderRef.c_str(), requestId, orderRef.length()+1);
	string lockRef = string(data->UserID) + "_" + string(data->OrderRef) + "_" + to_string(account_units[account_index].front_id) + "_" + to_string(account_units[account_index].session_id);
	pos_manage->setLockInfo(lockRef);

	int rCode = account_units[account_index].api->ReqOrderInsert(&req, requestId);
	while (rCode != 0)
	{
		if (rCode == -3)
		{
			OPTIONHEDGE_LOG_ERROR(logger, "[request] wait 1 sec before send order insert again!" << " (rid)" << requestId);
			sleep(1);
			rCode = account_units[account_index].api->ReqOrderInsert(&req, requestId);
		}
		else
		{
			OPTIONHEDGE_LOG_ERROR(logger, "[request] order insert failed!" << " (rid)" << requestId);
			break;
		}
	}
}

void TDEngineCTP::req_order_action(const OrderActionField* data, int account_index, int requestId, long rcv_time)
{
	struct CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	req = parseTo(*data);
	auto& unit = account_units[account_index];
	req.FrontID = unit.front_id;
	req.SessionID = unit.session_id;

	OPTIONHEDGE_LOG_DEBUG(logger, "[req_order_action]" << " (rid)" << requestId
		<< " (BrokerID)" << req.BrokerID
		<< " (Iid)" << req.InvestorID
		<< " (InstrumentID)" << req.InstrumentID
		<< " (ExchangeID)" << req.ExchangeID
		<< " (UserID)" << req.UserID
		<< " (OrderRef)" << req.OrderRef
		<< " (OrderSysID) " << req.OrderSysID
		<< " (ActionFlag) " << req.ActionFlag
		<< " (LimitPirce) " << req.LimitPrice
		<< " (OrderActionRef) " << req.OrderActionRef
		<< " (VolumnChange)" << req.VolumeChange
		<< " (FrontID) " << req.FrontID
		<< " (SessionID) " << req.SessionID
	);

	int rCode = unit.api->ReqOrderAction(&req, requestId);
	while (rCode != 0)
	{
		if (rCode == -3)
		{
			OPTIONHEDGE_LOG_ERROR(logger, "[request] wait 1 sec before send order action again!" << " (rid)" << requestId);
			sleep(1);
			rCode = unit.api->ReqOrderAction(&req, requestId);
		}
		else
		{
			OPTIONHEDGE_LOG_ERROR(logger, "[request] order action failed!" << " (rid)" << requestId);
			break;
		}
	}
}

/*
 * SPI functions
 */

void TDEngineCTP::OnFrontConnected(CThostFtdcTraderApi* api)
{
	std::lock_guard<std::mutex> lock(mtx);
	for (size_t idx = 0; idx < account_units.size(); ++idx)
	{
		if (api == account_units[idx].api)
		{
			AccountUnitCTP& unit = account_units[idx];
			TradeAccount& account = accounts[idx];
			OPTIONHEDGE_LOG_INFO(logger, "[OnFrontConnected] new (idx)" << idx);
			unit.connected = true;
			if (need_authenticate && !unit.authenticated)
			{
				struct CThostFtdcReqAuthenticateField req = {};
				strcpy(req.BrokerID, account.BrokerID);
				strcpy(req.UserID, account.UserID);
				strcpy(req.AppID, unit.appID.c_str());
				strcpy(req.AuthCode, unit.auth_code.c_str());

				unit.auth_rid = request_id;
				int rCode = unit.api->ReqAuthenticate(&req, request_id++);

				while (rCode != 0)
				{
					if (rCode == -3)
					{
						OPTIONHEDGE_LOG_ERROR(logger, "[request] wait 1 sec before send ReqAuthenticate again!" << " (rid)" << request_id);
						sleep(1);
						rCode = unit.api->ReqAuthenticate(&req, request_id++);
					}
					else
					{
						OPTIONHEDGE_LOG_ERROR(logger, "[request] auth failed!" << " (Bid)" << req.BrokerID
							<< " (Uid)" << req.UserID
							<< " (Auth)" << req.AuthCode);
						break;
					}
				}
			}
			else if (!unit.logged_in)
			{
				struct CThostFtdcReqUserLoginField req = {};
				strcpy(req.TradingDay, "");
				strcpy(req.UserID, account.UserID);
				strcpy(req.BrokerID, account.BrokerID);
				strcpy(req.Password, account.Password);
				unit.login_rid = request_id;

				int rCode = unit.api->ReqUserLogin(&req, request_id++);
				while (rCode != 0)
				{
					if (rCode == -3)
					{
						OPTIONHEDGE_LOG_ERROR(logger, "[request] wait 1 sec before send ReqUserLogin again!" << " (rid)" << request_id);
						sleep(1);
						rCode = unit.api->ReqUserLogin(&req, request_id++);
					}
					else
					{
						OPTIONHEDGE_LOG_ERROR(logger, "[request] login failed!" << " (Bid)" << req.BrokerID
							<< " (Uid)" << req.UserID);
						break;
					}
				}
			}
		}
	}
}

void TDEngineCTP::OnFrontDisconnected(int nReason, CThostFtdcTraderApi* api)
{
	std::lock_guard<std::mutex> lock(mtx);
	for (size_t idx = 0; idx < account_units.size(); ++idx)
	{
		AccountUnitCTP& unit = account_units[idx];
		TradeAccount& account = accounts[idx];
		if (unit.api == api)
		{
			string username(account.UserID);
			pos_manage->clearPos(username);
			unit.connected = false;
			unit.authenticated = false;
			unit.settle_confirmed = false;
			unit.logged_in = false;
			unit.bYesterdayPosInitialized = false;
			unit.bTodayPosInitialized = false;
			unit.bOrderInitialized = false;
			OPTIONHEDGE_LOG_INFO(logger, "[OnFrontDisconnected] account " << account.UserID << " with reason=" << nReason);
		}
	}
}

#define GBK2UTF8(msg) optionHedge::memoryManage::gbk2utf8(string(msg))

void TDEngineCTP::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcTraderApi* api)
{
	std::lock_guard<std::mutex> lock(mtx);
	if (pRspInfo != nullptr && pRspInfo->ErrorID != 0)
	{
		OPTIONHEDGE_LOG_ERROR(logger, "[OnRspAuthenticate]" << " (errId)" << pRspInfo->ErrorID
			<< " (errMsg)" << GBK2UTF8(pRspInfo->ErrorMsg));
	}
	else
	{
		OPTIONHEDGE_LOG_INFO(logger, "[OnRspAuthenticate]" << " (userId)" << pRspAuthenticateField->UserID
			<< " (brokerId)" << pRspAuthenticateField->BrokerID
			<< " (product)" << pRspAuthenticateField->UserProductInfo
			<< " (rid)" << nRequestID);
		for (size_t idx = 0; idx < account_units.size(); ++idx)
		{
			AccountUnitCTP& unit = account_units[idx];
			TradeAccount& account = accounts[idx];

			if (unit.api != api)
				continue;

			unit.authenticated = true;
			if (!unit.logged_in)
			{
				struct CThostFtdcReqUserLoginField req = {};
				strcpy(req.TradingDay, "");
				strcpy(req.UserID, account.UserID);
				strcpy(req.BrokerID, account.BrokerID);
				strcpy(req.Password, account.Password);
				unit.login_rid = request_id;

				int rCode = unit.api->ReqUserLogin(&req, request_id++);
				while (rCode != 0)
				{
					if (rCode == -3)
					{
						OPTIONHEDGE_LOG_ERROR(logger, "[request] wait 1 sec before send ReqUserLogin again!" << " (rid)" << request_id);
						sleep(1);
						rCode = unit.api->ReqUserLogin(&req, request_id++);
					}
					else
					{
						OPTIONHEDGE_LOG_ERROR(logger, "[request] login failed!" << " (Bid)" << req.BrokerID
							<< " (Uid)" << req.UserID);
						break;
					}
				}
			}
		}
	}
}

void TDEngineCTP::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast, CThostFtdcTraderApi* api)
{
	std::lock_guard<std::mutex> lock(mtx);
	if (pRspInfo != nullptr && pRspInfo->ErrorID != 0)
	{
		OPTIONHEDGE_LOG_ERROR(logger, "[OnRspUserLogin]" << " (errId)" << pRspInfo->ErrorID
			<< " (errMsg)" << GBK2UTF8(pRspInfo->ErrorMsg));
	}
	else
	{
		OPTIONHEDGE_LOG_INFO(logger, "[OnRspUserLogin]" << " (Bid)" << pRspUserLogin->BrokerID
			<< " (Uid)" << pRspUserLogin->UserID
			<< " (maxRef)" << pRspUserLogin->MaxOrderRef
			<< " (Fid)" << pRspUserLogin->FrontID
			<< " (Sid)" << pRspUserLogin->SessionID);
		for (size_t idx = 0; idx < account_units.size(); ++idx)
		{
			AccountUnitCTP& unit = account_units[idx];
			TradeAccount& account = accounts[idx];

			if (unit.api == api)
			{
				unit.front_id = pRspUserLogin->FrontID;
				unit.session_id = pRspUserLogin->SessionID;
				unit.logged_in = true;

				td_helper->set_trade_date(pRspUserLogin->TradingDay);
				td_helper->deserilizeOrderInfo();
				if (need_settleConfirm && !unit.settle_confirmed)
				{
					CThostFtdcSettlementInfoConfirmField  req = {};
					memset(&req, 0, sizeof(req));
					strcpy(req.BrokerID, account.BrokerID);
					strcpy(req.InvestorID, account.InvestorID);
					unit.settle_rid = request_id;
					int rCode = unit.api->ReqSettlementInfoConfirm(&req, request_id++);
					while (rCode != 0)
					{
						if (rCode == -3)
						{
							OPTIONHEDGE_LOG_ERROR(logger, "[request] wait 1 sec before send ReqSettlementInfoConfirm again!" << " (rid)" << request_id);
							sleep(1);
							rCode = unit.api->ReqSettlementInfoConfirm(&req, request_id++);
						}
						else
						{
							OPTIONHEDGE_LOG_ERROR(logger, "[request] QrySettlementInfoConfirm failed!" << " (Bid)" << req.BrokerID
								<< " (Iid)" << req.InvestorID);
							break;
						}
					}
				}
			}
		}
		int max_ref = atoi(pRspUserLogin->MaxOrderRef) + 1;
		local_id = (max_ref > local_id) ? max_ref : local_id;
	}
}

void TDEngineCTP::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcTraderApi* api)
{
	std::lock_guard<std::mutex> lock(mtx);
	if (pRspInfo != nullptr && pRspInfo->ErrorID != 0)
	{
		OPTIONHEDGE_LOG_ERROR(logger, "[OnRspSettlementInfoConfirm]" << " (errId)" << pRspInfo->ErrorID
			<< " (errMsg)" << GBK2UTF8(pRspInfo->ErrorMsg));
	}
	else
	{
		OPTIONHEDGE_LOG_INFO(logger, "[OnRspSettlementInfoConfirm]" << " (brokerID)" << pSettlementInfoConfirm->BrokerID
			<< " (investorID)" << pSettlementInfoConfirm->InvestorID
			<< " (confirmDate)" << pSettlementInfoConfirm->ConfirmDate
			<< " (confirmTime)" << pSettlementInfoConfirm->ConfirmTime);

		for (size_t idx = 0; idx < account_units.size(); ++idx)
		{
			AccountUnitCTP& unit = account_units[idx];
			TradeAccount& account = accounts[idx];
			if (unit.api == api)
			{
				unit.settle_confirmed = true;
				OPTIONHEDGE_LOG_INFO(logger, "set settle confirmed to " << unit.settle_confirmed << " with rid " << unit.settle_rid);
				if (!unit.bOrderInitialized)
				{
					struct CThostFtdcQryOrderField req = {};
					memset(&req, 0, sizeof(req));
					strcpy(req.BrokerID, account.BrokerID);
					strcpy(req.InvestorID, account.InvestorID);
					unit.setOrder_rid = request_id;
					int rCode = unit.api->ReqQryOrder(&req, request_id++);
					
					while (rCode != 0)
					{
						if (rCode == -3)
						{
							OPTIONHEDGE_LOG_ERROR(logger, "[request] wait 1 sec before send ReqQryOrder again!" << " (rid)" << request_id);
							sleep(1);
							rCode = unit.api->ReqQryOrder(&req, request_id);
						}
						else
						{
							OPTIONHEDGE_LOG_ERROR(logger, "[request] ReqQryOrder failed!" << " (Bid)" << req.BrokerID
								<< " (Iid)" << req.InvestorID);
							break;
						}
					}
				}
			}
		}
	}
}

void TDEngineCTP::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast, CThostFtdcTraderApi* api)
{
	std::lock_guard<std::mutex> lock(mtx);
	if (pRspInfo != nullptr && pRspInfo->ErrorID == 0)
	{
		OPTIONHEDGE_LOG_ERROR(logger, "[OnRspUserLogout]" << " (errId)" << pRspInfo->ErrorID
			<< " (errMsg)" << GBK2UTF8(pRspInfo->ErrorMsg));
	}
	else
	{
		OPTIONHEDGE_LOG_INFO(logger, "[OnRspUserLogout]" << " (brokerId)" << pUserLogout->BrokerID
			<< " (userId)" << pUserLogout->UserID);

		for (auto& unit : account_units)
		{
			if (unit.api == api)
			{
				unit.logged_in = false;
				unit.authenticated = false;
				unit.settle_confirmed = false;
			}
		}
	}
}

void TDEngineCTP::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast, CThostFtdcTraderApi* api)
{
	std::lock_guard<std::mutex> lock(mtx);
	int errorId = (pRspInfo == nullptr) ? 0 : pRspInfo->ErrorID;
	const char* errorMsg = nullptr;

	//logic should be here not in TDPosmanage,using setValue instead since it need to get tdOrderInfo for some help
	for (size_t idx = 0; idx < account_units.size(); ++idx)
	{
		AccountUnitCTP& unit = account_units[idx];
		TradeAccount& account = accounts[idx];

		if (unit.api == api)
		{
			if (unit.accountType == "CTP")
			{
				errorMsg = (pRspInfo == nullptr) ? nullptr : EngineUtil::gbkErrorMsg2utf8(pRspInfo->ErrorMsg);
			}
			else
			{
				errorMsg = (pRspInfo == nullptr) ? nullptr : EngineUtil::gbkErrorMsg2utf8(pRspInfo->ErrorMsg);
			}
			string orderRef = pInputOrder->OrderRef;
			char offsetFlag = pInputOrder->CombOffsetFlag[0];

			string searchDir;
			if (offsetFlag != THOST_FTDC_OF_Open)
			{
				(pInputOrder->Direction == THOST_FTDC_D_Buy) ? searchDir = THOST_FTDC_PD_Short : searchDir = THOST_FTDC_PD_Long;
				string searchKey = string(pInputOrder->InstrumentID) + "_" + pInputOrder->CombHedgeFlag[0] + "_" + searchDir;
				string username = account.UserID;
				string exchangeID(pInputOrder->ExchangeID);

				ValueType valueType;
				if (pInputOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Close || pInputOrder->CombOffsetFlag[0] == THOST_FTDC_OF_CloseYesterday)
				{
					if (exchangeID == string("CZCE") || exchangeID == string("DCE") || exchangeID == string("CFFEX"))
					{
						valueType = lockClose;
					}
					else if (exchangeID == string("SHFE") || exchangeID == string("INE"))
					{
						valueType = lockYesterdayClose;
					}
				}
				else if (pInputOrder->CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday)
				{
					valueType = lockTodayClose;
				}
				pos_manage->setValue(username, searchKey, valueType, Sub, pInputOrder->VolumeTotalOriginal);
				pos_manage->display();
			}
		}
	}

	
	auto data = parseFrom(*pInputOrder);
	on_rsp_order_insert(&data, nRequestID, errorId, errorMsg);
}

void TDEngineCTP::OnRspOrderAction(CThostFtdcOrderActionField *pOrderAction,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast, CThostFtdcTraderApi* api)
{
	std::lock_guard<std::mutex> lock(mtx);
	int errorId = (pRspInfo == nullptr) ? 0 : pRspInfo->ErrorID;
	const char* errorMsg = nullptr;
	for (size_t idx = 0; idx < account_units.size(); ++idx)
	{
		AccountUnitCTP& unit = account_units[idx];
		TradeAccount& account = accounts[idx];

		if (unit.api == api)
		{
			if (unit.accountType == "CTP")
			{
				errorMsg = (pRspInfo == nullptr) ? nullptr : EngineUtil::gbkErrorMsg2utf8(pRspInfo->ErrorMsg);
			}
			else
			{
				errorMsg = (pRspInfo == nullptr) ? nullptr : EngineUtil::gbkErrorMsg2utf8(pRspInfo->ErrorMsg);
			}
		}
	}
	auto data = parseFrom(*pOrderAction);
	on_rsp_order_action(&data, nRequestID, errorId, errorMsg);
}

void TDEngineCTP::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast, CThostFtdcTraderApi* api)
{
	std::lock_guard<std::mutex> lock(mtx);
	const char* errorMsg = nullptr;
	for (size_t idx = 0; idx < account_units.size(); ++idx)
	{
		AccountUnitCTP& unit = account_units[idx];
		TradeAccount& account = accounts[idx];

		if (unit.api == api)
		{
			if (unit.accountType == "CTP")
			{
				errorMsg = (pRspInfo == nullptr) ? nullptr : EngineUtil::gbkErrorMsg2utf8(pRspInfo->ErrorMsg);
			}
			else
			{
				errorMsg = (pRspInfo == nullptr) ? nullptr : EngineUtil::gbkErrorMsg2utf8(pRspInfo->ErrorMsg);
			}
		}

		if (unit.api == api && !unit.bYesterdayPosInitialized)//&& unit.setYesterdayPos_rid==nRequestID possible fault in test env
		{
			if (pInvestorPosition == nullptr)
			{
				OPTIONHEDGE_LOG_INFO(logger, "POS info after setYesterday Position,all Done");
				unit.bYesterdayPosInitialized = true;
				pos_manage->display();
			}
			else
			{
				string userID(account.UserID);
				string instrumentKey = string(pInvestorPosition->InstrumentID) + "_" + pInvestorPosition->HedgeFlag + "_" + pInvestorPosition->PosiDirection;

				OPTIONHEDGE_LOG_DEBUG(logger, "SET POS in TDEngineCTP with UserID " << account.UserID << " with Key " << instrumentKey << " and value " << pInvestorPosition->YdPosition);
				pos_manage->setYesterdayPos(userID, instrumentKey, pInvestorPosition->YdPosition);


				if (bIsLast)
				{
					OPTIONHEDGE_LOG_INFO(logger, "POS info after setYesterday Position,all Done");
					pos_manage->display(userID);
					unit.bYesterdayPosInitialized = true;
				}
			}
		}
	}

	int errorId = (pRspInfo == nullptr) ? 0 : pRspInfo->ErrorID;
	CThostFtdcInvestorPositionField emptyCtp = {};
	if (pInvestorPosition == nullptr)
		pInvestorPosition = &emptyCtp;
	auto pos = parseFrom(*pInvestorPosition);
	on_rsp_position(&pos, bIsLast, nRequestID, errorId, errorMsg);
}

void TDEngineCTP::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcTraderApi* api)
{
	std::lock_guard<std::mutex> lock(mtx);
	const char* errorMsg = nullptr;
	for (size_t idx = 0; idx < account_units.size(); ++idx)
	{
		AccountUnitCTP& unit = account_units[idx];
		TradeAccount& account = accounts[idx];
		string userID(account.UserID);
		
		if (unit.api == api)
		{
			if (unit.accountType == "CTP")
			{
				errorMsg = (pRspInfo == nullptr) ? nullptr : EngineUtil::gbkErrorMsg2utf8(pRspInfo->ErrorMsg);
			}
			else
			{
				errorMsg = (pRspInfo == nullptr) ? nullptr : EngineUtil::gbkErrorMsg2utf8(pRspInfo->ErrorMsg);
			}
		}

		if (unit.api == api && unit.setOrder_rid == nRequestID)
		{
			if (pOrder == nullptr || string(pOrder->InstrumentID) == "")
			{
				unit.bOrderInitialized = true;
				OPTIONHEDGE_LOG_DEBUG(logger, "Qry Order finished,Display Pos detail");
				pos_manage->display(userID);

				struct CThostFtdcQryTradeField req = {};
				memset(&req, 0, sizeof(req));
				strcpy(req.BrokerID, account.BrokerID);
				strcpy(req.InvestorID, account.InvestorID);

				unit.setTodayPos_rid = request_id;
				int rCode = unit.api->ReqQryTrade(&req, request_id++);


				while (rCode != 0)
				{
					if (rCode == -3)
					{
						OPTIONHEDGE_LOG_ERROR(logger, "[request] wait 1 sec before send ReqQryTrade again to initial pos manage!" << " (rid)" << request_id);
						sleep(1);
						rCode = unit.api->ReqQryTrade(&req, request_id);
					}
					else
					{
						OPTIONHEDGE_LOG_ERROR(logger, "[request] ReqQryTrade failed!" << " (Bid)" << req.BrokerID
							<< " (Iid)" << req.InvestorID);
						break;
					}
				}
			}
			else
			{
				string instrumentKey = string(pOrder->InstrumentID) + "_" + pOrder->CombHedgeFlag[0] + "_" + pos_manage->calPositionDirection(pOrder->Direction, pOrder->CombOffsetFlag[0]);
				string exchangeID(pOrder->ExchangeID);
				string orderRef(pOrder->OrderRef);

				string lockRef = userID + "_" + orderRef+"_"+to_string(pOrder->FrontID)+"_"+to_string(pOrder->SessionID);
				if (!pos_manage->checkLockInfo(lockRef))
				{
					if (!(pOrder->OrderStatus == THOST_FTDC_OST_Canceled))
					{
						if (pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Close || pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_CloseYesterday)
						{
							if (exchangeID == string("CZCE") || exchangeID == string("DCE") || exchangeID == string("CFFEX"))
							{
								pos_manage->setValue(userID, instrumentKey, lockClose, Add, pOrder->VolumeTotalOriginal);
							}
							else if (exchangeID == string("SHFE") || exchangeID == string("INE"))
							{
								pos_manage->setValue(userID, instrumentKey, lockYesterdayClose, Add, pOrder->VolumeTotalOriginal);
							}
						}
						else if (pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday)
						{
							pos_manage->setValue(userID, instrumentKey, lockTodayClose, Add, pOrder->VolumeTotalOriginal);
						}
					}
					
					pos_manage->setLockInfo(lockRef);
				}


				string orderKey = orderRef + "_"+to_string(pOrder->FrontID) +"_"+ to_string(pOrder->SessionID);

				if (td_helper->get_order_id(orderKey.c_str()) == -1)
				{
					int dsRid = -2;
					auto it = directStratgeRid.find(userID);
					if (it != directStratgeRid.end())
					{
						dsRid = it->second + REQUEST_ID_RANGE - 100;
					}
					
					td_helper->record_order(orderKey.c_str(), dsRid, pOrder->OrderStatus, orderKey.length()+1);
					td_helper->record_order_orderSysID(orderKey.c_str(), pOrder->OrderSysID, 21);
				}

				else
				{
					td_helper->set_order_status(orderKey.c_str(), pOrder->OrderStatus);
					td_helper->record_order_orderSysID(orderKey.c_str(), pOrder->OrderSysID, 21);
				}

				if (bIsLast)
				{
					unit.bOrderInitialized = true;
					OPTIONHEDGE_LOG_DEBUG(logger, "Display all the trade after qry order");
					pos_manage->display(userID);

					struct CThostFtdcQryTradeField req = {};
					memset(&req, 0, sizeof(req));
					strcpy(req.BrokerID, account.BrokerID);
					strcpy(req.InvestorID, account.InvestorID);

					unit.setTodayPos_rid = request_id;
					int rCode = unit.api->ReqQryTrade(&req, request_id++);


					while (rCode != 0)
					{
						if (rCode == -3)
						{
							OPTIONHEDGE_LOG_ERROR(logger, "[request] wait 1 sec before send ReqQryTrade again to initial pos manage!" << " (rid)" << request_id);
							sleep(1);
							rCode = unit.api->ReqQryTrade(&req, request_id);
						}
						else
						{
							OPTIONHEDGE_LOG_ERROR(logger, "[request] ReqQryTrade failed!" << " (Bid)" << req.BrokerID
								<< " (Iid)" << req.InvestorID);
							break;
						}
					}
				}
			}
		}
	}

	int errorId = (pRspInfo == nullptr) ? 0 : pRspInfo->ErrorID;
	CThostFtdcOrderField emptyCtp = {};
	if (pOrder == nullptr)
	{
		pOrder = &emptyCtp;
	}
	auto order = parseFrom(*pOrder);
	on_rsp_qry_order(&order, bIsLast, nRequestID, errorId, errorMsg);
}

void TDEngineCTP::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcTraderApi* api)
{
	std::lock_guard<std::mutex> lock(mtx);
	const char* errorMsg = nullptr;
	for (size_t idx = 0; idx < account_units.size(); ++idx)
	{
		AccountUnitCTP& unit = account_units[idx];
		TradeAccount& account = accounts[idx];
		string userID(account.UserID);
		
		if (unit.api == api)
		{
			if (unit.accountType == "CTP")
			{
				errorMsg = (pRspInfo == nullptr) ? nullptr : EngineUtil::gbkErrorMsg2utf8(pRspInfo->ErrorMsg);
			}
			else
			{
				errorMsg = (pRspInfo == nullptr) ? nullptr : EngineUtil::gbkErrorMsg2utf8(pRspInfo->ErrorMsg);
			}
		}

		if (unit.api == api && !unit.bTodayPosInitialized && unit.setTodayPos_rid == nRequestID)
		{
			if (pTrade == nullptr || string(pTrade->InstrumentID) == "")
			{
				unit.bTodayPosInitialized = true;
				OPTIONHEDGE_LOG_DEBUG(logger, "Display all the trade after qry trader");
				pos_manage->display(userID);

				struct CThostFtdcQryInvestorPositionField req = {};
				memset(&req, 0, sizeof(req));
				strcpy(req.BrokerID, account.BrokerID);
				strcpy(req.InvestorID, account.InvestorID);
				unit.setYesterdayPos_rid = request_id;
				int rCode = unit.api->ReqQryInvestorPosition(&req, request_id++);
				while (rCode != 0)
				{
					if (rCode == -3)
					{
						OPTIONHEDGE_LOG_ERROR(logger, "[request] wait 1 sec before send ReqQryInvestorPosition again to initial pos manage!" << " (rid)" << request_id);
						sleep(1);
						rCode = unit.api->ReqQryInvestorPosition(&req, request_id);
					}
					else
					{
						OPTIONHEDGE_LOG_ERROR(logger, "[request] ReqQryInvestorPosition failed!" << " (Bid)" << req.BrokerID
							<< " (Iid)" << req.InvestorID);
						break;
					}
				}
			}
			else
			{
				string instrumentKey = string(pTrade->InstrumentID) + "_" + pTrade->HedgeFlag + "_" + pos_manage->calPositionDirection(pTrade->Direction, pTrade->OffsetFlag);
				pos_manage->setTodayPos(userID, instrumentKey, pTrade->OffsetFlag, pTrade->Volume);
				

				if (bIsLast)
				{
					unit.bTodayPosInitialized = true;
					OPTIONHEDGE_LOG_DEBUG(logger, "Display all the trade after qry trader");
					pos_manage->display(userID);
					struct CThostFtdcQryInvestorPositionField req = {};
					memset(&req, 0, sizeof(req));
					strcpy(req.BrokerID, account.BrokerID);
					strcpy(req.InvestorID, account.InvestorID);
					unit.setYesterdayPos_rid = request_id;
					int rCode = unit.api->ReqQryInvestorPosition(&req, request_id++);
					while (rCode != 0)
					{
						if (rCode == -3)
						{
							OPTIONHEDGE_LOG_ERROR(logger, "[request] wait 1 sec before send ReqQryInvestorPosition again to initial pos manage!" << " (rid)" << request_id);
							sleep(1);
							rCode = unit.api->ReqQryInvestorPosition(&req, request_id);
						}
						else
						{
							OPTIONHEDGE_LOG_ERROR(logger, "[request] ReqQryInvestorPosition failed!" << " (Bid)" << req.BrokerID
								<< " (Iid)" << req.InvestorID);
							break;
						}
					}
				}
			}
		}
	}
	int errorId = (pRspInfo == nullptr) ? 0 : pRspInfo->ErrorID;
	CThostFtdcTradeField emptyCtp = {};
	if (pTrade == nullptr)
	{
		pTrade = &emptyCtp;
	}
	auto trade = parseFrom(*pTrade);
	on_rsp_qry_trade(&trade, bIsLast, nRequestID, errorId, errorMsg);
}

void TDEngineCTP::OnRtnOrder(CThostFtdcOrderField *pOrder, CThostFtdcTraderApi* api)
{
	std::lock_guard<std::mutex> lock(mtx);
	for (size_t idx = 0; idx < account_units.size(); ++idx)
	{
		AccountUnitCTP& unit = account_units[idx];
		TradeAccount& account = accounts[idx];
		
		string userID(account.UserID);
		string exchangeID(pOrder->ExchangeID);
		string orderRef(pOrder->OrderRef);
		string orderKey = orderRef + "_" + to_string(pOrder->FrontID) + "_" + to_string(pOrder->SessionID);

		if (unit.api == api && pOrder->CombOffsetFlag[0] != THOST_FTDC_OF_Open)
		{
			string searchDir;
			(pOrder->Direction == THOST_FTDC_D_Buy) ? searchDir = THOST_FTDC_PD_Short : searchDir = THOST_FTDC_PD_Long;
			string searchKey = string(pOrder->InstrumentID) + "_" + pOrder->CombHedgeFlag[0] + "_" + searchDir;
			
			string lockRef = userID + "_" + orderRef+"_"+to_string(pOrder->FrontID)+"_"+to_string(pOrder->SessionID);
			
			//Get rsp for the order from other terminal
			if (td_helper->get_order_id(orderKey.c_str()) == -1)
			{
				OPTIONHEDGE_LOG_DEBUG(logger, "Receive order from other terminal with orderRef " << orderKey
					<< " InstrumentID " << pOrder->InstrumentID << " direction " << pOrder->Direction <<
					" and volume " << pOrder->VolumeTotalOriginal);

				if (pOrder->OrderStatus != THOST_FTDC_OST_Canceled)
				{
					if (pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Close || pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_CloseYesterday)
					{
						if (exchangeID == string("CZCE") || exchangeID == string("DCE") || exchangeID == string("CFFEX"))
						{
							pos_manage->setValue(userID, searchKey, lockClose, Add, pOrder->VolumeTotalOriginal);
						}
						else if (exchangeID == string("SHFE") || exchangeID == string("INE"))
						{
							pos_manage->setValue(userID, searchKey, lockYesterdayClose, Add, pOrder->VolumeTotalOriginal);
						}
					}
					else if (pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday)
					{
						pos_manage->setValue(userID, searchKey, lockTodayClose, Add, pOrder->VolumeTotalOriginal);
					}
					pos_manage->setLockInfo(lockRef);
					pos_manage->display(userID);
				}
			}

			else
			{
				if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled && td_helper->get_order_info(orderKey.c_str()) != nullptr)
				{
					TDBasicOrderInfo* orderInfo = td_helper->get_order_info(orderKey.c_str());
					if (!pos_manage->checkLockInfo(lockRef))
					{
						OPTIONHEDGE_LOG_ERROR(logger, "can not the lockinfo for order with orderRef " << pOrder->OrderRef);
					}
					else
					{
						lockInfo li = pos_manage->getLockInfo(lockRef);

						if ((pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Close || pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_CloseYesterday) && !li.bunlocked)
						{
							if (exchangeID == string("CZCE") || exchangeID == string("DCE") || exchangeID == string("CFFEX"))
							{
								pos_manage->setValue(userID, searchKey, lockClose, Sub, pOrder->VolumeTotal);
							}
							else if (exchangeID == string("SHFE") || exchangeID == string("INE"))
							{
								pos_manage->setValue(userID, searchKey, lockYesterdayClose, Sub, pOrder->VolumeTotal);
							}
						}
						else if (pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday && !li.bunlocked)
						{
							pos_manage->setValue(userID, searchKey, lockTodayClose, Sub, pOrder->VolumeTotal);
						}


						OPTIONHEDGE_LOG_DEBUG(logger, "unlock order " << pOrder->OrderRef
							<< " InstrumentID " << pOrder->InstrumentID << " direction " << pOrder->Direction <<
							" and volume " << pOrder->VolumeTotalOriginal);
						pos_manage->display(userID);
						pos_manage->setLockInfo(lockRef, true, true);
					}

				}
			}
		}
		if (unit.api == api && td_helper->get_order_id(orderKey.c_str()) == -1)
		{
			int dsRid = -2;
			auto it = directStratgeRid.find(userID);
			if (it != directStratgeRid.end())
			{
				dsRid = it->second + REQUEST_ID_RANGE - 100;
			}

			td_helper->record_order(orderKey.c_str(), dsRid, orderKey.length()+1);
			td_helper->record_order_orderSysID(orderKey.c_str(), pOrder->OrderSysID, 21);
			OPTIONHEDGE_LOG_DEBUG(logger, "set orderKey for " << orderKey << " with Rid "<< dsRid<<" in OnRtnOrder");
		}
		else if (unit.api == api && td_helper->get_order_id(orderKey.c_str()) != -1)
		{
			td_helper->set_order_status(orderKey.c_str(), pOrder->OrderStatus);
			td_helper->record_order_orderSysID(orderKey.c_str(), pOrder->OrderSysID, 21);
		}
	}

	auto rtn_order = parseFrom(*pOrder);
	on_rtn_order(&rtn_order);
}

void TDEngineCTP::OnRtnTrade(CThostFtdcTradeField *pTrade, CThostFtdcTraderApi* api)
{
	std::lock_guard<std::mutex> lock(mtx);
	for (size_t idx = 0; idx < account_units.size(); ++idx)
	{
		AccountUnitCTP& unit = account_units[idx];
		TradeAccount& account = accounts[idx];
		string userID(account.UserID);
		
		if (unit.api == api)
		{
			if (td_helper->get_order_id(pTrade->OrderSysID, OrderSysId) == -1)
			{
				string orderRef(pTrade->OrderRef);
				orderRef = orderRef + "_" + to_string(unit.front_id) + "_" + to_string(unit.session_id);
				//in case onRtnTrade comes before onRtnOrder,then no orderSysID info available at this time
				//check orderRef with frontid and sessionid of this user to see if the order is from our system
				if (td_helper->get_order_id(orderRef.c_str()) == -1)
				{
					int dsRid = -2;
					auto it = directStratgeRid.find(userID);
					if (it != directStratgeRid.end())
					{
						dsRid = it->second + REQUEST_ID_RANGE - 100;
					}

					td_helper->record_order(orderRef.c_str(), dsRid, orderRef.length()+1);
					td_helper->record_order_orderSysID(orderRef.c_str(),pTrade->OrderSysID, 21);
					OPTIONHEDGE_LOG_DEBUG(logger, "set orderRef for " << pTrade->OrderRef << " with Rid in OnRtnTrade" << dsRid);
				}
			}
			string userID(account.UserID);
			string instrumentKey = string(pTrade->InstrumentID) + "_" + pTrade->HedgeFlag + "_" + pos_manage->calPositionDirection(pTrade->Direction, pTrade->OffsetFlag);
			pos_manage->setTodayPos(userID, instrumentKey, pTrade->OffsetFlag, pTrade->Volume);
			pos_manage->display();
		}
	}
	auto rtn_trade = parseFrom(*pTrade);
	on_rtn_trade(&rtn_trade);
}

void TDEngineCTP::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcTraderApi* api)
{
	std::lock_guard<std::mutex> lock(mtx);
	int errorId = (pRspInfo == nullptr) ? 0 : pRspInfo->ErrorID;
	const char* errorMsg = nullptr; 
	CThostFtdcTradingAccountField empty = {};

	for (size_t idx = 0; idx < account_units.size(); ++idx)
	{
		AccountUnitCTP& unit = account_units[idx];
		
		if (unit.api == api)
		{
			if (unit.accountType == "CTP")
			{
				errorMsg = (pRspInfo == nullptr) ? nullptr : EngineUtil::gbkErrorMsg2utf8(pRspInfo->ErrorMsg);
			}
			else
			{
				errorMsg = (pRspInfo == nullptr) ? nullptr : EngineUtil::gbkErrorMsg2utf8(pRspInfo->ErrorMsg);
			}
		}
	}
	
	if (pTradingAccount == nullptr)
	{
		pTradingAccount = &empty;
	}

	auto account = parseFrom(*pTradingAccount);
	on_rsp_account(&account, bIsLast, nRequestID, errorId, errorMsg);
}

void TDEngineCTP::OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus, CThostFtdcTraderApi* api)
{
	std::lock_guard<std::mutex> lock(mtx);
	for (size_t idx = 0; idx < account_units.size(); ++idx)
	{
		AccountUnitCTP& unit = account_units[idx];
		if (unit.api == api)
		{
			TradeAccount& account = accounts[idx];
			string userID(account.UserID);

			auto rspInsrument = parseFrom(*pInstrumentStatus);
			on_rtn_instrument_status(&rspInsrument,userID);
		}
	}
}

void TDEngineCTP::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcTraderApi* api)
{
	std::lock_guard<std::mutex> lock(mtx);
	if (pRspInfo)
	{
		OPTIONHEDGE_LOG_ERROR(logger, "OnRspError[ErrorID] " << pRspInfo->ErrorID << "[ErrorMsg] " << pRspInfo->ErrorMsg << "[nRequest=] " << nRequestID << "[bIsLast=%d] " << bIsLast);
	}
	else
	{
		OPTIONHEDGE_LOG_ERROR(logger, "OnRspError[nReqeustID] " << nRequestID << "[bIsLast] " << bIsLast);
	}
}
