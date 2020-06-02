
#ifndef _XTTWAPSTRUCT_DEFINE_H
#define _XTTWAPSTRUCT_DEFINE_H
#include "XTTradeStruct.h"

//定义Twap 的类型
#define XT_TWAP_UNKNOWN         '\0'            //未知

#define XT_TWAP_NORMAL          '1'             //普通Twap 

typedef char  XTTwapOrderType;

//定义Twap 的状态
#define XT_TWAP_AVOID           '\0'             //废

#define XT_TWAP_Accepted        '1'             //已接收

#define XT_TWAP_Executing       '2'             //执行中

#define XT_TWAP_End             '3'             //已完成

#define XT_TWAP_Cancelled       '4'             //已撤销


typedef char XTTwapOrderStatus;

typedef   char					char_128[128];

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack(1)

//输入报单操作
typedef struct
{
    ///经纪公司代码
	int						BrokerID;
	///用户代码
	char_16					UserID;
    //编号
    char_13                 TwapOrderID;
    //动作标志
    char                    ActionFlag;     //XTActionFlagType
}tagXTTwapOrderActionField;

//输入报单
typedef struct
{
    ///经纪公司代码
	int							BrokerID;
	///用户代码
	char_16						UserID;
    //Twap单 ID(由客户端编号)
    char_13                     TwapOrderID;

	///交易所+代码
	tagXTInstrument				ExCode;
	//买卖方向
	char						Direction;		//XTDirectionType
	//开平标志
	char						OffsetFlag;		//XTOffsetFlagType
	//投机套保标志
	char						HedgeFlag;		//XTHedgeFlagType
	//数量
	int							Volume;

    //Twap单 类型
    char                        TwapOrderType;      //XTTwapOrderType
    //起始时间(终端校验: 终止时间要大于起始时间;其他约定:如果启动时时间>终止，则不再运行)
    int                         BeginTime;          //HHMMSS
    //终止时间
    int                         EndTime;            //HHMMSS
}tagXTTwapInputOrderField;

//查询报单(如果TwapOrderID为空，就是请求该用户的所有报单，如果不为空，就是请求某一个ID的内容)
typedef struct
{
    ///经纪公司代码
	int							BrokerID;
	///用户代码
	char_16						UserID;
    //Twap单ID
    char_13                     TwapOrderID;
}tagXTTwapQryOrderField;


//报单及报单状态
struct tagXTTwapOrderField : public tagXTTwapInputOrderField
{
    //记录下单日期
    int                         InsertDate;         //YYMMDD         
    //记录下单时间
    int                         InsertTime;         //HHMMSS
    //对应的交易日(交易日的值，取决于Insert的当下)(仅在该交易日有效)
    int                         TradingDate;        //YYMMDD
    //报单状态
    char                        TwapOrderStatus;    //XTTwapOrderStatus
    //剩余数量(未下单数量)
    int                         RemainVolume;
    //消息备注
    char_128                     StatusMsg;
    //经过规整，经过交易时段对齐，真实的开始时间(48H)
    int                         RealStartTime;      //用于调试
    //经过规整，经过交易时段对齐，真实的结束时间(48H)
    int                         RealEndTime;        //用于调试
    //经过校验，真实的交易区段的序号
    int                         RealStartNo;        //用于调试
    //经过校验，真实的交易区段的序号
    int                         RealEndNo;          //用于调试
};


struct tagXTTwapOrderInsertRtnField
{
	///经纪公司代码
	int							BrokerID;
	///用户代码
	char_16						UserID;
	///交易所+代码
	tagXTInstrument				ExCode;
    //Twap单ID
    char_13                     TwapOrderID;
	//委托单的Ref
	char_13						RealOrderRef;
};

#pragma pack()



#endif
