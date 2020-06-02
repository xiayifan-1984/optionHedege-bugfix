#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <cmath>
#include <algorithm>

#include "parkunit.h"
#include "../util/MDateTime.h"
#include "../util/Platform.h"
#include "directstrategy.h"
#include "transform.h"
#include "timesection.h"
#include "transhelper.h"

static void  genOrderRef(char* outRef, const char* marker)
{
    static int lastTime =0;
    static int reqInsertNo = 1;

    MDateTime oDt;
    oDt.from_now();

    int ncurtime = oDt.timeToInt();
    if (ncurtime != lastTime)
    {
        lastTime = ncurtime;
        reqInsertNo = 1;
    }

    _snprintf(outRef, 7, "%06d", ncurtime);
    _snprintf(outRef+6, 4, "%03d", ++reqInsertNo%1000 );
    strncpy(outRef+9, marker, 3);
}


CParkUnit::CParkUnit(/* args */)
{
    memset(&m_oUserInfo, 0, sizeof(m_oUserInfo));
    m_pTradeSpi =0;
    m_pTransform =0;
    m_pTimeSection =0;

    m_mapOrders.clear();
    m_nreqId =0;
}

CParkUnit::~CParkUnit()
{

}

int         CParkUnit::Instance(tagTradeUserInfo* pInfo, directStrategy* pSpi, Transform* pTransform, CTimesection* pTimeSection)
{
    m_oUserInfo = *pInfo;
    m_pTradeSpi = pSpi;
    m_pTransform = pTransform;
    m_pTimeSection = pTimeSection;

    m_mapOrders.clear();

	inner_initMapFunc();

    return 1;
}

void        CParkUnit::Release()
{
    m_mapOrders.clear();
}

void        CParkUnit::DoReqQryParkedOrder(tagXTQryParkedOrderField* pField, int requestid)
{
    if (strlen(pField->ParkedOrderID) >0)
    {
        std::string parkid = pField->ParkedOrderID;
        auto itb = m_mapOrders.find(parkid);
        if (itb != m_mapOrders.end() )
        {
            m_pTransform->OnRspQryParkedOrder(&(itb->second), 0, requestid, true);
        }
        else
        {
            tagXTRspInfoField oRspInfo;
            memset(&oRspInfo, 0, sizeof(oRspInfo));
            oRspInfo.ErrorID =1;
            _snprintf(oRspInfo.ErrorMsg, 255, "未找到该ParkID[%s]对应的单", parkid.c_str() );
            m_pTransform->OnRspQryParkedOrder(0, &oRspInfo, requestid, true);
        }

    }
    else
    {
        //请求所有
        auto itb = m_mapOrders.begin();
        while (itb != m_mapOrders.end())
        {
            m_pTransform->OnRspQryParkedOrder(&(itb->second), 0, requestid, false);
            itb++;
        }
        m_pTransform->OnRspQryParkedOrder(0, 0, requestid, true);
    }
    
}
    
void        CParkUnit::DoReqParkedOrderInsert(tagXTParkedOrderInputField* pField, int requestid)
{
    //[1]赋值保存
    MDateTime  oDt;
    oDt.from_now();

    tagXTParkedOrderField  oField;
    memset(&oField, 0, sizeof(oField));
    memcpy(&oField, pField, sizeof(tagXTParkedOrderInputField));

    oField.InsertDate = oDt.dateToInt();
    oField.InsertTime = oDt.timeToInt();
    oField.TradingDate = m_pTradeSpi->GetTradingDay();
   
    oField.ParkedStatus = XT_PAOS_NotSend;
    
    //[2]校验
    //暂无

    //[3]加入到队列
    std::string parkid = pField->ParkedOrderID;
    {
        lock_guard<mutex> lock(m_mut);
        m_mapOrders.insert(make_pair(parkid, oField));
    }

    //[4]注册行情
    std::string  code(oField.ExCode.Code);
    m_pTradeSpi->subscribeCode(code);
    
    //[6]回推通知
    m_pTransform->OnRtnParkedOrder(&oField);
}

void        CParkUnit::DoReqParkedOrderAction(tagXTParkedOrderActionField* pField, int requestid)
{
    std::string parkid = pField->ParkedOrderID;
    auto itb = m_mapOrders.find(parkid);
    if (itb != m_mapOrders.end())
    {
        if (!inner_isEndState(itb->second.ParkedStatus))    //不是最终状态
        {
            if (pField->ActionFlag == XT_AF_Delete)
            {
                _snprintf(itb->second.StatusMsg, 63, "Park单已撤");
                itb->second.ParkedStatus = XT_PAOS_Deleted;

                //回推通知
                m_pTransform->OnRtnParkedOrder(&(itb->second));
            }
            else if (pField->ActionFlag == XT_AF_HUMANSEND)
            {
                //手工下单
                inner_insertOrder(itb->second);
            }
            
        }
    }
}

void        CParkUnit::OnPeriodCall()
{
    //周期性地检测
    auto itb = m_mapOrders.begin();
    while (itb != m_mapOrders.end())
    {
        if (!inner_isEndState(itb->second.ParkedStatus))
        {
            if (inner_isTrigged(itb->second))
            {
                inner_insertOrder(itb->second);
            }
            
        }
        
        itb++;
    }

}

void		CParkUnit::inner_initMapFunc()
{
	//XT_CC_LastPriceGreaterThanStopPrice
	m_mapCondFun[XT_CC_LastPriceGreaterThanStopPrice] = [](double stopPrice, double lastPrice, double buy1, double sell1)
	{
		return lastPrice > stopPrice;
	};

	//XT_CC_LastPriceGreaterEqualStopPrice
	m_mapCondFun[XT_CC_LastPriceGreaterEqualStopPrice] = [](double stopPrice, double lastPrice, double buy1, double sell1)
	{
		return lastPrice >= stopPrice;
	};

	//XT_CC_LastPriceLesserThanStopPrice
	m_mapCondFun[XT_CC_LastPriceLesserThanStopPrice] = [](double stopPrice, double lastPrice, double buy1, double sell1)
	{
		return lastPrice < stopPrice;
	};

	//XT_CC_LastPriceLesserEqualStopPrice
	m_mapCondFun[XT_CC_LastPriceLesserEqualStopPrice] = [](double stopPrice, double lastPrice, double buy1, double sell1)
	{
		return lastPrice <= stopPrice;
	};

	//XT_CC_AskPriceGreaterThanStopPrice
	m_mapCondFun[XT_CC_AskPriceGreaterThanStopPrice] = [](double stopPrice, double lastPrice, double buy1, double sell1)
	{
		return sell1 > stopPrice;
	};

	//XT_CC_AskPriceGreaterEqualStopPrice
	m_mapCondFun[XT_CC_AskPriceGreaterEqualStopPrice] = [](double stopPrice, double lastPrice, double buy1, double sell1)
	{
		return sell1 >= stopPrice;
	};

	//XT_CC_AskPriceLesserThanStopPrice
	m_mapCondFun[XT_CC_AskPriceLesserThanStopPrice] = [](double stopPrice, double lastPrice, double buy1, double sell1)
	{
		return sell1 < stopPrice;
	};

	//XT_CC_AskPriceLesserEqualStopPrice
	m_mapCondFun[XT_CC_AskPriceLesserEqualStopPrice] = [](double stopPrice, double lastPrice, double buy1, double sell1)
	{
		return sell1 <= stopPrice;
	};

	//XT_CC_BidPriceGreaterThanStopPrice
	m_mapCondFun[XT_CC_BidPriceGreaterThanStopPrice] = [](double stopPrice, double lastPrice, double buy1, double sell1)
	{
		return buy1 > stopPrice;
	};

	//XT_CC_BidPriceGreaterEqualStopPrice
	m_mapCondFun[XT_CC_BidPriceGreaterEqualStopPrice] = [](double stopPrice, double lastPrice, double buy1, double sell1)
	{
		return buy1 >= stopPrice;
	};

	//XT_CC_BidPriceLesserThanStopPrice
	m_mapCondFun[XT_CC_BidPriceLesserThanStopPrice] = [](double stopPrice, double lastPrice, double buy1, double sell1)
	{
		return buy1 < stopPrice;
	};

	//XT_CC_BidPriceLesserEqualStopPrice
	m_mapCondFun[XT_CC_BidPriceLesserEqualStopPrice] = [](double stopPrice, double lastPrice, double buy1, double sell1)
	{
		return buy1 <= stopPrice;
	};
}

bool        CParkUnit::inner_isEndState(char  ParkStatus)
{
    if (ParkStatus == XT_PAOS_Send || ParkStatus == XT_PAOS_Deleted)
    {
        return true;
    }
    return false;
}

/*
///开盘前
#define THOST_FTDC_IS_BeforeTrading '0'
///非交易
#define THOST_FTDC_IS_NoTrading '1'
///连续交易
#define THOST_FTDC_IS_Continous '2'
///集合竞价报单
#define THOST_FTDC_IS_AuctionOrdering '3'
///集合竞价价格平衡
#define THOST_FTDC_IS_AuctionBalance '4'
///集合竞价撮合
#define THOST_FTDC_IS_AuctionMatch '5'
///收盘
#define THOST_FTDC_IS_Closed '6'
*/
bool        CParkUnit::inner_isTrigged(tagXTParkedOrderField& oField)
{
	if (oField.ParkedType == XT_PT_TIME) //定时 预埋，检查时间是否达到，误差在几百毫秒以内
	{
		MDateTime oDt;
		oDt.from_now();
		int now = oDt.timeToInt();
		now = m_pTimeSection->NormalizeTime(now);

		int stoptime = m_pTimeSection->NormalizeTime(oField.StopTime);
		if (now >= stoptime)
		{
			return true;
		}
	}
	else if (oField.ParkedType == XT_PT_CONDITION)  //条件单
	{
		std::string strcode(oField.ExCode.Code);
		MarketDataField tick;
		if (m_pTradeSpi->getLastestTick(strcode, tick))
		{
			auto checkfun = m_mapCondFun.find(oField.ConditionType);
			if (checkfun == m_mapCondFun.end() || !(checkfun->second)(oField.StopPrice, tick.LastPrice, tick.BidPrice1, tick.AskPrice1))
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}
	else if (oField.ParkedType == XT_PT_EXCHCONTINUE)  //连续竞价
	{
		if (m_pTradeSpi->GetExchangeStatus() == '2')
		{
			return true;
		}
	}
	else if (oField.ParkedType == XT_PT_EXCHAUCTION)   //集合竞价
	{
		if (m_pTradeSpi->GetExchangeStatus() == '3')
		{
			return true;
		}
	}

	return false;
}

void        CParkUnit::inner_insertOrder(tagXTParkedOrderField& inField)
{
    //填写下单所需的字段
    CThostFtdcInputOrderField oField;
    memset(&oField, 0, sizeof(oField));
    inner_fillOrder(oField, inField);

    //下单
    m_pTradeSpi->DoReqOrderInsert(&oField, oField.RequestID);

    //更新状态
    std::string parkid = inField.ParkedOrderID;
    m_mapOrders[parkid].ParkedStatus = XT_PAOS_Send;
    char szbuf[255] = {0};
    {
        MDateTime oDt;
        oDt.from_now();
        int now = oDt.timeToInt();
        _snprintf(szbuf, 254, "下单时间[%d] 下单编号[%s]", now, oField.OrderRef );
        strcpy(m_mapOrders[parkid].StatusMsg, szbuf);
    }
    
    //通知
    m_pTransform->OnRtnParkedOrder(&m_mapOrders[parkid]);
}

void        CParkUnit::inner_fillOrder(CThostFtdcInputOrderField& outField, tagXTParkedOrderField& inField)
{
    tagXTInputOrderField tField;
    memset(&tField, 0, sizeof(tagXTInputOrderField));

    tField.BrokerID = m_oUserInfo.Broker;
    strncpy(tField.UserID, m_oUserInfo.User, sizeof(m_oUserInfo.User)-1);

    tField.ExCode = inField.ExCode;

    genOrderRef(tField.OrderRef, "130");
    tField.PriceType = XT_OPT_LimitPrice;
    tField.Direction = inField.Direction;
    tField.OffsetFlag[0] = inField.OffsetFlag[0];
    tField.OffsetFlag[1] = inField.OffsetFlag[1];
    tField.HedgeFlag[0] = inField.HedgeFlag[0];
    tField.HedgeFlag[1] = inField.HedgeFlag[1];
    tField.LimitPrice = inField.LimitPrice;
    tField.Volume = inField.Volume;
   
    printf("[inner_fillOrder]ori[code=%s][orderref=%s]\n", tField.ExCode.Code, tField.OrderRef);

    memset(&outField, 0, sizeof(CThostFtdcInputOrderField));
    _snprintf(outField.BrokerID, sizeof(outField.BrokerID)-1, "%d", tField.BrokerID);
    strncpy(outField.InvestorID, tField.UserID, sizeof(outField.InvestorID)-1 );
    strncpy(outField.UserID, tField.UserID, sizeof(outField.UserID)-1 );
    outField.RequestID = ++m_nreqId;
    transhelper::Trans_ReqOrderInsert(&tField, outField);
}










