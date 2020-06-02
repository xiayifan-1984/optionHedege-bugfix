

#include <stdio.h>
#include <stdlib.h>

#include "./util/Platform.h"

#include <unistd.h>
#include <signal.h>


#include "./handler/config.h"
#include "./handler/timesection.h"
#include "./handler/tdhandler.h"
//#include "twapcontext.h"

bool	global_exit = false;

void signalhandler(int signal)
{
	printf("SIGINT receive\n");
	global_exit = true;
}

int main(int argc, char* argv[])
{
	printf("config module start\n");
	//[1]
	CConfig* pConfig = new CConfig();
	int iret = pConfig->Instance();
	if (iret <0)
	{
		printf("Config Instance failed\n");
		return -1;
	}
	else
	{
		printf("Config Instance success\n");
	}
	
	//[2]
	char szTimeFile[255] = {0};
	pConfig->GetTimeSectionFile(szTimeFile);
	CTimesection* pTimeSection = new CTimesection();
	iret = pTimeSection->Instance(szTimeFile);
	if (iret <0)
	{
		printf("TimeSection Instance failed\n");
		return -2;
	}
	else
	{
		printf("TimeSection Instance success\n");
	}
	
	//[3]
	printf("ctphandler module start\n");
	tdhandler* pHandler = new tdhandler();
	iret = pHandler->Instance(pConfig, pTimeSection);
	if (iret <0)
	{
		return -2;
	}

	//[4]
	sigset( SIGINT, signalhandler );

	//[5]
	while(!global_exit)
	{
		usleep(250);
		pHandler->handleTwapOrder();
	}

	//[6]
	printf("ctphandler module end\n");
	if(pHandler)
	{
		pHandler->Release();
		delete pHandler;
		pHandler = 0;
	}

	//[7]
	printf("timesection module end\n");
	if(pTimeSection)
	{
		pTimeSection->Release();
		delete pTimeSection;
		pTimeSection =0;
	}

	//[8]
	printf("config module end\n");
	if(pConfig)
	{
		pConfig->Release();
		delete pConfig;
		pConfig = 0;
	}

	printf("main end\n");

	return 0;
}


