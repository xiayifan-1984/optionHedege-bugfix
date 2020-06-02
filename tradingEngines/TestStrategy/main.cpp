#include "../strategy/IStrategy.h"
#include "../../memoryManage/utils/Timer.h"
#include "../../memoryManage/log/MM_Log.h"
#include <iostream>
#include <fstream>

USING_TE_NAMESPACE

#define SOURCE_INDEX SOURCE_CTP

class Strategy :public IStrategy
{
protected:
	bool td_connected;
	int md_num;
	vector<string> tickers;
	int rid;
	bool bSend;
	int count;
	string orderRef;
	string instrumentID;

public:
	Strategy(const string& name);
	virtual void init();
	virtual void on_market_data(const MarketDataField* data, short source, long rcv_time);
	virtual void on_rtn_order(const RtnOrderField* data, int request_id, short source, long rcv_time);
	virtual void on_rtn_trade(const RtnTradeField* rtn_trade, int request_id, short source, long rcv_time);
	virtual void on_rsp_order(const InputOrderField* order, int request_id, short source, long rcv_time, short errorId, const char* errorMsg);
	virtual void on_rsp_position(const RspPositionField* pos, int request_id, short source, long rcv_time,bool isLast,short errorId, const char* errorMsg);
	virtual void on_rsp_qry_trade(const RtnTradeField* data, int request_id, short source, long rcv_time,bool isLast,short errorId, const char* errorMsg);
	virtual void on_rsp_qry_order(const RtnOrderField* data, int request_id, short source, long rcv_time,bool isLast,short errorId, const char* errorMsg);
	virtual void on_rsp_account(const RspAccountField * data, int request_id, short source, long rcv_time,bool isLast, short errorId, const char* errorMsg);
	bool isInstrumentSubscribed(const char* Instrument);
};

Strategy::Strategy(const string&name) :IStrategy(name)
{
	rid = -1;
	bSend = false;
	orderRef="";
	count = 0;
	instrumentID = "";
}

void Strategy::init()
{
	data->add_market_data(SOURCE_INDEX);
	data->add_register_td(SOURCE_INDEX);

	tickers.push_back("cu1912");
	tickers.push_back("al2001");
	tickers.push_back("au1912");
	util->subscribeMarketData(tickers, SOURCE_INDEX);

	td_connected = false;
	md_num = 0;
}

bool Strategy::isInstrumentSubscribed(const char* Instrument)
{
	for (auto it = tickers.begin(); it != tickers.end(); ++it)
	{
		if (strcmp(Instrument, (*it).c_str()))
		{
			return true;
		}
	}
	return false;
}
void Strategy::on_market_data(const MarketDataField* md, short source, long rcv_time)
{
	std::fstream outfile;
	count++;
	if (isInstrumentSubscribed(md->InstrumentID))
	{
		/*
		if (!bSend)
		{
			long nano = optionHedge::memoryManage::getNanoTime();
			nano = nano % 1000000000;
			std::cout << "preparing sending order" << std::endl;
			orderRef = std::to_string(nano);
			instrumentID = string(md->InstrumentID);
			rid = util->insert_limit_order(SOURCE_INDEX, md->InstrumentID, EXCHANGE_SHFE, md->BidPrice1 - 100, 1, CHAR_Buy, CHAR_Open, orderRef);
			//util->req_qry_order(SOURCE_INDEX);
			//util->req_position(SOURCE_INDEX);
			//util->req_tradingAccount(SOURCE_INDEX);
			//util->req_qry_trade(SOURCE_INDEX);
			bSend = true;
		}
		
		if (count==50)
		{
			string exchangeID("SHFE");
			util->cancel_order(SOURCE_INDEX, orderRef, instrumentID, exchangeID);
		}
		*/
		//util->req_position(SOURCE_INDEX);
		util->req_qry_order(SOURCE_INDEX);
	}
}

void Strategy::on_rtn_order(const RtnOrderField* data, int request_id, short source, long rcv_time)
{
	OPTIONHEDGE_LOG_DEBUG(logger, "[ORDER]" << " (t)" << data->InstrumentID << " (rid)" << request_id << " (v)" << data->OrderStatus<<" orderRef "<<data->OrderRef<<" order SysID"<<data->OrderSysID<<" brokerOrderSeq "<<data->BrokerOrderSeq);
}

void Strategy::on_rtn_trade(const RtnTradeField* rtn_trade, int request_id, short source, long rcv_time)
{
	OPTIONHEDGE_LOG_DEBUG(logger, "[TRADE]" << " (t)" << rtn_trade->InstrumentID << " (p)" << rtn_trade->Price << " (v)" << rtn_trade->Volume<<" orderSysID "<<rtn_trade->OrderSysID);
}

void Strategy::on_rsp_order(const InputOrderField* order, int request_id, short source, long rcv_time, short errorId, const char* errorMsg)
{
	if (errorId != 0)
		OPTIONHEDGE_LOG_ERROR(logger, " (err_id)" << errorId << " (err_msg)" << errorMsg << "(order_id)" << request_id << " (source)" << source);

	OPTIONHEDGE_LOG_INFO(logger, " (orderRef is )" << order->OrderRef << "(order_id)" << request_id << " (source)" << source);
}


void Strategy::on_rsp_position(const RspPositionField* pos, int request_id, short source, long rcv_time,bool isLast, short errorId, const char* errorMsg)
{
	if (request_id == -1 && source == SOURCE_INDEX)
	{
		td_connected = true;
		OPTIONHEDGE_LOG_INFO(logger, "td connected");
	}
	else
	{
		OPTIONHEDGE_LOG_DEBUG(logger, "[RSP_POS] " << "broker ID "<<pos->BrokerID
		<<" YPos "<<pos->YdPosition<<"Instrument ID "<<pos->InstrumentID<<" Pos "<<pos->Position
		<<" Direction "<<pos->PosiDirection<<" HedgeFlag "<<pos->HedgeFlag << " isLast " << isLast);
	}
}

void Strategy::on_rsp_account(const RspAccountField * data, int request_id, short source, long rcv_time, bool isLast, short errorId, const char* errorMsg)
{
	OPTIONHEDGE_LOG_DEBUG(logger, "[RSP_ACCOUNT] " << "broker ID " << data->BrokerID <<" Balance "<<data->Balance
	<<" Available "<<data->Available<<" cash profit "<<data->CloseProfit<<" isLast "<<isLast);
}

void Strategy::on_rsp_qry_order(const RtnOrderField* data, int request_id, short source, long rcv_time, bool isLast,short errorId, const char* errorMsg)
{
	OPTIONHEDGE_LOG_DEBUG(logger, "[RSP_QRY_ORDER] " << "broker ID " << data->BrokerID << " Instrument ID " << data->InstrumentID
		<< "order ref " << data->OrderRef <<"order sysID "<<data->OrderSysID<<" order status " << data->OrderStatus << " isLast " << isLast);
}

void Strategy::on_rsp_qry_trade(const RtnTradeField* data, int request_id, short source, long rcv_time, bool isLast,short errorId, const char* errorMsg)
{
	OPTIONHEDGE_LOG_DEBUG(logger, "[RSP_QRY_TRADE] " << "broker ID " << data->BrokerID << " Instrument ID " << data->InstrumentID
		<< "order ref " << data->OrderRef << " orderSysID "<<data->OrderSysID<<" isLast " << isLast);
}

int main(int argc, const char* argv[])
{
	Strategy str(string("testingStrategyxiayf"));
	str.init();
	str.start();
	str.block();
	return 0;
}
