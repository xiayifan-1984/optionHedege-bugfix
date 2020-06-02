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

bool            CTimesection::isTradingTime(const char* pCode)
{
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

    int gid = itb->second;
    std::vector<tagSection>&  arr = m_mapid2Section[gid];
    //[3]寻找 真实的开始
    for (int i = 0; i < arr.size(); i++)
    {
        if (now >= arr[i].begin && now <= arr[i].end )
        {
            return true;
        }
    }
  
    return false;
}


