
#ifndef TRANSHELPER_DEFINE_H
#define TRANSHELPER_DEFINE_H

#include "../../../system/api/ctp/v6.3.15/include/ThostFtdcTraderApi.h"
#include "../xtcomm/XTTradeStruct.h"

namespace transhelper
{
	int		Trans_TradingAccountField(CThostFtdcTradingAccountField* pIn, tagXTTradingAccountField& oField);

	int		Trans_PositionField(CThostFtdcInvestorPositionField* pIn, tagXTInvestorPositionField& oField);

	int		Trans_RspOrderInsert(CThostFtdcInputOrderField* pIn, tagXTInputOrderField& oField);

	int		Trans_RspOrderAction(CThostFtdcInputOrderActionField* pIn, tagXTInputOrderActionField& oField);

	int     Trans_OrderField(CThostFtdcOrderField* pIn, tagXTOrderField& oField);

	int		Trans_TradeField(CThostFtdcTradeField* pIn, tagXTTradeField& oField);

	int     Trans_ReqOrderInsert(tagXTInputOrderField* pIn, CThostFtdcInputOrderField& oField);

	int		Trans_ReqOrderAction(tagXTInputOrderActionField* pIn, CThostFtdcInputOrderActionField& oField);

	int		Trans_Time(char* dottime);

};


#endif
