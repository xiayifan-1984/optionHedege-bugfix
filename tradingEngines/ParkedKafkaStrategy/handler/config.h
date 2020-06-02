#ifndef CONFIG_DEFINE_H
#define CONFIG_DEFINE_H

#include "../pubstruct.h"

class MIniFile;
class CConfig
{
public:
	CConfig();
	~CConfig();

	int				Instance();
	void			Release();

	void			GetStrategyName(char*  pszName);
	void			GetKafkaBroker(char* pszbroker);
	void 			GetTimeSectionFile(char* pszFile);

protected:
	int				get_exec_path(char* processdir, char* processname, int nlen);

protected:
	char			m_szStrategyName[255];
	char			m_szExePath[255];
	char			m_szKafkaBroker[255];
	char 			m_szSectionFile[255];

protected:
	MIniFile*		m_pIniFile;

};




#endif


















