
#include "directstrategy.h"
#include "tdhelper.h"
#include "transform.h"
#include <string.h>
#include <stdlib.h>

#define SOURCE_INDEX	SOURCE_CTP

directStrategy::directStrategy(const string& name) :IStrategy(name)
{
	logger = optionHedge::memoryManage::Log::getLogger("parkstrategy");
	m_pTransform = nullptr;
	m_mapReqID.clear();
	m_tickers.clear();

	m_lastExchStatus = 0;
}

int			directStrategy::setTransform(Transform* pTransform)
{
	m_pTransform = pTransform;

	return 1;
}

void		directStrategy::getUserInfo(int& outbrokerid, string& outuserid)
{
	outuserid = userId;
	outbrokerid = atoi(brokerId.c_str());
}

void		directStrategy::writeLog(int level, const char* message)
{
	//0 = debug   1 = info  2 = error   3 = fatal
	switch (level)
	{
	case 1:
		OPTIONHEDGE_LOG_INFO( logger, "[INFO](user)" << userId << " (broker)" << brokerId << " (msg)" << message );
		break;
	case 2:
		OPTIONHEDGE_LOG_ERROR(logger, "[ERROR](user)" << userId << " (broker)" << brokerId << " (msg)" << message );
		break;
	case 3:
		OPTIONHEDGE_LOG_FATAL(logger, "[FATAL](user)" << userId << " (broker)" << brokerId << " (msg)" << message );
		break;
	default:
		OPTIONHEDGE_LOG_DEBUG(logger, "[DEBUG](user)" << userId << " (broker)" << brokerId << " (msg)" << message );
		break;
	}
	printf("%s\n", message);
}

void		directStrategy::init()
{
	data->add_market_data(SOURCE_INDEX);
	data->add_register_td(SOURCE_INDEX);

}

void		directStrategy::on_market_data(const MarketDataField* data, short source, long rcv_time)
{
	if (data)
	{
		if (isInstrumentSubscribed(data->InstrumentID))
		{
			string code(data->InstrumentID);
			m_latestTick[code] = *data;
		}
	}
}

void		directStrategy::on_rtn_order(const RtnOrderField* data, int request_id, short source, long rcv_time)
{
	writeLog(0, "on_rtn_order");
	if(data && (strlen(data->BrokerID) > 0) )
	{
		CThostFtdcOrderField oField;
		memset(&oField, 0, sizeof(oField));
		tdhelper::Trans_OrderField(data, oField);
		OPTIONHEDGE_LOG_DEBUG(logger, "[DEBUG](user)" << userId << " (broker)" << brokerId << "on_rtn_order"
		<< "OrderRef" <<data->OrderRef
		<< "OrderSysID" << data->OrderSysID << "OrderStatus" << data->OrderStatus << "InsertTime" << data->InsertTime
		<< "BrokerOrderSeq" << data->BrokerOrderSeq );
		if (m_pTransform)
		{
			m_pTransform->OnRtnOrder(&oField);
		}
	}
	
}

void		directStrategy::on_rtn_trade(const RtnTradeField* data, int request_id, short source, long rcv_time)
{
	writeLog(0, " on_rtn_trade" );
	if(data && (strlen(data->BrokerID) > 0) )
	{
		CThostFtdcTradeField oField;
		memset(&oField, 0, sizeof(oField));
		tdhelper::Trans_TradeField(data, oField);

		if (m_pTransform)
		{
			m_pTransform->OnRtnTrade(&oField);
		}
	}

}

void		directStrategy::on_rsp_order(const InputOrderField* data, int request_id, short source, long rcv_time, short errorId, const char* errorMsg)
{
	writeLog(0, " on_rsp_orderinsert" );

	//OnRspOrderInsert
	//转换得到CThostFtdcRspInfoField
	CThostFtdcRspInfoField oRspInfo;
	memset(&oRspInfo, 0, sizeof(oRspInfo));
	tdhelper::Trans_RspInfo(errorId, errorMsg, oRspInfo);

	int realreqid = 0;
	map<int, int>::iterator itb = m_mapReqID.find(request_id);
	if (itb != m_mapReqID.end())
	{
		realreqid = itb->second;
	}

	if (m_pTransform)
	{
		if (data && (strlen(data->BrokerID) > 0) )
		{
			CThostFtdcInputOrderField oField;
			memset(&oField, 0, sizeof(oField));
			tdhelper::Trans_OrderInsert(data, oField);

			m_pTransform->OnRspOrderInsert(&oField, &oRspInfo, realreqid, true);
		}
		else
		{
			m_pTransform->OnRspOrderInsert(0, &oRspInfo, realreqid, true);
		}
	}
}

void		directStrategy::on_rsp_action(const RspActionField * data, int request_id, short source, long rcv_time)
{
	writeLog(0,"on_rsp_orderaction");
	//do nothing
}

void		directStrategy::on_rsp_qry_trade(const RtnTradeField* data, int request_id, short source, long rcv_time, bool isLast, short errorId, const char* errorMsg)
{
	if (data)
	{
		OPTIONHEDGE_LOG_DEBUG(logger, "[DEBUG](user)" << userId << " (broker)" << brokerId << "on_rsp_qry_trade"
		<<"TradeID" << data->TradeID << "Volume" << data->Volume );
	}
	else
	{
		writeLog(0,"on_rsp_qry_trade");
	}

	//转换得到CThostFtdcRspInfoField
	CThostFtdcRspInfoField oRspInfo;
	memset(&oRspInfo, 0, sizeof(oRspInfo));
	tdhelper::Trans_RspInfo(errorId, errorMsg, oRspInfo);

	int realreqid = 0;
	map<int, int>::iterator itb = m_mapReqID.find(request_id);
	if (itb != m_mapReqID.end())
	{
		realreqid = itb->second;
	}
	
	if (m_pTransform)
	{
		if(data && (strlen(data->BrokerID) > 0) )
		{
			//转换得到CThostFtdcTradeField
			CThostFtdcTradeField  oField;
			memset(&oField, 0, sizeof(oField));
			tdhelper::Trans_TradeField(data, oField);

			m_pTransform->OnRspQryTrade(&oField, &oRspInfo, realreqid, isLast);
		}
		else
		{
			m_pTransform->OnRspQryTrade(0, &oRspInfo, realreqid, isLast);
		}
	}
}

void		directStrategy::on_rsp_qry_order(const RtnOrderField* data, int request_id, short source, long rcv_time, bool isLast, short errorId, const char* errorMsg)
{
	writeLog(0,"on_rsp_qry_order");

	//转换得到CThostFtdcRspInfoField
	CThostFtdcRspInfoField oRspInfo;
	memset(&oRspInfo, 0, sizeof(oRspInfo));
	tdhelper::Trans_RspInfo(errorId, errorMsg, oRspInfo);

	int realreqid = 0;
	map<int, int>::iterator itb = m_mapReqID.find(request_id);
	if (itb != m_mapReqID.end())
	{
		realreqid = itb->second;
	}
	
	if (m_pTransform)
	{
		if(data && (strlen(data->BrokerID) > 0) )
		{
			//转换得到CThostFtdcOrderField
			CThostFtdcOrderField  oField;
			memset(&oField, 0, sizeof(oField));
			tdhelper::Trans_OrderField(data, oField);
			OPTIONHEDGE_LOG_DEBUG(logger, "[DEBUG](user)" << userId << " (broker)" << brokerId << "on_rsp_qry_order"
			<< "code " <<data->InstrumentID << "data" << data->OffsetFlag << "oFiled" << oField.CombOffsetFlag[0]
			);
			m_pTransform->OnRspQryOrder(&oField, &oRspInfo, realreqid, isLast);
		}
		else
		{
			m_pTransform->OnRspQryOrder(0, &oRspInfo, realreqid, isLast);
		}
	}
}

void		directStrategy::on_rsp_position(const RspPositionField* pos, int request_id, short source, long rcv_time, bool isLast, short errorId, const char* errorMsg)
{
	writeLog(0,"on_rsp_position");

	//转换得到CThostFtdcRspInfoField
	CThostFtdcRspInfoField oRspInfo;
	memset(&oRspInfo, 0, sizeof(oRspInfo));
	tdhelper::Trans_RspInfo(errorId, errorMsg, oRspInfo);

	int realreqid = 0;
	map<int, int>::iterator itb = m_mapReqID.find(request_id);
	if (itb != m_mapReqID.end())
	{
		realreqid = itb->second;
	}

	if (m_pTransform)
	{
		if(pos && (strlen(pos->BrokerID) > 0)  )
		{
			//转换得到CThostFtdcInvestorPositionField
			CThostFtdcInvestorPositionField  oField;
			memset(&oField, 0, sizeof(oField));
			tdhelper::Trans_InvestorPositionField(pos, oField);
			m_pTransform->OnRspQryInvestorPosition(&oField, &oRspInfo, realreqid, isLast);
		}
		else
		{
			m_pTransform->OnRspQryInvestorPosition(0, &oRspInfo, realreqid, isLast);
		}
	}
}

void		directStrategy::on_rsp_account(const RspAccountField * data, int request_id, short source, long rcv_time, bool isLast, short errorId, const char* errorMsg)
{
	writeLog(0,"on_rsp_account");
	
	//转换得到CThostFtdcRspInfoField
	CThostFtdcRspInfoField oRspInfo;
	memset(&oRspInfo, 0, sizeof(oRspInfo));
	tdhelper::Trans_RspInfo(errorId, errorMsg, oRspInfo);
	
	int realreqid = 0;
	map<int, int>::iterator itb = m_mapReqID.find(request_id);
	if (itb != m_mapReqID.end() )
	{
		realreqid = itb->second;
	}

	if (m_pTransform)
	{
		if (data && (strlen(data->BrokerID) > 0) )
		{
			//转换得到CThostFtdcTradingAccountField
			CThostFtdcTradingAccountField  oField;
			memset(&oField, 0, sizeof(oField));
			tdhelper::Trans_TradingAccountField(data, oField);
			m_pTransform->OnRspQryTradingAccount(&oField, &oRspInfo, realreqid, isLast);
		}
		else
		{
			m_pTransform->OnRspQryTradingAccount(0, &oRspInfo, realreqid, isLast);
		}
	}
}

void		directStrategy::on_rtn_instrument_status(const RspInstrumentStatus* data, int request_id, short source, long rcv_time)
{
	if (data->InstrumentStatus != m_lastExchStatus)
	{
		m_lastExchStatus = data->InstrumentStatus;
	}
}

bool		directStrategy::isInstrumentSubscribed(const char* pcode)
{
	string strcode = pcode;
	auto itb = m_tickers.find(strcode);
	if (itb != m_tickers.end())
	{
		return true;
	}
	
	return false;
}

//=============================================================================================================================================================================================================
//=============================================================================================================================================================================================================
//=============================================================================================================================================================================================================
//=============================================================================================================================================================================================================
void		directStrategy::DoReqQryTradingAccount(int reqid)
{
	writeLog(0,"DoReqQryTradingAccount");
	int retid = util->req_tradingAccount(SOURCE_INDEX);
	m_mapReqID[retid] = reqid;
}

void		directStrategy::DoReqQryInvestorPosition(int reqid)
{
	writeLog(0,"DoReqQryInvestorPosition");
	int retid = util->req_position(SOURCE_INDEX);
	m_mapReqID[retid] = reqid;
}

void		directStrategy::DoReqQryOrder(int reqid)
{
	writeLog(0,"DoReqQryOrder");
	int retid = util->req_qry_order(SOURCE_INDEX);
	m_mapReqID[retid] = reqid;
}

void		directStrategy::DoReqQryTrade(int reqid)
{
	writeLog(0,"DoReqQryTrade");
	int retid = util->req_qry_trade(SOURCE_INDEX);
	m_mapReqID[retid] = reqid;
}

void		directStrategy::DoReqOrderInsert(CThostFtdcInputOrderField* pField, int reqid)
{
	writeLog(0,"DoReqOrderInsert");
	if (pField->OrderPriceType == THOST_FTDC_OPT_AnyPrice)
	{
		//insert_market_order(short source, string instrument_id, string exchange_id, int volume, DirectionType direction, OffsetFlagType offset,string orderRef="");
		int retid = util->insert_market_order(SOURCE_INDEX, pField->InstrumentID, pField->ExchangeID, pField->VolumeTotalOriginal, pField->Direction, pField->CombOffsetFlag[0], pField->OrderRef);
		m_mapReqID[retid] = reqid;
	}
	else if (pField->OrderPriceType == THOST_FTDC_OPT_LimitPrice)
	{
		//insert_limit_order(short source, string instrument_id, string exchange_id, double price, int volume, DirectionType direction, OffsetFlagType offset, string orderRef = "");
		int retid = util->insert_limit_order(SOURCE_INDEX, pField->InstrumentID, pField->ExchangeID, pField->LimitPrice, pField->VolumeTotalOriginal, pField->Direction, pField->CombOffsetFlag[0], pField->OrderRef);
		m_mapReqID[retid] = reqid;
	}
	
}

void		directStrategy::DoReqOrderAction(CThostFtdcInputOrderActionField* pField, int reqid)
{
	writeLog(0,"DoReqOrderAction");
	int retid = util->cancel_order(SOURCE_INDEX, pField->OrderRef, pField->InstrumentID, pField->ExchangeID, pField->OrderSysID);
	m_mapReqID[retid] = reqid;
}

int 		directStrategy::GetTradingDay()
{
	std::string s = tradedate;
	return atoi(s.c_str());
}

void 		directStrategy::subscribeCode(std::string& code)
{
	auto itb = m_tickers.find(code);
	if (itb != m_tickers.end())
	{
		return;
	}
	
	m_tickers.insert(code);
	
	vector<string> arrCode;
	arrCode.push_back(code);
	util->subscribeMarketData(arrCode, SOURCE_INDEX);
}

bool 		directStrategy::getLastestTick(const std::string& code, MarketDataField& otick)
{
	auto itb = m_latestTick.find(code);
	if(itb != m_latestTick.end())
	{
		otick = itb->second;
		return true;
	}
	return false;
}

char		directStrategy::GetExchangeStatus()
{
	return m_lastExchStatus;
}