#ifndef _XTUSERDATASTRUCT_DEFINE_H
#define _XTUSERDATASTRUCT_DEFINE_H

#include "XTBase.h"

//=============================================================================================================================================
//Trade Type
//=============================================================================================================================================
///XTOrderPriceType是一个报单价格类型，市价，限价
///任意价
#define		XT_OPT_AnyPrice '1'
///限价
#define		XT_OPT_LimitPrice '2'

typedef		char		XTOrderPriceType;


///XTDirectionType是一个买卖方向类型
///普通买入
#define		XT_D_Buy '0'
///普通卖出
#define		XT_D_Sell '1'
///担保品买入
#define		XT_D_BuyCollateral	'8'
///担保品卖出
#define		XT_D_SellCollateral	'9'
///融资买入
#define		XT_D_BuyMargin	'A'
///融资卖出
#define		XT_D_SellMargin	'B'

typedef		char		XTDirectionType;


///XTOffsetFlagType是一个开平标志类型
///开仓
#define		XT_OF_Open '0'
///平仓
#define		XT_OF_Close '1'
///强平
#define		XT_OF_ForceClose '2'
///平今
#define		XT_OF_CloseToday '3'
///平昨
#define		XT_OF_CloseYesterday '4'
///强减
#define		XT_OF_ForceOff '5'
///本地强平
#define		XT_OF_LocalForceClose '6'

typedef		char		XTOffsetFlagType;

///XTHedgeFlagType是一个投机套保标志类型
///其他（不显示文字）
#define		XT_HF_Others '0'
///投机
#define		XT_HF_Speculation '1'
///套利
#define		XT_HF_Arbitrage '2'
///套保
#define		XT_HF_Hedge '3'
///备兑
#define		XT_HF_Covered '4'
///做市商
#define		XT_HF_MarketMaker	'5'

typedef		char		XTHedgeFlagType;


///XTActionFlagType是一个操作标志类型
///删除
#define		XT_AF_Delete '0'
///修改
//#define		XT_AF_Modify '3'

typedef		char		XTActionFlagType;

///XTOrderStatusType是一个报单状态类型
///已成交
#define		XT_OST_Traded '1'
///待报
#define		XT_OST_InsertSubmitted '2'
///已报
#define		XT_OST_Accepted '3'
///待撤
#define		XT_OST_CancelSubmitted '4'
///已撤单
#define		XT_OST_Canceled '5'
///废单
#define		XT_OST_Void 'a'
///未知1
#define		XT_OST_Unknown 'b'

typedef char			XTOrderStatusType;

///XTPosiDirectionType是持仓的多空方向
///净
#define		XT_PD_Net			'1'
///多头
#define		XT_PD_Long			'2'
///空头
#define		XT_PD_Short		'3'
///备兑
#define		XT_PD_Covered		'4'

typedef char			XTPosiDirectionType;


///组合持仓拆分为单一持仓,初始化不应包含该类型的持仓
#define		XT_TT_SplitCombination '#'
///普通成交
#define		XT_TT_Common			'0'
///期权执行
#define		XT_TT_OptionsExecution	'1'
///OTC成交
#define		XT_TT_OTC				'2'
///期转现衍生成交
#define		XT_TT_EFPDerived		'3'
///组合衍生成交
#define		XT_TT_CombinationDerived '4'

typedef char			XTTradeType;
//=============================================================================================================================================
//Trade Struct
//=============================================================================================================================================
typedef   char					char_11[11];
typedef   char					char_13[13];
typedef   char					char_16[16];
typedef	  char					char_21[21];
typedef   char					char_256[256];
typedef   char					char_41[41];


///用户登录请求
typedef struct 
{
	
	///经纪公司代码
	int						BrokerID;
	///用户代码
	char_16					UserID;
	///密码
	char_41					Password;
	///用户端产品信息
	char_11					UserProductInfo;
	
	char					reserved[512];

}tagXTReqUserLoginField;

///用户登录应答
typedef struct 
{
	///交易日
	int						TradingDay;
	///登录成功时间
	int						LoginTime;
	///经纪公司代码
	int						BrokerID;
	///用户代码
	char_16					UserID;
	///交易系统名称
	char_41					SystemName;
	///前置编号
	int						FrontID;
	///会话编号
	int						SessionID;
	///最大报单引用
	char_13					MaxOrderRef;
	///上期所时间
	int 					SHFETime;
	///大商所时间
	int						DCETime;
	///郑商所时间
	int						CZCETime;
	///中金所时间
	int						FFEXTime;
	///能源中心时间
	int						INETime;
}tagXTRspUserLoginField;


//响应信息（各种请求错误信息的标准返回格式）
typedef struct 
{
	///错误代码：SPI应答错误信息，ErrorID不等于0表示有错误，错误信息在下面的字段表示
	int							ErrorID;
	///错误信息：应答发生错误时记录具体的错误信息
	char_256					ErrorMsg;
}tagXTRspInfoField;

//输入报单
typedef struct  
{
	///经纪公司代码
	int							BrokerID;
	///用户代码
	char_16						UserID;
	///交易所+代码
	tagXTInstrument				ExCode;
	//报单引用
	char_13						OrderRef;
	//报单价格类型
	char						PriceType;		//XTOrderPriceType
	//买卖方向
	char						Direction;		//XTDirectionType
	//开平标志
	char						OffsetFlag[5];		//XTOffsetFlagType
	//投机套保标志
	char						HedgeFlag[5];		//XTHedgeFlagType
	//价格
	double						LimitPrice;
	//数量
	int							Volume;
}tagXTInputOrderField;


//输入报单操作
typedef struct  
{
	///经纪公司代码
	int							BrokerID;
	///用户代码
	char_16						UserID;
	///交易所+代码
	tagXTInstrument				ExCode;
	//撤单引用
	char_13						ActionRef;
	//动作标志
	char						ActionFlag;		//XTActionFlagType
	//原报单引用
	char_13						OrderRef;
	//前置机ID
	int							FrontID;
	//前置机上的sessionid
	int							SessionID;
	//后台的SYSID
	char_21						OrderSysID;
}tagXTInputOrderActionField;


//委托单
typedef struct  
{
	///经纪公司代码
	int							BrokerID;
	///用户代码
	char_16						UserID;
	///交易所+代码
	tagXTInstrument				ExCode;
	//报单引用
	char_13						OrderRef;
	//报单价格类型
	char						PriceType;			//XTOrderPriceType
	//买卖方向
	char						Direction;			//XTDirectionType
	//开平标志
	char						OffsetFlag[5];			//XTOffsetFlagType
	//投机套保标志
	char						HedgeFlag[5];			//XTHedgeFlagType
	//价格
	double						LimitPrice;
	//数量
	int							Volume;
	//前置机ID
	int							FrontID;
	//前置机SessionID
	int							SessionID;
	//报单编号：由交易所或柜台生成的报单编号，在撤单时需要使用
	char_21						OrderSysID;
	//报单状态：用于标示当前委托单状态，如已报、待报、已撤、待撤等
	char						OrderStatus;		//XTOrderStatusType
	//状态信息：如废单原因等
	char_256					StatusMsg;
	//今成交数量：该报单已经成交的数量
	int							VolumeTraded;
	//剩余数量：该报单还未成交的数量
	int							VolumeTotal;
	//报单日期：报单的日期，YYYYMMDD格式
	int							InsertDate;
	//委托时间：报单的时间，HHMMSS格式
	int							InsertTime;
	//最后修改时间
	int							UpdateTime;
	//在经纪公司里的编号
	int							BrokerOrderSeq;
}tagXTOrderField;


//成交单（查询成交或撮合成交后返回的信息）
typedef struct  
{
	///经纪公司代码
	int							BrokerID;
	///用户代码
	char_16						UserID;
	//交易所+代码
	tagXTInstrument				ExCode;
	//报单引用
	char_13						OrderRef;
	//买卖方向
	char						Direction;			//XTDirectionType
	//开平标志
	char						OffsetFlag;			//XTOffsetFlagType
	//投机套保标志
	char						HedgeFlag;			//XTHedgeFlagType
	//价格
	double						Price;
	//数量
	int							Volume;
	//成交时赋予的ID
	char_21						TradeID;
	//委托单的引用
	char_21						OrderSysID;
	//成交时期：成交的日期，YYYYMMDD格式
	int							TradeDate;
	//成交时间：成交的时间，HHMMSS格式
	int							TradeTime;
	//成交类型
	char						TradeType;			//XTTradeType
}tagXTTradeField;



//持仓信息(查询持仓返回)
typedef struct 
{
	///经纪公司代码
	int							BrokerID;
	///用户代码
	char_16						UserID;
	//交易所+代码
	tagXTInstrument				ExCode;
	///持仓多空方向：买入还是卖出
	char						PosiDirection;		//XTPosiDirectionType
	///投机套保标：投机、套利、套保、备对
	char						HedgeFlag;			//XTHedgeFlagType
	///持仓日期
	int 						PositionDate;
	///上日持仓：上日结算持仓当前数量（昨结 - 昨平）
	int							YdPosition;
	///今日总持仓（包括冻结）：今日总持仓，包括可使用的持仓和冻结的持仓
	int							Position;

	///今日持仓
	int                                 TodayPosition;
	///持仓成本
	double                              PositionCost;
	///持仓盈亏
	double                              PositionProfit;

        ///开仓量
        int				OpenVolume;				//统计的今日开仓量
        ///平仓量
        int				CloseVolume;                            //统计的今日平仓量


	/*
	
	///资金差额
	TThostFtdcMoneyType	CashIn;
	///手续费
	TThostFtdcMoneyType	Commission;
	///平仓盈亏
	TThostFtdcMoneyType	CloseProfit;
	
	///上次结算价
	TThostFtdcPriceType	PreSettlementPrice;
	///本次结算价
	TThostFtdcPriceType	SettlementPrice;
	///交易日
	TThostFtdcDateType	TradingDay;
	///开仓成本
	TThostFtdcMoneyType	OpenCost;
	///逐日盯市平仓盈亏
	TThostFtdcMoneyType	CloseProfitByDate;
	///逐笔对冲平仓盈亏
	TThostFtdcMoneyType	CloseProfitByTrade;
	
	*/

	///保留字段
	char						_reserved[256];			

}tagXTInvestorPositionField;


//资金信息(总资产=可用+持仓保证金)
typedef struct  
{
	int							OriSize;		//原始ctp结构体大小(因为资金信息的字段多样，不同需求的字段也不同，所以把原样的结构传出来)
	int							BrokerID;
	int							UserType;
	char						_reserved[500];
}tagXTTradingAccountField;


//=============================================================================================================================================
//=============================================================================================================================================
typedef struct  
{
	///经纪公司代码
	int							BrokerID;
	///用户代码
	char_16						UserID;
}tagXTQryOrderField;


typedef struct  
{
	///经纪公司代码
	int							BrokerID;
	///用户代码
	char_16						UserID;
}tagXTQryTradeField;


typedef struct  
{
	///经纪公司代码
	int							BrokerID;
	///用户代码
	char_16						UserID;
}tagXTQryInvestorPositionField;


typedef struct  
{
	///经纪公司代码
	int							BrokerID;
	///用户代码
	char_16						UserID;
}tagXTQryTradingAccountField;



//=============================================================================================================================================
//=============================================================================================================================================









#endif


