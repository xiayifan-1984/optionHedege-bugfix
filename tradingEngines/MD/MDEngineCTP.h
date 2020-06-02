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
	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected(CThostFtdcMdApi* api);

	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	///@param nReason ����ԭ��
	///        0x1001 �����ʧ��
	///        0x1002 ����дʧ��
	///        0x2001 ����������ʱ
	///        0x2002 ��������ʧ��
	///        0x2003 �յ�������
	virtual void OnFrontDisconnected(int nReason, CThostFtdcMdApi* api);

	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast,CThostFtdcMdApi* api);

	///�ǳ�������Ӧ
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcMdApi* api);

	///��������Ӧ��
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcMdApi* api);

	///�������֪ͨ
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcMdApi* api);
};

DECLARE_PTR(MDEngineCTP);

TE_NAMESPACE_END
#endif
