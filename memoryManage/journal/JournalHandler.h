#ifndef MEMORYMANAGE_JOUNRALHANDLER_H
#define MEMORYMANAGE_JOUNRALHANDLER_H

/*                  |- JournalReader (read)
 * JournalHandler  -|
 *       |          |- JournalWriter (write)
 *       |
 *       |- add journal
 *       |- connect to PageEngine-service via page_provider
 */

#include "../utils/memoryManage_declare.h"

MM_NAMESPACE_START

FORWARD_DECLARE_PTR(Journal)
FORWARD_DECLARE_PTR(PageProvider)

class JournalHandler
{
protected:
	/** all page-engine-interact stuff is handled by pageProvider */
	PageProviderPtr page_provider;
	/** journals */
	vector<JournalPtr> journals;
	/** current journal */
	JournalPtr  curJournal;

public:
	/** default constructor with outside page provider */
	JournalHandler(PageProviderPtr ptr) : page_provider(ptr) {};
	/** default destructor */
	virtual ~JournalHandler();
	/** return the journal's index in the vector */
	virtual size_t addJournal(const string& dir, const string& jname);
	/** default name */
	static string getDefaultName(const string& prefix);
};

MM_NAMESPACE_END
#endif
