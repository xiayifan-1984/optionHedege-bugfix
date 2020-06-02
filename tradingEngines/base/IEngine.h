#ifndef TRADEINGENGINE_IENGINE_H
#define TRADEINGENGINE_IENGINE_H

#include "TE_DECLEAR.h"
#include "../../memoryManage/journal/JournalReader.h"
#include "../../memoryManage/journal/JournalWriter.h"
#include "EngineUtil.hpp"
#include "../../memoryManage/log/MM_Log.h"
#include "../../memoryManage/utils/json.hpp"

using json = nlohmann::json;

#define CONNECT_TIMEOUT_NANO_SECONDS 5000000000 //5s

TE_NAMESPACE_START

using optionHedge::memoryManage::JournalReaderPtr;
using optionHedge::memoryManage::JournalWriterPtr;
using optionHedge::memoryManage::LogPtr;

class IEngine
{
protected:
	/** source identifier */
	const short source_id;
	/** reader, keep reading system journal */
	JournalReaderPtr reader;
	/** writer, output. */
	JournalWriterPtr writer;
	/** logger. */
	LogPtr logger;
	/** flag of reader_thread */
	bool isRunning;
	/** reader thread with reader keep reading */
	ThreadPtr reader_thread;

protected:
	IEngine(short source_id);
	virtual ~IEngine();
	/** try to connect and login, return login status */
	bool try_login();
	/* signal received */
	static volatile int signal_received;
	/* signal handler */
	static void signal_handler(int signum) { signal_received = signum; exit(signum);}

public:
	/** initialize engine, pass-in parameters as json format
	 * internally call init virtual function to further setup */
	void initialize(const string& json_str);
	/** engine start, entrance */
	bool start();
	/** officially stop all threading */
	bool stop();
	/** block main thread and waiting for reader-thread */
	void wait_for_stop();
	/** on engine service open */
	void on_engine_open();
	/** on engine service close */
	void on_engine_close();
	/** on engine other command in string */
	virtual void on_command(const string& cmd) {};
	/** after login pre-run something before start service*/
	virtual void pre_run() {};
	/** before load config information, pre_load */
	virtual void pre_load(const json& j_config) {};
public:
	// virtual functions that needs engines to implement.
	/** set reader logic to keep listening */
	virtual void set_reader_thread() = 0;
	/** init writer, reader, api, etc.
	  * will throw exception if error. */
	virtual void init() = 0;
	/** load necessary config information
	  * throw exception if some data is missed. */
	virtual void load(const json& j_config) = 0;
	/** use api to connect to front */
	virtual void connect(long timeout_nsec) = 0;
	/** use api to log in account */
//	virtual void login(long timeout_nsec) = 0;
	/** use api to log out */
	virtual void logout() = 0;
	/** release api*/
	virtual void release_api() = 0;
	/** return true if engine connected to server */
	virtual bool is_connected() const = 0;
	/** return true if all accounts have been logged in */
	virtual bool is_logged_in() const = 0;
	/** get engine's name */
	virtual string name() const = 0;
};

DECLARE_PTR(IEngine);

TE_NAMESPACE_END
#endif
