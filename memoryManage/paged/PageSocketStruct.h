#ifndef MEMORYMANAGE_PAGESOCKETSTRUCT_H
#define MEMORYMANAGE_PAGESOCKETSTRUCT_H

#include "../utils/constants.h"

#define PAGED_SOCKET_FILE OPTIONHEDGE_SOCKET_FOLDER "paged.sock"

MM_NAMESPACE_START

#define SOCKET_MESSAGE_MAX_LENGTH       500 /**< max length of a socket buffer */
#define SOCKET_ERROR_MAX_LENGTH         100 /**< max length of error msg for socket */

//////////////////////////////////////////
/// (byte) PagedSocketTypeConstants
//////////////////////////////////////////
// test 0~9
#define PAGED_SOCKET_CONNECTION_TEST    0  /**< test connection, return "hello world!" */
// register / exit 10 ~ 19
#define PAGED_SOCKET_STRATEGY_REGISTER  10 /**< register strategy (has to be writer) */
#define PAGED_SOCKET_JOURNAL_REGISTER   11 /**< register journal */
#define PAGED_SOCKET_READER_REGISTER    12 /**< register client (reader) */
#define PAGED_SOCKET_WRITER_REGISTER    13 /**< register client (writer) */
#define PAGED_SOCKET_CLIENT_EXIT        19 /**< exit a client */
// subscribe 20 ~ 29
#define PAGED_SOCKET_SUBSCRIBE          20 /**< subscribe market data */
#define PAGED_SOCKET_SUBSCRIBE_TBC      21 /**< subscribe market data (to be continued)
											 * affected by SOCKET_MESSAGE_MAX_LENGTH */
											 // trade engine req 30 ~ 39
#define PAGED_SOCKET_TD_LOGIN           22 /**< login trade engine */
// timer req
#define TIMER_SEC_DIFF_REQUEST          99 /**< timer update request */

/**
 * Socket Request msg, fully utilized by "register"
 */
struct PagedSocketRequest
{
	/** PagedSocketTypeConstants */
	byte    type;
	/** name utilized for CLIENT / JOURNAL / STRATEGY */
	char    name[JOURNAL_SHORT_NAME_MAX_LENGTH];
	/** process id (only utilized when registering client) */
	int     pid;
	/** source id (only take effect when login trade engine) */
	short   source;
}__attribute__((packed));

struct PageSocketResponse
{
	/** PagedSocketTypeConstants */
	byte    type;
	/** return true if success */
	bool    success;
	/** error message if failure '\0' if success. */
	char error_msg[SOCKET_ERROR_MAX_LENGTH];
}__attribute__((packed));

struct PagedSocketRspClient :public PageSocketResponse
{
	char comm_file[JOURNAL_FOLDER_MAX_LENGTH];
	int file_size;
};

struct PagedSocketRspJournal : public PageSocketResponse
{
	int comm_idx;
};

struct PagedSocketRspStrategy : public PageSocketResponse
{
	/** start of request id */
	int     rid_start;
	/** end of request id */
	int     rid_end;
};

MM_NAMESPACE_END
#endif
