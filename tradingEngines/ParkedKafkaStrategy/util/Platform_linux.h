#ifndef _SPIDER_PLATFORM_LINUX_DEFINE_H
#define _SPIDER_PLATFORM_LINUX_DEFINE_H

///[1]包含linux特有头文件
#include <unistd.h>
#include <dlfcn.h> 
#include <strings.h>


///[2]重定义linux特有类型或补定义缺失类型
typedef		int					handle_t;

typedef		void*				module_t;


#ifndef		ftell64
#define		ftell64				ftello64
#endif

#ifndef		fseek64
#define		fseek64				fseeko64
#endif

#ifndef		localtime_s
#define		localtime_s(tm_,  tt_)	localtime_r(tt_, tm_)
#endif

#ifndef		strnicmp
#define		strnicmp	strncasecmp
#endif

#ifndef		_strnicmp
#define		_strnicmp	strncasecmp
#endif

#ifndef		_snprintf
#define		_snprintf	snprintf
#endif



#endif
