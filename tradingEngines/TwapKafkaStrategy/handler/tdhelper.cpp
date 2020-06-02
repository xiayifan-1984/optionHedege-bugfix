#include "tdhelper.h"


int		tdhelper::Trans_RspInfo(short errorId, const char* errorMsg, CThostFtdcRspInfoField& oRspInfo)
{
	if (errorId != 0)
	{
		oRspInfo.ErrorID = errorId;
		if (errorMsg)
		{
			strncpy(oRspInfo.ErrorMsg, errorMsg, sizeof(oRspInfo.ErrorMsg));
		}
	}
	else
	{
		oRspInfo.ErrorID = 0;
	}

	return 1;
}

int		tdhelper::Trans_TradingAccountField(const RspAccountField* pIn, CThostFtdcTradingAccountField& oField)
{
	strncpy(oField.BrokerID, pIn->BrokerID, sizeof(oField.BrokerID) );
	strncpy(oField.AccountID, pIn->InvestorID, sizeof(oField.AccountID) );

	oField.PreMortgage = pIn->PreMortgage;
	oField.PreCredit = pIn->PreCredit;
	oField.PreDeposit = pIn->PreDeposit;
	oField.PreBalance = pIn->PreBalance;

	oField.PreMargin = pIn->PreMargin;
	oField.Deposit = pIn->Deposit;
	oField.Withdraw = pIn->Withdraw;

	oField.FrozenMargin = pIn->FrozenMargin;
	oField.FrozenCash = pIn->FrozenCash;
	oField.FrozenCommission = pIn->FrozenCommission;
	oField.CurrMargin = pIn->CurrMargin;
	oField.CashIn = pIn->CashIn;
	oField.Commission = pIn->Commission;

	oField.CloseProfit = pIn->CloseProfit;
	oField.PositionProfit = pIn->PositionProfit;
	oField.Balance = pIn->Balance;
	oField.Available = pIn->Available;
	oField.WithdrawQuota = pIn->WithdrawQuota;
	oField.Reserve = pIn->Reserve;
	strncpy(oField.TradingDay, pIn->TradingDay, sizeof(oField.TradingDay));

	oField.Credit = pIn->Credit;
	oField.Mortgage = pIn->Mortgage;
	oField.ExchangeMargin = pIn->ExchangeMargin;
	oField.DeliveryMargin = pIn->DeliveryMargin;
	oField.ExchangeDeliveryMargin = pIn->ExchangeDeliveryMargin;
	oField.ReserveBalance = pIn->ReserveBalance;
	
	strncpy(oField.CurrencyID, pIn->CurrencyID, sizeof(oField.CurrencyID) );

	return 1;
}


int		tdhelper::Trans_InvestorPositionField(const RspPositionField* pIn, CThostFtdcInvestorPositionField& oField)
{
	strncpy(oField.BrokerID, pIn->BrokerID, sizeof(oField.BrokerID));
	strncpy(oField.InvestorID, pIn->InvestorID, sizeof(oField.InvestorID));

	strncpy(oField.InstrumentID, pIn->InstrumentID, sizeof(oField.InstrumentID));
	oField.YdPosition = pIn->YdPosition;
	oField.Position = pIn->Position;
	oField.TodayPosition = pIn->TodayPosition;

	oField.PositionCost = pIn->PositionCost;
	oField.HedgeFlag = pIn->HedgeFlag;
	oField.PosiDirection = pIn->PosiDirection;

	return 1;
}


int		tdhelper::Trans_OrderField(const RtnOrderField* pIn, CThostFtdcOrderField& oField)
{
	strncpy(oField.BrokerID, pIn->BrokerID, sizeof(oField.BrokerID));
	strncpy(oField.InvestorID, pIn->InvestorID, sizeof(oField.AccountID));

	strncpy(oField.InstrumentID, pIn->InstrumentID, sizeof(oField.InstrumentID));
	strncpy(oField.OrderRef, pIn->OrderRef, sizeof(oField.OrderRef));
	strncpy(oField.UserID, pIn->UserID, sizeof(oField.UserID));

	oField.OrderPriceType = pIn->OrderPriceType;
	oField.Direction = pIn->Direction;

	oField.CombOffsetFlag[0] = pIn->OffsetFlag;
	oField.CombHedgeFlag[0] = pIn->HedgeFlag;
	oField.LimitPrice = pIn->LimitPrice;

	oField.VolumeTotalOriginal = pIn->VolumeTotalOriginal;
	oField.TimeCondition = pIn->TimeCondition;
	strncpy(oField.GTDDate, pIn->GTDDate, sizeof(oField.GTDDate));

	oField.VolumeCondition = pIn->VolumeCondition;
	oField.MinVolume = pIn->MinVolume;
	oField.ContingentCondition = pIn->ContingentCondition;
	oField.StopPrice = pIn->StopPrice;
	oField.ForceCloseReason = pIn->ForceCloseReason;


	oField.IsAutoSuspend = pIn->IsAutoSuspend;
	strncpy(oField.BusinessUnit, pIn->BusinessUnit, sizeof(oField.BusinessUnit));
	oField.RequestID = pIn->RequestID;
	strncpy(oField.OrderLocalID, pIn->OrderLocalID, sizeof(oField.OrderLocalID));
	strncpy(oField.ExchangeID, pIn->ExchangeID, sizeof(oField.ExchangeID));

	strncpy(oField.ParticipantID, pIn->ParticipantID, sizeof(oField.ParticipantID));
	strncpy(oField.ClientID, pIn->ClientID, sizeof(oField.ClientID));
	strncpy(oField.ExchangeInstID, pIn->ExchangeInstID, sizeof(oField.ExchangeInstID));
	strncpy(oField.TraderID, pIn->TraderID, sizeof(oField.TraderID));
	oField.InstallID = pIn->InstallID;

	oField.OrderSubmitStatus = pIn->OrderSubmitStatus;
	oField.NotifySequence = pIn->NotifySequence;
	strncpy(oField.TradingDay, pIn->TradingDay, sizeof(oField.TradingDay));
	oField.SettlementID = pIn->SettlementID;
	strncpy(oField.OrderSysID, pIn->OrderSysID, sizeof(oField.OrderSysID));

	oField.OrderSource = pIn->OrderSource;
	oField.OrderStatus = pIn->OrderStatus;
	oField.OrderType = pIn->OrderType;
	oField.VolumeTraded = pIn->VolumeTraded;
	oField.VolumeTotal = pIn->VolumeTotal;


	strncpy(oField.InsertDate, pIn->InsertDate, sizeof(oField.InsertDate));
	strncpy(oField.InsertTime, pIn->InsertTime, sizeof(oField.InsertTime));
	strncpy(oField.ActiveTime, pIn->ActiveTime, sizeof(oField.ActiveTime));
	strncpy(oField.SuspendTime, pIn->SuspendTime, sizeof(oField.SuspendTime));
	strncpy(oField.UpdateTime, pIn->UpdateTime, sizeof(oField.UpdateTime));

	strncpy(oField.CancelTime, pIn->CancelTime, sizeof(oField.CancelTime));
	strncpy(oField.ActiveTraderID, pIn->ActiveTraderID, sizeof(oField.ActiveTraderID));
	strncpy(oField.ClearingPartID, pIn->ClearingPartID, sizeof(oField.ClearingPartID));
	oField.SequenceNo = pIn->SequenceNo;
	oField.FrontID = pIn->FrontID;

	oField.SessionID = pIn->SessionID;
	strncpy(oField.UserProductInfo, pIn->UserProductInfo, sizeof(oField.UserProductInfo));
	strncpy(oField.StatusMsg, pIn->StatusMsg, sizeof(oField.StatusMsg));
	oField.UserForceClose = pIn->UserForceClose;
	strncpy(oField.ActiveUserID, pIn->ActiveUserID, sizeof(oField.ActiveUserID));

	oField.BrokerOrderSeq = pIn->BrokerOrderSeq;
	strncpy(oField.RelativeOrderSysID, pIn->RelativeOrderSysID, sizeof(oField.RelativeOrderSysID));
	oField.ZCETotalTradedVolume = pIn->ZCETotalTradedVolume;
	oField.IsSwapOrder = pIn->IsSwapOrder;
	strncpy(oField.BranchID, pIn->BranchID, sizeof(oField.BranchID));

	strncpy(oField.InvestUnitID, pIn->InvestUnitID, sizeof(oField.InvestUnitID));
	strncpy(oField.AccountID, pIn->AccountID, sizeof(oField.AccountID));
	strncpy(oField.CurrencyID, pIn->CurrencyID, sizeof(oField.CurrencyID));
	strncpy(oField.IPAddress, pIn->IPAddress, sizeof(oField.IPAddress));
	strncpy(oField.MacAddress, pIn->MacAddress, sizeof(oField.MacAddress));
	
	return 1;
}

int		tdhelper::Trans_TradeField(const RtnTradeField* pIn, CThostFtdcTradeField& oField)
{
	strncpy(oField.BrokerID, pIn->BrokerID, sizeof(oField.BrokerID));
	strncpy(oField.UserID, pIn->UserID, sizeof(oField.UserID));
	strncpy(oField.InvestorID, pIn->InvestorID, sizeof(oField.InvestorID));
	strncpy(oField.BusinessUnit, pIn->BusinessUnit, sizeof(oField.BusinessUnit));
	strncpy(oField.InstrumentID, pIn->InstrumentID, sizeof(oField.InstrumentID));

	strncpy(oField.OrderRef, pIn->OrderRef, sizeof(oField.OrderRef));
	strncpy(oField.ExchangeID, pIn->ExchangeID, sizeof(oField.ExchangeID));
	strncpy(oField.TradeID, pIn->TradeID, sizeof(oField.TradeID));
	strncpy(oField.OrderSysID, pIn->OrderSysID, sizeof(oField.OrderSysID));
	strncpy(oField.ParticipantID, pIn->ParticipantID, sizeof(oField.ParticipantID));

	strncpy(oField.ClientID, pIn->ClientID, sizeof(oField.ClientID));
	oField.Price = pIn->Price;
	oField.Volume = pIn->Volume;
	strncpy(oField.TradingDay, pIn->TradingDay, sizeof(oField.TradingDay));
	strncpy(oField.TradeTime, pIn->TradeTime, sizeof(oField.TradeTime));

	oField.Direction = pIn->Direction;
	oField.OffsetFlag = pIn->OffsetFlag;
	oField.HedgeFlag = pIn->HedgeFlag;
	
	return 1;
}

int		tdhelper::Trans_OrderInsert(const InputOrderField* pIn, CThostFtdcInputOrderField& oField)
{
	strncpy(oField.BrokerID, pIn->BrokerID, sizeof(oField.BrokerID));
	strncpy(oField.InvestorID, pIn->InvestorID, sizeof(oField.AccountID));

	strncpy(oField.InstrumentID, pIn->InstrumentID, sizeof(oField.InstrumentID));
	strncpy(oField.OrderRef, pIn->OrderRef, sizeof(oField.OrderRef));
	strncpy(oField.UserID, pIn->UserID, sizeof(oField.UserID));

	oField.OrderPriceType = pIn->OrderPriceType;
	oField.Direction = pIn->Direction;
	oField.CombOffsetFlag[0] = pIn->OffsetFlag;
	oField.CombHedgeFlag[0] = pIn->HedgeFlag;
	oField.LimitPrice = pIn->LimitPrice;
	oField.VolumeTotalOriginal = pIn->Volume;

	strncpy(oField.ExchangeID, pIn->ExchangeID, sizeof(oField.ExchangeID));

	return 1;
}
