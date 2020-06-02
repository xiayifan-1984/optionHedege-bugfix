#include "TradingStrategyUtil.h"
#include "IDataProcessor.h"
#include "../../memoryManage/utils/util.hpp"
#include "../../memoryManage/utils/Timer.h"
#include "../base/DataStruct.h"
#include "../../system/systemConfig/sys_messages.h"

USING_TE_NAMESPACE

TradingStrategyUtil::TradingStrategyUtil(const string& strategyName) :StrategyUtil(strategyName, false), strategy_name(strategyName)
{
	cur_nano = 0;
	md_nano = 0;
}

long TradingStrategyUtil::get_nano()
{
	return optionHedge::memoryManage::getNanoTime();
}

const char time_format[] = "%Y-%m-%d %H:%M:%S";

string TradingStrategyUtil::get_time()
{
	return optionHedge::memoryManage::parseNano(optionHedge::memoryManage::getNanoTime(), time_format);
}

long TradingStrategyUtil::parse_time(string time_str)
{
	return optionHedge::memoryManage::parseTime(time_str.c_str(), time_format);
}

string TradingStrategyUtil::parse_nano(long nano)
{
	return optionHedge::memoryManage::parseNano(nano, time_format);
}

int TradingStrategyUtil::insert_market_order(short source,
	string instrument_id,
	string exchange_id,
	int volume,
	DirectionType direction,
	OffsetFlagType offset,
	string orderRef)
{
	int rid = get_rid();
	InputOrderField order = {};
	strcpy(order.ExchangeID, exchange_id.c_str());
	strcpy(order.InstrumentID, instrument_id.c_str());
	strcpy(order.OrderRef, orderRef.c_str());
	order.LimitPrice = 0;
	order.Volume = volume;
	order.MinVolume = 1;
	order.TimeCondition = CHAR_IOC;
	order.VolumeCondition = CHAR_AV;
	order.OrderPriceType = CHAR_AnyPrice;
	order.Direction = direction;
	order.OffsetFlag = offset;
	order.HedgeFlag = CHAR_Speculation;
	order.ForceCloseReason = CHAR_NotForceClose;
	order.StopPrice = 0;
	order.IsAutoSuspend = true;
	order.ContingentCondition = CHAR_Immediately;
	strcpy(order.MiscInfo, strategy_name.c_str());
	write_frame_extra(&order, sizeof(InputOrderField), source, MSG_TYPE_ORDER, 1/*lastflag*/, rid, md_nano);
	return rid;
}

int TradingStrategyUtil::insert_limit_order(short source, string instrument_id, string exchange_id, double price, int volume, DirectionType direction, OffsetFlagType offset, string orderRef)
{
	int rid = get_rid();
	InputOrderField order = {};
	strcpy(order.ExchangeID, exchange_id.c_str());
	strcpy(order.InstrumentID, instrument_id.c_str());
	strcpy(order.OrderRef, orderRef.c_str());
	order.LimitPrice = price;
	order.Volume = volume;
	order.MinVolume = 1;
	order.TimeCondition = CHAR_GFD;
	order.VolumeCondition = CHAR_AV;
	order.OrderPriceType = CHAR_LimitPrice;
	order.Direction = direction;
	order.OffsetFlag = offset;
	order.HedgeFlag = CHAR_Speculation;
	order.ForceCloseReason = CHAR_NotForceClose;
	order.StopPrice = 0;
	order.IsAutoSuspend = true;
	order.ContingentCondition = CHAR_Immediately;
	strcpy(order.MiscInfo, strategy_name.c_str());
	write_frame_extra(&order, sizeof(InputOrderField), source, MSG_TYPE_ORDER, 1/*lastflag*/, rid, md_nano);
	return rid;
}

int TradingStrategyUtil::insert_fok_order(short source, string instrument_id, string exchange_id, double price, int volume, DirectionType direction, OffsetFlagType offset, string orderRef)
{
	int rid = get_rid();
	InputOrderField order = {};
	strcpy(order.ExchangeID, exchange_id.c_str());
	strcpy(order.InstrumentID, instrument_id.c_str());
	strcpy(order.OrderRef, orderRef.c_str());
	order.LimitPrice = price;
	order.Volume = volume;
	order.MinVolume = 1;
	order.TimeCondition = CHAR_IOC;
	order.VolumeCondition = CHAR_CV;
	order.OrderPriceType = CHAR_LimitPrice;
	order.Direction = direction;
	order.OffsetFlag = offset;
	order.HedgeFlag = CHAR_Speculation;
	order.ForceCloseReason = CHAR_NotForceClose;
	order.StopPrice = 0;
	order.IsAutoSuspend = true;
	order.ContingentCondition = CHAR_Immediately;
	strcpy(order.MiscInfo, strategy_name.c_str());
	write_frame_extra(&order, sizeof(InputOrderField), source, MSG_TYPE_ORDER, 1/*lastflag*/, rid, md_nano);
	return rid;
}

int TradingStrategyUtil::insert_fak_order(short source, string instrument_id, string exchange_id, double price, int volume, DirectionType direction, OffsetFlagType offset, string orderRef)
{
	int rid = get_rid();
	InputOrderField order = {};
	strcpy(order.ExchangeID, exchange_id.c_str());
	strcpy(order.InstrumentID, instrument_id.c_str());
	strcpy(order.OrderRef, orderRef.c_str());
	order.LimitPrice = price;
	order.Volume = volume;
	order.MinVolume = 1;
	order.TimeCondition = CHAR_IOC;
	order.VolumeCondition = CHAR_AV;
	order.OrderPriceType = CHAR_LimitPrice;
	order.Direction = direction;
	order.OffsetFlag = offset;
	order.HedgeFlag = CHAR_Speculation;
	order.ForceCloseReason = CHAR_NotForceClose;
	order.StopPrice = 0;
	order.IsAutoSuspend = true;
	order.ContingentCondition = CHAR_Immediately;
	strcpy(order.MiscInfo, strategy_name.c_str());
	write_frame_extra(&order, sizeof(InputOrderField), source, MSG_TYPE_ORDER, 1/*lastflag*/, rid, md_nano);
	return rid;
}

int TradingStrategyUtil::req_position(short source)
{
	int rid = get_rid();
	QryPositionField req = {};
	write_frame(&req, sizeof(QryPositionField), source, MSG_TYPE_QRY_POS, 1/*lastflag*/, rid);
	return rid;
}

int TradingStrategyUtil::req_tradingAccount(short source)
{
	int rid = get_rid();
	QryAccountField req = {};
	write_frame(&req, sizeof(QryAccountField), source, MSG_TYPE_QRY_ACCOUNT, 1/*lastflag*/, rid);
	return rid;
}

int TradingStrategyUtil::req_qry_order(short source, string order_sysid)
{
	int rid = get_rid();
	QryOrderField req = {};
	strcpy(req.OrderSysID, order_sysid.c_str());
	write_frame(&req, sizeof(QryOrderField), source, MSG_TYPE_QRY_ORDER, 1/*lastflag*/, rid);
	return rid;
}

int TradingStrategyUtil::req_qry_trade(short source, string investmentId, string tradeId)
{
	int rid = get_rid();
	QryTradeField req = {};
	strcpy(req.InstrumentID, investmentId.c_str());
	strcpy(req.TradeID, tradeId.c_str());
	write_frame(&req,sizeof(QryTradeField),source, MSG_TYPE_QRY_TRADE,1,rid);
}

int TradingStrategyUtil::cancel_order(short source,string orderRef, string instrumentID,string exchangeID,string orderSysID)
{
	if (orderRef=="")
		return -1;
	int rid = get_rid();
	OrderActionField req = {};
	req.InternalOrderID = rid;
	req.ActionFlag = CHAR_Delete;
//	req.LimitPrice = 0;
//	req.VolumeChange = 0;
	req.RequestID = rid;
	strcpy(req.OrderSysID, orderSysID.c_str());
	strcpy(req.OrderRef, orderRef.c_str());
	strcpy(req.InstrumentID,instrumentID.c_str());
	strcpy(req.ExchangeID, exchangeID.c_str());
	write_frame(&req, sizeof(OrderActionField), source, MSG_TYPE_ORDER_ACTION, 1/*lastflag*/, rid);
	return rid;
}

/*
void TradingStrategyUtil::set_pos_back(short source, const char* pos_str)
{
	write_frame(pos_str, strlen(pos_str) + 1, source, MSG_TYPE_STRATEGY_POS_SET, 1, -1);
}
*/