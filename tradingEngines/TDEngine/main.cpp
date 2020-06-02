#include "../TD/TDEngineCTP.h"

USING_TE_NAMESPACE


#define CONFIGURATION_JSON_FILE OPTIONHEDGE_ROOT_DIR "configFiles/tradingConfig.json"

int main()
{
	TDEngineCTP* td = new TDEngineCTP();

	td->initialize(CONFIGURATION_JSON_FILE);

	td->start();

	while (true)
	{

	}

	return 0;
}
