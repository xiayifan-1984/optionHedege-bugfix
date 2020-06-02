#ifndef MM_PAGE_H
#define MM_PAGE_H

#include "../utils/memoryManage_declare.h"
#include "../utils/constants.h"
#include "FrameHeader.h"
#include "Frame.hpp"

MM_NAMESPACE_START
FORWARD_DECLARE_PTR(Page)

class Page
{
public:
	//current frame
	Frame frame;
	//address of mmap-file associated with this page
	void* const buffer;
	//current pos in page
	int position;
	//index of current frame in the page
	int frameNum;
	//num of page for the journal
	short pageNum;

	Page(void* buffer);

private:
	inline FH_TYPE_STATUS getCurStatus() const
	{
		return frame.getStatus();
	}

public:
	inline void* getBuffer() { return buffer; }

	inline short getPageNum() const { return pageNum; }

	void finishPage();
	
	void* locateWritableFrame();

	void* locateReadableFrame();

	bool isAtPageEnd() const;

	void passFrame();

	void passWrittenFrame();

	void passToTime(long time);

public:
	static PagePtr load(const string&dir, const string& jname, short pageNum, bool isWriting, bool quickMode);
};

inline bool Page::isAtPageEnd() const
{
	return getCurStatus() == JOURNAL_FRAME_STATUS_PAGE_END;
}

inline void Page::passFrame()
{
	position += frame.next();
	frameNum++;
}

inline void Page::passWrittenFrame()
{
	while (getCurStatus() == JOURNAL_FRAME_STATUS_WRITTEN)
	{
		passFrame();
	}
}

inline void Page::passToTime(long time)
{
	while (getCurStatus() == JOURNAL_FRAME_STATUS_WRITTEN && frame.getNano() < time)
	{
		passFrame();
	}
}

inline void* Page::locateWritableFrame()
{
	passWrittenFrame();
	return (getCurStatus() == JOURNAL_FRAME_STATUS_RAW && (position + PAGE_MIN_HEADROOM < JOURNAL_PAGE_SIZE)) ? frame.get_address() : nullptr;
}

inline void* Page::locateReadableFrame()
{
	return (getCurStatus() == JOURNAL_FRAME_STATUS_WRITTEN) ? frame.get_address() : nullptr;
}

MM_NAMESPACE_END
#endif
