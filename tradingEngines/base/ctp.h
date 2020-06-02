#ifndef TRADINGENGINE_CTP_H
#define TRADINGENGINE_CTP_H

#include "../../system/api/ctp/v6.3.15/include/ThostFtdcUserApiStruct.h"
#include "DataStruct.h"
#include "transfer.h"

inline struct MarketDataField parseFrom(const struct CThostFtdcDepthMarketDataField& ori)
{
	struct MarketDataField res = {};
	memcpy(res.TradingDay, ori.TradingDay, 9);
	memcpy(res.InstrumentID, ori.InstrumentID, 31);
	memcpy(res.ExchangeID, ori.ExchangeID, 9);
	memcpy(res.ExchangeInstID, ori.ExchangeInstID, 64);
	res.LastPrice = ori.LastPrice;
	res.PreSettlementPrice = ori.PreSettlementPrice;
	res.PreClosePrice = ori.PreClosePrice;
	res.PreOpenInterest = ori.PreOpenInterest;
	res.OpenPrice = ori.OpenPrice;
	res.HighestPrice = ori.HighestPrice;
	res.LowestPrice = ori.LowestPrice;
	res.Volume = ori.Volume;
	res.Turnover = ori.Turnover;
	res.OpenInterest = ori.OpenInterest;
	res.ClosePrice = ori.ClosePrice;
	res.SettlementPrice = ori.SettlementPrice;
	res.UpperLimitPrice = ori.UpperLimitPrice;
	res.LowerLimitPrice = ori.LowerLimitPrice;
	res.PreDelta = ori.PreDelta;
	res.CurrDelta = ori.CurrDelta;
	memcpy(res.UpdateTime, ori.UpdateTime, 9);
	res.UpdateMillisec = ori.UpdateMillisec;
	res.BidPrice1 = ori.BidPrice1;
	res.BidVolume1 = ori.BidVolume1;
	res.AskPrice1 = ori.AskPrice1;
	res.AskVolume1 = ori.AskVolume1;
	res.BidPrice2 = ori.BidPrice2;
	res.BidVolume2 = ori.BidVolume2;
	res.AskPrice2 = ori.AskPrice2;
	res.AskVolume2 = ori.AskVolume2;
	res.BidPrice3 = ori.BidPrice3;
	res.BidVolume3 = ori.BidVolume3;
	res.AskPrice3 = ori.AskPrice3;
	res.AskVolume3 = ori.AskVolume3;
	res.BidPrice4 = ori.BidPrice4;
	res.BidVolume4 = ori.BidVolume4;
	res.AskPrice4 = ori.AskPrice4;
	res.AskVolume4 = ori.AskVolume4;
	res.BidPrice5 = ori.BidPrice5;
	res.BidVolume5 = ori.BidVolume5;
	res.AskPrice5 = ori.AskPrice5;
	res.AskVolume5 = ori.AskVolume5;
	return res;
}

inline struct QryPositionField parseFrom(const struct CThostFtdcQryInvestorPositionField& ori)
{
	struct QryPositionField res = {};
	memcpy(res.BrokerID, ori.BrokerID, 11);
	memcpy(res.InvestorID, ori.InvestorID, 13);
	memcpy(res.InstrumentID, ori.InstrumentID, 31);
	return res;
}

inline struct CThostFtdcQryInvestorPositionField parseTo(const struct QryPositionField& lf)
{
	struct CThostFtdcQryInvestorPositionField res = {};
	memcpy(res.BrokerID, lf.BrokerID, 11);
	memcpy(res.InvestorID, lf.InvestorID, 13);
	memcpy(res.InstrumentID, lf.InstrumentID, 31);
	return res;
}

inline struct RspPositionField parseFrom(const struct CThostFtdcInvestorPositionField& ori)
{
	struct RspPositionField res = {};
	memcpy(res.InstrumentID, ori.InstrumentID, 31);
	res.YdPosition = ori.YdPosition;
	res.Position = ori.Position;
	res.TodayPosition = ori.TodayPosition;
	res.PositionDate = ori.PositionDate;
	memcpy(res.BrokerID, ori.BrokerID, 11);
	memcpy(res.InvestorID, ori.InvestorID, 13);
	res.PositionCost = ori.PositionCost;
	res.HedgeFlag = ori.HedgeFlag;
	res.PosiDirection = ori.PosiDirection;
	return res;
}

inline struct InputOrderField parseFrom(const struct CThostFtdcInputOrderField& ori)
{
	struct InputOrderField res = {};
	memcpy(res.BrokerID, ori.BrokerID, 11);
	memcpy(res.UserID, ori.UserID, 16);
	memcpy(res.InvestorID, ori.InvestorID, 13);
	memcpy(res.BusinessUnit, ori.BusinessUnit, 21);
	memcpy(res.ExchangeID, ori.ExchangeID, 9);
	memcpy(res.InstrumentID, ori.InstrumentID, 31);
	memcpy(res.OrderRef, ori.OrderRef, 13);
	res.LimitPrice = ori.LimitPrice;
	res.Volume = ori.VolumeTotalOriginal;
	res.MinVolume = ori.MinVolume;
	res.TimeCondition = ori.TimeCondition;
	res.VolumeCondition = ori.VolumeCondition;
	res.OrderPriceType = ori.OrderPriceType;
	res.Direction = ori.Direction;
	res.OffsetFlag = ori.CombOffsetFlag[0];
	res.HedgeFlag = ori.CombHedgeFlag[0];
	res.ForceCloseReason = ori.ForceCloseReason;
	res.StopPrice = ori.StopPrice;
	res.IsAutoSuspend = ori.IsAutoSuspend;
	res.ContingentCondition = ori.ContingentCondition;
	return res;
}

inline struct CThostFtdcInputOrderField parseTo(const struct InputOrderField& lf)
{
	struct CThostFtdcInputOrderField res = {};
	memcpy(res.BrokerID, lf.BrokerID, 11);
	memcpy(res.UserID, lf.UserID, 16);
	memcpy(res.InvestorID, lf.InvestorID, 13);
	memcpy(res.BusinessUnit, lf.BusinessUnit, 21);
	memcpy(res.ExchangeID, lf.ExchangeID, 9);
	memcpy(res.InstrumentID, lf.InstrumentID, 31);
	memcpy(res.OrderRef, lf.OrderRef, 13);
	res.LimitPrice = lf.LimitPrice;
	res.VolumeTotalOriginal = lf.Volume;
	res.MinVolume = lf.MinVolume;
	res.TimeCondition = lf.TimeCondition;
	res.VolumeCondition = lf.VolumeCondition;
	res.OrderPriceType = lf.OrderPriceType;
	res.Direction = lf.Direction;
	res.CombOffsetFlag[0] = lf.OffsetFlag;
	res.CombHedgeFlag[0] = lf.HedgeFlag;
	res.ForceCloseReason = lf.ForceCloseReason;
	res.StopPrice = lf.StopPrice;
	res.IsAutoSuspend = lf.IsAutoSuspend;
	res.ContingentCondition = lf.ContingentCondition;
	return res;
}

inline struct RtnOrderField parseFrom(const struct CThostFtdcOrderField& ori)
{
	struct RtnOrderField res = {};
	memcpy(res.BrokerID, ori.BrokerID, 11);
	memcpy(res.UserID, ori.UserID, 16);
	memcpy(res.ParticipantID, ori.ParticipantID, 11);
	memcpy(res.InvestorID, ori.InvestorID, 13);
	memcpy(res.BusinessUnit, ori.BusinessUnit, 21);
	memcpy(res.InstrumentID, ori.InstrumentID, 31);
	memcpy(res.OrderRef, ori.OrderRef, 13);
	memcpy(res.ExchangeID, ori.ExchangeID, 9);
	res.LimitPrice = ori.LimitPrice;
	res.VolumeTraded = ori.VolumeTraded;
	res.VolumeTotal = ori.VolumeTotal;
	res.VolumeTotalOriginal = ori.VolumeTotalOriginal;
	res.TimeCondition = ori.TimeCondition;
	res.VolumeCondition = ori.VolumeCondition;
	res.OrderPriceType = ori.OrderPriceType;
	res.Direction = ori.Direction;
	res.OffsetFlag = ori.CombOffsetFlag[0];
	res.HedgeFlag = ori.CombHedgeFlag[0];
	res.OrderStatus = ori.OrderStatus;
	res.RequestID = ori.RequestID;
	res.FrontID = ori.FrontID;
	res.SessionID = ori.SessionID;
	memcpy(res.InsertDate, ori.InsertDate, 9);
	memcpy(res.InsertTime, ori.InsertTime, 9);
	memcpy(res.ActiveTime, ori.ActiveTime, 9);
	memcpy(res.SuspendTime, ori.SuspendTime, 9);
	memcpy(res.UpdateTime, ori.UpdateTime, 9);
	memcpy(res.CancelTime, ori.CancelTime, 9);
	memcpy(res.OrderSysID, ori.OrderSysID, 21);
	res.BrokerOrderSeq = ori.BrokerOrderSeq;
	return res;
}

inline struct RtnTradeField parseFrom(const struct CThostFtdcTradeField& ori)
{
	struct RtnTradeField res = {};
	memcpy(res.BrokerID, ori.BrokerID, 11);
	memcpy(res.UserID, ori.UserID, 16);
	memcpy(res.InvestorID, ori.InvestorID, 13);
	memcpy(res.BusinessUnit, ori.BusinessUnit, 21);
	memcpy(res.InstrumentID, ori.InstrumentID, 31);
	memcpy(res.OrderRef, ori.OrderRef, 13);
	memcpy(res.ExchangeID, ori.ExchangeID, 9);
	memcpy(res.TradeID, ori.TradeID, 21);
	memcpy(res.OrderSysID, ori.OrderSysID, 21);
	memcpy(res.ParticipantID, ori.ParticipantID, 11);
	memcpy(res.ClientID, ori.ClientID, 11);
	res.Price = ori.Price;
	res.Volume = ori.Volume;
	memcpy(res.TradingDay, ori.TradingDay, 13);
	memcpy(res.TradeTime, ori.TradeTime, 9);
	res.Direction = ori.Direction;
	res.OffsetFlag = ori.OffsetFlag;
	res.HedgeFlag = ori.HedgeFlag;
	return res;
}

inline struct OrderActionField parseFrom(const struct CThostFtdcInputOrderActionField& ori)
{
	struct OrderActionField res = {};
	memcpy(res.BrokerID, ori.BrokerID, 11);
	memcpy(res.InvestorID, ori.InvestorID, 13);
	memcpy(res.InstrumentID, ori.InstrumentID, 31);
	memcpy(res.ExchangeID, ori.ExchangeID, 9);
	memcpy(res.UserID, ori.UserID, 16);
	memcpy(res.OrderRef, ori.OrderRef, 13);
	memcpy(res.OrderSysID, ori.OrderSysID, 21);
	res.RequestID = ori.RequestID;
	res.ActionFlag = ori.ActionFlag;
	res.LimitPrice = ori.LimitPrice;
	res.VolumeChange = ori.VolumeChange;
	return res;
}

inline struct CThostFtdcInputOrderActionField parseTo(const struct OrderActionField& lf)
{
	struct CThostFtdcInputOrderActionField res = {};
	memcpy(res.BrokerID, lf.BrokerID, 11);
	memcpy(res.InvestorID, lf.InvestorID, 13);
	memcpy(res.InstrumentID, lf.InstrumentID, 31);
	memcpy(res.ExchangeID, lf.ExchangeID, 9);
	memcpy(res.UserID, lf.UserID, 16);
	memcpy(res.OrderRef, lf.OrderRef, 13);
	memcpy(res.OrderSysID, lf.OrderSysID, 21);
	res.RequestID = lf.RequestID;
	res.ActionFlag = lf.ActionFlag;
	res.LimitPrice = lf.LimitPrice;
	res.VolumeChange = lf.VolumeChange;
	res.OrderActionRef = lf.InternalOrderID;
	return res;
}

inline struct QryAccountField parseFrom(const struct CThostFtdcQryTradingAccountField& ori)
{
	struct QryAccountField res = {};
	memcpy(res.BrokerID, ori.BrokerID, 11);
	memcpy(res.InvestorID, ori.InvestorID, 13);
	return res;
}

inline struct CThostFtdcQryTradingAccountField parseTo(const struct QryAccountField& lf)
{
	struct CThostFtdcQryTradingAccountField res = {};
	memcpy(res.BrokerID, lf.BrokerID, 11);
	memcpy(res.InvestorID, lf.InvestorID, 13);
	return res;
}

inline struct RspAccountField parseFrom(const struct CThostFtdcTradingAccountField& ori)
{
	struct RspAccountField res = {};
	memcpy(res.BrokerID, ori.BrokerID, 11);
	memcpy(res.InvestorID, ori.AccountID, 13);
	res.PreMortgage = ori.PreMortgage;
	res.PreCredit = ori.PreCredit;
	res.PreDeposit = ori.PreDeposit;
	res.PreBalance = ori.PreBalance;
	res.PreMargin = ori.PreMargin;
	res.Deposit = ori.Deposit;
	res.Withdraw = ori.Withdraw;
	res.FrozenMargin = ori.FrozenMargin;
	res.FrozenCash = ori.FrozenCash;
	res.FrozenCommission = ori.FrozenCommission;
	res.CurrMargin = ori.CurrMargin;
	res.CashIn = ori.CashIn;
	res.Commission = ori.Commission;
	res.CloseProfit = ori.CloseProfit;
	res.PositionProfit = ori.PositionProfit;
	res.Balance = ori.Balance;
	res.Available = ori.Available;
	res.WithdrawQuota = ori.WithdrawQuota;
	res.Reserve = ori.Reserve;
	memcpy(res.TradingDay, ori.TradingDay, 9);
	memcpy(res.CurrencyID, ori.CurrencyID, 4);
	res.Credit = ori.Credit;
	res.Mortgage = ori.Mortgage;
	res.ExchangeMargin = ori.ExchangeMargin;
	res.DeliveryMargin = ori.DeliveryMargin;
	res.ExchangeDeliveryMargin = ori.ExchangeDeliveryMargin;
	res.ReserveBalance = ori.ReserveBalance;
	res.Equity = ori.PreBalance - ori.PreCredit - ori.PreMortgage + ori.Mortgage - ori.Withdraw + ori.Deposit + ori.CloseProfit + ori.PositionProfit + ori.CashIn - ori.Commission;
	return res;
}

inline struct CThostFtdcQryOrderField parseTo(const struct QryOrderField& lf)
{
	struct CThostFtdcQryOrderField res = {};
	memcpy(res.BrokerID, lf.BrokerID, 11);
	memcpy(res.InvestorID, lf.InvestorID, 13);
	memcpy(res.InstrumentID, lf.InstrumentID, 31);
	memcpy(res.ExchangeID, lf.ExchangeID, 11);
	memcpy(res.InsertTimeStart, lf.InsertTimeStart, 9);
	memcpy(res.InsertTimeEnd, lf.InsertTimeEnd, 9);
	memcpy(res.InvestUnitID, lf.InvestUnitID, 17);
	return res;
}

inline struct CThostFtdcQryTradeField parseTo(const struct QryTradeField& lf)
{
	struct CThostFtdcQryTradeField res = {};
	memcpy(res.BrokerID, lf.BrokerID, 11);
	memcpy(res.InvestorID, lf.InvestorID, 13);
	memcpy(res.InstrumentID, lf.InstrumentID, 31);
	memcpy(res.ExchangeID, lf.ExchangeID, 11);
	memcpy(res.TradeID, lf.TradeID, 21);
	return res;
}

inline struct RspActionField parseFrom(CThostFtdcOrderActionField &lf)
{
	struct RspActionField res = {};
	memcpy(res.BrokerID, lf.BrokerID, 11);
	memcpy(res.InvestorID, lf.InvestorID, 13);
	res.OrderActionRef = lf.OrderActionRef;
	res.RequestID = lf.RequestID;
	res.FrontID = lf.FrontID;
	res.SessionID = lf.SessionID;
	memcpy(res.ExchangeID, lf.ExchangeID, 9);
	memcpy(res.OrderRef, lf.OrderRef, 13);
	memcpy(res.OrderSysID, lf.OrderSysID, 21);
	res.ActionFlag = lf.ActionFlag;
	res.LimitPrice = lf.LimitPrice;
	res.VolumeChange = lf.VolumeChange;
	memcpy(res.ActionDate, lf.ActionDate, 9);
	memcpy(res.ActionTime, lf.ActionTime, 9);
	memcpy(res.StatusMsg, lf.StatusMsg, 81);
	return res;
}

inline struct RspInstrumentStatus parseFrom(CThostFtdcInstrumentStatusField& lf)
{
	struct RspInstrumentStatus res = {};
	memcpy(res.ExchangeID, lf.ExchangeID, 9);
	memcpy(res.ExchangeInstID, lf.ExchangeInstID, 31);
	memcpy(res.SettlementGroupID, lf.SettlementGroupID, 9);
	memcpy(res.InstrumentID, lf.InstrumentID, 31);
	res.InstrumentStatus = lf.InstrumentStatus;
	res.TradingSegmentSN = lf.TradingSegmentSN;
	memcpy(res.EnterTime, lf.EnterTime, 9);
	res.EnterReason = lf.EnterReason;
	return res;
}
#endif
