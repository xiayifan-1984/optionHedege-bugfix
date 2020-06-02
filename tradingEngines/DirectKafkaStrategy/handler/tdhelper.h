#ifndef TDHELPER_DEFINE_H
#define TDHELPER_DEFINE_H

#include "../../../system/api/ctp/v6.3.15/include/ThostFtdcTraderApi.h"
#include "../../base/DataStruct.h"


namespace tdhelper
{
	int		Trans_TradingAccountField(const RspAccountField* pIn, CThostFtdcTradingAccountField& oField);
	int		Trans_RspInfo(short errorId, const char* errorMsg, CThostFtdcRspInfoField& oRspInfo);

	int		Trans_InvestorPositionField(const RspPositionField* pIn, CThostFtdcInvestorPositionField& oField);

	int		Trans_OrderField(const RtnOrderField* pIn, CThostFtdcOrderField& oField);

	int		Trans_TradeField(const RtnTradeField* pIn, CThostFtdcTradeField& oField);

	int		Trans_OrderInsert(const InputOrderField* pIn, CThostFtdcInputOrderField& oField);

};



#endif











