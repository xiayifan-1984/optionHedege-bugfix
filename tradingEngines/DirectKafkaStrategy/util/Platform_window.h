#ifndef _SPIDER_PLATFORM_WINDOW_DEFINE_H
#define _SPIDER_PLATFORM_WINDOW_DEFINE_H


///使用windows特有定义
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

///包含windows特有头文件
#include <windows.h>

///重定义windows特有类型或补定义缺失类型
typedef		HANDLE		handle_t;

typedef		HMODULE		module_t;


#ifndef	ftell64
#define		ftell64     _ftelli64
#endif

#ifndef	fseek64
#define		fseek64     _fseeki64
#endif


#ifndef	vsnprintf
#define	vsnprintf(buf, size, format, argptr)	_vsnprintf_s(buf, size, _TRUNCATE, format, argptr)
#endif//vsnprintf





#endif