
#include "config.h"
#include "../util/MIniFile.h"
#include <string>
using namespace std;

#include "../util/Platform.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

CConfig::CConfig()
{
	m_pIniFile = 0;
	memset(m_szStrategyName, 0, sizeof(m_szStrategyName));
	memset(m_szExePath, 0, sizeof(m_szExePath));

	memset(m_szKafkaBroker, 0, sizeof(m_szKafkaBroker));
}

CConfig::~CConfig()
{

}

int				CConfig::Instance()
{
	if (0 == m_pIniFile)
	{
		m_pIniFile = new MIniFile();
	}

	char szSelfName[255]={0};
	get_exec_path(m_szExePath ,  szSelfName, 255);
	
	char szIniFile[255] = {0};
	strcpy(szIniFile, m_szExePath);
	strcat(szIniFile,  szSelfName);
	strcat(szIniFile, ".ini");
	printf("%s\n", szIniFile);

	int iret = m_pIniFile->loadFile( szIniFile );
	if (iret <0)
	{
		return -1;
	}

	string strtemp;

	//
	{
		m_pIniFile->getPrivateProfileString("main", "name", "", strtemp);
		strcpy(m_szStrategyName, strtemp.c_str());

		m_pIniFile->getPrivateProfileString("main", "broker", "", strtemp);
		strcpy(m_szKafkaBroker, strtemp.c_str());
	
	}

	

	return 1;
}

void			CConfig::Release()
{
	if (m_pIniFile)
	{
		delete m_pIniFile;
		m_pIniFile = 0;
	}
}

void			CConfig::GetStrategyName(char*  pszName)
{
	strcpy(pszName, m_szStrategyName);
}

void			CConfig::GetKafkaBroker(char* pszbroker)
{
	strcpy(pszbroker, m_szKafkaBroker);
}

int				CConfig::get_exec_path(char* processdir, char* processname, int nlen)
{
	char* path_end;
	if(readlink("/proc/self/exe", processdir, nlen) <=0)
	{
		return -1;
	}

	path_end = strrchr(processdir, '/');
	if(path_end == NULL)
	{
		return -1;
	}

	++path_end;

	strcpy(processname, path_end);

	*path_end = '\0';
	return (path_end - processdir);
}
















