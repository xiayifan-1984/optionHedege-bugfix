#include "transhelper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../util/Platform.h"

int	transhelper::Trans_TradingAccountField(CThostFtdcTradingAccountField* pIn, tagXTTradingAccountField& oField)
{
	int nsize = sizeof(CThostFtdcTradingAccountField);
	if (nsize >= 500)
	{
		nsize = 500;
	}

	oField.OriSize = nsize;
	oField.BrokerID = atoi(pIn->BrokerID);
	oField.UserType = 4;
	memcpy(oField._reserved, (char*)pIn, nsize);

	return 1;
}


int	transhelper::Trans_Time(char* dottime)
{
	int hh, mm, ss;
	hh = mm = ss =0;

	sscanf(dottime,"%d:%d:%d", &hh, &mm, &ss);

	return hh*10000 + mm*100 + ss;
}

int				transhelper::Trans_PositionField(CThostFtdcInvestorPositionField* pIn, tagXTInvestorPositionField& oField)
{
	if (_strnicmp(pIn->ExchangeID, "CFFEX", 5) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_CFFEX;
	}
	else if (_strnicmp(pIn->ExchangeID, "DCE", 3) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_DCE;
	}
	else if (_strnicmp(pIn->ExchangeID, "CZCE", 4) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_CZCE;
	}
	else if (_strnicmp(pIn->ExchangeID, "SHFE", 4) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_SHFE;
	}
	else if (_strnicmp(pIn->ExchangeID, "INE", 3) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_INE;
	}
	else
	{
		oField.ExCode.ExchID = XT_EXCH_UNKONW;
	}

	strncpy(oField.ExCode.Code, pIn->InstrumentID, sizeof(oField.ExCode.Code));
	
	switch (pIn->PosiDirection)
	{
	case THOST_FTDC_PD_Long:
		oField.PosiDirection = XT_PD_Long;
		break;
	case THOST_FTDC_PD_Short:
		oField.PosiDirection = XT_PD_Short;
		break;
	case THOST_FTDC_PD_Net:
		oField.PosiDirection = XT_PD_Net;
		break;
	}
	
	oField.HedgeFlag = XT_HF_Speculation;

	oField.YdPosition = pIn->YdPosition;
	oField.Position = pIn->Position;

	oField.TodayPosition = pIn->TodayPosition;
	oField.PositionCost = pIn->PositionCost;
	oField.PositionProfit = pIn->PositionProfit;

	/*
	oField.OpenVolume = 0;
	oField.CloseVolume= 0;
	*/

	oField.BrokerID = atoi(pIn->BrokerID);
	strncpy(oField.UserID, pIn->InvestorID, 16);

	return 1;
}



int				transhelper::Trans_RspOrderInsert(CThostFtdcInputOrderField* pIn, tagXTInputOrderField& oField)
{
	if (_strnicmp(pIn->ExchangeID, "CFFEX", 5) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_CFFEX;
	}
	else if (_strnicmp(pIn->ExchangeID, "DCE", 3) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_DCE;
	}
	else if (_strnicmp(pIn->ExchangeID, "CZCE", 4) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_CZCE;
	}
	else if (_strnicmp(pIn->ExchangeID, "SHFE", 4) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_SHFE;
	}
	else if (_strnicmp(pIn->ExchangeID, "INE", 3) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_INE;
	}
	else
	{
		oField.ExCode.ExchID = XT_EXCH_UNKONW;
	}

	strncpy(oField.ExCode.Code, pIn->InstrumentID, sizeof(oField.ExCode.Code));
	strncpy(oField.OrderRef, pIn->OrderRef, sizeof(oField.OrderRef));
	oField.Volume = pIn->VolumeTotalOriginal;
	oField.LimitPrice = (pIn->LimitPrice);
	
	if (pIn->OrderPriceType == THOST_FTDC_OPT_AnyPrice)
	{
		oField.PriceType = XT_OPT_AnyPrice;
	}
	else
	{
		oField.PriceType = XT_OPT_LimitPrice;
	}

	switch (pIn->Direction)
	{
	case THOST_FTDC_D_Buy:
		oField.Direction = XT_D_Buy;
		break;
	case THOST_FTDC_D_Sell:
		oField.Direction =XT_D_Sell;
		break;

	}

	for (int i=0; i<5; i++)
	{
		char cflag = 0;
		switch (pIn->CombOffsetFlag[i])
		{
		case THOST_FTDC_OF_Open:
			cflag = XT_OF_Open;
			break;
		case THOST_FTDC_OF_Close:
			cflag = XT_OF_Close;
			break;
		case THOST_FTDC_OF_CloseToday:
			cflag = XT_OF_CloseToday;
			break;
		case THOST_FTDC_OF_CloseYesterday:
			cflag = XT_OF_CloseYesterday;
			break;
		default:
			cflag = 0;
			break;
		}

		oField.OffsetFlag[i] = cflag;
	}
	
	for (int i=0; i<5; i++)
	{
		char cflag = 0;
		switch (pIn->CombHedgeFlag[i])
		{
		case THOST_FTDC_HF_Speculation:
			cflag = XT_HF_Speculation;
			break;
		case THOST_FTDC_HF_Arbitrage:
			cflag = XT_HF_Arbitrage;
			break;
		case THOST_FTDC_HF_MarketMaker:
			cflag = XT_HF_MarketMaker;
			break;
		case THOST_FTDC_HF_Hedge:
			cflag = XT_HF_Hedge;
			break;
		default:
			break;
		}
		oField.HedgeFlag[i] = cflag;
	}

	oField.BrokerID = atoi(pIn->BrokerID);
	strncpy(oField.UserID, pIn->UserID, 16);

	return 1;
}

int				transhelper::Trans_RspOrderAction(CThostFtdcInputOrderActionField* pIn, tagXTInputOrderActionField& oField)
{
	if (_strnicmp(pIn->ExchangeID, "CFFEX", 5) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_CFFEX;
	}
	else if (_strnicmp(pIn->ExchangeID, "DCE", 3) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_DCE;
	}
	else if (_strnicmp(pIn->ExchangeID, "CZCE", 4) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_CZCE;
	}
	else if (_strnicmp(pIn->ExchangeID, "SHFE", 4) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_SHFE;
	}
	else if (_strnicmp(pIn->ExchangeID, "INE", 3) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_INE;
	}
	else
	{
		oField.ExCode.ExchID = XT_EXCH_UNKONW;
	}

	strncpy(oField.ExCode.Code, pIn->InstrumentID, sizeof(oField.ExCode.Code));
	strncpy(oField.OrderRef, pIn->OrderRef, sizeof(oField.OrderRef));
	
	oField.FrontID = pIn->FrontID;
	oField.SessionID = pIn->SessionID;

	_snprintf(oField.ActionRef, sizeof(oField.ActionRef)-1, "%d", pIn->OrderActionRef);
	//strncpy(oField.OrderSysID, pIn->OrderSysID, sizeof(oField.OrderSysID));
	
	oField.ActionFlag = THOST_FTDC_AF_Delete;

	oField.BrokerID = atoi(pIn->BrokerID);
	strncpy(oField.UserID, pIn->UserID, 16);
	
	return 1;
}

int				transhelper::Trans_OrderField(CThostFtdcOrderField* pIn, tagXTOrderField& oField)
{
	if (_strnicmp(pIn->ExchangeID, "CFFEX", 5) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_CFFEX;
	}
	else if (_strnicmp(pIn->ExchangeID, "DCE", 3) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_DCE;
	}
	else if (_strnicmp(pIn->ExchangeID, "CZCE", 4) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_CZCE;
	}
	else if (_strnicmp(pIn->ExchangeID, "SHFE", 4) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_SHFE;
	}
	else if (_strnicmp(pIn->ExchangeID, "INE", 3) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_INE;
	}
	else
	{
		oField.ExCode.ExchID = XT_EXCH_UNKONW;
	}

	strncpy(oField.ExCode.Code, pIn->InstrumentID, sizeof(oField.ExCode.Code));
	strncpy(oField.OrderRef, pIn->OrderRef, sizeof(oField.OrderRef));
	strncpy(oField.OrderSysID, pIn->OrderSysID, sizeof(oField.OrderSysID));
	
	if (pIn->OrderPriceType == THOST_FTDC_OPT_AnyPrice)
	{
		oField.PriceType = XT_OPT_AnyPrice;
	}
	else
	{
		oField.PriceType = XT_OPT_LimitPrice;
	}
	
	switch (pIn->Direction)
	{
	case THOST_FTDC_D_Buy:
		oField.Direction = XT_D_Buy;
		break;
	case THOST_FTDC_D_Sell:
		oField.Direction =XT_D_Sell;
		break;
	}
	

	for (int i = 0; i < 5; i++)
	{
		char cflag = 0;
		switch (pIn->CombOffsetFlag[i])
		{
		case THOST_FTDC_OF_Open:
			cflag = XT_OF_Open;
			break;
		case THOST_FTDC_OF_Close:
			cflag = XT_OF_Close;
			break;
		case THOST_FTDC_OF_CloseToday:
			cflag = XT_OF_CloseToday;
			break;
		case THOST_FTDC_OF_CloseYesterday:
			cflag = XT_OF_CloseYesterday;
			break;
		default:
			cflag = 0;
			break;
		}

		oField.OffsetFlag[i] = cflag;
	}

	for (int i = 0; i < 5; i++)
	{
		char cflag = 0;
		switch (pIn->CombHedgeFlag[i])
		{
		case THOST_FTDC_HF_Speculation:
			cflag = XT_HF_Speculation;
			break;
		case THOST_FTDC_HF_Arbitrage:
			cflag = XT_HF_Arbitrage;
			break;
		case THOST_FTDC_HF_MarketMaker:
			cflag = XT_HF_MarketMaker;
			break;
		case THOST_FTDC_HF_Hedge:
			cflag = XT_HF_Hedge;
			break;
		default:
			break;
		}
		oField.HedgeFlag[i] = cflag;
	}
	
	oField.FrontID = pIn->FrontID;
	oField.SessionID = pIn->SessionID;
	oField.Volume = pIn->VolumeTotalOriginal;
	oField.VolumeTraded = pIn->VolumeTraded;
	oField.VolumeTotal = pIn->VolumeTotal;
	oField.LimitPrice = (pIn->LimitPrice);

	strncpy(oField.StatusMsg, pIn->StatusMsg, sizeof(oField.StatusMsg));

	oField.InsertDate = atoi( pIn->InsertDate);
	oField.InsertTime = Trans_Time( pIn->InsertTime);
	oField.UpdateTime = Trans_Time( pIn->UpdateTime);
	oField.BrokerOrderSeq = pIn->BrokerOrderSeq;

	oField.OrderStatus = XT_OST_Unknown;
	switch (pIn->OrderSubmitStatus)
	{
	case THOST_FTDC_OSS_InsertSubmitted:
		oField.OrderStatus = XT_OST_InsertSubmitted;
		break;
	case THOST_FTDC_OSS_CancelSubmitted:
		oField.OrderStatus = XT_OST_CancelSubmitted;
		break;
	case THOST_FTDC_OSS_InsertRejected:
		oField.OrderStatus = XT_OST_Void;
		break;
	}

	switch (pIn->OrderStatus)
	{
	case THOST_FTDC_OST_Unknown:
		oField.OrderStatus = XT_OST_Unknown;
		break;
	case THOST_FTDC_OST_NoTradeQueueing:
		oField.OrderStatus = XT_OST_Accepted;
		break;
	case THOST_FTDC_OST_AllTraded:
		oField.OrderStatus = XT_OST_Traded;
		break;
	case THOST_FTDC_OST_Canceled:
		oField.OrderStatus = XT_OST_Canceled;
		break;
	}

	oField.BrokerID = atoi(pIn->BrokerID);
	strncpy(oField.UserID, pIn->InvestorID, 16);
	
	return 1;
}

int				transhelper::Trans_TradeField(CThostFtdcTradeField* pIn, tagXTTradeField& oField)
{
	if (_strnicmp(pIn->ExchangeID, "CFFEX", 5) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_CFFEX;
	}
	else if (_strnicmp(pIn->ExchangeID, "DCE", 3) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_DCE;
	}
	else if (_strnicmp(pIn->ExchangeID, "CZCE", 4) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_CZCE;
	}
	else if (_strnicmp(pIn->ExchangeID, "SHFE", 4) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_SHFE;
	}
	else if (_strnicmp(pIn->ExchangeID, "INE", 3) == 0)
	{
		oField.ExCode.ExchID = XT_EXCH_INE;
	}
	else
	{
		oField.ExCode.ExchID = XT_EXCH_UNKONW;
	}

	strncpy(oField.ExCode.Code, pIn->InstrumentID, sizeof(oField.ExCode.Code));
	strncpy(oField.OrderRef, pIn->OrderRef, sizeof(oField.OrderRef));
	strncpy(oField.OrderSysID, pIn->OrderSysID, sizeof(oField.OrderSysID));
	strncpy(oField.TradeID, pIn->TradeID, sizeof(oField.TradeID));

	oField.Price = (pIn->Price);
	oField.Volume = pIn->Volume;
	oField.TradeDate = atoi(pIn->TradeDate);
	oField.TradeTime = Trans_Time(pIn->TradeTime);
	oField.TradeType = pIn->TradeType;

	switch (pIn->Direction)
	{
	case THOST_FTDC_D_Buy:
		oField.Direction = XT_D_Buy;
		break;
	case THOST_FTDC_D_Sell:
		oField.Direction =XT_D_Sell;
		break;
	}

	switch (pIn->OffsetFlag)
	{
	case THOST_FTDC_OF_Open:
		oField.OffsetFlag = XT_OF_Open;
		break;
	case THOST_FTDC_OF_Close:
		oField.OffsetFlag = XT_OF_Close;
		break;
	case THOST_FTDC_OF_CloseToday:
		oField.OffsetFlag = XT_OF_CloseToday;
		break;
	case THOST_FTDC_OF_CloseYesterday:
		oField.OffsetFlag = XT_OF_CloseYesterday;
		break;
	}
	
	oField.HedgeFlag = XT_HF_Speculation;
	oField.BrokerID = atoi(pIn->BrokerID);
	strncpy(oField.UserID, pIn->InvestorID, 16);
	
	return 1;
}

int				transhelper::Trans_ReqOrderInsert(tagXTInputOrderField* pIn, CThostFtdcInputOrderField& oField)
{
	//从结构tagXTInputOrderField 转为 CThostFtdcInputOrderField
	//字段 BrokerID InvestorID  UserID RequestID

	strncpy(oField.InstrumentID, pIn->ExCode.Code, sizeof(oField.InstrumentID) - 1);
	strncpy(oField.OrderRef, pIn->OrderRef, sizeof(oField.OrderRef));

	switch (pIn->ExCode.ExchID)
	{
	case XT_EXCH_CFFEX:
		strcpy(oField.ExchangeID, "CFFEX");
		break;
	case XT_EXCH_DCE:
		strcpy(oField.ExchangeID, "DCE");
		break;
	case XT_EXCH_CZCE:
		strcpy(oField.ExchangeID, "CZCE");
		break;
	case XT_EXCH_SHFE:
		strcpy(oField.ExchangeID, "SHFE");
		break;
	case XT_EXCH_INE:
		strcpy(oField.ExchangeID, "INE");
		break;
	}

	oField.VolumeTotalOriginal = pIn->Volume;
	oField.ContingentCondition = THOST_FTDC_CC_Immediately;
	oField.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;

	if (pIn->PriceType == XT_OPT_AnyPrice)
	{
		oField.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
		oField.TimeCondition = THOST_FTDC_TC_IOC;
		oField.VolumeCondition = THOST_FTDC_VC_AV;
	}
	else
	{
		oField.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		oField.TimeCondition = THOST_FTDC_TC_GFD;
		oField.VolumeCondition = THOST_FTDC_VC_AV;
		oField.LimitPrice = pIn->LimitPrice;
	}

	switch (pIn->Direction)
	{
	case XT_D_Buy:
		oField.Direction = THOST_FTDC_D_Buy;
		break;
	case XT_D_Sell:
		oField.Direction = THOST_FTDC_D_Sell;
		break;

	}

	///组合开平标志
	for (int i = 0; i < 5; i++)
	{
		if (pIn->OffsetFlag[i] == '\0')
		{
			oField.CombOffsetFlag[i] = '\0';
			continue;
		}

		switch (pIn->OffsetFlag[i])
		{
		case XT_OF_Open:
			oField.CombOffsetFlag[i] = THOST_FTDC_OF_Open;
			break;
		case XT_OF_Close:
			oField.CombOffsetFlag[i] = THOST_FTDC_OF_Close;
			break;
		case XT_OF_CloseToday:
			oField.CombOffsetFlag[i] = THOST_FTDC_OF_CloseToday;
			break;
		case XT_OF_CloseYesterday:
			oField.CombOffsetFlag[i] = THOST_FTDC_OF_CloseYesterday;
			break;
		}
	}

	//投机套保标志
	for (int i = 0; i < 5; i++)
	{
		if (pIn->HedgeFlag[i] == '\0')
		{
			oField.CombHedgeFlag[i] = '\0';
			continue;
		}

		switch (pIn->HedgeFlag[i])
		{
		case XT_HF_Arbitrage:
			oField.CombHedgeFlag[i] = THOST_FTDC_HF_Arbitrage;
			break;
		case XT_HF_Covered:
			break;
		case XT_HF_Speculation:
			oField.CombHedgeFlag[i] = THOST_FTDC_HF_Speculation;
			break;
		case XT_HF_Hedge:
			oField.CombHedgeFlag[i] = THOST_FTDC_HF_Hedge;
			break;
		case XT_HF_MarketMaker:
			oField.CombHedgeFlag[i] = THOST_FTDC_HF_MarketMaker;
			break;
		case XT_HF_Others:
			break;
		}
	}

	return 1;
}



int				transhelper::Trans_ReqOrderAction(tagXTInputOrderActionField* pIn, CThostFtdcInputOrderActionField& oField)
{
	//从 tagXTInputOrderActionField 转为 CThostFtdcInputOrderActionField
	//字段 BrokerID InvestorID  UserID RequestID

	oField.ActionFlag = THOST_FTDC_AF_Delete;
	oField.FrontID = pIn->FrontID;
	oField.SessionID = pIn->SessionID;

	strncpy(oField.OrderRef, pIn->OrderRef, sizeof(oField.OrderRef) - 1);
	oField.OrderActionRef = atoi(pIn->ActionRef);

	strncpy(oField.OrderSysID, pIn->OrderSysID, sizeof(oField.OrderSysID) - 1);

	switch (pIn->ExCode.ExchID)
	{
	case XT_EXCH_CFFEX:
		strcpy(oField.ExchangeID, "CFFEX");
		break;
	case XT_EXCH_DCE:
		strcpy(oField.ExchangeID, "DCE");
		break;
	case XT_EXCH_CZCE:
		strcpy(oField.ExchangeID, "CZCE");
		break;
	case XT_EXCH_SHFE:
		strcpy(oField.ExchangeID, "SHFE");
		break;
	case XT_EXCH_INE:
		strcpy(oField.ExchangeID, "INE");
		break;
	}

	strncpy(oField.InstrumentID, pIn->ExCode.Code, sizeof(oField.InstrumentID) - 1);

	return 1;
}
