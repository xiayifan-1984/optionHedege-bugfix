
#ifndef TDHANDLER_DEFINE_H
#define TDHANDLER_DEFINE_H

//Æô¶¯directstrategy Æô¶¯transform, 
class directStrategy;
class Transform;
class CConfig;
class tdhandler
{
public:
	tdhandler();
	virtual ~tdhandler();

public:
	int						Instance(CConfig* p);
	void					Release();

protected:
	int						inner_Instance();
	void					inner_Release();

protected:
	directStrategy*			m_pStrategy;
	Transform*				m_pTransform;
	bool					m_bInitOK;

private:
	CConfig*				m_pConfig;

};






#endif

















