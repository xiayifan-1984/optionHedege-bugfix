#ifndef _SPIDER_MINIFILE_DEFINE_H
#define _SPIDER_MINIFILE_DEFINE_H

#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

class MIniFile
{
public:
	MIniFile();
	~MIniFile();

public:
	int				loadFile(const char* file);
	int				getPrivateProfileInt(const char* appName, const char* keyName,  int ndefault);
	int				getPrivateProfileString(const char* appName, const char* keyName, const char* sdefault, std::string& out);

public:
	int				getPrivateProfileInt(const char* nodeName, int ndefault);
	int				getPrivateProfileString(const char* nodeName, const char* sdefault, std::string& out);

private:
	std::string						strfile;
	boost::property_tree::ptree		pt_;

};




#endif