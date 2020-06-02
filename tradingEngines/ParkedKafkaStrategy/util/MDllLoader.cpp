
#include "MDllLoader.h"

MDllLoader::MDllLoader()
{
	m_dll = 0;
}

MDllLoader::~MDllLoader()
{
	_unloadFile();
}

bool 	MDllLoader::isLoaded()
{
	return 0 == m_dll;
}

template<typename FuncType>
FuncType 		MDllLoader::getProc(const char* procname)
{
	return static_cast<FuncType>(_doGetProc(procname));
}

void*			MDllLoader::getProc2(const char* procname)
{
	return _doGetProc(procname);
}


#if defined(WINDOWS) || defined(WIN32)
int 			MDllLoader::loadFile(const char* pszfile)
{
	m_dll = ::LoadLibrary(pszfile);

	return (0 == m_dll)? (-1):(1) ;
}

void* 			MDllLoader::_doGetProc(const char* procname)
{
	if (0 == m_dll)
	{
		return 0;
	}
	else
	{
		return ::GetProcAddress(m_dll, procname);
	}
}

void 			MDllLoader::_unloadFile()
{
	if (0 != m_dll)
	{
		::FreeLibrary(m_dll);
		m_dll = 0;
	}
}

#elif defined(LINUX)

int 			MDllLoader::loadFile(const char* pszfile)
{
	m_dll = dlopen(pszfile, RTLD_LAZY | RTLD_GLOBAL);

	return (0 == m_dll)? (-1):(1) ;
}

void* 			MDllLoader::_doGetProc(const char* procname)
{
	return (0 == m_dll) ? 0 : dlsym(m_dll, procname);
}

void 			MDllLoader::_unloadFile()
{
	if (0 != m_dll)
	{
		dlclose(m_dll);
		m_dll = 0;
	}
}

#endif







