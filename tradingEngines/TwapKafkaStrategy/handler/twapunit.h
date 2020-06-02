#ifndef TWAPUNIT_DEFINE_H
#define TWAPUNIT_DEFINE_H
#include <map>
#include <string>
#include <unordered_map>
#include <set>
#include <vector>
#include <mutex>
#include "../pubstruct.h"
#include "../xtcomm/XTTwapStruct.h"
#include "../../../system/api/ctp/v6.3.15/include/ThostFtdcTraderApi.h"

class directStrategy;
class Transform;
class CTimesection;

class CTwapContext;
class CTwapUnit;

typedef struct 
{
    /* data */
    CTwapContext*       pCxt;
    tagXTInstrument     ExCode;
    int                 Volume;
    char                Direction;
    char                Offset;
    char_13             OrderRef;
    char_13             TwapID;

}tagTwapEvent;



//一个Twap单的上下文
class CTwapContext
{
public:
    CTwapContext(tagXTTwapOrderField* pField,CTimesection* pTimeSection,directStrategy* pSpi);
    ~CTwapContext();

public:
    bool        DoCheck(tagTwapEvent& oOut, CTwapUnit* pUnit);
    void        RollbackVolume(int rollvol);

protected:
    void        InitCalc();

protected:
    int                                             m_nGap;             //间隔，指这次下单和下次下单之间的间隔（秒）
    int                                             m_nTimes;           //执行次数，指总共需要执行的次数
    double                                          m_dExpectVol;       //每次期望下单的数量

    int                                             m_nThisTime;        //本次下单的期望时间（要进行规整，规整到实际的交易时段内）
    int                                             m_nNextTime;        //下次下单的期望时间（要进行规整，规整到实际的交易时段内）
    
    double                                          m_dTotalSecond;     //用户设定的总秒数
    int                                             m_nTotalVol;         //用户设定的数量
    
    double                                          m_dShouldVol;        //期待下单的数量(随执行的次数，进行累计)
    int                                             m_nRealVol;          //实际下单的数量(随执行的次数，进行累计)

protected:
    tagXTTwapOrderField                             m_oRecord;
    CTimesection*                                   m_pTimeSection;
    directStrategy*                                 m_pTradeSpi;
};




//TWap单处理模块

class CTwapUnit
{
public:
    CTwapUnit();
    ~CTwapUnit();

public:
    int         Instance(tagTradeUserInfo* pInfo, directStrategy* pSpi, Transform* pTransform, CTimesection* pTimeSection);
    void        Release();

public:
    void        DoReqQryTwapOrder(tagXTTwapQryOrderField* pField, int requstid);
    void        DoReqTwapOrderInsert(tagXTTwapInputOrderField* pField, int requestid);
    void        DoReqTwapOrderAction(tagXTTwapOrderActionField* pField, int requestid);
    void        DoReqQryTwapOrderDetail(tagXTTwapQryOrderField* pField, int requstid);

public:
    void        OnPeriodCall();
    void        UpdateStatus(const char* ptwapid, char status);
    void        handleOrderRtn(tagXTInstrument& oExCode, const char* orderRef);

protected:
    bool        inner_isEndState(char cStatus);
    void        inner_fillOrder(CThostFtdcInputOrderField& outField, tagTwapEvent& oEvent, double dPrice);

protected:
    std::map<std::string, tagXTTwapOrderField>      m_mapOrders;        //twapID <====> Order
    std::map<std::string, CTwapContext* >           m_mapContexts;      //twapID <====> Context
    std::unordered_map<std::string, std::string>    m_mapOrderRef2ID;   //orderRef <====>  twapID

private:
    tagTradeUserInfo                                m_oUserInfo;
    directStrategy*                                 m_pTradeSpi;
    Transform*                                      m_pTransform;
    CTimesection*                                   m_pTimeSection;
    int                                             m_nreqId;

    std::mutex                                      m_mut;
};






#endif
