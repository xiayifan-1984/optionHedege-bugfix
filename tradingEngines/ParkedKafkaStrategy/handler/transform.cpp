

#include "transform.h"
#include "transhelper.h"

#include "../util2/MConsume.h"
#include "../util2/MProduce.h"
#include "../util2/RpcStream.h"

#include "../util/Platform.h"
#include "../util/MDateTime.h"


//#define		brokers		"192.168.1.7:9092"

#include <string.h>
#include <stdio.h>

#include <unistd.h>

#include "directstrategy.h"
#include "parkunit.h"

#include <sstream>
#define 	STRATEGY_WRITELOG(level, msgstream) \
	{	\
		std::ostringstream oss; 	\
		oss << msgstream; 		\
		m_pTradeSpi->writeLog(level, oss.str().c_str());		\
	}
Transform::Transform()
{
	m_pProduce = 0;
	m_pConsume = 0;
	m_pTradeSpi = 0;
	memset(&m_oUserInfo, 0, sizeof(m_oUserInfo));

	m_pthread = 0;
	m_bstopflag = false;
}

Transform::~Transform()
{

}

int			Transform::Instance(tagTradeUserInfo* pInfo, directStrategy* pSpi, CParkUnit* pParkUnit, const char* pszbroker)
{
	m_oUserInfo = *pInfo;
	m_pTradeSpi = pSpi;
	m_pParkUnit = pParkUnit;

	m_pProduce = new MProduce();
	m_pProduce->Init(pszbroker);

	m_pConsume = new MConsume();

	MDateTime  oDt;
	oDt.from_now();

	char szConsumeName[255]={0};
	_snprintf(szConsumeName, 254, "parked_%s_%d_%d", pInfo->User,oDt.dateToInt(), oDt.timeToInt() );
	m_pConsume->Init(pszbroker, szConsumeName, pfnConsumeCB, this);

	char* topics[] =
	{
		"422"
	};
	m_pConsume->Subscribe(1, topics);

	m_pthread = new std::thread(std::bind(&Transform::Run, this));

	//@@@@@@@@@@@
	//@@@@@@@@@@@

	return 1;
}

void		Transform::Release()
{
	m_bstopflag = true;
	m_pthread->join();

	if (m_pProduce)
	{
		m_pProduce->Release();
		delete m_pProduce;
		m_pProduce = 0;
	}

	if (m_pConsume)
	{
		m_pConsume->Release();
		delete m_pConsume;
		m_pConsume = 0;
	}

	if (m_pthread)
	{
		delete m_pthread;
		m_pthread = 0;
	}
}

int		Transform::pfnConsumeCB(void* pthis, const char* topic, int partition, long long offset, int buflen, const char* pbuf)
{
	Transform* pobj = (Transform*)pthis;
	pobj->onConsumeCB(topic, partition, offset, buflen, pbuf);
	return 1;
}

int	Transform::onConsumeCB(const char* topic, int partition, long long offset, int buflen, const char* pbuf)
{
	printf("Transform::onConsumeCB\n");
	//[1]校验长度
	int minlen = sizeof(tagTDUserInfo) + sizeof(tagComm_FrameHead);
	if (buflen < minlen)
	{
		printf("-1 [buflen=%d][minlen=%d]\n", buflen, minlen);
		return -1;
	}

	tagTDUserInfo* pUser = (tagTDUserInfo*)(pbuf);
	//比较用户的broker和用户名，是否为本程序配置的一样
	{
		if (pUser->broker != m_oUserInfo.Broker)
		{
			printf("-2[broker=%d][Broker=%d]\n", pUser->broker, m_oUserInfo.Broker);
			return -2;
		}
		if (_strnicmp(pUser->userid, m_oUserInfo.User, 16) != 0)
		{
			printf("-3[userid= %s][User= %s]\n", pUser->userid, m_oUserInfo.User);
			return -3;
		}
	}

	STRATEGY_WRITELOG(0, "Transform::onConsumeCB[ ]");
	tagComm_FrameHead* pHead = (tagComm_FrameHead*)(pbuf + sizeof(tagTDUserInfo));
	if (buflen < (minlen + pHead->size))
	{
		printf("-4[buflen=%d][%d]\n", buflen, minlen + pHead->size);
		return -4;
	}

	//[2]
	if (pHead->size <= 0)
	{
		printf("-5[%d]\n", pHead->size);
		return -5;
	}

	STRATEGY_WRITELOG(0, "Transform::onConsumeCB(childtype)" << int(pHead->childtype) );
	char* pcontent = (char*)(pbuf + minlen);
	int contentlen = pHead->size;
	switch (pHead->childtype)
	{
	case 20:
		{
			RpcRead rr;
			rr.BindStream(pcontent, contentlen);
			int argc = rr.ReadInt32();
			tagXTQryParkedOrderField* pField = (tagXTQryParkedOrderField*)rr.ReadSolidPtr();
			int reqid = rr.ReadInt32();
			if (!rr.IsError())
			{
				STRATEGY_WRITELOG(0, "Transform::DoReqQryParkedOrder");
				m_pParkUnit->DoReqQryParkedOrder(pField, reqid);
			}
		}
	break;
	case 21:
		{
			RpcRead rr;
			rr.BindStream(pcontent, contentlen);
			int argc = rr.ReadInt32();
			tagXTParkedOrderInputField* pField = (tagXTParkedOrderInputField*)rr.ReadSolidPtr();
			int reqid = rr.ReadInt32();
			if (!rr.IsError())
			{
				STRATEGY_WRITELOG(0, "Transform::DoReqParkedOrderInsert" << pField->ParkedOrderID );
				m_pParkUnit->DoReqParkedOrderInsert(pField,reqid);
			}
		}
	break;
	case 22:
		{
			RpcRead rr;
			rr.BindStream(pcontent, contentlen);
			int argc = rr.ReadInt32();
			tagXTParkedOrderActionField* pField = (tagXTParkedOrderActionField*)rr.ReadSolidPtr();
			int reqid = rr.ReadInt32();
			if (!rr.IsError())
			{
				STRATEGY_WRITELOG(0, "Transform::DoReqParkedOrderAction" << pField->ParkedOrderID);
				m_pParkUnit->DoReqParkedOrderAction(pField,reqid);
			}
		}
	break;
	default:
		break;
	}

	return 1;
}

void	Transform::Run()
{
	while (!m_bstopflag)
	{
		m_pProduce->PeriodCall();
		m_pConsume->PeriodCall();

		sleep(1);
	}

}

void 		Transform::OnRspQryParkedOrder(tagXTParkedOrderField* pOrder, tagXTRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	char tempbuf[4096] = {0};
	RpcWrite rw(tempbuf, 4095);
	rw.WriteInt32(4);
	if(pOrder)
	{
		tagXTParkedOrderField		oField;
		memset(&oField, 0, sizeof(oField));
		memcpy(&oField, pOrder, sizeof(tagXTParkedOrderField));
		rw.WriteSolidPtr((char*)&oField, sizeof(oField));
	}
	else
	{
		rw.WriteSolidPtr( 0, 0);
	}

	if(pRspInfo && pRspInfo->ErrorID != 0 )
	{
		tagXTRspInfoField oField;
		memset(&oField, 0, sizeof(oField));
		oField.ErrorID = pRspInfo->ErrorID;
		strcpy(oField.ErrorMsg, pRspInfo->ErrorMsg);
		
		rw.WriteSolidPtr((char*)&oField, sizeof(oField) );
	}
	else
	{
		rw.WriteSolidPtr( 0, 0);
	}

	rw.WriteInt32(nRequestID);
	rw.WriteInt8( bIsLast?(1):(0) );
	if(!rw.IsError() )
	{
		sendResponse("423", 2, 20, tempbuf, rw.GetOffset());
	}
}

void 		Transform::OnRtnParkedOrder(tagXTParkedOrderField* pOrder)
{
	char tempbuf[4096] = {0};
	RpcWrite rw(tempbuf, 4095);
	rw.WriteInt32(1);
	
	if(pOrder)
	{
		tagXTParkedOrderField oField;
		memset(&oField, 0, sizeof(oField));
			
		memcpy(&oField, pOrder, sizeof(tagXTParkedOrderField));
		rw.WriteSolidPtr((char*)&oField, sizeof(oField) );
	}
	else
	{
		rw.WriteSolidPtr( 0, 0);
	}
	
	if(!rw.IsError() )
	{
		sendResponse("423", 2, 23, tempbuf, rw.GetOffset());
	}
}




//======================================================================================================================
int			Transform::sendResponse(const char* topic, unsigned char main, unsigned char child, const char* pbuf, int buflen)
{
	char tempbuf[8192] = { 0 };

	int offset = 0;
	tagTDUserInfo* pUser = (tagTDUserInfo*)(tempbuf + offset);
	{
		pUser->broker = m_oUserInfo.Broker;
		strncpy(pUser->userid, m_oUserInfo.User, 16);
	}

	offset += sizeof(tagTDUserInfo);
	tagComm_FrameHead* pHead = (tagComm_FrameHead*)(tempbuf + offset);
	{
		pHead->maintype = main;
		pHead->childtype = child;
		pHead->size = buflen;
	}

	offset += sizeof(tagComm_FrameHead);
	char* pbuffer = (char*)(tempbuf + offset);
	memcpy(pbuffer, pbuf, buflen);

	offset += buflen;
	STRATEGY_WRITELOG(0, "(main)" << main << "(child)" << child << "=============>[" <<topic <<"]len" << buflen);
	m_pProduce->Send(topic, tempbuf, offset);
	return 1;
}



void		Transform::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	return;

	char tempbuf[4096] = { 0 };
	RpcWrite rw(tempbuf, 4095);
	rw.WriteInt32(4);
	if (pTradingAccount)
	{
		tagXTTradingAccountField oField;
		memset(&oField, 0, sizeof(oField));
		transhelper::Trans_TradingAccountField(pTradingAccount, oField);
		rw.WriteSolidPtr((char*)&oField, sizeof(oField));
	}
	else
	{
		rw.WriteSolidPtr(0, 0);
	}

	if (pRspInfo && pRspInfo->ErrorID != 0)
	{
		tagXTRspInfoField oField;
		memset(&oField, 0, sizeof(oField));
		oField.ErrorID = pRspInfo->ErrorID;
		strcpy(oField.ErrorMsg, pRspInfo->ErrorMsg);

		rw.WriteSolidPtr((char*)&oField, sizeof(oField));
	}
	else
	{
		rw.WriteSolidPtr(0, 0);
	}

	rw.WriteInt32(nRequestID);
	rw.WriteInt8(bIsLast ? (1) : (0));
	if (!rw.IsError())
	{
		sendResponse("402",2, 0, tempbuf, rw.GetOffset());
	}
}

void		Transform::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	return;

	char tempbuf[4096] = { 0 };
	RpcWrite rw(tempbuf, 4095);
	rw.WriteInt32(4);
	if (pInvestorPosition)
	{
		tagXTInvestorPositionField	oField;
		memset(&oField, 0, sizeof(oField));

		transhelper::Trans_PositionField(pInvestorPosition, oField);
		rw.WriteSolidPtr((char*)&oField, sizeof(oField));
	}
	else
	{
		rw.WriteSolidPtr(0, 0);
	}

	if (pRspInfo && pRspInfo->ErrorID != 0)
	{
		tagXTRspInfoField oField;
		memset(&oField, 0, sizeof(oField));
		oField.ErrorID = pRspInfo->ErrorID;
		strcpy(oField.ErrorMsg, pRspInfo->ErrorMsg);

		rw.WriteSolidPtr((char*)&oField, sizeof(oField));
	}
	else
	{
		rw.WriteSolidPtr(0, 0);
	}

	rw.WriteInt32(nRequestID);
	rw.WriteInt8(bIsLast ? (1) : (0));
	if (!rw.IsError())
	{
		sendResponse("402",2, 1, tempbuf, rw.GetOffset());
	}
}

void		Transform::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	return;

	char tempbuf[4096] = { 0 };
	RpcWrite rw(tempbuf, 4095);
	rw.WriteInt32(4);
	if (pOrder)
	{
		tagXTOrderField		oField;
		memset(&oField, 0, sizeof(oField));
		transhelper::Trans_OrderField(pOrder, oField);
		STRATEGY_WRITELOG(0, "Transform::OnRspQryOrder(order)" << pOrder->CombOffsetFlag[0] << "(Field)" <<oField.OffsetFlag[0]);
		rw.WriteSolidPtr((char*)&oField, sizeof(oField));
	}
	else
	{
		rw.WriteSolidPtr(0, 0);
	}

	if (pRspInfo && pRspInfo->ErrorID != 0)
	{
		tagXTRspInfoField oField;
		memset(&oField, 0, sizeof(oField));
		oField.ErrorID = pRspInfo->ErrorID;
		strcpy(oField.ErrorMsg, pRspInfo->ErrorMsg);

		rw.WriteSolidPtr((char*)&oField, sizeof(oField));
	}
	else
	{
		rw.WriteSolidPtr(0, 0);
	}

	rw.WriteInt32(nRequestID);
	rw.WriteInt8(bIsLast ? (1) : (0));
	if (!rw.IsError())
	{
		sendResponse("402",2, 2, tempbuf, rw.GetOffset());
	}
}

void		Transform::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	return;

	char tempbuf[4096] = { 0 };
	RpcWrite rw(tempbuf, 4095);
	rw.WriteInt32(4);
	if (pTrade)
	{
		tagXTTradeField		oField;
		memset(&oField, 0, sizeof(oField));
		transhelper::Trans_TradeField(pTrade, oField);
		rw.WriteSolidPtr((char*)&oField, sizeof(oField));
	}
	else
	{
		rw.WriteSolidPtr(0, 0);
	}

	if (pRspInfo && pRspInfo->ErrorID != 0)
	{
		tagXTRspInfoField oField;
		memset(&oField, 0, sizeof(oField));
		oField.ErrorID = pRspInfo->ErrorID;
		strcpy(oField.ErrorMsg, pRspInfo->ErrorMsg);

		rw.WriteSolidPtr((char*)&oField, sizeof(oField));
	}
	else
	{
		rw.WriteSolidPtr(0, 0);
	}

	rw.WriteInt32(nRequestID);
	rw.WriteInt8(bIsLast ? (1) : (0));
	if (!rw.IsError())
	{
		sendResponse("402",2, 3, tempbuf, rw.GetOffset());
	}
}

void		Transform::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//日志记录
	if (pRspInfo)
	{
		STRATEGY_WRITELOG(0, "[ParkStrategy]Transform::OnRspOrderInsert RspInfo" << pRspInfo->ErrorID <<" ErrorMsg" << pRspInfo->ErrorMsg );
	}
	else
	{
		STRATEGY_WRITELOG(0, "[ParkStrategy]Transform::OnRspOrderInsert RspInfo" );
	}
	
	return;

	char tempbuf[4096] = { 0 };
	RpcWrite rw(tempbuf, 4095);
	rw.WriteInt32(4);
	if (pInputOrder)
	{
		tagXTInputOrderField oField;
		memset(&oField, 0, sizeof(oField));
		transhelper::Trans_RspOrderInsert(pInputOrder, oField);
		rw.WriteSolidPtr((char*)&oField, sizeof(oField));
	}
	else
	{
		rw.WriteSolidPtr(0, 0);
	}

	if (pRspInfo && pRspInfo->ErrorID != 0)
	{
		tagXTRspInfoField oField;
		memset(&oField, 0, sizeof(oField));
		oField.ErrorID = pRspInfo->ErrorID;
		strcpy(oField.ErrorMsg, pRspInfo->ErrorMsg);

		rw.WriteSolidPtr((char*)&oField, sizeof(oField));
	}
	else
	{
		rw.WriteSolidPtr(0, 0);
	}

	rw.WriteInt32(nRequestID);
	rw.WriteInt8(bIsLast ? (1) : (0));
	if (!rw.IsError())
	{
		sendResponse("402",2, 4, tempbuf, rw.GetOffset());
	}
}

void		Transform::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	return;

	char tempbuf[4096] = { 0 };
	RpcWrite rw(tempbuf, 4095);
	rw.WriteInt32(4);
	if (pInputOrderAction)
	{
		tagXTInputOrderActionField oField;
		memset(&oField, 0, sizeof(oField));
		transhelper::Trans_RspOrderAction(pInputOrderAction, oField);
		rw.WriteSolidPtr((char*)&oField, sizeof(oField));
	}
	else
	{
		rw.WriteSolidPtr(0, 0);
	}

	if (pRspInfo && pRspInfo->ErrorID != 0)
	{
		tagXTRspInfoField oField;
		memset(&oField, 0, sizeof(oField));
		oField.ErrorID = pRspInfo->ErrorID;
		strcpy(oField.ErrorMsg, pRspInfo->ErrorMsg);

		rw.WriteSolidPtr((char*)&oField, sizeof(oField));
	}
	else
	{
		rw.WriteSolidPtr(0, 0);
	}

	rw.WriteInt32(nRequestID);
	rw.WriteInt8(bIsLast ? (1) : (0));
	if (!rw.IsError())
	{
		sendResponse("402",2, 5, tempbuf, rw.GetOffset());
	}
}

void		Transform::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	return;

	char tempbuf[4096] = { 0 };
	RpcWrite rw(tempbuf, 4095);
	rw.WriteInt32(3);
	if (pRspInfo && pRspInfo->ErrorID != 0)
	{
		tagXTRspInfoField oField;
		memset(&oField, 0, sizeof(oField));
		oField.ErrorID = pRspInfo->ErrorID;
		strcpy(oField.ErrorMsg, pRspInfo->ErrorMsg);

		rw.WriteSolidPtr((char*)&oField, sizeof(oField));
	}
	else
	{
		rw.WriteSolidPtr(0, 0);
	}

	rw.WriteInt32(nRequestID);
	rw.WriteInt8(bIsLast ? (1) : (0));
	if (!rw.IsError())
	{
		sendResponse("402",2, 6, tempbuf, rw.GetOffset());
	}

}

void		Transform::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	return;

	char tempbuf[4096] = { 0 };
	RpcWrite rw(tempbuf, 4095);
	rw.WriteInt32(1);

	if (pOrder)
	{
		tagXTOrderField oField;
		memset(&oField, 0, sizeof(oField));

		transhelper::Trans_OrderField(pOrder, oField);
		rw.WriteSolidPtr((char*)&oField, sizeof(oField));
	}
	else
	{
		rw.WriteSolidPtr(0, 0);
	}

	if (!rw.IsError())
	{
		sendResponse("402",2, 7, tempbuf, rw.GetOffset());
	}
}

void		Transform::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	return;

	char tempbuf[4096] = { 0 };
	RpcWrite rw(tempbuf, 4095);
	rw.WriteInt32(1);

	if (pTrade)
	{
		tagXTTradeField oField;
		memset(&oField, 0, sizeof(oField));

		transhelper::Trans_TradeField(pTrade, oField);
		rw.WriteSolidPtr((char*)&oField, sizeof(oField));
	}
	else
	{
		rw.WriteSolidPtr(0, 0);
	}

	if (!rw.IsError())
	{
		sendResponse("402",2, 8, tempbuf, rw.GetOffset());
	}
}






















