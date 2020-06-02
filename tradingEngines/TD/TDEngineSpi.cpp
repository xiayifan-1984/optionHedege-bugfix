#include "TDEngineSpi.h"

USING_TE_NAMESPACE

TDEngineSpi::TDEngineSpi(TradeAccount tradeAccont, string appID, string authCode, CThostFtdcTraderApi* pApi, TDEngineCTP* pTDEngine):tdAccount(tradeAccont), appID(appID), authCode(authCode),api(pApi),pTDEngine(pTDEngine)
{
	logger = optionHedge::memoryManage::Log::getLogger("tradingEngine");
	OPTIONHEDGE_LOG_INFO(logger, "basic account info in spi,userID " << tradeAccont.UserID<<" appID is "<<appID);
}

void TDEngineSpi::OnFrontConnected()
{
	pTDEngine->OnFrontConnected(api);
}

void TDEngineSpi::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	pTDEngine->OnRspAuthenticate(pRspAuthenticateField, pRspInfo, nRequestID, bIsLast,api);
}

void TDEngineSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	pTDEngine->OnRspUserLogin(pRspUserLogin, pRspInfo, nRequestID, bIsLast, api);
}

void TDEngineSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	pTDEngine->OnRspSettlementInfoConfirm(pSettlementInfoConfirm, pRspInfo, nRequestID, bIsLast, api);
}

void TDEngineSpi::OnFrontDisconnected(int nReason)
{
	pTDEngine->OnFrontDisconnected(nReason, api);
}

void TDEngineSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	pTDEngine->OnRspUserLogout(pUserLogout, pRspInfo, nRequestID, bIsLast, api);
}

void TDEngineSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	pTDEngine->OnRspOrderInsert(pInputOrder, pRspInfo, nRequestID, bIsLast,api);
}

void TDEngineSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	pTDEngine->OnRspQryOrder(pOrder, pRspInfo, nRequestID, bIsLast,api);
}

void TDEngineSpi::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	pTDEngine->OnRspQryTrade(pTrade, pRspInfo, nRequestID, bIsLast,api);
}

void TDEngineSpi::OnRspOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	pTDEngine->OnRspOrderAction(pOrderAction, pRspInfo, nRequestID, bIsLast, api);
}

void TDEngineSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	pTDEngine->OnRspQryInvestorPosition(pInvestorPosition, pRspInfo, nRequestID, bIsLast,api);
}

void TDEngineSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	pTDEngine->OnRtnOrder(pOrder, api);
}

void TDEngineSpi::OnRtnTrade(CThostFtdcTradeField *pTrade) 
{
	pTDEngine->OnRtnTrade(pTrade, api);
}

void TDEngineSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	pTDEngine->OnRspQryTradingAccount(pTradingAccount, pRspInfo, nRequestID, bIsLast, api);
}

void TDEngineSpi::OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus)
{
	pTDEngine->OnRtnInstrumentStatus(pInstrumentStatus,api);
}

void TDEngineSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	pTDEngine->OnRspError(pRspInfo, nRequestID, bIsLast, api);
}
