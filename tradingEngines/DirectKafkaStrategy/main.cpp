
#include "./handler/config.h"
#include "./handler/tdhandler.h"
#include <stdio.h>
#include <stdlib.h>

#include "./util/Platform.h"

#include <unistd.h>
#include <signal.h>

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

	printf("tdhandler module start\n");
	tdhandler* pHandler = new tdhandler();
	iret = pHandler->Instance(pConfig);
	if (iret <0)
	{
		return -2;
	}

	sigset( SIGINT, signalhandler );

	while (!global_exit)
	{
		sleep(1);

	}

	printf("tdhandler module end\n");
	{
		pHandler->Release();
		delete pHandler;
		pHandler = 0;
	}

	printf("config module end\n");
	{
		pConfig->Release();
		delete pConfig;
		pConfig = 0;
	}

	printf("main end\n");

	return 0;
}


