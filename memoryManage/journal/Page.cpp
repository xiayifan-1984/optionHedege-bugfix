#include "Page.h"
#include "PageHeader.h"
#include "PageUtil.h"
#include "../utils/Timer.h"
#include <sstream>
#include <iostream>

USING_MM_NAMESPACE

#define PAGE_INIT_POSITION sizeof(PageHeader)

Page::Page(void *buffer) :frame(ADDRESS_ADD(buffer, PAGE_INIT_POSITION)), buffer(buffer), position(PAGE_INIT_POSITION), frameNum(0), pageNum(-1) {}

void Page::finishPage()
{
	PageHeader* header = (PageHeader*)buffer;
	header->close_nano = getNanoTime();
	header->frame_num = frameNum;
	header->last_pos = position;
	frame.setStatusPageClosed();
}

PagePtr Page::load(const string& dir, const string& jname, short pageNum, bool isWriting, bool quickMode)
{
	string path = PageUtil::GenPageFullPath(dir, jname, pageNum);
	void* buffer = PageUtil::LoadPageBuffer(path, JOURNAL_PAGE_SIZE, isWriting, quickMode);
	
	if (buffer == nullptr)
	{
		return PagePtr();
	}

	PageHeader* header = (PageHeader*)buffer;
	if (header->status == JOURNAL_PAGE_STATUS_RAW)
	{
		//Journal reader can not read an raw page
		if (!isWriting)
		{
			return PagePtr();
		}
		memcpy(header->journal_name, jname.c_str(),jname.length()+1);
		header->start_nano = getNanoTime();
		header->close_nano = -1;
		header->page_num = pageNum;
		header->status = JOURNAL_PAGE_STATUS_INITED;
		header->frame_version = __FRAME_HEADER_VERSION__;
	}

	else if (header->frame_version > 0 && header->frame_version != __FRAME_HEADER_VERSION__)
	{
		std::stringstream ss;
		ss << "page version mismatch: (program)" << __FRAME_HEADER_VERSION__ << " (page)" << header->frame_version;
		throw std::runtime_error(ss.str().c_str());
	}

	PagePtr page = PagePtr(new Page(buffer));
	page->pageNum = pageNum;
	return page;
}

