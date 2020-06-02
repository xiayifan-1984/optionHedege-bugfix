#ifndef TRADINGENGINE_TRADINGCONSTANT_H
#define TRADINGENGINE_TRADINGCONSTANT_H

#include <memory.h>
#include <cstdlib>
#include <stdio.h>
// Index for Sources...
const short SOURCE_CTP = 1;
const short SOURCE_XTP = 15;

// Exchange names
#define EXCHANGE_SSE "SSE" //�Ϻ�֤ȯ������
#define EXCHANGE_SZE "SZE" //����֤ȯ������
#define EXCHANGE_CFFEX "CFFEX" //�й������ڻ�������
#define EXCHANGE_SHFE "SHFE" //�Ϻ��ڻ�������
#define EXCHANGE_DCE "DCE" //������Ʒ������
#define EXCHANGE_CZCE "CZCE" //֣����Ʒ������

// Exchange ids
#define EXCHANGE_ID_SSE 1 //�Ϻ�֤ȯ������
#define EXCHANGE_ID_SZE 2 //����֤ȯ������
#define EXCHANGE_ID_CFFEX 11 //�й������ڻ�������
#define EXCHANGE_ID_SHFE 12 //�Ϻ��ڻ�������
#define EXCHANGE_ID_DCE 13 //������Ʒ������
#define EXCHANGE_ID_CZCE 14 //֣����Ʒ������

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
// ActionFlagType: ����������־
///////////////////////////////////
//ɾ��
#define CHAR_Delete          '0'
//����
#define CHAR_Suspend         '1'
//����
#define CHAR_Active          '2'
//�޸�
#define CHAR_Modify          '3'

typedef char ActionFlagType;

///////////////////////////////////
// ContingentConditionType: ��������
///////////////////////////////////
//����
#define CHAR_Immediately     '1'
//ֹ��
#define CHAR_Touch           '2'
//ֹӮ
#define CHAR_TouchProfit     '3'
//Ԥ��
#define CHAR_ParkedOrder     '4'
//���¼۴���������
#define CHAR_LastPriceGreaterThanStopPrice '5'
//���¼۴��ڵ���������
#define CHAR_LastPriceGreaterEqualStopPrice '6'
//���¼�С��������
#define CHAR_LastPriceLesserThanStopPrice '7'
//���¼�С�ڵ���������
#define CHAR_LastPriceLesserEqualStopPrice '8'
//��һ�۴���������
#define CHAR_AskPriceGreaterThanStopPrice '9'
//��һ�۴��ڵ���������
#define CHAR_AskPriceGreaterEqualStopPrice 'A'
//��һ��С��������
#define CHAR_AskPriceLesserThanStopPrice 'B'
//��һ��С�ڵ���������
#define CHAR_AskPriceLesserEqualStopPrice 'C'
//��һ�۴���������
#define CHAR_BidPriceGreaterThanStopPrice 'D'
//��һ�۴��ڵ���������
#define CHAR_BidPriceGreaterEqualStopPrice 'E'
//��һ��С��������
#define CHAR_BidPriceLesserThanStopPrice 'F'
//��һ��С�ڵ���������
#define CHAR_BidPriceLesserEqualStopPrice 'H'

typedef char ContingentConditionType;

///////////////////////////////////
// DirectionType: ��������
///////////////////////////////////
//��
#define CHAR_Buy             '0'
//��
#define CHAR_Sell            '1'

typedef char DirectionType;

///////////////////////////////////
// ForceCloseReasonType: ǿƽԭ��
///////////////////////////////////
//��ǿƽ
#define CHAR_NotForceClose   '0'
//�ʽ���
#define CHAR_LackDeposit     '1'
//�ͻ�����
#define CHAR_ClientOverPositionLimit '2'
//��Ա����
#define CHAR_MemberOverPositionLimit '3'
//�ֲַ�������
#define CHAR_NotMultiple     '4'
//Υ��
#define CHAR_Violation       '5'
//����
#define CHAR_Other           '6'
//��Ȼ���ٽ�����
#define CHAR_PersonDeliv     '7'

typedef char ForceCloseReasonType;

///////////////////////////////////
// HedgeFlagType: Ͷ���ױ���־
///////////////////////////////////
//Ͷ��
#define CHAR_Speculation     '1'
//����
#define CHAR_Argitrage       '2'
//�ױ�
#define CHAR_Hedge           '3'
//������(femas)
#define CHAR_MarketMaker     '4'
//ƥ�����е�ֵ(femas)
#define CHAR_AllValue        '9'

typedef char HedgeFlagType;

///////////////////////////////////
//OffsetFlagType: ��ƽ��־
///////////////////////////////////
//����
#define CHAR_Open            '0'
//ƽ��
#define CHAR_Close           '1'
//ǿƽ
#define CHAR_ForceClose      '2'
//ƽ��
#define CHAR_CloseToday      '3'
//ƽ��
#define CHAR_CloseYesterday  '4'
//ǿ��
#define CHAR_ForceOff        '5'
//����ǿƽ
#define CHAR_LocalForceClose '6'
//���ֿ�ƽ
#define CHAR_Non             'N'

typedef char OffsetFlagType;

///////////////////////////////////
// OrderPriceTypeType: �����۸�����
///////////////////////////////////
//�����
#define CHAR_AnyPrice        '1'
//�޼�
#define CHAR_LimitPrice      '2'
//���ż�
#define CHAR_BestPrice       '3'

typedef char OrderPriceTypeType;

///////////////////////////////////
// OrderStatusType: ����״̬
///////////////////////////////////
//ȫ���ɽ�������״̬��
#define CHAR_AllTraded       '0'
//���ֳɽ����ڶ�����
#define CHAR_PartTradedQueueing '1'
//���ֳɽ����ڶ����У����ɲ����� ����״̬��
#define CHAR_PartTradedNotQueueing '2'
//δ�ɽ����ڶ�����
#define CHAR_NoTradeQueueing '3'
//δ�ɽ����ڶ����У����ܾ�������״̬��
#define CHAR_NoTradeNotQueueing '4'
//����
#define CHAR_Canceled        '5'
//�����ѱ��뽻����δӦ��
#define CHAR_AcceptedNoReply '6'
//δ֪
#define CHAR_Unknown         'a'
//��δ����
#define CHAR_NotTouched      'b'
//�Ѵ���
#define CHAR_Touched         'c'
//�ϵ���������״̬��
#define CHAR_Error           'd'
//������д��
#define CHAR_OrderInserted   'i'
//ǰ���ѽ���
#define CHAR_OrderAccepted   'j'

typedef char OrderStatusType;

///////////////////////////////////
// LfPosiDirectionType: �ֲֶ�շ���
///////////////////////////////////
//��
#define CHAR_Net             '1'
//��ͷ
#define CHAR_Long            '2'
//��ͷ
#define CHAR_Short           '3'

typedef char PosiDirectionType;

///////////////////////////////////
// PositionDateType: �ֲ�����
///////////////////////////////////
//���ճֲ�
#define CHAR_Today           '1'
//��ʷ�ֲ�
#define CHAR_History         '2'
//���ֲֳ�
#define CHAR_Both            '3'

typedef char PositionDateType;

///////////////////////////////////
// TimeConditionType: ��Ч������
///////////////////////////////////
//������ɣ�������
#define CHAR_IOC             '1'
//������Ч
#define CHAR_GFS             '2'
//������Ч
#define CHAR_GFD             '3'
//ָ������ǰ��Ч
#define CHAR_GTD             '4'
//����ǰ��Ч
#define CHAR_GTC             '5'
//���Ͼ�����Ч
#define CHAR_GFA             '6'

typedef char TimeConditionType;

///////////////////////////////////
// VolumeConditionType: �ɽ�������
///////////////////////////////////
//�κ�����
#define CHAR_AV              '1'
//��С����
#define CHAR_MV              '2'
//ȫ������
#define CHAR_CV              '3'

typedef char VolumeConditionType;
#endif
