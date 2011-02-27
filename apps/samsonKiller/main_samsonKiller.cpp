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
unsigned short   port;
char             name[80];



/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-port",       &port,        "PORT",        PaShortU,  PaOpt,  1025,   1025,  65000,  "port of prcess to kill"         },
	{ " ",            name,        "NAME",        PaString,  PaOpt,  PaND,   PaNL,   PaNL,  "name of prcess to kill"         },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	ss::Endpoint  ep;
	ss::Endpoint  me;

	name[0] = 0;

	paParse(paArgs, argC, (char**) argV, 1, false);

#if 0
	printf("Started with arguments:\n");
	for (int ix = 0; ix < argC; ix++)
		printf("  %02d: '%s'\n", ix, argV[ix]);
#endif

	if ((port == 1025) && (name[0] == 0))
	{
		printf("Must specify either the option '-port' or an argument giving the name of the process\n");
		paUsage(paArgs);
		exit(1);
	}


	if (name[0] != 0)
	{
		if (strcasecmp(name, "spawner") == 0)
			port = SPAWNER_PORT;
		else if (strcasecmp(name, "worker") == 0)
			port = WORKER_PORT;
		else if (strcasecmp(name, "controller") == 0)
			port = CONTROLLER_PORT;
		else
		{
			printf("non-familiar process '%s'\n", name);
			exit(1);
		}
	}


	if ((port != SPAWNER_PORT) && (port != WORKER_PORT) && (port != CONTROLLER_PORT))
	{
		printf("non-familiar port: %d\n", port);
		exit(2);
	}

	ep.name = "toBeKilled";
	ep.wFd = iomConnect("localhost", port);
	
	if (ep.wFd == -1)
	{
		printf("error connecting to 'localhost', port %d - probably means the process isn't running\n", port);
		exit(3);
	}

	me.name = "samsonKiller";

	if (iomMsgSend(&ep, &me, ss::Message::Die, ss::Message::Msg) != 0)
	{
		printf("error sending 'Die' message to endpoint (port %d)\n", port);
		exit(4);
	}

	return 0;
}
