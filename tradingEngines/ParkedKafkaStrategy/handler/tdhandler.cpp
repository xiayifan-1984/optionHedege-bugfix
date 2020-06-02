
#include "tdhandler.h"
#include "directstrategy.h"
#include "transform.h"
#include "config.h"
#include "parkunit.h"

tdhandler::tdhandler()
{
	m_pConfig =0;
	m_pTimeSection =0;
	m_pParkUnit =0;

	m_pStrategy = 0;
	m_pTransform = 0;
	m_bInitOK = false;
}

tdhandler::~tdhandler()
{

}

int			tdhandler::Instance(CConfig* pConfig, CTimesection* pTimeSection)
{
	m_pConfig = pConfig;
	m_pTimeSection = pTimeSection;

	int iret = inner_Instance();
	if (iret < 0)
	{
		return (-1);
	}
	return 1;
}

void		tdhandler::Release()
{
	inner_Release();
}

int			tdhandler::inner_Instance()
{
	//[1]
	char szName[64] = {0};
	m_pConfig->GetStrategyName(szName);
	std::string strname;
	strname = szName; 
	m_pStrategy = new directStrategy(strname);

	//[2]
	char szbroker[256]={0};
	m_pConfig->GetKafkaBroker(szbroker);

	int brokerid = 0;
	std::string userid;
	m_pStrategy->getUserInfo(brokerid, userid);
	m_pStrategy->init();
	m_pStrategy->start();

	//[3]
	m_pParkUnit = new CParkUnit();
	m_pTransform = new Transform();
	tagTradeUserInfo  oUserInfo;
	memset(&oUserInfo, 0, sizeof(oUserInfo));
	oUserInfo.Broker = brokerid;
	strcpy(oUserInfo.User, userid.c_str());
	int iret = m_pTransform->Instance(&oUserInfo, m_pStrategy, m_pParkUnit, szbroker);
	if (iret <0)
	{
		return -1;
	}
	
	//[4]
	m_pStrategy->setTransform(m_pTransform);

	//[5]
	iret = m_pParkUnit->Instance(&oUserInfo, m_pStrategy, m_pTransform, m_pTimeSection);
	if (iret <0)
	{
		return -2;
	}
	
	return iret;
}

void		tdhandler::inner_Release()
{
	if (m_pStrategy)
	{
		m_pStrategy->setTransform(0);

		m_pStrategy->terminate();
	}

	if (m_pTransform)
	{
		m_pTransform->Release();

		delete m_pTransform;
		m_pTransform = 0;
	}

	if (m_pParkUnit)
	{
		m_pParkUnit->Release();
		delete m_pParkUnit;
		m_pParkUnit =0;
	}
	
	if (m_pStrategy)
	{
		delete m_pStrategy;
		m_pStrategy = 0;
	}
}


void 	tdhandler::handleParkOrder()
{
	if (m_pParkUnit)
	{
		m_pParkUnit->OnPeriodCall();
	}
	
}







