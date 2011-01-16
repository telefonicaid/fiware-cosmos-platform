/* ****************************************************************************
*
* FILE                     main_samsonSupervisor.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <stdlib.h>             // atoi

#include "baTerm.h"             // baTermSetup
#include "logMsg.h"             // LM_*
#include "parseArgs.h"          // parseArgs

#include "Endpoint.h"           // Endpoint
#include "Network.h"            // Network
#include "Process.h"            // Process, processAdd, ...
#include "qt.h"                 // qtRun, ...
#include "TabManager.h"         // TabManager
#include "SamsonSupervisor.h"   // SamsonSupervisor



/* ****************************************************************************
*
* Global variables
*/
ss::Network*       networkP     = NULL;
SamsonSupervisor*  supervisorP  = NULL;
ss::Endpoint*      controller   = NULL;
TabManager*        tabManager   = NULL;



/* ****************************************************************************
*
* Option variables
*/
int     endpoints;
char    controllerName[80];
char    cfPath[80];
bool    qt;



#define CFP (long int)  "/opt/samson/etc/platformProcesses"
#define NOC  (long int) "no controller"
/* ****************************************************************************
*
* Parse arguments
*/
PaArgument paArgs[] =
{
	{ "-controller",  controllerName,  "CONTROLLER",  PaString,  PaOpt,   NOC,  PaNL,   PaNL,  "controller IP:port"  },
	{ "-endpoints",   &endpoints,      "ENDPOINTS",   PaInt,     PaOpt,    20,     3,    100,  "number of endpoints" },
	{ "-config",      &cfPath,         "CF_FILE",     PaStr,     PaOpt,   CFP,  PaNL,   PaNL,  "path to config file" },
	{ "-qt",          &qt,             "QT",          PaBool,    PaOpt,  true, false,   true,  "graphical"           },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
int logFd = -1;



/* ****************************************************************************
*
* argsParse - 
*/
static int argsParse(char* line, char* host, char* process, char** args, int* argCount)
{
	int  ix;
	int  argIx = 0;
	

	LM_D(("=============================================================="));
	LM_D(("Parsing line: %s", line));

	/* 1. Remove comment part of line */
	ix = 0;
	while (line[ix] != 0)
	{
		if (line[ix] == '#')
		{
			line[ix] = 0;
			break;
		}

		ix += 1;
	}
	LM_D(("w/o comments: %s", line));

	/* 2. Remove '\n' at end of line */
	if (line[strlen(line) - 1] == '\n')
		line[strlen(line) - 1] = 0;
	LM_D(("w/o trailing newline: '%s'", line));


	/* 3. Eat leading whitespace */
	while (*line != 0)
	{
		if ((*line == ' ') || (*line == '\t'))
			++line;
		else
			break;
	}
	LM_D(("w/o leading whitespace: '%s'", line));

	if (line[0] == 0)
		return -1;

	/* 4. Get 'host' */
	ix = 0;
	while (line[ix] != 0)
	{
		if (line[ix] == ' ')
		{
			line[ix] = 0;
			strcpy(host, line);
			line = &line[ix + 1];
			LM_D(("Got host '%s'. line: '%s'", host, line));
			break;
		}
		ix += 1;
	}



	/* 5. Get 'process' */
	LM_M(("Get process: '%s'", line));
	while (*line != 0)
	{
		if ((*line == ' ') || (*line == '\t'))
			++line;
		else
			break;
	}

    ix = 0;
    while (line[ix] != 0)
    {
        if (line[ix] == ' ')
        {
            line[ix] = 0;
            strcpy(process, line);
            line = &line[ix + 1];
			LM_D(("Got process '%s'. line: '%s'", process, line));
            break;
        }
        ix += 1;
    }



	/* 6. Get rest of args */
	argIx = 0;
	while (*line != 0)
	{
		char* start;

		/* 7.1. Eat leading whitespace */
		LM_D(("line: %s", line));
		while ((*line == ' ') || (*line == '\t'))
			++line;
		LM_D(("line: %s", line));

		/* 7.2. Find next space (or newline) */
		start = line;
		LM_D(("start: %s", start));
		while ((*line != ' ') && (*line != '\t') && (*line != '\n') && (*line != 0))
			++line;

		LM_D(("line: %s", line));
		if (*line != 0)
		{
			*line = 0;
			++line;
			LM_D(("line: %s", line));
		}

		LM_D(("Got arg %d: '%s'. line: '%s'", argIx, start, line));
		args[argIx++] = strdup(start);
		LM_D(("REST: '%s'", line));
		LM_D(("host: '%s', host"));
	}

	*argCount = argIx;
	LM_D(("host:    '%s'", host));
	LM_D(("process: '%s'", process));

	return 0;
}



/* ****************************************************************************
*
* cfParse - 
*/
static void cfParse(char* cfPath)
{
	char    line[160];
	FILE*   fP;

	fP = fopen(cfPath, "r");
	if (fP == NULL)
		LM_X(1, ("opening '%s': %s", cfPath, strerror(errno)));

	while (fgets(line, sizeof(line), fP) != NULL)
	{
		char   host[32];
		char   process[32];
		char*  args[100];
		int    argCount;

		memset(args, 0, sizeof(args));
		LM_D(("parsing line: %s", line));
		if (argsParse(line, host, process, args, &argCount) == 0)
		{
			LM_M(("Adding process '%s' in %s with %d args", process, host, argCount));
			processAdd(process, host, args, argCount);
		}
	}
}



/* ****************************************************************************
*
* cfPresent - 
*/
static void cfPresent(void)
{
	Process* p;
	int      ix = 0;

	while (1)
	{
		int aIx;

		p = processGet(ix);
		if (p == NULL)
			break;

		printf("Process %d:\n", ix);
		printf("  Name:     %s\n", p->name);
		printf("  Host:     %s\n", p->host);
		printf("  Args:     %d\n", p->argCount);

		for (aIx = 0; aIx < p->argCount; aIx++)
			printf("    Arg %02d:  %s\n", aIx, p->arg[aIx]);

		printf("\n");
		++ix;
	}
}



void* runNetworkAsThread(void* nP)
{
	ss::Network* networkP = (ss::Network*) nP;

	networkP->run();

	return NULL;
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	paConfig("prefix",                        (void*) "SSS_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	LM_M(("parsing input file '%s'", cfPath));
	cfParse(cfPath);
	cfPresent();

	networkP = new ss::Network(endpoints, 0);

	networkP->init(ss::Endpoint::Supervisor, "supervisor", 0, controllerName);
	supervisorP = new SamsonSupervisor(networkP);


	networkP->setDataReceiver(supervisorP);
	networkP->setEndpointUpdateReceiver(supervisorP);

	baTermSetup();
	networkP->fdSet(0, "stdin", "stdin");

	if ((networkP->controller != NULL) && (networkP->controller->state == ss::Endpoint::Connected))
	{
		LM_M(("Connected to controller - Send workerVector message !"));
		// Send workerVector message to controller and then
		// connect to all Workers and update ProcessList
	}

	if (qt)
	{
		pthread_t t;

		pthread_create(&t, NULL, runNetworkAsThread, networkP);
		// Await ready ?
		qtRun(argC, argV);
	}
	else
		networkP->run();

	return 0;
}
