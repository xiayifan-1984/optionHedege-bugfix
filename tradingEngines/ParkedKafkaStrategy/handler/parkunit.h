#ifndef _PARKUNIT_DEFINE_H
#define _PARKUNIT_DEFINE_H

#include <map>
#include <string>
#include <unordered_map>
#include <set>
#include <vector>
#include <mutex>
#include <functional>
#include "../pubstruct.h"
#include "../xtcomm/XTTParkedStruct.h"
#include "../../../system/api/ctp/v6.3.15/include/ThostFtdcTraderApi.h"

class directStrategy;
class Transform;
class CTimesection;

class CParkUnit
{
public:
    CParkUnit(/* args */);
    ~CParkUnit();

public:
    int         Instance(tagTradeUserInfo* pInfo, directStrategy* pSpi, Transform* pTransform, CTimesection* pTimeSection);
    void        Release();

public:
    void        DoReqQryParkedOrder(tagXTQryParkedOrderField* pField, int requestid);
    void        DoReqParkedOrderInsert(tagXTParkedOrderInputField* pField, int requestid);
    void        DoReqParkedOrderAction(tagXTParkedOrderActionField* pField, int requestid);

public:
    void        OnPeriodCall();

protected:
    bool        inner_isEndState(char  ParkStatus); 
    bool        inner_isTrigged(tagXTParkedOrderField& inField);
    void        inner_insertOrder(tagXTParkedOrderField& inField);
	void		inner_initMapFunc();
    void        inner_fillOrder(CThostFtdcInputOrderField& outField, tagXTParkedOrderField& inField);

protected:
    std::map<std::string, tagXTParkedOrderField>    m_mapOrders;        //orderID <===>  Struct
	std::map<char, std::function<bool(double, double, double, double)>> m_mapCondFun;

private:
    tagTradeUserInfo                                m_oUserInfo;
    directStrategy*                                 m_pTradeSpi;
    Transform*                                      m_pTransform;
    CTimesection*                                   m_pTimeSection;
    int                                             m_nreqId;

    std::mutex                                      m_mut;

};





#endif
