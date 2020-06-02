#ifndef MEMORYMANAGE_IPAGEPROVIDER_H
#define MEMORYMANAGE_IPAGEPROVIDER_H

#include "../utils/memoryManage_declare.h"

MM_NAMESPACE_START

FORWARD_DECLARE_PTR(Page)

class IPageProvider
{
public:
	/** return wrapped Page via directory / journal short name / serviceIdx assigned / page number */
	virtual PagePtr getPage(const string &dir, const string &jname, int serviceIdx, short pageNum) = 0;
	/** release page after using */
	virtual void releasePage(void* buffer, int size, int serviceIdx) = 0;
	/** return true if this is for writing */
	virtual bool isWriter() const = 0;
	/** destructor */
	virtual ~IPageProvider() {};
};

DECLARE_PTR(IPageProvider);

MM_NAMESPACE_END
#endif
