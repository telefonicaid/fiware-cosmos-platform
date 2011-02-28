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
	{ " ",            name,        "NAME",        PaString,  PaOpt,  PaND,   PaNL,   PaNL,  "name of prcess to kill"         },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* killProcess - 
*/
static int killProcess(const char* name, unsigned short port)
{
	ss::Endpoint  ep;
	ss::Endpoint  me;

	ep.name = name;
	ep.wFd  = iomConnect("localhost", port);
	
	if (ep.wFd == -1)
	{
		printf("error connecting to %s at 'localhost', port %d - probably means the process isn't running\n", name, port);
		return 3;
	}

	me.name = "samsonKiller";

	if (iomMsgSend(&ep, &me, ss::Message::Die, ss::Message::Msg) != 0)
	{
		printf("error sending 'Die' message to endpoint (port %d)\n", port);
		return 4;
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

	paParse(paArgs, argC, (char**) argV, 1, false);

#if 0
	printf("Started with arguments:\n");
	for (int ix = 0; ix < argC; ix++)
		printf("  %02d: '%s'\n", ix, argV[ix]);
#endif

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
