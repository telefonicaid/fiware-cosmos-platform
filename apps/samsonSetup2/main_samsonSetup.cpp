/* ****************************************************************************
*
* FILE                     main_samsonSetup.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <stdio.h>              // printf, ...
#include <sys/stat.h>           // struct stat
#include <unistd.h>             // stat
#include <fcntl.h>              // open, O_RDWR, O_CREAT, O_TRUNC, ...
#include <stdlib.h>             // free

#include "parseArgs.h"          // parseArgs
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace levels

#if 0
#include "samsonDirectories.h"  // SAMSON_IMAGES
#include "samsonConfig.h"       // SAMSON_MAX_HOSTS
#include "Process.h"            // Process, ProcessVector
#include "Host.h"               // Host
#include "HostMgr.h"            // HostMgr
#include "ports.h"              // WORKER_PORT
#include "Endpoint.h"           // ss::Endpoint
#include "Message.h"            // ss::Message
#include "iomConnect.h"         // iomConnect
#include "iomMsgSend.h"         // iomMsgSend
#include "iomMsgAwait.h"        // iomMsgAwait
#include "iomMsgRead.h"         // iomMsgRead
#include "platformProcesses.h"  // ss::platformProcessesGet, ss::platformProcessesSave
#include "Process.h"            // Process
#endif
#include "Endpoint2.h"          // Endpoint2
#include "SamsonSetup.h"        // SamsonSetup



/* ****************************************************************************
*
* Option variables
*/
char           controllerHost[256];
int            workers;
const char*    ips[100];
bool           reset;
bool           pList;



/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-controller",   controllerHost,  "CONTROLLER", PaString,  PaOpt,  PaND,   PaNL,  PaNL,  "Controller host"               },
	{ "-reset",       &reset,           "RESET",      PaBool,    PaOpt,  false, false,  true,  "reset platform"                },
	{ "-plist",       &pList,           "P_LIST",     PaBool,    PaOpt,  false, false,  true,  "process list of platform"      },
	{ "-ips",          ips,             "IP_LIST",    PaSList,   PaOpt,  PaND,   PaNL,  PaNL,  "list of worker IPs"            },
	{ "-workers",     &workers,         "WORKERS",    PaInt,     PaOpt,     0,     0,   100,   "number of workers"             },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* Global variables
*/
int               logFd        = -1;
ss::SamsonSetup*  samsonSetup  = NULL;
int               startTime;



/* ****************************************************************************
*
* helloCheck - 
*/
void helloCheck(void* callbackData, void* userParam)
{
	int unhelloed = 0;
	int now;
	int helloed = 0;

	callbackData  = NULL;
	userParam     = NULL;

	LM_M(("IN"));

	for (int ix = 0; ix < samsonSetup->networkP->epMgr->endpointCapacity(); ix++)
	{
		ss::Endpoint2* ep;

		ep = samsonSetup->networkP->epMgr->get(ix);
		if (ep == NULL)
			continue;   // Could do break here, really ...

		if (ep->typeGet() == ss::Endpoint2::Spawner)
		{
			if (ep->stateGet() != ss::Endpoint2::Ready)
				++unhelloed;
			else
				++helloed;
		}			

		if (ep->typeGet() == ss::Endpoint2::Unhelloed)
			LM_W(("Endpoint %02d is Unhelloed - could it be a spawner-to-be ... ?", ix));
	}

	if (unhelloed == 0)
	{
		if (helloed != samsonSetup->spawners)
			LM_W(("Helloed: %d, Spawners: %d - what has happened?", helloed, samsonSetup->spawners));
		else
			LM_M(("All Helloes interchanged - READY TO ROLL!"));
	}
	else
	{
		now = time(NULL);
		if ((now - startTime) > 5)
			LM_X(1, ("five seconds elapsed since connecting to spawners and still not all of them have helloed - I die"));
	}
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	ss::Endpoint2::Status  s;

	memset(controllerHost, 0, sizeof(controllerHost));

	paConfig("prefix",                        (void*) "SSP_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE@TIME  EXEC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	LM_T(LmtInit, ("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtInit, ("  %02d: '%s'", ix, argV[ix]));

	
	if ((int) ips[0] != workers)
		LM_X(1, ("%d workers specified on command line, but %d ips in ip-list", workers, (int) ips[0]));

	samsonSetup = new ss::SamsonSetup();
	samsonSetup->procVecCreate(controllerHost, workers, ips);

	startTime = time(NULL);
	if (samsonSetup->connect() != ss::Endpoint2::OK)
		LM_X(1, ("Error connecting to all spawners"));

	samsonSetup->networkP->epMgr->callbackSet(ss::EndpointManager::Timeout,  helloCheck, NULL);
	samsonSetup->networkP->epMgr->callbackSet(ss::EndpointManager::Periodic, helloCheck, NULL);

	samsonSetup->networkP->epMgr->show("Before calling run");
	LM_M(("Calling run"));
	samsonSetup->run();


	if (pList)
	{
		if ((s = samsonSetup->processList()) != ss::Endpoint2::OK)
			LM_X(1, ("Error sending Process List Message to spawners"));

		return 0;
	}

	if (samsonSetup->reset() != ss::Endpoint2::OK)
	   LM_X(1, ("Error sending RESET to all spawners"));

	if (reset)
	   return 0;

	if (samsonSetup->procVecSend() != ss::Endpoint2::OK)
		LM_X(1, ("Error sending Process Vector to all spawners"));

	return 0;
}
