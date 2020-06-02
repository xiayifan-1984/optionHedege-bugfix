#include "JournalHandler.h"
#include "PageProvider.h"
#include "journal.h"
#include "../utils/Timer.h"
#include <sstream>

USING_MM_NAMESPACE

string JournalHandler::getDefaultName(const string& prefix)
{
	std::stringstream ss;
	ss << prefix << "_" << getNanoTime();
	return ss.str();
}

size_t JournalHandler::addJournal(const string& _dir, const string&  jname)
{
	// directory should not contain '/' in the back
	string dir;

	if (_dir.back() == '/')
	{
		dir = _dir.substr(0, _dir.length() - 1);
	}
	else
	{
		dir = _dir;
	}

	int service_idx = page_provider->register_journal(dir, jname);
	journals.push_back(Journal::create(dir, jname, service_idx, page_provider));
	return journals.size() - 1;
}

JournalHandler::~JournalHandler()
{
	page_provider->exit_client();
}


