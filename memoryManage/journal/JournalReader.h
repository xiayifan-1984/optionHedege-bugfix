#ifndef MEMORYMANAGE_JOUNRALREADER_H
#define MEMORYMANAGE_JOUNRALREADER_H

#include "JournalHandler.h"
#include "IJournalVisitor.h"
#include "Frame.hpp"
#include "journal.h"

MM_NAMESPACE_START

FORWARD_DECLARE_PTR(JournalReader);

class JournalReader :public JournalHandler
{
private:
	JournalPtr curJournal;
	vector<IJournalVisitor*> visitors;
	map<string, size_t> journalMap;
	JournalReader(PageProviderPtr ptr);

public:
	/** [usage]: next frame, and process the frame */
	FramePtr getNextFrame();
	string getFrameName() const;
	/** [usage]: keep looping and visiting */
	void startVisiting();

	/** override JournalHandler's addJournal,
	 * allow re-add journal with same name */
	virtual size_t addJournal(const string& dir, const string& jname);
	/** add visitor for "startVisiting" usage  */
	bool  addVisitor(IJournalVisitor* visitor);
	/** all journals jump to start time */
	void jumpStart(long startTime);
	/** expire one of the journal by index,
	 * return true if expire the journal successfully */
	bool  expireJournal(size_t idx);
	/** expire one of the journal by journal short name,
	 * return true if expire the journal successfully */
	bool  expireJournalByName(const string& jname);
	/** seek nano-time of a journal by index,
	 * return true if that journal exists and seeked successfully */
	bool  seekTimeJournal(size_t idx, long nano);
	/** seek nano-time of a journal by journal short name,
	 * return true if that journal exists and seeked successfully */
	bool  seekTimeJournalByName(const string& jname, long nano);

public:
	static JournalReaderPtr create(const vector<string>& dirs,
		const vector<string>& jnames,
		long startTime,
		const string& readerName);

	static JournalReaderPtr create(const vector<string>& dirs,
		const vector<string>& jnames,
		const vector<IJournalVisitor*>& visitors,
		long startTime,
		const string& readerName);

	static JournalReaderPtr create(const string& dir,
		const string& jname,
		long startTime,
		const string& readerName);

	static JournalReaderPtr create(const vector<string>& dirs,
		const vector<string>& jnames,
		long startTime);

	static JournalReaderPtr create(const string& dir,
		const string& jname,
		long startTime);

	static JournalReaderPtr createReaderWithSys(const vector<string>& dirs,
		const vector<string>& jnames,
		long startTime,
		const string& readerName);

	static JournalReaderPtr createSysReader(const string& readerName);
	/** revisable reader is a reader with authority to revise data it reads */
	static JournalReaderPtr createRevisableReader(const string& readerName);

	static const string PREFIX;
};

inline FramePtr JournalReader::getNextFrame()
{
	long minNano = TIME_TO_LAST;

	void* res_address = nullptr;
	for (JournalPtr& journal : journals)
	{
		FrameHeader* header = (FrameHeader*)(journal->locateFrame());
		if (header != nullptr)
		{
			long nano = header->nano;
			if (minNano == TIME_TO_LAST || nano < minNano)
			{
				minNano = nano;
				res_address = header;
				curJournal = journal;
			}
		}
	}

	if (res_address != nullptr)
	{
		curJournal->passFrame();
		return FramePtr(new Frame(res_address));
	}
	else
	{
		return FramePtr();
	}
}

MM_NAMESPACE_END
#endif
