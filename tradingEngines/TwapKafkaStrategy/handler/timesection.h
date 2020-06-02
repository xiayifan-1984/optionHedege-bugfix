#ifndef TIMESECTION_DEFINE_H
#define TIMESECTION_DEFINE_H
#include <string>
#include <vector>
#include <map>
#include "../xtcomm/XTTwapStruct.h"

//管理各品种的交易时段
class CTimesection
{
public:
    CTimesection();
    ~CTimesection();

public:
    int             Instance(const char* pconfigfile);
    void            Release();

public:
    bool            getProduct(const char* pCode, std::string& outstr);

    bool            isExpectTime(int begintime, int endtime);

    bool            isInSection(tagXTTwapOrderField& oField);

    int             NormalizeTime(int intime);
    
public:
    double          getTotalSecond(tagXTTwapOrderField* pField);

    int             getStandardTime(const char* pCode, int  begintime, int gap);

protected:
    typedef struct
    {
        int         begin;      //HHMMSS
        int         end;        //HHMMSS
    }tagSection;

protected:
    void            inner_parseOneSection(tagSection& oSec, std::string& strsec);
    void            inner_parseOneGroup(int gid, std::string strpro);
    int             inner_HHMMSS2Second(int hhmmss);
    int             inner_Second2HHMMSS(int second);

protected:
    std::map<int, std::vector<tagSection> >         m_mapid2Section;        // groupid <====>  sections
    std::map<std::string, int>                      m_mapcode2id;           // product(code) <====>  groupid
    int                                             m_nClearTime;           //HHMMSS
};
























#endif
