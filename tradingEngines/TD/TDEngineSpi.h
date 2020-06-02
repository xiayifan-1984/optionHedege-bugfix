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
	///�ǳ�������Ӧ
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///����¼��������Ӧ (this only be called when there is error)
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///��������������Ӧ
	virtual void OnRspOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯͶ���ֲ߳���Ӧ
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����֪ͨ
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	///�ɽ�֪ͨ
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///��Լ����״̬֪ͨ
	virtual void OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus);
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

};

TE_NAMESPACE_END
#endif
