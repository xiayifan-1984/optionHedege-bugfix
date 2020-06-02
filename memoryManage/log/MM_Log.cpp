#include "MM_Log.h"

#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/loglevel.h>

#include <sstream>
#include <fstream>
#include <unistd.h>


#define LOG_CONFIGURATION_FILE OPTIONHEDGE_ROOT_DIR "configFiles/loggerConfig.properties"


USING_MM_NAMESPACE
using namespace log4cplus;

static bool configured = false;

bool Log::doConfigure(string configureFile)
{
	if (!configured)
	{
		log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(configureFile));
		configured = true;
		return true;
	}
	else
	{
		return false;
	}
	
}

LogPtr Log::getLogger(string name)
{
	return LogPtr(new Log(name));
}

Log::Log(string name)
{
	doConfigure(LOG_CONFIGURATION_FILE);
	logger = log4cplus::Logger::getInstance(name);
}
