#include "MDEngineCTP.h"
#include "../../memoryManage/utils/util.hpp"
#include "../../memoryManage/utils/Timer.h"
#include "../base/ctp.h"
#include "../base/EngineUtil.hpp"
#include "MDEngineSpi.h"

USING_TE_NAMESPACE

MDEngineCTP :: MDEngineCTP() : IMDEngine(SOURCE_CTP), api(nullptr), connected(false), logged_in(false), reqId(0)
{
	logger = optionHedge::memoryManage::Log::getLogger("CtpMD");
}

void MDEngineCTP::load(const json& j_config)
{
	broker_id = j_config[TE_CONFIG_KEY_MD][TE_CONFIG_KEY_SOURCECTP][TE_CONFIG_KEY_BROKER_ID].get<string>();
	user_id= j_config[TE_CONFIG_KEY_MD][TE_CONFIG_KEY_SOURCECTP][TE_CONFIG_KEY_USER_ID].get<string>();
	password=j_config[TE_CONFIG_KEY_MD][TE_CONFIG_KEY_SOURCECTP][TE_CONFIG_KEY_PASSWORD].get<string>();
	front_uri= j_config[TE_CONFIG_KEY_MD][TE_CONFIG_KEY_SOURCECTP][TE_CONFIG_KEY_FRONT_URI].get<string>();
}

void MDEngineCTP::connect(long timeout_nsec)
{
	typedef CThostFtdcMdApi*(*createapiFunctor)(const char* ,const bool,const bool);
	
	if (api == nullptr)
	{
		createapiFunctor cf = (createapiFunctor)dlsym(mdapiHandler, "_ZN15CThostFtdcMdApi15CreateFtdcMdApiEPKcbb");
		char* error;
		if ((error = dlerror()) != NULL)
		{
			std::cout << error << std::endl;
			OPTIONHEDGE_LOG_ERROR(logger, error);
			std::runtime_error("CTP_MD failed to load function for create api");
		}
		api = cf("",false,false);
		if (!api)
		{
			throw std::runtime_error("CTP_MD failed to create api");
		}
		MDEngineSpi* spi = new MDEngineSpi(api, this);
		api->RegisterSpi(spi);
	}
	if (!connected)
	{
		api->RegisterFront((char*)front_uri.c_str());
		api->Init();
	}
}

/*
void MDEngineCTP::login(long timeout_nsec)
{
	if (!logged_in)
	{
		CThostFtdcReqUserLoginField req = {};
		strcpy(req.BrokerID, broker_id.c_str());
		strcpy(req.UserID, user_id.c_str());
		strcpy(req.Password,password.c_str());

		if (api->ReqUserLogin(&req, reqId++))
		{
			OPTIONHEDGE_LOG_ERROR(logger, "[request] login failed!" << " (Bid)" << req.BrokerID<< " (Uid)" << req.UserID);
		}

		long start_time = optionHedge::memoryManage::getNanoTime();
		while (!logged_in && optionHedge::memoryManage::getNanoTime() - start_time < timeout_nsec)
		{

		}
	}
}
*/
void MDEngineCTP::logout()
{
	if (logged_in)
	{
		CThostFtdcUserLogoutField req = {};
		strcpy(req.BrokerID, broker_id.c_str());
		strcpy(req.UserID, user_id.c_str());
		if (api->ReqUserLogout(&req, reqId++))
		{
			OPTIONHEDGE_LOG_ERROR(logger, "[request] logout failed!" << " (Bid)" << req.BrokerID<< " (Uid)" << req.UserID);
		}
	}

	connected = false;
	logged_in = false;
}

void MDEngineCTP::release_api()
{
	if (api != nullptr)
	{
		api->Release();
		api = nullptr;
	}
}

void MDEngineCTP::subscribeMarketData(const vector<string>& instruments, const vector<string>& markets)
{
	int nCount = instruments.size();
	char* insts[nCount];
	for (int i = 0; i < nCount; i++)
	{
		insts[i] = (char*)instruments[i].c_str();
	}
		
	api->SubscribeMarketData(insts, nCount);
}

/*
 * SPI functions
 */

void MDEngineCTP::OnFrontConnected(CThostFtdcMdApi* api)
{
	OPTIONHEDGE_LOG_INFO(logger, "[OnFrontConnected]");
	connected = true;

	if (!logged_in)
	{
		CThostFtdcReqUserLoginField req = {};
		strcpy(req.BrokerID, broker_id.c_str());
		strcpy(req.UserID, user_id.c_str());
		strcpy(req.Password, password.c_str());

		int rCode = api->ReqUserLogin(&req, reqId++);
		while (rCode != 0)
		{
			if (rCode == -3)
			{
				OPTIONHEDGE_LOG_INFO(logger, "[request] wait 1 sec before send ReqUserLogin again!");
				sleep(1);
				rCode = api->ReqUserLogin(&req, reqId);
			}
			else
			{
				OPTIONHEDGE_LOG_ERROR(logger, "[request] login failed!" << " (Bid)" << req.BrokerID << " (Uid)" << req.UserID);
				break;
			}
		}
	}
}

void MDEngineCTP::OnFrontDisconnected(int nReason, CThostFtdcMdApi* api)
{
	OPTIONHEDGE_LOG_INFO(logger, "[OnFrontDisconnected] reason=" << nReason);
	connected = false;
	logged_in = false;
}

#define GBK2UTF8(msg) optionHedge::memoryManage::gbk2utf8(string(msg))

void MDEngineCTP::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcMdApi* api)
{
	if (pRspInfo != nullptr && pRspInfo->ErrorID != 0)
	{
		OPTIONHEDGE_LOG_ERROR(logger, "[OnRspUserLogin]" << " (errID)" << pRspInfo->ErrorID
			<< " (errMsg)" << GBK2UTF8(pRspInfo->ErrorMsg));
	}
	else
	{
		OPTIONHEDGE_LOG_INFO(logger, "[OnRspUserLogin]" << " (Bid)" << pRspUserLogin->BrokerID
			<< " (Uid)" << pRspUserLogin->UserID
			<< " (SName)" << pRspUserLogin->SystemName);
		logged_in = true;
		subscribeHistorySubs();
	}
}

void MDEngineCTP::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcMdApi* api)
{
	if (pRspInfo != nullptr && pRspInfo->ErrorID != 0)
	{
		OPTIONHEDGE_LOG_ERROR(logger, "[OnRspUserLogout]" << " (errID)" << pRspInfo->ErrorID
			<< " (errMsg)" << GBK2UTF8(pRspInfo->ErrorMsg));
	}
	else
	{
		OPTIONHEDGE_LOG_INFO(logger, "[OnRspUserLogout]" << " (Bid)" << pUserLogout->BrokerID
			<< " (Uid)" << pUserLogout->UserID);
		logged_in = false;
	}
}

void MDEngineCTP::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcMdApi* api)
{
	if (pRspInfo != nullptr && pRspInfo->ErrorID != 0)
	{
		OPTIONHEDGE_LOG_ERROR(logger, "[OnRspSubMarketData]" << " (errID)" << pRspInfo->ErrorID
			<< " (errMsg)" << GBK2UTF8(pRspInfo->ErrorMsg)
			<< " (Tid)" << ((pSpecificInstrument != nullptr) ?
				pSpecificInstrument->InstrumentID : "null"));
	}
}

void MDEngineCTP::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcMdApi* api)
{
	auto data = parseFrom(*pDepthMarketData);
	on_market_data(&data);
}
