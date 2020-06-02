#include "../MD/MDEngineCTP.h"

USING_TE_NAMESPACE


#define CONFIGURATION_JSON_FILE OPTIONHEDGE_ROOT_DIR "configFiles/tradingConfig.json"

int main()
{
	MDEngineCTP* md = new MDEngineCTP();
	
	md->initialize(CONFIGURATION_JSON_FILE);

	md->start();

	while (true)
	{

	}

	return 0;
}
