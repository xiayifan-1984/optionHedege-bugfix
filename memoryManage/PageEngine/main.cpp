#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include "../utils/memoryManage_declare.h"
#include "../journal/journal.h"
#include "../log/MM_Log.h"
#include "../journal/Frame.hpp"
#include "../journal/PageHeader.h"
#include "../utils/Timer.h"
#include "../journal/Page.h"
#include "../journal/PageProvider.h"
#include "../journal/JournalWriter.h"

#include "../paged/PageEngine.h"
#include "../../system/systemConfig/sys_messages.h"
#include "../../tradingEngines/base/TradingConstant.h"
#include <unistd.h>

#include <sys/mman.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>


using namespace boost::program_options;
USING_MM_NAMESPACE
using namespace log4cplus;

using std::cout;
using std::endl;
using json = nlohmann::json;

#define TRADING_CONFIGURATION_FILE OPTIONHEDGE_ROOT_DIR "configFiles/tradingConfig.json"

int main()
{
	
	PageEngine* pe = new PageEngine();
	pe->start();

	cout << "PageEngine started" << endl;

	while (true)
	{
	}

	return 0;
}
