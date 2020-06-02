#ifndef TRADINGENGINE_MDENGINESPI_H
#define TRADINGENGINE_MDENGINESPI_H

#include "IMDEngine.h"
#include "MDEngineCTP.h"
#include "../base/TradingConstant.h"
#include "../../system/api/ctp/v6.3.15/include/ThostFtdcMdApi.h"
#include "../../memoryManage/log/MM_Log.h"
#include "../../memoryManage/utils/json.hpp"

TE_NAMESPACE_START

class MDEngineSpi :public CThostFtdcMdSpi
{
public:
	MDEngineSpi() {}
	MDEngineSpi(CThostFtdcMdApi* papi, MDEngineCTP* MDEngine);
public:
	// SPI
	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();

	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	///@param nReason ����ԭ��
	///        0x1001 �����ʧ��
	///        0x1002 ����дʧ��
	///        0x2001 ����������ʱ
	///        0x2002 ��������ʧ��
	///        0x2003 �յ�������
	virtual void OnFrontDisconnected(int nReason);

	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�ǳ�������Ӧ
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///��������Ӧ��
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�������֪ͨ
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

private:
	MDEngineCTP* pMDEngine;
	CThostFtdcMdApi* api;
	LogPtr logger;
};

TE_NAMESPACE_END
#endif
