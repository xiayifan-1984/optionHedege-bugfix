#ifndef _XTTPARKEDSTRUCT_DEFINE_H
#define _XTTPARKEDSTRUCT_DEFINE_H

#include "XTTradeStruct.h"

/////////////////////////////////////////////////////////////////////////
///XTParkedOrderStatusType是一个预埋单/条件单状态类型
/////////////////////////////////////////////////////////////////////////
///未发送
#define XT_PAOS_NotSend     '1'
///已发送
#define XT_PAOS_Send         '2'
///已删除
#define XT_PAOS_Deleted     '3'

typedef     char    XTParkedOrderStatusType;

/////////////////////////////////////////////////////////////////////////
///XTParkedOrderType是一个类型
/////////////////////////////////////////////////////////////////////////

/// 条件单
#define XT_PT_CONDITION     '1'
/// 交易所开盘时
#define XT_PT_EXCHCONTINUE  '2'
/// 集合竞价时
#define XT_PT_EXCHAUCTION   '3'
/// 定时，时间到达某个点
#define XT_PT_TIME          '4'
///预埋/手工发出
#define XT_PT_HUMAN         '5'

typedef     char    XTParkedOrderType;

/////////////////////////////////////////////////////////////////////////
///XTContingentConditionType是一个触发条件类型
/////////////////////////////////////////////////////////////////////////

///最新价大于条件价
#define XT_CC_LastPriceGreaterThanStopPrice '5'
///最新价大于等于条件价
#define XT_CC_LastPriceGreaterEqualStopPrice '6'
///最新价小于条件价
#define XT_CC_LastPriceLesserThanStopPrice '7'
///最新价小于等于条件价
#define XT_CC_LastPriceLesserEqualStopPrice '8'
///卖一价大于条件价
#define XT_CC_AskPriceGreaterThanStopPrice '9'
///卖一价大于等于条件价
#define XT_CC_AskPriceGreaterEqualStopPrice 'A'
///卖一价小于条件价
#define XT_CC_AskPriceLesserThanStopPrice 'B'
///卖一价小于等于条件价
#define XT_CC_AskPriceLesserEqualStopPrice 'C'
///买一价大于条件价
#define XT_CC_BidPriceGreaterThanStopPrice 'D'
///买一价大于等于条件价
#define XT_CC_BidPriceGreaterEqualStopPrice 'E'
///买一价小于条件价
#define XT_CC_BidPriceLesserThanStopPrice 'F'
///买一价小于等于条件价
#define XT_CC_BidPriceLesserEqualStopPrice 'H'

typedef     char    XTContingentConditionType;


//补充定义XTActionFlagType

#define     XT_AF_HUMANSEND         'A'


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack(1)

//[1]请求下单
typedef struct  
{
    //基本要素, 和tagXTInputOrderField相比，去掉了OrderRef和PriceType
	int					BrokerID;
	char_16				UserID;
	tagXTInstrument		ExCode;
	
	char				Direction;		//XTDirectionType
	char				OffsetFlag[5];		//XTOffsetFlagType
	char				HedgeFlag[5];		//XTHedgeFlagType
	double				LimitPrice;
	int					Volume;

    //其他要素
    char                ParkedType;         //XTParkedOrderType
    char_13             ParkedOrderID;
    
    union
    {
        struct
        {
            char                ConditionType;  //XTContingentConditionType
            double              StopPrice;      //条件价
        };
        struct
        {
            int                 StopTime;       //定时HHMMSS
        };

        char    reserved[32];
    };

}tagXTParkedOrderInputField;

//[2]请求操作(包括撤单和手工下单)
typedef struct
{
    int                 BrokerID;
    char_16             UserID;
    tagXTInstrument     ExCode;

    char                ParkedType;         //XTParkedOrderType
    char_13             ParkedOrderID;
    char                ActionFlag;         //注意： XTActionFlagType 可以是撤单，也可以是 手工发单

}tagXTParkedOrderActionField;

//[3]请求查询
typedef struct
{
    int                 BrokerID;
    char_16             UserID;
    tagXTInstrument     ExCode;

    char                ParkedType;         //XTParkedOrderType[可以不填]
    char_13             ParkedOrderID;      //[可以不填]
    
}tagXTQryParkedOrderField;


//[4]回应查询和推送 Park结构
struct tagXTParkedOrderField : public tagXTParkedOrderInputField
{
    //记录下单日期
    int                 InsertDate;         //YYMMDD         
    //记录下单时间
    int                 InsertTime;         //HHMMSS
    //对应的交易日(交易日的值，取决于Insert的当下)(仅在该交易日有效)
    int                 TradingDate;        //YYMMDD
    //状态
    char                ParkedStatus;       //XTParkedOrderStatusType
    //消息备注
    char                StatusMsg[128];

};



#pragma pack()




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif
