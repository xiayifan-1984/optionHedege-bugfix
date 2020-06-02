#ifndef _SPIDER_PLATFORM_DEFINE_H
#define _SPIDER_PLATFORM_DEFINE_H

#define  LINUX



//[1]base include
#include <stdio.h>


//[2]specific platform
#if defined(WINDOWS) || defined(WIN32)
	#include "Platform_window.h"

#elif defined(LINUX)
	#include "Platform_linux.h"

#endif


//[3]common define
#define			UNUSED(e)				e



#endif
