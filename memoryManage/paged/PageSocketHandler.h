#ifndef MEMORYMANAGE_PAGESOCKETHANDLER_H
#define MEMORYMANAGE_PAGESOCKETHANDLER_H

#include "../utils/memoryManage_declare.h"
#include "../utils/json.hpp"
#include "../log/MM_Log.h"
#include "PageSocketStruct.h"
#include <boost/enable_shared_from_this.hpp>

MM_NAMESPACE_START

using json = nlohmann::json;

/** utilities for socket usage */
class IPageSocketUtil
{
public:
	/** return logger */
	virtual LogPtr get_logger() const = 0;
	/** return journal index in comm file */
	virtual int reg_journal(const string& clientName) = 0;
	/** return (rid_start, rid_end) */
	virtual IntPair register_strategy(const string& strategyName)=0;
	/** return true if this client exists, and fill in commfile, size of commfile */
	virtual bool reg_client(string& commFile, int& fileSize,const string& clientName, int pid, bool isWriter) = 0;
	/** exit client */
	virtual void exit_client(const string& clientName) = 0;
	/** subscribe market data */
	virtual bool sub_md(const vector<string>& tickers, short source, short msgType, bool isLast) = 0;
	/** login trade engine */
	virtual bool login_td(const string& clientName, short source) = 0;
	/** acquire mutex */
	virtual void    acquire_mutex() const = 0;
	/** release the mutex */
	virtual void    release_mutex() const = 0;
};

/** socket handler for page engine */
class PageSocketHandler :public boost::enable_shared_from_this<PageSocketHandler>
{
private:
	/** flag for io running */
	bool io_running;
	/** logger, from paged */
	LogPtr logger;
	/** util as page engine */
	IPageSocketUtil* util;
	/*singleton*/
	static boost::shared_ptr<PageSocketHandler> m_ptr;

private:
	/** callback when accept new msg*/
	void handle_accept();
	/** minor unit for msg processing */
	void process_msg();
	/** private constructor for singleton */
	PageSocketHandler();

public:
	/** start run with page engine */
	void run(IPageSocketUtil* _util);
	/** stop io service and collect resource*/
	void stop();
	/** return true if socket io is running */
	bool is_running();
	/** singleton */
	static PageSocketHandler* getInstance();
};

MM_NAMESPACE_END

#endif
