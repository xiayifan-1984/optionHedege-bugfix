#include "MDEngineSpi.h"

USING_TE_NAMESPACE

MDEngineSpi::MDEngineSpi(CThostFtdcMdApi* papi, MDEngineCTP* MDEngine):api(papi),pMDEngine(MDEngine)
{
	logger = optionHedge::memoryManage::Log::getLogger("CtpMD");
	OPTIONHEDGE_LOG_INFO(logger, "MDSpi Initialized");
}

void MDEngineSpi::OnFrontConnected()
{
	pMDEngine->OnFrontConnected(api);
}

void MDEngineSpi::OnFrontDisconnected(int nReason)
{
	pMDEngine->OnFrontDisconnected(nReason, api);
}

void MDEngineSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	pMDEngine->OnRspUserLogin(pRspUserLogin, pRspInfo, nRequestID, bIsLast, api);
}

void MDEngineSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	pMDEngine->OnRspUserLogout(pUserLogout, pRspInfo, nRequestID, bIsLast, api);
}

void MDEngineSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	pMDEngine->OnRspSubMarketData(pSpecificInstrument, pRspInfo, nRequestID, bIsLast, api);
}

void MDEngineSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	pMDEngine->OnRtnDepthMarketData(pDepthMarketData, api);
}