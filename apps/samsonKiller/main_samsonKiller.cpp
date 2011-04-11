/* ****************************************************************************
*
* FILE                     main_samsonKiller.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 25 2011
*
*/
#include "parseArgs.h"          // parseArgs
#include "paUsage.h"            // paUsage
#include "traceLevels.h"        // Trace levels

#include "Endpoint.h"           // Endpoint
#include "ports.h"              // SPAWNER_PORT, CONTROLLER_PORT, WORKER_PORT
#include "iomConnect.h"         // iomConnect
#include "iomMsgSend.h"         // iomMsgSend




/* ****************************************************************************
*
* Option variables
*/
char             name[80];



/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ " ",            name,        "NAME",        PaString,  PaOpt,  (long) "all",   PaNL,   PaNL,  "name of prcess to kill"         },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* sysKill - 
*/
static void sysKill(const char* name)
{
	char com[256];

	snprintf(com, sizeof(com), "killall -9 %s > /dev/null 2>&1", name);
	if (system(com) == 0)
		printf("Killed %s\n", name);
}



/* ****************************************************************************
*
* killProcess - 
*/
static int killProcess(const char* name, unsigned short port)
{
	ss::Endpoint  ep;
	ss::Endpoint  me;
	int           s;

	ep.name = name;
	ep.wFd  = iomConnect("localhost", port);

	me.name = "samsonKiller";
	
	if ((ep.wFd == -1) || ((s = iomMsgSend(&ep, &me, ss::Message::Die, ss::Message::Msg)) != 0))
		sysKill(name);
	else
	{
		int fd = iomConnect("localhost", port);

		if (fd != -1)
			sysKill(name);
		else
			printf("killed '%s'\n", name);
	}

	return 0;
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	name[0] = 0;

	paConfig("log to file", (void*) true);
	paParse(paArgs, argC, (char**) argV, 1, false);

	if (name[0] != 0)
	{
		if ((strcasecmp(name, "worker") == 0) || (strcasecmp(name, "samsonWorker") == 0))
            return killProcess("samsonWorker", WORKER_PORT);
		else if ((strcasecmp(name, "controller") == 0) || (strcasecmp(name, "samsonController") == 0))
            return killProcess("samsonController", CONTROLLER_PORT);
		else if ((strcasecmp(name, "spawner") == 0) || (strcasecmp(name, "samsonSpawner") == 0))
			return killProcess("samsonSpawner", SPAWNER_PORT);
		else if (strcasecmp(name, "all") == 0)
		{
		   killProcess("samsonWorker",     WORKER_PORT);
		   killProcess("samsonController", CONTROLLER_PORT);
		   killProcess("samsonSpawner",    SPAWNER_PORT);

		   return 0;
		}
		else
		{
			printf("non-familiar process '%s'\n", name);
			exit(1);
		}
	}

	return 0;
}
