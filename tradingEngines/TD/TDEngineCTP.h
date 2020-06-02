#ifndef TRADINGENGINE_TDENGINECTP_H
#define TRADINGENGINE_TDENGINECTP_H

#include "ITDEngine.h"
#include "../base/TradingConstant.h"
#include "../../system/api/ctp/v6.3.15/include/ThostFtdcTraderApi.h"
#include "TDEngineSpi.h"

TE_NAMESPACE_START

struct AccountUnitCTP
{
	CThostFtdcTraderApi* api;
	string appID;
	string auth_code;
	int front_id;
	int session_id;
	//internal flags
	bool initialized;
	bool connected;
	bool authenticated;
	bool settle_confirmed;
	bool logged_in;
	//some rid
	int auth_rid;
	int settle_rid;
	int login_rid;

	int setYesterdayPos_rid;
	int setTodayPos_rid;
	int setOrder_rid;

	string accountType;
	bool bYesterdayPosInitialized;
	bool bTodayPosInitialized;
	bool bOrderInitialized;
};

class TDEngineCTP :public ITDEngine
{
public:
	virtual void init();
	/** for settleconfirm and authenticate setting */
	virtual void pre_load(const json&j_config);
	virtual TradeAccount load_account(int idx, const json& j_account);
	virtual void resize_accounts(int account_num);
	/** connect && login related */
	virtual void connect(long timeout_nsec);
//	virtual void login(long timeout_nsec);
	virtual void logout();
	virtual void release_api();
	virtual bool is_connected() const;
	virtual bool is_logged_in() const;
	virtual string name() const { return "TDEngineCTP"; };

	// req functions
	virtual void req_investor_position(const QryPositionField* data, int account_index, int requestId);
	virtual void req_qry_account(const QryAccountField* data, int account_index, int requestId);
	virtual void req_qry_order(const QryOrderField* data, int account_index, int requestId);
	virtual void req_qry_trade(const QryTradeField* data, int account_index, int requestId);
	virtual void req_order_insert(const InputOrderField* data, int account_index, int requestId, long rcv_time);
	virtual void req_order_action(const OrderActionField* data, int account_index, int requestId, long rcv_time);

public:
	TDEngineCTP();

private:
	// journal writers
	optionHedge::memoryManage::JournalWriterPtr raw_writer;
	// from config
	string front_uri;
	string front_uri_kn;
	bool need_settleConfirm;
	bool need_authenticate;
	int curAccountIdx;
	vector<AccountUnitCTP> account_units;
public:
	// SPI
	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected(CThostFtdcTraderApi* api);

	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	///@param nReason ����ԭ��
	///        0x1001 �����ʧ��
	///        0x1002 ����дʧ��
	///        0x2001 ����������ʱ
	///        0x2002 ��������ʧ��
	///        0x2003 �յ�������
	virtual void OnFrontDisconnected(int nReason, CThostFtdcTraderApi* api);

	///�ͻ�����֤��Ӧ
	virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);

	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);

	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);

	///�ǳ�������Ӧ
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);

	///����¼��������Ӧ (this only be called when there is error)
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);

	virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo,int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);
	virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade,CThostFtdcRspInfoField *pRspInfo,int nRequestID,bool bIsLast, CThostFtdcTraderApi* api);
	///��������������Ӧ
	virtual void OnRspOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);

	///�����ѯͶ���ֲ߳���Ӧ
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);

	///����֪ͨ
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder, CThostFtdcTraderApi* api);

	///�ɽ�֪ͨ
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade, CThostFtdcTraderApi* api);

	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);
	///��Լ����״̬֪ͨ
	virtual void OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus, CThostFtdcTraderApi* api);
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);
};
TE_NAMESPACE_END
#endif
