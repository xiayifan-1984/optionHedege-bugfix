#include "timesection.h"
#include <stdio.h>
#include <memory>
using namespace std;

#include "../util/MIniFile.h"
#include "../util/Platform.h"
#include "../util/MDateTime.h"


CTimesection::CTimesection()
{
    m_nClearTime = 170000;
}

CTimesection::~CTimesection()
{

}

int             CTimesection::Instance(const char* pconfigfile)
{
    shared_ptr<MIniFile> pIniFile = make_shared<MIniFile>();
    int iret = pIniFile->loadFile(pconfigfile);
    if (iret <0)
    {
        return -1;
    }
    
    int gcount = pIniFile->getPrivateProfileInt("main","count", 0);
    if (gcount <= 0)
    {
        return 1;
    }

    m_nClearTime = pIniFile->getPrivateProfileInt("main", "cleartime", 170000);
    
    for (int i = 0; i < gcount; i++)
    {
        char szsection[32] ={0};
        _snprintf(szsection, 31, "group_%d", i);

        std::string strout;
        pIniFile->getPrivateProfileString(szsection, "product", "", strout);
        int scount = pIniFile->getPrivateProfileInt(szsection, "count", 0);

        vector<tagSection> arrSec;
        for (int j = 0; j < scount; j++)
        {
            char szkey[32] = {0};
            _snprintf(szkey, 31, "sec_%d", j);
            std::string strsec;
            pIniFile->getPrivateProfileString(szsection, szkey, "",  strsec);
            
            tagSection oSection;
            inner_parseOneSection(oSection, strsec);
            arrSec.push_back( oSection);
        }
        m_mapid2Section.insert( make_pair( i, arrSec ));

        inner_parseOneGroup(i, strout);
        
    }
    
    return 1;
}

void            CTimesection::Release()
{
    m_mapid2Section.clear();
    m_mapcode2id.clear();
}

int             CTimesection::NormalizeTime(int intime)
{
    if (intime < m_nClearTime)
    {
        return (intime += 240000);
    }
    return intime;
}

void            CTimesection::inner_parseOneSection(tagSection& oSec, std::string& strsec)
{
    const char* pstr = strsec.c_str();
    for (int i = 0; i < strlen(pstr); i++)
    {
        if (pstr[i] == ',')
        {
            int begin = atoi(pstr);
            int end = atoi(pstr+i+1);
            
            oSec.begin = NormalizeTime(begin);
            oSec.end= NormalizeTime(end);

            break;
        }
        
    }
}

void            CTimesection::inner_parseOneGroup(int gid, std::string strpro)
{
    const char* pstr = strpro.c_str();
    int b =0;

    int nlen = strlen(pstr);
    for (int i = 0; i < nlen; i++)
    {
        if (pstr[i] == ',' )
        {
            if (i >b)
            {
                string str(pstr+b, i-b);
                m_mapcode2id.insert(make_pair(str, gid));
            }
            
            b = i+1;
        }
    }
    
    if ( nlen > b)
    {
        string str(pstr+b, nlen-b);
        m_mapcode2id.insert(make_pair(str, gid));
    }
    
}

bool            CTimesection::isExpectTime(int begintime, int endtime)
{
    MDateTime oDt;
    oDt.from_now();

    int now = oDt.timeToInt();
    now = NormalizeTime(now);

    int begin = NormalizeTime(begintime);
    int end = NormalizeTime(endtime);

    if (begin > end)
    {
        return false;
    }

    if (now >= end)
    {
        return false;
    }
    
    return true;
}

bool            CTimesection::getProduct(const char* pCode, std::string& outstr)
{
    int b =0;
    for (int i = 0; i < strlen(pCode); i++)
    {
        if (pCode[i] >='0' && pCode[i] <= '9')
        {
            b = i;
            break;  
        }
    }

    if (b >0)
    {
        //获取产品代码，AU1909 的产品代码是AU
        outstr.assign(pCode, b -0);
        transform(outstr.begin(), outstr.end(), outstr.begin(), ::tolower);
        return true;
    }

    return false;
}

bool            CTimesection::isInSection(tagXTTwapOrderField& oField)
{
    const char* pCode = oField.ExCode.Code;
    int begintime = oField.BeginTime;
    int endtime = oField.EndTime;

    //[0]能否获取产品代码
    std::string result;
    if (!getProduct(pCode, result))
    {
        return false;
    }
    
    //[1]能否找到该产品 配置的交易区段信息
    auto itb = m_mapcode2id.find(result);
    if (itb == m_mapcode2id.end())
    {
        return false;
    }

    //[2]赋值，
    MDateTime oDt;
    oDt.from_now();
    int now = oDt.timeToInt();
    now = NormalizeTime(now);

    begintime = NormalizeTime(begintime);
    endtime = NormalizeTime(endtime);
    int realStartTime = -1;
    int realEndTime = -1;
    int realStartNo = -1;
    int realEndNo = -1;
    if (now > begintime)        //如果此时， 已经超过了开始时间
    {
        begintime = now;
    }

    int gid = itb->second;
    std::vector<tagSection>&  arr = m_mapid2Section[gid];
    //[3]寻找 真实的开始
    for (int i = 0; i < arr.size(); i++)
    {
        if (begintime <= arr[i].begin)
        {
            realStartTime = arr[i].begin;
            realStartNo = i;
            break;
        }
        if (begintime < arr[i].end)
        {
            realStartTime = begintime;
            realStartNo = i;
            break;
        }

    }
    if (realStartTime <0)  //无法开始 {此情况就是：开始时间在盘后}
    {
        return false;
    }
    //[4]寻找 真实的结束
    for (int i = arr.size()-1; i >=0; i--)
    {
        if (endtime >= arr[i].end)
        {
            realEndTime = arr[i].end;
            realEndNo = i;
            break;
        }
        if (endtime > arr[i].begin)
        {
            realEndTime = endtime;
            realEndNo = i;
            break;
        }
    }
    if (realEndTime <0) //无法结束 (此情况就是：结束时间在盘前)
    {
        return false;
    }

    //[5]结束时间必须小于开始时间
    if (realEndTime <= realStartTime )  //无执行时间 (此情况是： 开始和结束时间 的时间区段，在休息时段)
    {
        return false;
    }
    
    //[6]检验成功
    oField.RealStartTime = realStartTime;
    oField.RealEndTime = realEndTime;
    oField.RealStartNo = realStartNo;
    oField.RealEndNo = realEndNo;
    return true;
}

int             CTimesection::inner_Second2HHMMSS(int ttm)
{
    int hour = ttm/3600;
    int minute = (ttm-hour*3600)/60;
    int second = ttm-hour*3600 - minute*60;
    return hour*10000 + minute*100 + second;
}

int             CTimesection::inner_HHMMSS2Second(int hhmmss)
{
    int hour = hhmmss/10000;
    int minute = (hhmmss%10000)/100;
    int second = hhmmss%100;
    return hour*3600 + minute*60 + second;
}

double          CTimesection::getTotalSecond(tagXTTwapOrderField* pField)
{
    if (pField->RealStartNo == pField->RealEndNo)  //直接换算成秒数，再相减
    {
        int begin = inner_HHMMSS2Second(pField->RealStartTime);
        int end = inner_HHMMSS2Second(pField->RealEndTime);
        return (end - begin);
    }
    else
    {
        //[0]能否获取产品代码
        std::string result;
        getProduct(pField->ExCode.Code, result);
        //[1]能否找到该产品 配置的交易区段信息
        auto itb = m_mapcode2id.find(result);
        int gid = itb->second;
        std::vector<tagSection>&  arr = m_mapid2Section[gid];

        int gap =0;
        for (int i = pField->RealStartNo; i <= pField->RealEndNo; i++)
        {
            if (i == pField->RealStartNo)
            {
                gap += inner_HHMMSS2Second(arr[i].end) - inner_HHMMSS2Second(pField->RealStartTime);
            }
            else if (i == pField->RealEndNo)
            {
                gap += inner_HHMMSS2Second(pField->RealEndTime) - inner_HHMMSS2Second(arr[i].begin);
            }
            else
            {
                gap += inner_HHMMSS2Second(arr[i].end) - inner_HHMMSS2Second(arr[i].begin);
            }
        }
        return gap;
    }
    
}
    
int             CTimesection::getStandardTime(const char* pCode, int  begintime, int gap)
{
    //[0]能否获取产品代码
    std::string result;
    getProduct(pCode, result);
    //[1]能否找到该产品 配置的交易区段信息
    auto itb = m_mapcode2id.find(result);
    int gid = itb->second;
    std::vector<tagSection>&  arr = m_mapid2Section[gid];

    int thisNo = -1;
    //找到begintime所在的交易区段(  不出意外，begintime一定在某个区段内)
    for (int i = 0; i < arr.size(); i++)
    {
        if (arr[i].begin <= begintime && begintime <= arr[i].end)
        {
            //找到了
            thisNo = i;
            break;
        }
    }

    if (thisNo <0)
    {
        return (480000);
    }
    
    //找到了,计算期待的下次时间
    int expect =  inner_Second2HHMMSS( inner_HHMMSS2Second(begintime) + gap );
    if (expect <= arr[thisNo].end)
    {
        //在本区段内，直接返回
        return expect;
    }

    int left = inner_HHMMSS2Second(expect) - inner_HHMMSS2Second(arr[thisNo].end);
    while (++thisNo < arr.size())
    {
        int interval = inner_HHMMSS2Second(arr[thisNo].end) - inner_HHMMSS2Second(arr[thisNo].begin);
        if (interval >= left)
        {
            return inner_Second2HHMMSS( inner_HHMMSS2Second(arr[thisNo].begin) + left );
        }
        left -= interval;
    }

    return (480000);
}