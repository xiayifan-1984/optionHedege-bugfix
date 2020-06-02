#ifndef TRADINGENGINE_TDENGINESPI_H
#define TRADINGENGINE_TDENGINESPI_H

#include "ITDEngine.h"
#include "TDEngineCTP.h"
#include "../base/TradingConstant.h"
#include "../../system/api/ctp/v6.3.15/include/ThostFtdcTraderApi.h"
#include "../../memoryManage/log/MM_Log.h"
#include "../../memoryManage/utils/json.hpp"

using json = nlohmann::json;

TE_NAMESPACE_START
class TDEngineCTP;
class TDEngineSpi :public CThostFtdcTraderSpi
{
private:
	TradeAccount tdAccount;
	TDEngineCTP* pTDEngine;
	string appID;
	string authCode;
	CThostFtdcTraderApi* api;
	LogPtr logger;
public:
	TDEngineSpi() {}
	TDEngineSpi(TradeAccount tradeAccont,string appID,string authCode, CThostFtdcTraderApi* pApi, TDEngineCTP* pTDEngine);
	
	virtual void OnFrontConnected();
	virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField,CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnFrontDisconnected(int nReason);
	///登出请求响应
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///报单录入请求响应 (this only be called when there is error)
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///报单操作请求响应
	virtual void OnRspOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询投资者持仓响应
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///报单通知
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	///成交通知
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	///请求查询资金账户响应
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///合约交易状态通知
	virtual void OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus);
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

};

TE_NAMESPACE_END
#endif
