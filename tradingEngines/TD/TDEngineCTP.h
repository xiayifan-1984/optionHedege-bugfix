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
	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected(CThostFtdcTraderApi* api);

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(int nReason, CThostFtdcTraderApi* api);

	///客户端认证响应
	virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);

	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);

	///投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);

	///登出请求响应
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);

	///报单录入请求响应 (this only be called when there is error)
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);

	virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo,int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);
	virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade,CThostFtdcRspInfoField *pRspInfo,int nRequestID,bool bIsLast, CThostFtdcTraderApi* api);
	///报单操作请求响应
	virtual void OnRspOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);

	///请求查询投资者持仓响应
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);

	///报单通知
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder, CThostFtdcTraderApi* api);

	///成交通知
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade, CThostFtdcTraderApi* api);

	///请求查询资金账户响应
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);
	///合约交易状态通知
	virtual void OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus, CThostFtdcTraderApi* api);
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcTraderApi* api);
};
TE_NAMESPACE_END
#endif
