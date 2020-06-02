#ifndef MEMORYMANAGE_JOURNAL_H
#define MEMORYMANAGE_JOURNAL_H

#include "../utils/memoryManage_declare.h"
#include "Page.h"

MM_NAMESPACE_START

FORWARD_DECLARE_PTR(Journal);
FORWARD_DECLARE_PTR(IPageProvider);

/**
 * Journal class, the abstraction of continuous memory access
 */
class Journal
{
public:
	/** page provider for further page acquire / release */
	IPageProviderPtr pageProvider;
	/** service index is allocated by provider, and will be utilized when asking page provider */
	int     serviceIdx;
	/** basic information, directory path */
	string  directory;
	/** basic information, journal short name */
	string  shortName;
	/** !!for performance only, cache pageProvider->isWriter() */
	bool    isWriting;
	/** flag to determine whether read its frame */
	bool    expired;
	/** current page in use */
	PagePtr curPage;
	/** private constructor, make create the only builder */
	Journal() : expired(false) {};
public:
	/** the only entrance of creating a Journal */
	static JournalPtr create(const string& dir, const string& jname, int serviceIdx, IPageProviderPtr provider);
	/** expire this journal, won't get any frame until reset by seekTime */
	void    expire();
	/** seek to time in nanoseconds-timestamp */
	void    seekTime(long time);
	/** get frame address return nullptr if no available */
	void*   locateFrame();
	/** move forward to next frame */
	void    passFrame();
	/** load next page, current page will be released if not empty */
	void    loadNextPage();
	/** get current page number */
	short   getCurPageNum() const;
	/** get journal short name */
	string  getShortName() const;

	/*testing function,can be remove*/
	PagePtr getCurPage() { return curPage; }
};

inline void* Journal::locateFrame()
{
	if (expired)
	{
		return nullptr;
	}

	if (isWriting)
	{
		void* frame = curPage->locateWritableFrame();
		while (frame == nullptr)
		{
			loadNextPage();
			frame = curPage->locateWritableFrame();
		}

		return frame;
	}
	else
	{
		if ((curPage.get() != nullptr && curPage->isAtPageEnd()) || curPage.get() == nullptr)
		{
			loadNextPage();
		}
		if (curPage.get() != nullptr)
		{
			return curPage->locateReadableFrame();
		}
	}
}

inline void Journal::passFrame()
{
	curPage->passFrame();
}

inline short Journal::getCurPageNum() const
{
	return curPage->getPageNum();
}

inline string Journal::getShortName() const
{
	return shortName;
}


MM_NAMESPACE_END
#endif
