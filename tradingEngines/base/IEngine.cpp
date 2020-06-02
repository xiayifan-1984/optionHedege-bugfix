#include "IEngine.h"
#include "../../system/systemConfig/sys_messages.h"
#include "../../memoryManage/utils/util.hpp"
#include <unistd.h>
#include <csignal>
#include <fstream>

USING_TE_NAMESPACE

#define WRITE_ENGINE_STATUS(name) \
    if (writer.get() != nullptr) \
    {\
        writer->write_frame(name, sizeof(name), source_id, MSG_TYPE_ENGINE_STATUS, 1, getpid());\
    }


volatile int IEngine::signal_received = -1;

IEngine::IEngine(short source) :source_id(source), isRunning(false)
{

}

IEngine::~IEngine()
{
	stop();
}

bool IEngine::start()
{
	isRunning = false;
	if (try_login())
	{
		pre_run();
		isRunning = true;
		if(reader.get() == nullptr)
		{
			throw std::runtime_error("reader is not initiated,call init before start");
		}

		std::signal(SIGTERM, IEngine::signal_handler);
		std::signal(SIGINT, IEngine::signal_handler);
		std::signal(SIGHUP, IEngine::signal_handler);
		std::signal(SIGQUIT, IEngine::signal_handler);
		std::signal(SIGKILL, IEngine::signal_handler);
		set_reader_thread();
	}
	return isRunning;
}

bool IEngine::stop()
{
	isRunning = false;
	if (reader_thread.get() != nullptr)
	{
		reader_thread->join();
		reader_thread.reset();
		OPTIONHEDGE_LOG_INFO(logger, "exit reader thread........");
		WRITE_ENGINE_STATUS(TE_ENGINE_STATUS_STOPPED);
		return true;
	}
	WRITE_ENGINE_STATUS(TE_ENGINE_STATUS_STOP_FAIL);
	return false;
}

bool IEngine::try_login()
{
	WRITE_ENGINE_STATUS(TE_ENGINE_STATUS_CONNECTING);

	if (!is_connected())
	{
		connect(CONNECT_TIMEOUT_NANO_SECONDS);
	}
	WRITE_ENGINE_STATUS(is_connected() ? TE_ENGINE_STATUS_CONNECTED : TE_ENGINE_STATUS_CONNECT_FAIL);
	/*
	if (is_connected() && !is_logged_in())
	{
		login(CONNECT_TIMEOUT_NANO_SECONDS);
	}*/
	sleep(30);
	WRITE_ENGINE_STATUS(is_logged_in() ? TE_ENGINE_STATUS_LOGIN_SUCCESS : TE_ENGINE_STATUS_LOGIN_FAIL);
	return is_logged_in();
}

void IEngine::on_engine_open()
{
	OPTIONHEDGE_LOG_INFO(logger, "[CMD] engine open service");
	if (is_logged_in())
	{
		OPTIONHEDGE_LOG_INFO(logger, "Already running");
	}

	else
	{
		if (try_login())
		{
			pre_run();
			OPTIONHEDGE_LOG_INFO(logger, "restart");
		}
		else
		{
			OPTIONHEDGE_LOG_INFO(logger, "fail to restart");
		}
	}
}

void IEngine::on_engine_close()
{
	OPTIONHEDGE_LOG_INFO(logger, "[CMD] ENGINE CLOSE SERVICE!");
	logout();
	release_api();
	WRITE_ENGINE_STATUS(TE_ENGINE_STATUS_IDLE);
}

void IEngine::wait_for_stop()
{
	if (reader_thread.get() != nullptr)
	{
		reader_thread->join();
		reader_thread.reset();
	}
}

void IEngine::initialize(const string& config_file)
{
	init();
	WRITE_ENGINE_STATUS(TE_ENGINE_STATUS_IDLE);
	std::ifstream ifs(config_file);
	json j_config = json::parse(ifs);
	for (json::const_iterator it = j_config.begin(); it != j_config.end(); ++it)
	{
		OPTIONHEDGE_LOG_INFO(logger, "IEngine::init " << it.key() << "->" << it.value().dump());
	}

	// pre_load process is mainly for IMDEngine or ITDEngine,
   // engine-wise control information loading, sometimes needs to be loaded before.
	pre_load(j_config);
	// load config information
	load(j_config);
}

static string utf8_error_msg;

const char* EngineUtil::gbkErrorMsg2utf8(const char* errorMsg)
{
	if (errorMsg == nullptr)
	{
		return nullptr;
	}
	utf8_error_msg = optionHedge::memoryManage::gbk2utf8(string(errorMsg));
	return utf8_error_msg.c_str();
}
