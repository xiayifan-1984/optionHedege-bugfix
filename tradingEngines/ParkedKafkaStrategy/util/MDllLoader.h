#ifndef _SPIDER_MDLLLOADER_DEFINE_H
#define _SPIDER_MDLLLOADER_DEFINE_H

#include "Platform.h"

class MDllLoader
{
public:
	MDllLoader();
	~MDllLoader();

public:
	int 			loadFile(const char* pszfile);

	template<typename FuncType>
	FuncType 		getProc(const char* procname);	

	void*			getProc2(const char* procname);

	bool 			isLoaded();

protected:
	void* 			_doGetProc(const char* procname);
	void 			_unloadFile();

private:
	module_t 		m_dll;

};



#endif
