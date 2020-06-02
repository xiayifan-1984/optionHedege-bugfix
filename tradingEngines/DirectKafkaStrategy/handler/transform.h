

#ifndef TRANSFORM_DEFINE_H
#define TRANSFORM_DEFINE_H

#include "../pubstruct.h"
#include "../../../system/api/ctp/v6.3.15/include/ThostFtdcTraderApi.h"
#include <thread>

class MProduce;
class MConsume;
class directStrategy;
class Transform
{
public:
	Transform();
	virtual ~Transform();

public:
	int			Instance(tagTradeUserInfo* pInfo, directStrategy* pSpi, const char* pszbroker);
	void		Release();

public:
	void		OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void		OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void		OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void		OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void		OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void		OnRtnOrder(CThostFtdcOrderField *pOrder);
	void		OnRtnTrade(CThostFtdcTradeField *pTrade);
	void		OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void		OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

protected:
	static		int		pfnConsumeCB(void* pthis, const char* topic, int partition, long long offset, int buflen, const char* pbuf);
	int			onConsumeCB(const char* topic, int partition, long long offset, int buflen, const char* pbuf);

	void		Run();

protected:
	int			sendResponse(unsigned char main, unsigned char child, const char* pbuf, int buflen);

protected:
	MProduce*								m_pProduce;
	MConsume*								m_pConsume;
	tagTradeUserInfo						m_oUserInfo;
	directStrategy*							m_pTradeSpi;

	std::thread*							m_pthread;
	bool									m_bstopflag;
};














#endif












