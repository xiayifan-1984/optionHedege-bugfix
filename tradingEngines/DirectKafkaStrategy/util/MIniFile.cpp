
#include "MIniFile.h"
#include "Platform.h"

MIniFile::MIniFile()
{

}

MIniFile::~MIniFile()
{

}


int		MIniFile::loadFile(const char* pszfile)
{
	int iret = (-1);
	strfile = pszfile;

	try
	{
		boost::property_tree::read_ini(pszfile, pt_);

		iret = 1;
	}
	catch (const std::exception& e)
	{
		printf("MIniFile::loadFile error[%s]\n", e.what());
	}

	return iret;
}

int		MIniFile::getPrivateProfileInt(const char* appName, const char* keyName,  int ndefault)
{
	char nodeName[64] = {0};
	snprintf(nodeName, 63, "%s.%s", appName, keyName);

	return getPrivateProfileInt(nodeName, ndefault);
}

int		MIniFile::getPrivateProfileString(const char* appName, const char* keyName, const char* sdefault, std::string& out)
{
	char nodeName[64] = {0};
	snprintf(nodeName, 63, "%s.%s", appName, keyName);

	return getPrivateProfileString(nodeName, sdefault, out);
}

int		MIniFile::getPrivateProfileInt(const char* nodeName, int ndefault)
{
	int iret = ndefault;

	try
	{
		iret = pt_.get<int>(nodeName, ndefault);
	}
	catch (const std::exception& e)
	{
		UNUSED(e);
		iret = ndefault;
	}
	
	return iret;
	
}

int		MIniFile::getPrivateProfileString(const char* nodeName, const char* sdefault, std::string& out)
{
	int iret = (-1);
	std::string strdef;
	if(sdefault)
	{
		strdef = sdefault;
	}
	else
	{
		strdef = "";
	}

	try
	{
		out = pt_.get<std::string>(nodeName, strdef);
		iret = 1;
	}
	catch (const std::exception& e)
	{
		UNUSED(e);
		out = strdef;
	}

	return iret;
}


