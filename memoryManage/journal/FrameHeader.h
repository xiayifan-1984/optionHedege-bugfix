#define __FRAME_HEADER_VERSION__ 2

#ifndef MEMORYMANAGE_FRAMEHEARDER_H
#define MEMORYMANAGE_FRAMEHEARDER_H

#include "../utils/constants.h"

MM_NAMESPACE_START
#define ERROR_MSG_MAX_LENGTH 100

// data types of all fields in frame header
// which need to be fully controlled for data length.
typedef byte    FH_TYPE_STATUS;
typedef short   FH_TYPE_SOURCE;
typedef long    FH_TYPE_NANOTM;
typedef int     FH_TYPE_LENGTH;
typedef uint    FH_TYPE_HASHNM;
typedef short   FH_TYPE_MSG_TP;
typedef byte    FH_TYPE_LASTFG;
typedef int     FH_TYPE_REQ_ID;
typedef int     FH_TYPE_ERR_ID;

//////////////////////////////////////////
/// (byte) JournalFrameStatus
//////////////////////////////////////////
#define JOURNAL_FRAME_STATUS_RAW        0
#define JOURNAL_FRAME_STATUS_WRITTEN    1
#define JOURNAL_FRAME_STATUS_PAGE_END   2

//////////////////////////////////////////
/// (byte) JournalFrameLastFlag
//////////////////////////////////////////
#define JOURNAL_FRAME_NOT_LAST          0 /**< false */
#define JOURNAL_FRAME_IS_LAST           1 /**< true */

struct FrameHeader
{
	/** JournalFrameStatus */
	volatile FH_TYPE_STATUS status;
	/** source (system id) of this frame */
	FH_TYPE_SOURCE      source;
	/** nano time of the frame data */
	FH_TYPE_NANOTM      nano;
	/** total frame length (including header and errorMsg) */
	FH_TYPE_LENGTH      length;
	/** hash code of data part (not whole frame) */
	FH_TYPE_HASHNM      hash;
	/** msg type of the data in frame */
	FH_TYPE_MSG_TP      msg_type;
	/** JournalFrameLastFlag */
	FH_TYPE_LASTFG      last_flag;
	/** request id of this frame data */
	FH_TYPE_REQ_ID      req_id;
	FH_TYPE_NANOTM      extra_nano;
	/** error id
	 * =0 if not error and no errorMsg,
	 * !=0 means some info in errorMsg */
	FH_TYPE_ERR_ID      err_id;
}__attribute__((packed));

/** length of frame header */
const FH_TYPE_LENGTH BASIC_FRAME_HEADER_LENGTH = sizeof(FrameHeader);

/** length of frame header with error msg */
const FH_TYPE_LENGTH ERROR_FRAME_HEADER_LENGTH = BASIC_FRAME_HEADER_LENGTH + sizeof(char) * ERROR_MSG_MAX_LENGTH;
MM_NAMESPACE_END
#endif
