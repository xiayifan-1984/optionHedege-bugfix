#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <cmath>
#include <algorithm>

#include "twapunit.h"
#include "../util/MDateTime.h"
#include "../util/Platform.h"
#include "directstrategy.h"
#include "transform.h"
#include "timesection.h"
#include "transhelper.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CTwapUnit::CTwapUnit()
{
    memset(&m_oUserInfo, 0, sizeof(m_oUserInfo));
    m_pTradeSpi =0;
    m_pTransform =0;

    m_mapOrders.clear();
    m_mapContexts.clear();
    m_mapOrderRef2ID.clear();
    m_nreqId =0;
}

CTwapUnit::~CTwapUnit()
{

}

int         CTwapUnit::Instance(tagTradeUserInfo* pInfo, directStrategy* pSpi, Transform* pTransform,CTimesection* pTimeSection)
{
    m_oUserInfo = *pInfo;
    m_pTradeSpi = pSpi;
    m_pTransform = pTransform;
    m_pTimeSection = pTimeSection;

    m_mapOrders.clear();
    m_mapContexts.clear();
    m_mapOrderRef2ID.clear();
    return 1;
}

void        CTwapUnit::Release()
{
    auto itb = m_mapContexts.begin();
    while (itb != m_mapContexts.end() )
    {
        CTwapContext* pContext = itb->second;
        if (pContext)
        {
            delete pContext;
        }
        itb++;
    }
    
    m_mapOrders.clear();
    m_mapContexts.clear();
    m_mapOrderRef2ID.clear();
}

void        CTwapUnit::DoReqQryTwapOrder(tagXTTwapQryOrderField* pField, int requstid)
{
    if (strlen(pField->TwapOrderID) >0)
    {
        std::string twapid = pField->TwapOrderID;
        auto itb = m_mapOrders.find(twapid);
        if (itb != m_mapOrders.end() )
        {
            m_pTransform->OnRspQryTwapOrder(&(itb->second), 0, requstid, true);
        }
        else
        {
            tagXTRspInfoField oRspInfo;
            memset(&oRspInfo, 0, sizeof(oRspInfo));
            oRspInfo.ErrorID =1;
            _snprintf(oRspInfo.ErrorMsg, 255, "未找到该TwapID[%s]对应的单", twapid.c_str() );
            m_pTransform->OnRspQryTwapOrder(0, &oRspInfo, requstid, true);
        }
        
    }
    else
    {
        //请求所有
        auto itb = m_mapOrders.begin();
        while (itb != m_mapOrders.end())
        {
            m_pTransform->OnRspQryTwapOrder(&(itb->second), 0, requstid, false);
            itb++;
        }
        m_pTransform->OnRspQryTwapOrder(0, 0, requstid, true);
    }
    
}

void        CTwapUnit::DoReqQryTwapOrderDetail(tagXTTwapQryOrderField* pField, int requstid)
{
    //请求所有
    auto itb = m_mapOrderRef2ID.begin();
    while (itb != m_mapOrderRef2ID.end() )
    {
        tagXTTwapOrderInsertRtnField oField;
        memset(&oField, 0, sizeof(oField));
        oField.BrokerID = m_oUserInfo.Broker;
        strncpy(oField.UserID, m_oUserInfo.User, sizeof(m_oUserInfo.User)-1);
        strncpy(oField.RealOrderRef, itb->first.c_str(), 13);
        strncpy(oField.TwapOrderID, itb->second.c_str(), 13);
        oField.ExCode = m_mapOrders[itb->second].ExCode;
        
        m_pTransform->OnRspQryTwapOrderDetail(&oField, 0, requstid, false);
        itb++;
    }

    m_pTransform->OnRspQryTwapOrderDetail(0, 0, requstid, true);

}

void        CTwapUnit::DoReqTwapOrderInsert(tagXTTwapInputOrderField* pField, int requestid)
{
    //[1]赋值保存
    MDateTime  oDt;
    oDt.from_now();

    tagXTTwapOrderField  oField;
    memset(&oField, 0, sizeof(oField));
    memcpy(&oField, pField, sizeof(tagXTTwapInputOrderField));

    oField.InsertDate = oDt.dateToInt();
    oField.InsertTime = oDt.timeToInt();
    oField.TradingDate = m_pTradeSpi->GetTradingDay();
    oField.RemainVolume = pField->Volume;

    //[2]校验
    bool bvalid = true;
    if (pField->Volume <=0 )
    {
        _snprintf(oField.StatusMsg, 128, "设定数量无效[当前:%d]",  pField->Volume);
        bvalid = false;
    }
    //终止时间 要大于起始时间,且终止时间不能小于当前时间
    if (!m_pTimeSection->isExpectTime(oField.BeginTime, oField.EndTime))
    {
        _snprintf(oField.StatusMsg, 128, "起始时间和终止时间不在符合范围内[起始:%d][终止:%d][当前:%d]", oField.BeginTime, oField.EndTime, oDt.timeToInt() );
        bvalid = false;
    }
    
    //起止时间至少要在一个交易时段内
    if (!m_pTimeSection->isInSection(oField))
    {
        _snprintf(oField.StatusMsg, 128, "起始和终止时间至少要包含一个交易时段内[起始:%d][终止:%d]", oField.BeginTime, oField.EndTime);
        bvalid = false;
    }
    //校验结果
    if (!bvalid)
    {
        oField.TwapOrderStatus = XT_TWAP_AVOID;
    }
    else
    {
        _snprintf(oField.StatusMsg, 63, "Twap单已接受");
        oField.TwapOrderStatus = XT_TWAP_Accepted;
    }
    
    
    //[3]加入到队列
    std::string twapid = pField->TwapOrderID;
    {
        lock_guard<mutex> lock(m_mut);
        m_mapOrders.insert( make_pair(twapid, oField));
    }
    
    //[4]初始化上下文，并加入队列
    if (oField.TwapOrderStatus == XT_TWAP_Accepted)
    {
        CTwapContext* pContext = new CTwapContext(&oField, m_pTimeSection, m_pTradeSpi);
        m_mapContexts.insert(make_pair(twapid, pContext));

        //[5]注册行情
        std::string  code(oField.ExCode.Code);
        m_pTradeSpi->subscribeCode(code);
    }

    //[6]回推通知
    m_pTransform->OnRtnTwapOrder(&oField);
}

void        CTwapUnit::DoReqTwapOrderAction(tagXTTwapOrderActionField* pField, int requestid)
{
    std::string twapid = pField->TwapOrderID;
    auto itb = m_mapOrders.find(twapid);
    if (itb != m_mapOrders.end())
    {
        if (!inner_isEndState(itb->second.TwapOrderStatus))
        {
            _snprintf(itb->second.StatusMsg, 63, "Twap单已撤");
            itb->second.TwapOrderStatus = XT_TWAP_Cancelled;

            //回推通知
            m_pTransform->OnRtnTwapOrder(&(itb->second));
        }
    }
}

void        CTwapUnit::OnPeriodCall()
{
    std::vector<tagTwapEvent> arrEvents;

    auto itb = m_mapOrders.begin();
    while (itb != m_mapOrders.end())
    {
        if (!inner_isEndState(itb->second.TwapOrderStatus))
        {
            tagTwapEvent oEvent;
            std::string twapid = itb->first;
            if(m_mapContexts[twapid]->DoCheck(oEvent, this) )
            {
                arrEvents.push_back(oEvent);
            }
        }
        
        itb++;
    }
    
    for ( auto&  oEvent: arrEvents)
    {
        MarketDataField tick;
        double price = 0.0;

        std::string strcode(oEvent.ExCode.Code);
        if(m_pTradeSpi->getLastestTick(strcode, tick))
        {
            
            if (oEvent.Direction == XT_D_Buy)
            {
                printf("[CTwapUnit::OnPeriodCall]Buy [askprice]%.04lf\n", tick.AskPrice1);
                price = tick.AskPrice1;
            }
            else
            {
                printf("[CTwapUnit::OnPeriodCall]Sell [bidprice]%.04lf\n", tick.BidPrice1);
                price = tick.BidPrice1;
            }
            if (price <= tick.LowestPrice)
            {
                printf("[CTwapUnit::OnPeriodCall]222[%.04lf] < [%.04lf] [%.04lf]\n", 
                            price, tick.LowestPrice,tick.LastPrice );
                price = tick.LastPrice;
            }
            
        }
        else
        {
            printf("[CTwapUnit::OnPeriodCall]No tick info for trigger\n");
            //回退 实际下单数量
            oEvent.pCxt->RollbackVolume(oEvent.Volume );
            continue;
        }

        //继续填写下单所需的字段
        CThostFtdcInputOrderField oField;
        memset(&oField, 0, sizeof(oField));
        inner_fillOrder(oField, oEvent, price);

        //下单
        m_pTradeSpi->DoReqOrderInsert(&oField, oField.RequestID);

        //更新remainvol
        std::string twapid = oEvent.TwapID;
        m_mapOrders[twapid].RemainVolume -= oEvent.Volume;

        //更新下单orderRef 与tawpid 的map
        std::string strorderref(oField.OrderRef);
        m_mapOrderRef2ID[strorderref] = twapid;

        //通知
        if (m_mapOrders[twapid].RemainVolume <= 0)
        {
            m_mapOrders[twapid].TwapOrderStatus = XT_TWAP_End;
        }
        else
        {
            m_mapOrders[twapid].TwapOrderStatus = XT_TWAP_Executing;
        }
        m_pTransform->OnRtnTwapOrder(&m_mapOrders[twapid]);
        
    }
    
    
}

bool        CTwapUnit::inner_isEndState(char cStatus)
{
    if (cStatus == XT_TWAP_AVOID ||cStatus == XT_TWAP_Cancelled || cStatus == XT_TWAP_End)
    {
        return true;
    }
    return false;
}

void        CTwapUnit::UpdateStatus(const char* ptwapid, char status)
{
    std::string twapid = ptwapid;
    auto itb = m_mapOrders.find(twapid);
    if (itb != m_mapOrders.end() )
    {
        itb->second.TwapOrderStatus = status;
    }

    m_pTransform->OnRtnTwapOrder(&m_mapOrders[twapid]);
}

void        CTwapUnit::inner_fillOrder(CThostFtdcInputOrderField& outField, tagTwapEvent& oEvent, double dPrice)
{
    tagXTInputOrderField tField;
    memset(&tField, 0, sizeof(tagXTInputOrderField));

    tField.BrokerID = m_oUserInfo.Broker;
    strncpy(tField.UserID, m_oUserInfo.User, sizeof(m_oUserInfo.User)-1);
    tField.ExCode = oEvent.ExCode;
    genOrderRef(tField.OrderRef, "120");
    tField.PriceType = XT_OPT_LimitPrice;
    tField.Direction = oEvent.Direction;
    tField.OffsetFlag[0] = oEvent.Offset;
    tField.OffsetFlag[1] = oEvent.Offset;
    tField.HedgeFlag[0] = XT_HF_Speculation;
    tField.HedgeFlag[1] = XT_HF_Speculation;
    tField.LimitPrice = dPrice;
    tField.Volume = oEvent.Volume;
   
    printf("[inner_fillOrder]ori[code=%s][orderref=%s]\n", tField.ExCode.Code, tField.OrderRef);

    memset(&outField, 0, sizeof(CThostFtdcInputOrderField));
    _snprintf(outField.BrokerID, sizeof(outField.BrokerID)-1, "%d", tField.BrokerID);
    strncpy(outField.InvestorID, tField.UserID, sizeof(outField.InvestorID)-1 );
    strncpy(outField.UserID, tField.UserID, sizeof(outField.UserID)-1 );
    outField.RequestID = ++m_nreqId;
    transhelper::Trans_ReqOrderInsert(&tField, outField);

}

void        CTwapUnit::handleOrderRtn(tagXTInstrument& oExCode, const char* orderRef)
{
    string strRef(orderRef);
    auto itb = m_mapOrderRef2ID.find(strRef);
    if (itb != m_mapOrderRef2ID.end() )
    {
        tagXTTwapOrderInsertRtnField oField;
        memset(&oField, 0, sizeof(oField));
        oField.BrokerID = m_oUserInfo.Broker;
        strncpy(oField.UserID, m_oUserInfo.User, sizeof(m_oUserInfo.User)-1);
        oField.ExCode = oExCode;
        strncpy(oField.RealOrderRef, orderRef, 13);
        strcpy(oField.TwapOrderID, itb->second.c_str());
        m_pTransform->OnRtnTwapOrderInsert(&oField);
    }
    
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CTwapContext::CTwapContext(tagXTTwapOrderField* pField,CTimesection* pTimeSection,directStrategy* pSpi)
{
    m_oRecord = *pField;
    m_pTimeSection = pTimeSection;
    m_pTradeSpi = pSpi;

    InitCalc();
}

CTwapContext::~CTwapContext()
{

}

void        CTwapContext::InitCalc()
{
    m_nTotalVol = m_oRecord.Volume;         //用户设定的数量
    m_dTotalSecond = m_pTimeSection->getTotalSecond(&m_oRecord); //用户设定的总秒数

    m_nGap = max( (int)floor(m_dTotalSecond/m_nTotalVol), 1 );//间隔，指这次下单和下次下单之间的间隔（秒）
    m_nTimes = floor(m_dTotalSecond/m_nGap);    //执行次数，指总共需要执行的次数
    m_dExpectVol = double(m_nTotalVol)/m_nTimes;    //每次执行，期望的执行数量

    m_nThisTime = m_oRecord.RealStartTime;     //本次下单的期望时间（要进行规整，规整到实际的交易时段内）
    m_nNextTime = m_pTimeSection->getStandardTime(m_oRecord.ExCode.Code,  m_nThisTime , m_nGap);     //下次下单的期望时间（要进行规整，规整到实际的交易时段内）
    
    m_dShouldVol = 0;        //期待下单的数量(随执行的次数，进行累计)
    m_nRealVol = 0;          //实际下单的数量(随执行的次数，进行累计)
}

bool CTwapContext::DoCheck(tagTwapEvent& oOut, CTwapUnit* pUnit)
{
    char szlog[255]={0};
    //[1]校验
    if (m_nTotalVol - m_nRealVol <= 0)  //如果剩余数量<=0
    {
        _snprintf(szlog, 254, "[CTwapContext::DoCheck ]1111(%d)(%d)", m_nTotalVol, m_nRealVol);
        m_pTradeSpi->writeLog(0, szlog);
        pUnit->UpdateStatus(m_oRecord.TwapOrderID, XT_TWAP_End);
        return false;
    }
    
    MDateTime oDt;
    oDt.from_now();
    int now = oDt.timeToInt();
    now = m_pTimeSection->NormalizeTime(now);

    //[2]校验
    if (now > m_oRecord.RealEndTime)
    {
        _snprintf(szlog, 254, "[CTwapContext::DoCheck ]2222(%d)(%d)", now, m_oRecord.RealEndTime);
        m_pTradeSpi->writeLog(0, szlog);
        pUnit->UpdateStatus(m_oRecord.TwapOrderID, XT_TWAP_End);
        return false;
    }

    if (now  >= m_nThisTime) //如果时间> thistime
    {
        if (now  <= m_nNextTime)  //如果时间小于nexttime, 执行并更新时间
        {
            _snprintf(szlog, 254, "[CTwapContext::DoCheck ]3333(now=%d)(this=%d)(next=%d)(should=%.04lf)(expect=%.04lf)", 
                        now, m_nThisTime,m_nNextTime, m_dShouldVol, m_dExpectVol);
            m_pTradeSpi->writeLog(0, szlog);
            m_dShouldVol += m_dExpectVol;       

            m_nThisTime = m_nNextTime;
            m_nNextTime = m_pTimeSection->getStandardTime(m_oRecord.ExCode.Code,  m_nThisTime , m_nGap);

            int wantVol = (int)ceil(m_dShouldVol - m_nRealVol);
            wantVol = min(wantVol, m_nTotalVol - m_nRealVol);
            if (wantVol >0 )
            {
                oOut.pCxt = this;
                oOut.ExCode = m_oRecord.ExCode;
                strncpy(oOut.TwapID, m_oRecord.TwapOrderID, sizeof(m_oRecord.TwapOrderID));
                oOut.Direction = m_oRecord.Direction;
                oOut.Offset = m_oRecord.OffsetFlag;
                oOut.Volume = wantVol;

                m_nRealVol += wantVol;
                _snprintf(szlog, 254,"[CTwapContext::DoCheck ]4444(want=%d)(this=%d)(next=%d)(realvol=%d)", 
                        wantVol, m_nThisTime, m_nNextTime, m_nRealVol);
                m_pTradeSpi->writeLog(0, szlog);
                return true;
            }
            else
            {
                _snprintf(szlog, 254,"[CTwapContext::DoCheck ]5555(want=%d)(this=%d)(next=%d)", 
                            wantVol, m_nThisTime, m_nNextTime);
                m_pTradeSpi->writeLog(0, szlog);
                return false;
            }
        }
        else  //如果时间也超过了nexttime, 则不执行，累计数量，并更新时间
        {
            _snprintf(szlog, 254,"[CTwapContext::DoCheck ]6666(now=%d)(this=%d)(next=%d)", 
                        now, m_nThisTime,m_nNextTime);
            m_pTradeSpi->writeLog(0, szlog);
            m_dShouldVol += m_dExpectVol;     
            _snprintf(szlog, 254,"[CTwapContext::DoCheck ]7777(expect=%.04lf)(should=%.04lf)(realvol=%d)", 
                        m_dExpectVol, m_dShouldVol,m_nRealVol);  
            m_pTradeSpi->writeLog(0, szlog);

            m_nThisTime = m_nNextTime;
            m_nNextTime = m_pTimeSection->getStandardTime(m_oRecord.ExCode.Code,  m_nThisTime , m_nGap);
            _snprintf(szlog, 254,"[CTwapContext::DoCheck ]8888(this=%d)(next=%d)", m_nThisTime, m_nNextTime);
            m_pTradeSpi->writeLog(0, szlog);
            return false;
        }
    }
    
    return false;
}

void        CTwapContext::RollbackVolume(int rollvol)
{
    m_nRealVol -= rollvol;
}

