#ifndef MEMORYMANAGE_PAGEENGINE_H
#define MEMORYMANAGE_PAGEENGINE_H

#include "PageCommStruct.h"
#include "../log/MM_Log.h"
#include "PageSocketHandler.h"
#include "../journal/JournalWriter.h"
#include "../../tradingEngines/base/EngineUtil.hpp"
#include <iostream>
#include <fstream>
#include "../utils/json.hpp"

#include <utility>
#include <thread>

MM_NAMESPACE_START

typedef boost::shared_ptr<std::thread> ThreadPtr;

/** we call each journal handler (writer or reader)
 *      -- a client for page engine.
 *  we call each "journal" linked by client
 *      -- a user for page engine.
 *  then each writer client may only have 1 user,
 *  while each reader client may have several users.
 *  all the necessary information are stored here.
 */
struct PageClientInfo
{
	/** the index of each user linked by this client */
	vector<int> user_index_vec;
	/** register nano time */
	long  reg_nano;
	/** process id */
	int   pid;
	/** hash code for the client */
	int   hash_code;
	/** true if this client is a writer */
	bool  is_writing;
	/** true if this writer is associated with a strategy */
	bool  is_strategy;
	/** start rid of the strategy (strategy only) */
	int   rid_start;
	/** end rid of the strategy (strategy only) */
	int   rid_end;
	/** all sources of trade engine that registered (strategy only) */
	vector<short> trade_engine_vec;
};

class PageEngine :public IPageSocketUtil
{
private:
	// internal data structures. be careful on its thread-safety
	/** map: client -> all info (all journal usage) */
	map<string, PageClientInfo> clientJournals;
	/** map: pid -> client */
	map<int, vector<string>> pidClient;
	/** map: file attached with number of writers */
	map<PageCommMsg, int> fileWriterCounts;
	/** map: file attached with number of readers */
	map<PageCommMsg, int> fileReaderCounts;
	/** map: file to its page buffer */
	map<string, void*> fileAddrs;

public:
	PageEngine();
	virtual ~PageEngine();

	void start();
	void stop();

	/** set task frequency in seconds, default 1 second */
//	void set_freq(double second_interval);

	/** write string content to system journal */
	bool write(string content, byte msg_type, bool is_last = true, short source = 0);

public:
	LogPtr get_logger() const { return logger; }
	int reg_journal(const string& clientName);
	IntPair register_strategy(const string& strategyName);
	bool reg_client(string & commFile, int& fileSize, const string&clientName, int pid, bool isWriter);
	void exit_client(const string& clientName);
	bool sub_md(const vector<string>& ticker, short source, short msgType, bool isLast);
	bool login_td(const string& clientName, short source);
	void acquire_mutex() const;
	void release_mutex() const;
	int getRequestidIndex(string name,const std::string& configFile, short source);
private:
	JournalWriterPtr writer;
	LogPtr logger;
	void* commBuffer;
	string commFile;
	size_t maxIdx;
	int microsecFreq;
	bool task_running;
//	long last_switch_nano;
	volatile bool comm_running;

	ThreadPtr taskThread;
	ThreadPtr commThread;
	ThreadPtr socketThread;

private:
	void start_comm();
	void start_socket();
//	void start_task();

	/** release the page assigned in comm msg */
	void release_page(const PageCommMsg& msg);
	/** initialize the page assigned in comm msg */
	byte initiate_page(const PageCommMsg& msg);
};


MM_NAMESPACE_END
#endif
