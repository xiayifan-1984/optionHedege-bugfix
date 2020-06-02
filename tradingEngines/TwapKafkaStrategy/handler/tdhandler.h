
#ifndef TDHANDLER_DEFINE_H
#define TDHANDLER_DEFINE_H

//启动directstrategy 启动transform,  
class directStrategy;
class Transform;
class CConfig;
class CTimesection;
class CTwapUnit;
class tdhandler
{
public:
	tdhandler();
	virtual ~tdhandler();

public:
	int						Instance(CConfig* pConfig, CTimesection* pTimeSection);
	void					Release();

public:
	void 					handleTwapOrder();

protected:
	int						inner_Instance();
	void					inner_Release();

protected:
	directStrategy*			m_pStrategy;
	Transform*				m_pTransform;
	bool					m_bInitOK;

private:
	CConfig*				m_pConfig;
	CTimesection* 			m_pTimeSection;
	CTwapUnit* 				m_pTwapUnit;
	
};






#endif

















