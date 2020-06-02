#ifndef MM_LOG_H
#define MM_LOG_H

#include "../utils/memoryManage_declare.h"

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#define OPTIONHEDGE_LOG_FATAL(ohLogger, content) LOG4CPLUS_FATAL(ohLogger->getLogger(), content)
#define OPTIONHEDGE_LOG_ERROR(ohLogger, content) LOG4CPLUS_ERROR(ohLogger->getLogger(), content)
#define OPTIONHEDGE_LOG_INFO(ohLogger, content) LOG4CPLUS_INFO(ohLogger->getLogger(), content)
#define OPTIONHEDGE_LOG_DEBUG(ohLogger, content) LOG4CPLUS_DEBUG(ohLogger->getLogger(), content)

#define OPTIONHEDGE_LOG_FATAL_FMT(ohLogger, content, ...) LOG4CPLUS_FATAL_FMT(ohLogger->getLogger(), content, ##__VA_ARGS__)
#define OPTIONHEDGE_LOG_ERROR_FMT(ohLogger, content, ...) LOG4CPLUS_ERROR_FMT(ohLogger->getLogger(), content, ##__VA_ARGS__)
#define OPTIONHEDGE_LOG_INFO_FMT(ohLogger, content, ...) LOG4CPLUS_INFO_FMT(ohLogger->getLogger(), content, ##__VA_ARGS__)
#define OPTIONHEDGE_LOG_DEBUG_FMT(ohLogger, content, ...) LOG4CPLUS_DEBUG_FMT(ohLogger->getLogger(), content, ##__VA_ARGS__)

MM_NAMESPACE_START
FORWARD_DECLARE_PTR(Log)

class Log
{
protected:
	log4cplus::Logger logger;

	Log() {}
	Log(string name);

public:
	inline log4cplus::Logger& getLogger() { return logger; }
	inline void fatal(const char* message) 
	{
		LOG4CPLUS_FATAL(logger, message);
	}

	inline void error(const char* message)
	{
		LOG4CPLUS_ERROR(logger, message);
	}

	inline void info(const char* message)
	{
		LOG4CPLUS_INFO(logger, message);
	}
	inline void debug(const char* message)
	{
		LOG4CPLUS_DEBUG(logger, message);
	}

	static bool doConfigure(string configureName);

	static LogPtr getLogger(string name);
};
MM_NAMESPACE_END
#endif
