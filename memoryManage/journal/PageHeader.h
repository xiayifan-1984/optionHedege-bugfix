#ifndef MEMORYMANAGE_PAGEHEADER_H
#define MEMORYMANAGE_PAGEHEADER_H

#include "../utils/memoryManage_declare.h"
#include "../utils/constants.h"

MM_NAMESPACE_START

/** reserve space for page header (long) */
#define PAGE_HEADER_RESERVE 10
//////////////////////////////////////////
/// (byte) JournalPageStatus
//////////////////////////////////////////
#define JOURNAL_PAGE_STATUS_RAW     0
#define JOURNAL_PAGE_STATUS_INITED  1

struct PageHeader
{
	//JournalPageStatus
	byte status;
	//Journal name
	char journal_name[JOURNAL_SHORT_NAME_MAX_LENGTH];
	//number of this page in journal
	short page_num;
	//nano time of page started
	long start_nano;
	//nano time of page closed
	long close_nano;
	//how many frame in this page,filled when closed
	int frame_num;
	//pos of last frame
	int last_pos;
	short frame_version;
	//reserved space
	short reserve_short[3];
	long  reserve_long[PAGE_HEADER_RESERVE-1];
}__attribute__((packed));

enum PageStatus
{
	PAGE_RAW = 0,
	PAGE_INITED = 1,
};

MM_NAMESPACE_END
#endif
