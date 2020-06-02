
#ifndef TDHANDLER_DEFINE_H
#define TDHANDLER_DEFINE_H

//启动directstrategy 启动transform,  
class directStrategy;
class Transform;
class CConfig;
class CTimesection;
class CParkUnit;
class tdhandler
{
public:
	tdhandler();
	virtual ~tdhandler();

public:
	int						Instance(CConfig* pConfig, CTimesection* pTimeSection);
	void					Release();

public:
	void 					handleParkOrder();

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
	CParkUnit* 				m_pParkUnit;
};






#endif

















