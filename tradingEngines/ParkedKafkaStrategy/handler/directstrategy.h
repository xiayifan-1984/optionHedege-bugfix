
#ifndef DIRECT_STRATEGY_DEFINE_H
#define DIRECT_STRATEGY_DEFINE_H

#include "../../strategy/IStrategy.h"

#include "../../../system/api/ctp/v6.3.15/include/ThostFtdcTraderApi.h"

#include <string>
#include <vector>
#include <map>
#include <set>
using namespace std;

USING_TE_NAMESPACE

class Transform;
class directStrategy : public IStrategy
{
public:
	directStrategy(const string& name);

public:
	void					getUserInfo(int& outbrokerid, string& outuserid);		//获取当前策略的 用户账号/券商编号
	int						setTransform(Transform* pTransform);					
	void					writeLog(int level, const char* message);				//写入平台日志
	void 					subscribeCode(std::string& code);						//订阅合约
	bool 					getLastestTick(const std::string& code, MarketDataField& tick);		//获取最近一次的行情

public:
	virtual		void		init();
	virtual		void		on_market_data(const MarketDataField* data, short source, long rcv_time);

	virtual		void		on_rtn_order(const RtnOrderField* data, int request_id, short source, long rcv_time);
	
	virtual		void		on_rtn_trade(const RtnTradeField* data, int request_id, short source, long rcv_time);

	
	virtual		void		on_rsp_qry_order(const RtnOrderField* data, int request_id, short source, long rcv_time, bool isLast, short errorId = 0, const char* errorMsg = nullptr);
	
	virtual		void		on_rsp_qry_trade(const RtnTradeField* data, int request_id, short source, long rcv_time, bool isLast, short errorId = 0, const char* errorMsg = nullptr);
	
	virtual		void		on_rsp_position(const RspPositionField* pos, int request_id, short source, long rcv_time, bool isLast, short errorId = 0, const char* errorMsg = nullptr);
	
	virtual		void		on_rsp_account(const RspAccountField * data, int request_id, short source, long rcv_time, bool isLast, short errorId = 0, const char* errorMsg = nullptr);

	
	virtual		void		on_rsp_action(const RspActionField * data, int request_id, short source, long rcv_time);				//orderaction
	virtual		void		on_rsp_order(const InputOrderField* data, int request_id, short source, long rcv_time, short errorId = 0, const char* errorMsg = nullptr); //orderinsert

	virtual		void		on_rtn_instrument_status(const RspInstrumentStatus* data, int request_id, short source, long rcv_time);

public:
	void					DoReqQryTradingAccount(int reqid);
	void					DoReqQryInvestorPosition(int reqid);
	void					DoReqQryOrder(int reqid);
	void					DoReqQryTrade(int reqid);
	void					DoReqOrderInsert(CThostFtdcInputOrderField* pField, int reqid);
	void					DoReqOrderAction(CThostFtdcInputOrderActionField* pField, int reqid);

public: 					
	int 					GetTradingDay();
	char					GetExchangeStatus();

protected:
	bool					isInstrumentSubscribed(const char* pcode);

protected:
	set<string>				m_tickers;
	Transform*				m_pTransform;

	map<int, int>			m_mapReqID;		//reqID  <-->  realReqID
	std::map<std::string, MarketDataField>		m_latestTick;

private:
	char					m_lastExchStatus;		//最近一次交易所的状态

};



#endif



