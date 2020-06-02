#ifndef MEMORYMANAGE_UTIL_H
#define MEMORYMANAGE_UTIL_H

#include <vector>
#include "memoryManage_declare.h"
#include <boost/locale.hpp>

MM_NAMESPACE_START

/**
 * convert gbk to utf8
 */
inline string gbk2utf8(const string& str)
{
	return boost::locale::conv::between(str, "UTF-8", "GBK");
}


MM_NAMESPACE_END
#endif