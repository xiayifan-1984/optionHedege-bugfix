#ifndef MEMORYMANAGE_PAGEUTIL_H
#define MEMORYMANAGE_PAGEUTIL_H

#include "../utils/memoryManage_declare.h"

MM_NAMESPACE_START

class PageHeader;

class PageUtil
{
public:
	// memory access
	/** direct memory manipulation without service
	 * load page buffer, return address of the file-mapped memory
	 *  whether to write has to be specified in "isWriting"
	 *  if quickMode==True, no locking; if quickMode==False, mlock the memory for performance
	 * the address of memory is returned */
	static void* LoadPageBuffer(const string& path, int size, bool isWriting, bool quickMode);
	/** direct memory manipulation without service
	* release page buffer, buffer and size needs to be specified.
	*  if quickMode==True, no unlocking; if quickMode==False, munlock the memory */
	static void   ReleasePageBuffer(void* buffer, int size, bool quickMode);
	//Generate proper file name for maped file
	static string GenPageFileName(const string& jname, short PageNum);
	//Generate full path for page file
	static string GenPageFullPath(const string& dir, const string& jname, short PageNum);
	//get page file name pattern
	static string GetPageFileNamePattern(const string& jname);

	//get page num for the page file
	static short ExtractPageNum(const string& fileName, const string& jname);
	/** select page number from existing pages in directory which contains the nano time */
	static short  GetPageNumWithTime(const string& dir, const string& jname, long time);
	/** get existing page numbers in directory with jname */
	static vector<short> GetPageNums(const string& dir, const string& jname);

	// header
	/** get header from necessary information */
	static PageHeader GetPageHeader(const string& dir, const string& jname, short pageNum);

	// file
	static bool FileExists(const string& filename);
};

MM_NAMESPACE_END
#endif
