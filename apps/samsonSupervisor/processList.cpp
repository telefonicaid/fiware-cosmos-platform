/* ****************************************************************************
*
* FILE                     processList.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 18 2011
*
*/
#include <string.h>             // memset, strcpy, ...

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // LMT_*

#include "ports.h"              // SPAWNER_PORT
#include "spawnerList.h"        // spawnerAdd, spawnerListShow
#include "starterList.h"        // starterAdd, starterListShow
#include "Process.h"            // Process
#include "processList.h"        // Own interface



/* ****************************************************************************
*
* static global vars
*/
static Process**      processV    = NULL;
static unsigned int   processMax  = 0;
static unsigned int   processIx   = 0;



/* ****************************************************************************
*
* processListInit - 
*/
void processListInit(unsigned int pMax)
{
	processMax = pMax;
	processV   = (Process**) calloc(processMax, sizeof(Process*));
}



/* ****************************************************************************
*
* processAdd - 
*/
Process* processAdd(Process* processP)
{
	LM_T(LmtProcessList, ("Adding process '%s' in host '%s' (and with %d args)", processP->name, processP->host, processP->argCount));

	if (processIx >= processMax)
		LM_X(1, ("No room for more Processes (max index is %d) - change and recompile!", processMax));

	if (processLookup(processP->name, processP->host) != NULL)
		LM_X(1, ("process '%s' in host '%s' already in process list", processP->name, processP->host));

	processV[processIx] = processP;

	LM_T(LmtProcessList, ("Process '%s' in '%s' added - now adding its Spawner if necessary", processP->name, processP->host));
	processV[processIx]->spawnerP = spawnerLookup(processV[processIx]->host);
	if (processV[processIx]->spawnerP == NULL)
	{
		processV[processIx]->spawnerP = spawnerAdd(processV[processIx]->host, SPAWNER_PORT, -1);
		if (processV[processIx]->spawnerP == NULL)
			LM_X(1, ("Error creating spawner for host '%s'", processV[processIx]->host));
	}

	processListShow("Process added");
	spawnerListShow("Process added");

	++processIx;

	return processP;
}



/* ****************************************************************************
*
* processAdd - 
*/
Process* processAdd(char* name, char* host, unsigned short port, char** args, int argCount)
{
	int           argIx;
	Process*      processP;

	processP = (Process*) calloc(1, sizeof(Process));
	if (processP == NULL)
		LM_X(1, ("calloc: %s", strerror(errno)));

	processP->name     = strdup(name);
	processP->host     = strdup(host);
	processP->port     = port;

	processP->argCount = argCount;
	argIx = 0;
	while (argIx < argCount)
	{
		LM_T(LmtProcessList, ("Copying arg %d", argIx));
		processP->arg[argIx] = strdup(args[argIx]);
		LM_T(LmtProcessList, ("arg[%d]: '%s'", argIx, processP->arg[argIx]));
		++argIx;
	}

	return processAdd(processP);
}



/* ****************************************************************************
*
* processLookup - 
*/
Process* processLookup(char* name, char* host)
{
	for (unsigned int ix = 0; ix < processMax; ix++)
	{
		if (processV[ix] == NULL)
			continue;

		if ((strcmp(processV[ix]->name, name) == 0) && (strcmp(processV[ix]->host, host) == 0))
			return processV[ix];
	}

	return NULL;
}



/* ****************************************************************************
*
* processLookup - 
*/
Process* processLookup(unsigned int ix)
{
	if (ix > processMax)
		LM_X(1, ("cannot return process %d - max process id is %d", processMax));

	return processV[ix];
}



/* ****************************************************************************
*
* processMaxGet - 
*/
unsigned int processMaxGet(void)
{
	return processMax;
}



/* ****************************************************************************
*
* processListGet - 
*/
Process** processListGet(void)
{
	return processV;
}



/* ****************************************************************************
*
* processListShow - 
*/
void processListShow(const char* why)
{
	LM_T(LmtProcessListShow, ("---------- ProcessList: %s ----------", why));

	for (unsigned int ix = 0; ix < processMax; ix++)
	{
		if (processV[ix] == NULL)
			continue;

		LM_T(LmtProcessListShow, ("  %08p process %02d: %-20s %-20s   %d args (spawner at %p, starter at %p)", processV[ix], ix, processV[ix]->name, processV[ix]->host, processV[ix]->argCount, processV[ix]->spawnerP, processV[ix]->starterP));
	}
	LM_T(LmtProcessListShow, ("------------------------------------"));
}
