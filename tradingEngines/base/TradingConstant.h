#ifndef TRADINGENGINE_TRADINGCONSTANT_H
#define TRADINGENGINE_TRADINGCONSTANT_H

#include <memory.h>
#include <cstdlib>
#include <stdio.h>
// Index for Sources...
const short SOURCE_CTP = 1;
const short SOURCE_XTP = 15;

// Exchange names
#define EXCHANGE_SSE "SSE" //上海证券交易所
#define EXCHANGE_SZE "SZE" //深圳证券交易所
#define EXCHANGE_CFFEX "CFFEX" //中国金融期货交易所
#define EXCHANGE_SHFE "SHFE" //上海期货交易所
#define EXCHANGE_DCE "DCE" //大连商品交易所
#define EXCHANGE_CZCE "CZCE" //郑州商品交易所

// Exchange ids
#define EXCHANGE_ID_SSE 1 //上海证券交易所
#define EXCHANGE_ID_SZE 2 //深圳证券交易所
#define EXCHANGE_ID_CFFEX 11 //中国金融期货交易所
#define EXCHANGE_ID_SHFE 12 //上海期货交易所
#define EXCHANGE_ID_DCE 13 //大连商品交易所
#define EXCHANGE_ID_CZCE 14 //郑州商品交易所

// MsgTypes that used for trading data structure...
const short MSG_TYPE_MD = 101;
const short MSG_TYPE_L2_MD = 102;
const short MSG_TYPE_L2_INDEX = 103;
const short MSG_TYPE_L2_ORDER = 104;
const short MSG_TYPE_L2_TRADE = 105;
const short MSG_TYPE_BAR_MD = 110;
const short MSG_TYPE_QRY_POS = 201;
const short MSG_TYPE_RSP_POS = 202;
const short MSG_TYPE_ORDER = 204;
const short MSG_TYPE_RTN_ORDER = 205;
const short MSG_TYPE_RTN_TRADE = 206;
const short MSG_TYPE_ORDER_ACTION = 207;
const short MSG_TYPE_QRY_ACCOUNT = 208;
const short MSG_TYPE_RSP_ACCOUNT = 209;
const short MSG_TYPE_QRY_ORDER = 210;
const short MSG_TYPE_RTN_QRY_ORDER = 211;
const short MSG_TYPE_QRY_TRADE = 212;
const short MSG_TYPE_RTN_QRY_TRADE = 213;
const short MSG_TYPE_ORDER_ACTION_RSP = 214;
const short MSG_TYPE_RTN_INS_STATUS = 215;

// MsgTypes that from original data structures...
// ctp, idx=1
const short MSG_TYPE_MD_CTP = 1101; // CThostFtdcDepthMarketDataField from ctp/ThostFtdcUserApiStruct.h
const short MSG_TYPE_QRY_POS_CTP = 1201; // CThostFtdcQryInvestorPositionField from ctp/ThostFtdcUserApiStruct.h
const short MSG_TYPE_RSP_POS_CTP = 1202; // CThostFtdcInvestorPositionField from ctp/ThostFtdcUserApiStruct.h
const short MSG_TYPE_ORDER_CTP = 1204; // CThostFtdcInputOrderField from ctp/ThostFtdcUserApiStruct.h
const short MSG_TYPE_RTN_ORDER_CTP = 1205; // CThostFtdcOrderField from ctp/ThostFtdcUserApiStruct.h
const short MSG_TYPE_RTN_TRADE_CTP = 1206; // CThostFtdcTradeField from ctp/ThostFtdcUserApiStruct.h
const short MSG_TYPE_ORDER_ACTION_CTP = 1207; // CThostFtdcInputOrderActionField from ctp/ThostFtdcUserApiStruct.h
const short MSG_TYPE_QRY_ACCOUNT_CTP = 1208; // CThostFtdcQryTradingAccountField from ctp/ThostFtdcUserApiStruct.h
const short MSG_TYPE_RSP_ACCOUNT_CTP = 1209; // CThostFtdcTradingAccountField from ctp/ThostFtdcUserApiStruct.h

///////////////////////////////////
// ActionFlagType: 报单操作标志
///////////////////////////////////
//删除
#define CHAR_Delete          '0'
//挂起
#define CHAR_Suspend         '1'
//激活
#define CHAR_Active          '2'
//修改
#define CHAR_Modify          '3'

typedef char ActionFlagType;

///////////////////////////////////
// ContingentConditionType: 触发条件
///////////////////////////////////
//立即
#define CHAR_Immediately     '1'
//止损
#define CHAR_Touch           '2'
//止赢
#define CHAR_TouchProfit     '3'
//预埋单
#define CHAR_ParkedOrder     '4'
//最新价大于条件价
#define CHAR_LastPriceGreaterThanStopPrice '5'
//最新价大于等于条件价
#define CHAR_LastPriceGreaterEqualStopPrice '6'
//最新价小于条件价
#define CHAR_LastPriceLesserThanStopPrice '7'
//最新价小于等于条件价
#define CHAR_LastPriceLesserEqualStopPrice '8'
//卖一价大于条件价
#define CHAR_AskPriceGreaterThanStopPrice '9'
//卖一价大于等于条件价
#define CHAR_AskPriceGreaterEqualStopPrice 'A'
//卖一价小于条件价
#define CHAR_AskPriceLesserThanStopPrice 'B'
//卖一价小于等于条件价
#define CHAR_AskPriceLesserEqualStopPrice 'C'
//买一价大于条件价
#define CHAR_BidPriceGreaterThanStopPrice 'D'
//买一价大于等于条件价
#define CHAR_BidPriceGreaterEqualStopPrice 'E'
//买一价小于条件价
#define CHAR_BidPriceLesserThanStopPrice 'F'
//买一价小于等于条件价
#define CHAR_BidPriceLesserEqualStopPrice 'H'

typedef char ContingentConditionType;

///////////////////////////////////
// DirectionType: 买卖方向
///////////////////////////////////
//买
#define CHAR_Buy             '0'
//卖
#define CHAR_Sell            '1'

typedef char DirectionType;

///////////////////////////////////
// ForceCloseReasonType: 强平原因
///////////////////////////////////
//非强平
#define CHAR_NotForceClose   '0'
//资金不足
#define CHAR_LackDeposit     '1'
//客户超仓
#define CHAR_ClientOverPositionLimit '2'
//会员超仓
#define CHAR_MemberOverPositionLimit '3'
//持仓非整数倍
#define CHAR_NotMultiple     '4'
//违规
#define CHAR_Violation       '5'
//其它
#define CHAR_Other           '6'
//自然人临近交割
#define CHAR_PersonDeliv     '7'

typedef char ForceCloseReasonType;

///////////////////////////////////
// HedgeFlagType: 投机套保标志
///////////////////////////////////
//投机
#define CHAR_Speculation     '1'
//套利
#define CHAR_Argitrage       '2'
//套保
#define CHAR_Hedge           '3'
//做市商(femas)
#define CHAR_MarketMaker     '4'
//匹配所有的值(femas)
#define CHAR_AllValue        '9'

typedef char HedgeFlagType;

///////////////////////////////////
//OffsetFlagType: 开平标志
///////////////////////////////////
//开仓
#define CHAR_Open            '0'
//平仓
#define CHAR_Close           '1'
//强平
#define CHAR_ForceClose      '2'
//平今
#define CHAR_CloseToday      '3'
//平昨
#define CHAR_CloseYesterday  '4'
//强减
#define CHAR_ForceOff        '5'
//本地强平
#define CHAR_LocalForceClose '6'
//不分开平
#define CHAR_Non             'N'

typedef char OffsetFlagType;

///////////////////////////////////
// OrderPriceTypeType: 报单价格条件
///////////////////////////////////
//任意价
#define CHAR_AnyPrice        '1'
//限价
#define CHAR_LimitPrice      '2'
//最优价
#define CHAR_BestPrice       '3'

typedef char OrderPriceTypeType;

///////////////////////////////////
// OrderStatusType: 报单状态
///////////////////////////////////
//全部成交（最终状态）
#define CHAR_AllTraded       '0'
//部分成交还在队列中
#define CHAR_PartTradedQueueing '1'
//部分成交不在队列中（部成部撤， 最终状态）
#define CHAR_PartTradedNotQueueing '2'
//未成交还在队列中
#define CHAR_NoTradeQueueing '3'
//未成交不在队列中（被拒绝，最终状态）
#define CHAR_NoTradeNotQueueing '4'
//撤单
#define CHAR_Canceled        '5'
//订单已报入交易所未应答
#define CHAR_AcceptedNoReply '6'
//未知
#define CHAR_Unknown         'a'
//尚未触发
#define CHAR_NotTouched      'b'
//已触发
#define CHAR_Touched         'c'
//废单错误（最终状态）
#define CHAR_Error           'd'
//订单已写入
#define CHAR_OrderInserted   'i'
//前置已接受
#define CHAR_OrderAccepted   'j'

typedef char OrderStatusType;

///////////////////////////////////
// LfPosiDirectionType: 持仓多空方向
///////////////////////////////////
//净
#define CHAR_Net             '1'
//多头
#define CHAR_Long            '2'
//空头
#define CHAR_Short           '3'

typedef char PosiDirectionType;

///////////////////////////////////
// PositionDateType: 持仓日期
///////////////////////////////////
//今日持仓
#define CHAR_Today           '1'
//历史持仓
#define CHAR_History         '2'
//两种持仓
#define CHAR_Both            '3'

typedef char PositionDateType;

///////////////////////////////////
// TimeConditionType: 有效期类型
///////////////////////////////////
//立即完成，否则撤销
#define CHAR_IOC             '1'
//本节有效
#define CHAR_GFS             '2'
//当日有效
#define CHAR_GFD             '3'
//指定日期前有效
#define CHAR_GTD             '4'
//撤销前有效
#define CHAR_GTC             '5'
//集合竞价有效
#define CHAR_GFA             '6'

typedef char TimeConditionType;

///////////////////////////////////
// VolumeConditionType: 成交量类型
///////////////////////////////////
//任何数量
#define CHAR_AV              '1'
//最小数量
#define CHAR_MV              '2'
//全部数量
#define CHAR_CV              '3'

typedef char VolumeConditionType;
#endif
