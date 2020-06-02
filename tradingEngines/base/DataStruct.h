#ifndef TRADINGENGINE_DATASTRUCT_H
#define TRADINGENGINE_DATASTRUCT_H

#include "TradingConstant.h"

typedef char char_19[19];
typedef char char_21[21];
typedef char char_64[64];
typedef char char_7[7];
typedef char char_9[9];
typedef char char_30[30];
typedef char char_31[31];
typedef char char_16[16];
typedef char char_13[13];
typedef char char_2[2];
typedef char char_11[11];
typedef char char_81[81];
typedef char char_17[17];
typedef char char_4[4];

struct MarketDataField
{
	char_13  	TradingDay;            //������
	char_31  	InstrumentID;          //��Լ����
	char_9   	ExchangeID;            //����������
	char_64  	ExchangeInstID;        //��Լ�ڽ������Ĵ���
	double   	LastPrice;             //���¼�
	double   	PreSettlementPrice;    //�ϴν����
	double   	PreClosePrice;         //������
	double   	PreOpenInterest;       //��ֲ���
	double   	OpenPrice;             //����
	double   	HighestPrice;          //��߼�
	double   	LowestPrice;           //��ͼ�
	int      	Volume;                //����
	double   	Turnover;              //�ɽ����
	double   	OpenInterest;          //�ֲ���
	double   	ClosePrice;            //������
	double   	SettlementPrice;       //���ν����
	double   	UpperLimitPrice;       //��ͣ���
	double   	LowerLimitPrice;       //��ͣ���
	double   	PreDelta;              //����ʵ��
	double   	CurrDelta;             //����ʵ��
	char_13  	UpdateTime;            //����޸�ʱ��
	int      	UpdateMillisec;        //����޸ĺ���
	double   	BidPrice1;             //�����һ
	int      	BidVolume1;            //������һ
	double   	AskPrice1;             //������һ
	int      	AskVolume1;            //������һ
	double   	BidPrice2;             //����۶�
	int      	BidVolume2;            //��������
	double   	AskPrice2;             //�����۶�
	int      	AskVolume2;            //��������
	double   	BidPrice3;             //�������
	int      	BidVolume3;            //��������
	double   	AskPrice3;             //��������
	int      	AskVolume3;            //��������
	double   	BidPrice4;             //�������
	int      	BidVolume4;            //��������
	double   	AskPrice4;             //��������
	int      	AskVolume4;            //��������
	double   	BidPrice5;             //�������
	int      	BidVolume5;            //��������
	double   	AskPrice5;             //��������
	int      	AskVolume5;            //��������
};

struct L2MarketDataField
{
	char_9   	TradingDay;            //������
	char_9   	TimeStamp;             //ʱ���
	char_9   	ExchangeID;            //����������
	char_31  	InstrumentID;          //��Լ����
	double   	PreClosePrice;         //�����̼�
	double   	OpenPrice;             //���̼�
	double   	ClosePrice;            //���̼�
	double   	IOPV;                  //��ֵ��ֵ
	double   	YieldToMaturity;       //����������
	double   	AuctionPrice;          //��̬�ο��۸�
	char     	TradingPhase;          //���׽׶�
	char     	OpenRestriction;       //��������
	double   	HighPrice;             //��߼�
	double   	LowPrice;              //��ͼ�
	double   	LastPrice;             //���¼�
	double   	TradeCount;            //�ɽ�����
	double   	TotalTradeVolume;      //�ɽ�����
	double   	TotalTradeValue;       //�ɽ��ܽ��
	double   	OpenInterest;          //�ֲ���
	double   	TotalBidVolume;        //ί����������
	double   	WeightedAvgBidPrice;   //��Ȩƽ��ί���
	double   	AltWeightedAvgBidPrice;   //ծȯ��Ȩƽ��ί���
	double   	TotalOfferVolume;      //ί����������
	double   	WeightedAvgOfferPrice;   //��Ȩƽ��ί����
	double   	AltWeightedAvgOfferPrice;   //ծȯ��Ȩƽ��ί���۸�
	int      	BidPriceLevel;         //������
	int      	OfferPriceLevel;       //�������
	double   	BidPrice1;             //�����һ
	double   	BidVolume1;            //������һ
	int      	BidCount1;             //ʵ������ί�б���һ
	double   	BidPrice2;             //����۶�
	double   	BidVolume2;            //��������
	int      	BidCount2;             //ʵ������ί�б�����
	double   	BidPrice3;             //�������
	double   	BidVolume3;            //��������
	int      	BidCount3;             //ʵ������ί�б�����
	double   	BidPrice4;             //�������
	double   	BidVolume4;            //��������
	int      	BidCount4;             //ʵ������ί�б�����
	double   	BidPrice5;             //�������
	double   	BidVolume5;            //��������
	int      	BidCount5;             //ʵ������ί�б�����
	double   	BidPrice6;             //�������
	double   	BidVolume6;            //��������
	int      	BidCount6;             //ʵ������ί�б�����
	double   	BidPrice7;             //�������
	double   	BidVolume7;            //��������
	int      	BidCount7;             //ʵ������ί�б�����
	double   	BidPrice8;             //����۰�
	double   	BidVolume8;            //��������
	int      	BidCount8;             //ʵ������ί�б�����
	double   	BidPrice9;             //����۾�
	double   	BidVolume9;            //��������
	int      	BidCount9;             //ʵ������ί�б�����
	double   	BidPriceA;             //�����ʮ
	double   	BidVolumeA;            //������ʮ
	int      	BidCountA;             //ʵ������ί�б���ʮ
	double   	OfferPrice1;           //������һ
	double   	OfferVolume1;          //������һ
	int      	OfferCount1;           //ʵ������ί�б���һ
	double   	OfferPrice2;           //�����۶�
	double   	OfferVolume2;          //��������
	int      	OfferCount2;           //ʵ������ί�б�����
	double   	OfferPrice3;           //��������
	double   	OfferVolume3;          //��������
	int      	OfferCount3;           //ʵ������ί�б�����
	double   	OfferPrice4;           //��������
	double   	OfferVolume4;          //��������
	int      	OfferCount4;           //ʵ������ί�б�����
	double   	OfferPrice5;           //��������
	double   	OfferVolume5;          //��������
	int      	OfferCount5;           //ʵ������ί�б�����
	double   	OfferPrice6;           //��������
	double   	OfferVolume6;          //��������
	int      	OfferCount6;           //ʵ������ί�б�����
	double   	OfferPrice7;           //��������
	double   	OfferVolume7;          //��������
	int      	OfferCount7;           //ʵ������ί�б�����
	double   	OfferPrice8;           //�����۰�
	double   	OfferVolume8;          //��������
	int      	OfferCount8;           //ʵ������ί�б�����
	double   	OfferPrice9;           //�����۾�
	double   	OfferVolume9;          //��������
	int      	OfferCount9;           //ʵ������ί�б�����
	double   	OfferPriceA;           //������ʮ
	double   	OfferVolumeA;          //������ʮ
	int      	OfferCountA;           //ʵ������ί�б���ʮ
	char_7   	InstrumentStatus;      //��Լ״̬
	double   	PreIOPV;               //��ֵ��ֵ
	double   	PERatio1;              //��ӯ��һ
	double   	PERatio2;              //��ӯ�ʶ�
	double   	UpperLimitPrice;       //��ͣ��
	double   	LowerLimitPrice;       //��ͣ��
	double   	WarrantPremiumRatio;   //Ȩ֤�����
	double   	TotalWarrantExecQty;   //Ȩִ֤��������
	double   	PriceDiff1;            //����һ
	double   	PriceDiff2;            //������
	double   	ETFBuyNumber;          //ETF�깺����
	double   	ETFBuyAmount;          //ETF�깺����
	double   	ETFBuyMoney;           //ETF�깺���
	double   	ETFSellNumber;         //ETF��ر���
	double   	ETFSellAmount;         //ETF�������
	double   	ETFSellMoney;          //ETF��ؽ��
	double   	WithdrawBuyNumber;     //���볷������
	double   	WithdrawBuyAmount;     //���볷������
	double   	WithdrawBuyMoney;      //���볷�����
	double   	TotalBidNumber;        //�����ܱ���
	double   	BidTradeMaxDuration;   //����ί�гɽ����ȴ�ʱ��
	double   	NumBidOrders;          //��ί�м�λ��
	double   	WithdrawSellNumber;    //������������
	double   	WithdrawSellAmount;    //������������
	double   	WithdrawSellMoney;     //�����������
	double   	TotalOfferNumber;      //�����ܱ���
	double   	OfferTradeMaxDuration;   //����ί�гɽ����ȴ�ʱ��
	double   	NumOfferOrders;        //����ί�м�λ��
};

struct L2IndexField
{
	char_9   	TradingDay;            //������
	char_9   	TimeStamp;             //����ʱ�䣨�룩
	char_9   	ExchangeID;            //����������
	char_31  	InstrumentID;          //ָ������
	double   	PreCloseIndex;         //ǰ����ָ��
	double   	OpenIndex;             //����ָ��
	double   	CloseIndex;            //��������ָ��
	double   	HighIndex;             //���ָ��
	double   	LowIndex;              //���ָ��
	double   	LastIndex;             //����ָ��
	double   	TurnOver;              //���������Ӧָ���ĳɽ���Ԫ��
	double   	TotalVolume;           //���������Ӧָ���Ľ����������֣�
};

struct L2OrderField
{
	char_9   	OrderTime;             //ί��ʱ�䣨�룩
	char_9   	ExchangeID;            //����������
	char_31  	InstrumentID;          //��Լ����
	double   	Price;                 //ί�м۸�
	double   	Volume;                //ί������
	char_2   	OrderKind;             //��������
};


struct L2TradeField
{
	char_9   	TradeTime;             //�ɽ�ʱ�䣨�룩
	char_9   	ExchangeID;            //����������
	char_31  	InstrumentID;          //��Լ����
	double   	Price;                 //�ɽ��۸�
	double   	Volume;                //�ɽ�����
	char_2   	OrderKind;             //��������
	char_2   	OrderBSFlag;           //�����̱�־
};

struct BarMarketDataField
{
	char_9   	TradingDay;            //������
	char_31  	InstrumentID;          //��Լ����
	double   	UpperLimitPrice;       //��ͣ���
	double   	LowerLimitPrice;       //��ͣ���
	char_13  	StartUpdateTime;       //��tick�޸�ʱ��
	int      	StartUpdateMillisec;   //��tick����޸ĺ���
	char_13  	EndUpdateTime;         //βtick����޸�ʱ��
	int      	EndUpdateMillisec;     //βtick����޸ĺ���
	double   	Open;                  //��
	double   	Close;                 //��
	double   	Low;                   //��
	double   	High;                  //��
	double   	Volume;                //���佻����
	double   	StartVolume;           //��ʼ�ܽ�����
};

struct QryPositionField
{
	char_11  	BrokerID;              //���͹�˾����
	char_19  	InvestorID;            //Ͷ���ߴ���
	char_31  	InstrumentID;          //��Լ����
	char_9   	ExchangeID;            //����������
};

struct RspPositionField
{
	char_31              	InstrumentID;          //��Լ����
	int                  	YdPosition;            //���ճֲ�
	int                  	Position;              //�ֲܳ�
	int						TodayPosition;         //���ճֲ�
	char					PositionDate;
	char_11              	BrokerID;              //���͹�˾����
	char_19              	InvestorID;            //Ͷ���ߴ���
	double               	PositionCost;          //�ֲֳɱ�
	HedgeFlagType      	HedgeFlag;             //Ͷ���ױ���־
	PosiDirectionType  	PosiDirection;         //�ֲֶ�շ���
};

struct InputOrderField
{
	char_11                    	BrokerID;              //���͹�˾����
	char_16                    	UserID;                //�û�����
	char_19                    	InvestorID;            //Ͷ���ߴ���
	char_21                    	BusinessUnit;          //ҵ��Ԫ
	char_9                     	ExchangeID;            //����������
	char_31                    	InstrumentID;          //��Լ����
	char_13                    	OrderRef;              //��������
	double                     	LimitPrice;            //�۸�
	int                        	Volume;                //����
	int                        	MinVolume;             //��С�ɽ���
	TimeConditionType        	TimeCondition;         //��Ч������
	VolumeConditionType      	VolumeCondition;       //�ɽ�������
	OrderPriceTypeType       	OrderPriceType;        //�����۸�����
	DirectionType            	Direction;             //��������
	OffsetFlagType           	OffsetFlag;            //��ƽ��־
	HedgeFlagType            	HedgeFlag;             //Ͷ���ױ���־
	ForceCloseReasonType     	ForceCloseReason;      //ǿƽԭ��
	double                     	StopPrice;             //ֹ���
	int                        	IsAutoSuspend;         //�Զ������־
	ContingentConditionType  	ContingentCondition;   //��������
	char_30                    	MiscInfo;              //ί���Զ����ǩ
};


/*
struct RtnOrderField
{
	char_11                	BrokerID;              //���͹�˾����
	char_16                	UserID;                //�û�����
	char_11                	ParticipantID;         //��Ա����
	char_19                	InvestorID;            //Ͷ���ߴ���
	char_21                	BusinessUnit;          //ҵ��Ԫ
	char_31                	InstrumentID;          //��Լ����
	char_21                	OrderRef;              //��������
	char_11                	ExchangeID;            //����������
	double                 	LimitPrice;            //�۸�
	int                    	VolumeTraded;          //��ɽ�����
	int                    	VolumeTotal;           //ʣ������
	int                    	VolumeTotalOriginal;   //����
	TimeConditionType    	TimeCondition;         //��Ч������
	VolumeConditionType  	VolumeCondition;       //�ɽ�������
	OrderPriceTypeType   	OrderPriceType;        //�����۸�����
	DirectionType        	Direction;             //��������
	OffsetFlagType       	OffsetFlag;            //��ƽ��־
	HedgeFlagType        	HedgeFlag;             //Ͷ���ױ���־
	OrderStatusType      	OrderStatus;           //����״̬
	int                    	RequestID;             //������
};
*/

struct RtnOrderField
{
	char_11  	BrokerID;              //���͹�˾����
	char_19  	InvestorID;            //Ͷ���ߴ���
	char_31  	InstrumentID;          //��Լ����
	char_21     OrderRef;              //��������
	char_16     UserID;                //�û�����
	OrderPriceTypeType   	OrderPriceType;        //�����۸�����
	DirectionType        	Direction;             //��������
	OffsetFlagType       	OffsetFlag;            //��ƽ��־
	HedgeFlagType           HedgeFlag;             //Ͷ���ױ���־
	double                 	LimitPrice;            //�۸�
	int                    	VolumeTotalOriginal;   //����
	TimeConditionType    	TimeCondition;         //��Ч������
	char_9					GTDDate;
	VolumeConditionType  	VolumeCondition;       //�ɽ�������
	int                     MinVolume;             //��С�ɽ���
	ContingentConditionType  	ContingentCondition; //��������
	double                     	StopPrice;             //ֹ���
	ForceCloseReasonType     	ForceCloseReason;      //ǿƽԭ��
	int                        	IsAutoSuspend;         //�Զ������־
	char_21                    	BusinessUnit;          //ҵ��Ԫ
	int                    	RequestID;             //������
	char_13                 OrderLocalID;
	char_9   	ExchangeID;            //����������
	char_11                	ParticipantID;         //��Ա����
	char_21           	ClientID;              //�ͻ�����
	char_64  	ExchangeInstID;        //��Լ�ڽ������Ĵ���
	char_21     TraderID;
	int         InstallID;
	char		OrderSubmitStatus;
	int			NotifySequence;
	char_13  	TradingDay;            //������
	int         SettlementID;
	char_31     OrderSysID;            //�������
	char		OrderSource;
	OrderStatusType      	OrderStatus;           //����״̬
	char		OrderType;
	int         VolumeTraded;          //��ɽ�����
	int         VolumeTotal;           //ʣ������
	char_9		InsertDate;
	char_9		InsertTime;
	char_9		ActiveTime;
	char_9		SuspendTime;
	char_9		UpdateTime;
	char_9		CancelTime;
	char_21		ActiveTraderID;
	char_11		ClearingPartID;
	int			SequenceNo;
	int			FrontID;
	int			SessionID;
	char_11		UserProductInfo;
	char_81		StatusMsg;
	int			UserForceClose;
	char_16		ActiveUserID;
	int			BrokerOrderSeq;
	char_21		RelativeOrderSysID;
	int			ZCETotalTradedVolume;
	int			IsSwapOrder;
	char_9		BranchID;
	char_17		InvestUnitID;
	char_13		AccountID;
	char_4		CurrencyID;
	char_16		IPAddress;
	char_21		MacAddress;
};

struct RtnTradeField
{
	char_11           	BrokerID;              //���͹�˾����
	char_16           	UserID;                //�û�����
	char_19           	InvestorID;            //Ͷ���ߴ���
	char_21           	BusinessUnit;          //ҵ��Ԫ
	char_31           	InstrumentID;          //��Լ����
	char_13           	OrderRef;              //��������
	char_11           	ExchangeID;            //����������
	char_21           	TradeID;               //�ɽ����
	char_31           	OrderSysID;            //�������
	char_11           	ParticipantID;         //��Ա����
	char_21           	ClientID;              //�ͻ�����
	double            	Price;                 //�۸�
	int               	Volume;                //����
	char_13           	TradingDay;            //������
	char_13           	TradeTime;             //�ɽ�ʱ��
	DirectionType   	Direction;             //��������
	OffsetFlagType  	OffsetFlag;            //��ƽ��־
	HedgeFlagType   	HedgeFlag;             //Ͷ���ױ���־
};

struct OrderActionField
{
	char_11  	BrokerID;              //���͹�˾����
	char_19  	InvestorID;            //Ͷ���ߴ���
	char_31  	InstrumentID;          //��Լ����
	char_11  	ExchangeID;            //����������
	char_16  	UserID;                //�û�����
	char_13  	OrderRef;              //��������
	char_31  	OrderSysID;            //�������
	int      	RequestID;             //������
	char     	ActionFlag;            //����������־
	double   	LimitPrice;            //�۸�
	int      	VolumeChange;          //�����仯
	int      	InternalOrderID;       //ϵͳ�ڶ���ID
};

struct QryAccountField
{
	char_11  	BrokerID;              //���͹�˾����
	char_19  	InvestorID;            //Ͷ���ߴ���
};

struct RspAccountField
{
	char_11  	BrokerID;              //���͹�˾����
	char_19  	InvestorID;            //Ͷ���ߴ���
	double   	PreMortgage;           //�ϴ���Ѻ���
	double   	PreCredit;             //�ϴ����ö��
	double   	PreDeposit;            //�ϴδ���
	double   	PreBalance;            //�ϴν���׼����
	double   	PreMargin;             //�ϴ�ռ�õı�֤��
	double   	Deposit;               //�����
	double   	Withdraw;              //������
	double   	FrozenMargin;          //����ı�֤�𣨱���δ�ɽ�����ı�֤��
	double   	FrozenCash;            //������ʽ𣨱���δ�ɽ���������ʽ�
	double   	FrozenCommission;      //����������ѣ�����δ�ɽ�����������ѣ�
	double   	CurrMargin;            //��ǰ��֤���ܶ�
	double   	CashIn;                //�ʽ���
	double   	Commission;            //������
	double   	CloseProfit;           //ƽ��ӯ��
	double   	PositionProfit;        //�ֲ�ӯ��
	double   	Balance;               //����׼����
	double   	Available;             //�����ʽ�
	double   	WithdrawQuota;         //��ȡ�ʽ�
	double   	Reserve;               //����׼����
	char_9   	TradingDay;            //������
	char_4      CurrencyID;            //����
	double   	Credit;                //���ö��
	double   	Mortgage;              //��Ѻ���
	double   	ExchangeMargin;        //��������֤��
	double   	DeliveryMargin;        //Ͷ���߽��֤��
	double   	ExchangeDeliveryMargin;   //���������֤��
	double   	ReserveBalance;        //�����ڻ�����׼����
	double   	Equity;                //����Ȩ��
	double   	MarketValue;           //�˻���ֵ
};

struct QryOrderField
{
	char_11  	BrokerID;              //���͹�˾����
	char_19  	InvestorID;            //Ͷ���ߴ���
	char_31  	InstrumentID;          //��Լ����
	char_11  	ExchangeID;            //����������
	char_31  	OrderSysID;            //�������
	char_9		InsertTimeStart;
	char_9		InsertTimeEnd;
	char_17     InvestUnitID;
};

struct QryTradeField
{
	char_11  	BrokerID;              //���͹�˾����
	char_19  	InvestorID;            //Ͷ���ߴ���
	char_31  	InstrumentID;          //��Լ����
	char_11  	ExchangeID;            //����������
	char_21     TradeID;               //�ɽ����
};

struct RspActionField
{
	char_11  	BrokerID;              //���͹�˾����
	char_19  	InvestorID;            //Ͷ���ߴ���
	int			OrderActionRef;
	char_13     OrderRef;              //��������
	int         RequestID;             //������
	int			FrontID;
	int			SessionID;
	char_9   	ExchangeID;            //����������
	char_31     OrderSysID;            //�������
	char     	ActionFlag;            //����������־
	double      LimitPrice;            //�۸�
	int      	VolumeChange;          //�����仯
	char_9      ActionDate;
	char_9      ActionTime;
	char_81		StatusMsg;
};

struct RspInstrumentStatus
{
	char_9 ExchangeID;
	char_31 ExchangeInstID;
	char_9 SettlementGroupID;
	char_31 InstrumentID;
	char InstrumentStatus;
	int TradingSegmentSN;
	char_9 EnterTime;
	char EnterReason;
};
#endif
