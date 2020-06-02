#ifndef TRADINGENGINE_MDENGINECTP_H
#define TRADINGENGINE_MDENGINECTP_H

#include "IMDEngine.h"
#include "../base/TradingConstant.h"
#include "../../system/api/ctp/v6.3.15/include/ThostFtdcMdApi.h"

TE_NAMESPACE_START

class MDEngineCTP :public IMDEngine
{
public:
	virtual void load(const json& j_config);
	virtual void connect(long timeout_nsec);
//	virtual void login(long timeout_nsec);
	virtual void logout();
	virtual void release_api();
	virtual void subscribeMarketData(const vector<string>& instruments, const vector<string>& markets);
	virtual bool is_connected() const { return connected; }
	virtual bool is_logged_in() const { std::cout << "logged in status is " << logged_in << std::endl; return logged_in; }
	virtual string name() const { return "MDEngineCTP"; };
public:
	MDEngineCTP();

private:
	CThostFtdcMdApi* api;
	string broker_id;
	string user_id;
	string password;
	string front_uri;
	bool connected;
	bool logged_in;
	int reqId;

public:
	// SPI
	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected(CThostFtdcMdApi* api);

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(int nReason, CThostFtdcMdApi* api);

	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast,CThostFtdcMdApi* api);

	///登出请求响应
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcMdApi* api);

	///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcMdApi* api);

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcMdApi* api);
};

DECLARE_PTR(MDEngineCTP);

TE_NAMESPACE_END
#endif
