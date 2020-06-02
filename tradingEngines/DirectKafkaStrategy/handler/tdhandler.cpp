
#include "tdhandler.h"
#include "directstrategy.h"
#include "transform.h"
#include "config.h"

tdhandler::tdhandler()
{
	m_pStrategy = 0;
	m_pTransform = 0;
	m_bInitOK = false;
}

tdhandler::~tdhandler()
{

}

int			tdhandler::Instance(CConfig* p)
{
	m_pConfig = p;
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
	char szName[64] = {0};
	m_pConfig->GetStrategyName(szName);
	std::string strname;
	strname = szName; 
	m_pStrategy = new directStrategy(strname);

	char szbroker[256]={0};
	m_pConfig->GetKafkaBroker(szbroker);

	int brokerid = 0;
	std::string userid;
	m_pStrategy->getUserInfo(brokerid, userid);
	m_pStrategy->init();
	m_pStrategy->start();

	m_pTransform = new Transform();
	tagTradeUserInfo  oUserInfo;
	memset(&oUserInfo, 0, sizeof(oUserInfo));
	oUserInfo.Broker = brokerid;
	strcpy(oUserInfo.User, userid.c_str());
	int iret = m_pTransform->Instance(&oUserInfo, m_pStrategy, szbroker);

	m_pStrategy->setTransform(m_pTransform);

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

	if (m_pStrategy)
	{
		delete m_pStrategy;
		m_pStrategy = 0;
	}
}










