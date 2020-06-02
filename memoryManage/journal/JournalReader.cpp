#include "JournalReader.h"
#include "PageProvider.h"
#include "../utils/Timer.h"
#include <sstream>
#include <assert.h>

USING_MM_NAMESPACE

const string JournalReader::PREFIX = "reader";

JournalReader::JournalReader(PageProviderPtr ptr) :JournalHandler(ptr)
{
	journalMap.clear();
}

size_t JournalReader::addJournal(const string&dir, const string& jname)
{
	if (journalMap.find(jname) != journalMap.end())
	{
		return journalMap[jname];
	}
	else
	{
		size_t idx = JournalHandler::addJournal(dir, jname);
		journalMap[jname] = idx;
		return idx;
	}
}

JournalReaderPtr JournalReader::create(const vector<string>& dirs,const vector<string>& jnames,long startTime,const string& readerName)
{
	std::stringstream ss;
	ss << readerName << "_R";
	string clientName = ss.str();

#ifdef USE_PAGED_SERVICE
	PageProviderPtr provider = PageProviderPtr(new ClientPageProvider(clientName,false));
#else
	PageProviderPtr provider = PageProviderPtr(new LocalPageProvider(false));
#endif

	JournalReaderPtr jrp = JournalReaderPtr(new JournalReader(provider));

	assert(dirs.size() == jnames.size());

	for (size_t i = 0; i < dirs.size(); ++i)
	{
		jrp->addJournal(dirs[i], jnames[i]);
	}

	jrp->jumpStart(startTime);
	return jrp;
}

JournalReaderPtr JournalReader::create(const string&dir, const string&jname,long startTime,const string& readerName)
{
	vector<string> dirs = {dir};
	vector<string> jnames = {jname};
	return create(dirs, jnames, startTime, readerName);
}

JournalReaderPtr JournalReader::create(const vector<string>&dirs, const vector<string>&jnames, long startTime)
{
	return JournalReader::create(dirs, jnames, startTime, getDefaultName(JournalReader::PREFIX));
}

JournalReaderPtr JournalReader::create(const string& dir, const string& jname, long startTime)
{
	return JournalReader::create(dir, jname, startTime, getDefaultName(JournalReader::PREFIX));
}

JournalReaderPtr JournalReader::create(const vector<string>& dirs,
	const vector<string>& jnames,
	const vector<IJournalVisitor*>& visitors,
	long startTime,
	const string& readerName)
{
	JournalReaderPtr jrp = JournalReader::create(dirs, jnames, startTime, readerName);
	jrp->visitors = visitors;
	return jrp;
}

JournalReaderPtr JournalReader::createReaderWithSys(const vector<string>& dirs,
	const vector<string>& jnames,
	long startTime,
	const string& readerName)
{
	vector<string> ndirs(dirs.begin(), dirs.end());
	vector<string> njnames(jnames.begin(), jnames.end());
	ndirs.push_back(PAGED_JOURNAL_FOLDER);
	njnames.push_back(PAGED_JOURNAL_NAME);
	return create(ndirs, njnames, startTime, readerName);
}

JournalReaderPtr JournalReader::createSysReader(const string& clientName)
{
	return JournalReader::create(PAGED_JOURNAL_FOLDER, PAGED_JOURNAL_NAME, getNanoTime(), clientName);
}


JournalReaderPtr JournalReader::createRevisableReader(const string& readerName)
{
	std::stringstream ss;
	ss << readerName << "_SR";
	string clientName = ss.str();
#ifdef USE_PAGED_SERVICE
	PageProviderPtr provider = PageProviderPtr(new ClientPageProvider(clientName, false, true));
#else
	PageProviderPtr provider = PageProviderPtr(new LocalPageProvider(false, true));
#endif

	JournalReaderPtr jrp = JournalReaderPtr(new JournalReader(provider));
	jrp->addJournal(PAGED_JOURNAL_FOLDER, PAGED_JOURNAL_NAME);
	jrp->jumpStart(getNanoTime());

	return jrp;
}

void JournalReader::jumpStart(long startTime)
{
	for (JournalPtr& journal : journals)
	{
		journal->seekTime(startTime);
	}
}

string JournalReader::getFrameName() const
{
	if (curJournal.get() == nullptr)
	{
		return string("");
	}
	else
	{
		return curJournal->getShortName();
	}
}

void JournalReader::startVisiting()
{
	FramePtr frame;
	while (true)
	{
		frame = getNextFrame();
		if (frame.get() != nullptr)
		{
			string name = getFrameName();
			for (auto visitor : visitors)
			{
				visitor->visit(name, *frame);
			}
		}
	}
}

bool JournalReader::addVisitor(IJournalVisitor* visitor)
{
	visitors.push_back(visitor);
	return true;
}

bool JournalReader::expireJournal(size_t idx)
{
	if (idx < journals.size())
	{
		journals[idx]->expire();
		return true;
	}
	return false;
}

bool JournalReader::seekTimeJournal(size_t idx, long nano)
{
	if (idx < journals.size())
	{
		journals[idx]->seekTime(nano);
		return true;
	}
	return false;
}

bool JournalReader::expireJournalByName(const string& jname)
{
	auto it = journalMap.find(jname);
	
	if (it != journalMap.end())
	{
		return expireJournal(it->second);
	}
	else
	{
		return false;
	}
}

bool JournalReader::seekTimeJournalByName(const string&jname, long nano)
{
	auto it = journalMap.find(jname);
	if (it != journalMap.end())
	{
		return seekTimeJournal(it->second, nano);
	}
	else
	{
		return false;
	}
}

