#ifndef MEMORYMANAGE_FRAME_HPP
#define MEMORYMANAGE_FRAME_HPP

#include "../utils/memoryManage_declare.h"
#include "FrameHeader.h"
#include <string.h>

MM_NAMESPACE_START

class Frame
{
private:
	FrameHeader *frame;
	
public:
	Frame(void*);
	
	void set_address(void*);
	void* get_address() const;

	// get fields of header from frame
	FH_TYPE_STATUS  getStatus() const;
	FH_TYPE_SOURCE  getSource() const;
	FH_TYPE_NANOTM  getNano() const;
	FH_TYPE_LENGTH  getFrameLength() const;
	FH_TYPE_LENGTH  getHeaderLength() const;
	FH_TYPE_LENGTH  getDataLength() const;
	FH_TYPE_MSG_TP  getMsgType() const;
	FH_TYPE_LASTFG  getLastFlag() const;
	FH_TYPE_REQ_ID  getRequestId() const;
	FH_TYPE_NANOTM  getExtraNano() const;
	FH_TYPE_ERR_ID  getErrorId() const;

	/** return null if errorId == 0 */
	char* getErrorMsg() const;
	/** get address of data field */
	void* getData() const;
	/** parse data content as string with char */
	string getStr() const;
	// set fields of header
	void setSource(FH_TYPE_SOURCE);
	void setNano(FH_TYPE_NANOTM);
	void setExtraNano(FH_TYPE_NANOTM);
	void setMsgType(FH_TYPE_MSG_TP);
	void setLastFlag(FH_TYPE_LASTFG);
	void setRequestId(FH_TYPE_REQ_ID);

	/** set error msg with its length and errorid */
	void setErrorData(FH_TYPE_ERR_ID, const char* errorMsg, const void* data, size_t dataLen);

	/** set data with length */
	void setData(const void*, size_t);
	/** mark status as written */
	void setStatusWritten();
	/** mark status as page closed */
	void setStatusPageClosed();
	/** move the frame forward by length */
	FH_TYPE_LENGTH next();

private:
	/** return address of next frame header */
	FrameHeader* getNextEntry() const;
	/** set status, internally used */
	void setStatus(FH_TYPE_STATUS);
	/** set length, internally used */
	void setFrameLength(FH_TYPE_LENGTH);
};

DECLARE_PTR(Frame);

inline void Frame::set_address(void* fm)
{
	frame = (FrameHeader*)fm;
}

inline Frame::Frame(void* fm)
{
	set_address(fm);
}

inline void* Frame::get_address() const
{
	return frame;
}

inline FH_TYPE_LENGTH Frame::getHeaderLength() const
{
	if (getErrorId() == 0)
	{
		return BASIC_FRAME_HEADER_LENGTH;
	}
	else
	{
		return ERROR_FRAME_HEADER_LENGTH;
	}
}

inline FH_TYPE_STATUS Frame::getStatus() const
{
	return frame->status;
}

inline FH_TYPE_SOURCE Frame::getSource() const
{
	return frame->source;
}

inline FH_TYPE_NANOTM Frame::getNano() const
{
	return frame->nano;
}

inline FH_TYPE_LENGTH Frame::getFrameLength() const
{
	return frame->length;
}

inline FH_TYPE_MSG_TP Frame::getMsgType() const
{
	return frame->msg_type;
}

inline FH_TYPE_LASTFG Frame::getLastFlag() const
{
	return frame->last_flag;
}

inline FH_TYPE_REQ_ID Frame::getRequestId() const
{
	return frame->req_id;
}

inline FH_TYPE_NANOTM Frame::getExtraNano() const
{
	return frame->extra_nano;
}

inline FH_TYPE_ERR_ID Frame::getErrorId() const
{
	return frame->err_id;
}

inline char* Frame::getErrorMsg() const
{
	if (getErrorId() == 0)
	{
		return nullptr;
	}
	else
	{
		return (char*)ADDRESS_ADD(frame, BASIC_FRAME_HEADER_LENGTH);
	}
}

inline FH_TYPE_LENGTH Frame::getDataLength() const
{
	return getFrameLength() - getHeaderLength();
}

inline void* Frame::getData() const
{
	return ADDRESS_ADD(frame, getHeaderLength());
}

inline string Frame::getStr() const
{
	return string((char*)getData());
}

inline void Frame::setStatus(FH_TYPE_STATUS status)
{
	frame->status = status;
}

inline void Frame::setSource(FH_TYPE_SOURCE source)
{
	frame->source = source;
}

inline void Frame::setNano(FH_TYPE_NANOTM nano)
{
	frame->nano = nano;
}

inline void Frame::setFrameLength(FH_TYPE_LENGTH length)
{
	frame->length = length;
}

inline void Frame::setMsgType(FH_TYPE_MSG_TP msgType)
{
	frame->msg_type = msgType;
}

inline void Frame::setLastFlag(FH_TYPE_LASTFG lastFlag)
{
	frame->last_flag = lastFlag;
}

inline void Frame::setRequestId(FH_TYPE_REQ_ID requestID)
{
	frame->req_id = requestID;
}

inline void Frame::setExtraNano(FH_TYPE_NANOTM extraNano)
{
	frame->extra_nano = extraNano;
}

inline void Frame::setErrorData(FH_TYPE_ERR_ID errorID, const char* errorMsg, const void* data, size_t dataLength)
{
	if (errorID == 0)
	{
		setData(data, dataLength);
	}
	else
	{
		frame->err_id = errorID;
		char* errorMsgStartAddr = (char*)ADDRESS_ADD(frame, BASIC_FRAME_HEADER_LENGTH);
		if (errorMsg != nullptr)
		{
			strncpy(errorMsgStartAddr, errorMsg, ERROR_MSG_MAX_LENGTH);
		}
		memcpy(ADDRESS_ADD(frame, ERROR_FRAME_HEADER_LENGTH), data, dataLength);
		setFrameLength(ERROR_FRAME_HEADER_LENGTH + dataLength);
	}
}

inline void Frame::setData(const void* data,size_t dataLength)
{
	frame->err_id = 0;
	memcpy(ADDRESS_ADD(frame,BASIC_FRAME_HEADER_LENGTH),data,dataLength);
	setFrameLength(BASIC_FRAME_HEADER_LENGTH + dataLength);
}

inline void Frame::setStatusWritten()
{
	getNextEntry()->status = JOURNAL_FRAME_STATUS_RAW;
	setStatus(JOURNAL_FRAME_STATUS_WRITTEN);
}

inline void Frame::setStatusPageClosed()
{
	frame->status = JOURNAL_FRAME_STATUS_PAGE_END;
}

inline FH_TYPE_LENGTH Frame::next()
{
	FH_TYPE_LENGTH len = getFrameLength();
	frame = getNextEntry();
	return len;
}

inline FrameHeader* Frame::getNextEntry() const
{
	return (FrameHeader*)ADDRESS_ADD(frame, getFrameLength());
}
MM_NAMESPACE_END
#endif
