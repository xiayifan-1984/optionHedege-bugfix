#include "JournalWriter.h"
#include "journal.h"
#include "PageProvider.h"
#include "../utils/Timer.h"
#include "../../system/systemConfig/sys_messages.h"
#include <mutex>

USING_MM_NAMESPACE

const string JournalWriter::PREFIX = "writer";

void JournalWriter::init(const string& dir, const string& jname)
{
	addJournal(dir, jname);
	journal = journals[0];
	seekEnd();
}

void JournalWriter::seekEnd()
{
	journal->seekTime(TIME_TO_LAST);
	journal->locateFrame();
}

short JournalWriter::getPageNum() const
{
	return journal->getCurPageNum();
}

long JournalWriter::writeStr(const string& str)
{
	return 0l;
}

long JournalWriter::write_frame_full(const void* data, FH_TYPE_LENGTH length, FH_TYPE_SOURCE source, FH_TYPE_MSG_TP msgType,
	FH_TYPE_LASTFG lastFlag, FH_TYPE_REQ_ID requestId, FH_TYPE_NANOTM extraNano,
	FH_TYPE_ERR_ID errorId, const char* errorMsg)
{
	std::lock_guard<std::mutex> lock(mtx);
	void* buffer = journal->locateFrame();
	Frame frame(buffer);
	frame.setSource(source);
	frame.setMsgType(msgType);
	frame.setLastFlag(lastFlag);
	frame.setRequestId(requestId);
	frame.setErrorData(errorId, errorMsg, data, length);
	frame.setExtraNano(extraNano);
	long nano = getNanoTime();
	frame.setNano(nano);
	frame.setStatusWritten();
	journal->passFrame();
	return nano;
}

JournalWriterPtr JournalWriter::create(const string& dir, const string& jname, const string& writerName)
{
#ifdef USE_PAGED_SERVICE
	PageProviderPtr provider = PageProviderPtr(new ClientPageProvider(writerName,true));
#else
	PageProviderPtr provider = PageProviderPtr(new LocalPageProvider(true));
#endif

	return JournalWriter::create(dir, jname, provider);
}

JournalWriterPtr JournalWriter::create(const string& dir, const string& jname, PageProviderPtr provider)
{
	JournalWriterPtr jwp = JournalWriterPtr(new JournalWriter(provider));
	jwp->init(dir, jname);
	return jwp;
}

JournalWriterPtr JournalWriter::create(const string& dir, const string& jname)
{
	return JournalWriter::create(dir, jname, getDefaultName(JournalWriter::PREFIX));
}

