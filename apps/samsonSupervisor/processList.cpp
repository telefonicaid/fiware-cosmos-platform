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
#include "traceLevels.h"        // Trace Levels

#include "iomConnect.h"         // iomConnect
#include "globals.h"            // networkP
#include "ports.h"              // SPAWNER_PORT
#include "starterList.h"        // starterAdd, starterListShow
#include "Process.h"            // Process
#include "Popup.h"              // Popup
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
* processTypeName - 
*/
const char* processTypeName(ProcessType type)
{
	switch (type)
	{
	case PtWorkerStarter:       return "WorkerStarter";
	case PtControllerStarter:   return "ControllerStarter";
	case PtSpawner:             return "Spawner";
	}

	return "Unknown Process Type";
}



/* ****************************************************************************
*
* processTypeName - 
*/
const char* processTypeName(Process* processP)
{
	return processTypeName(processP->type);
}



/* ****************************************************************************
*
* processAdd - 
*/
Process* processAdd(Process* processP)
{
	Process* pP;

	LM_T(LmtProcessList, ("Adding process '%s' in host '%s'", processP->name, processP->host));

	if (processIx >= processMax)
		LM_X(1, ("No room for more Processes (max index is %d) - change and recompile!", processMax));

	if ((strcmp(processP->host, "ip") != 0) && (pP = processLookup(processP->name, processP->host)) != NULL)
	{
		LM_W(("process '%s' in host '%s' already in process list", processP->name, processP->host));
		LM_TODO(("Destroy processP"));
		return pP;
	}

	processV[processIx] = processP;

	processListShow("Process added");

	++processIx;

	LM_T(LmtProcess, ("Added process '%s@%s' with endpoint at %p", processP->name, processP->host, processP->endpoint));

	return processP;
}



/* ****************************************************************************
*
* processAdd - 
*/
Process* processAdd
(
	const char*     name,
	const char*     host,
	unsigned short  port,
	const char*     alias,
	ss::Endpoint*   endpoint
)
{
	Process* processP;

	LM_T(LmtProcess, ("Adding process '%s' in '%s'", name, host));

	if ((strcmp(host, "ip") != 0) && (processP = processLookup(name, host)) != NULL)
	{
		LM_W(("There is already a '%s' process in host '%s'", name, host));
		if (processP->endpoint != NULL)
		{
			LM_W(("And it has an endpoint, in state '%s'", processP->endpoint->stateName()));
			if (processP->endpoint->state != ss::Endpoint::Disconnected)
				LM_W(("WHAT DO I DO HERE ?"));
		}
	}
	else
	{
		processP = (Process*) calloc(1, sizeof(Process));
		if (processP == NULL)
			LM_X(1, ("calloc: %s", strerror(errno)));

		if (name == NULL)
			name = "noname";

		strncpy(processP->name, name, sizeof(processP->name));
		strncpy(processP->host, host, sizeof(processP->host));
		
		processP->port       = port;
		processP->endpoint   = endpoint;

		if (alias != NULL)
		   strncpy(processP->alias, alias, sizeof(processP->alias));
	}

	processP->sendsLogs  = false;

	if (strcmp(name, "Controller") == 0)
		processP->type = PtControllerStarter;
	else if (strcmp(name, "Worker") == 0)
		processP->type = PtWorkerStarter;
	else
		LM_X(1, ("name ('%s') should be either 'Controller' or 'Worker'", name));

	return processAdd(processP);
}



/* ****************************************************************************
*
* processLookup - 
*/
Process* processLookup(const char* name, const char* host)
{
	if ((name == NULL) || (name[0] == 0))
		return NULL;

	if ((host == NULL) || (host[0] == 0))
		return NULL;

	for (unsigned int ix = 0; ix < processMax; ix++)
	{
		if (processV[ix] == NULL)
			continue;

		if ((processV[ix]->host == NULL) || (processV[ix]->host[0] == 0))
			continue;

		if ((processV[ix]->name == NULL) || (processV[ix]->name[0] == 0))
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
Process* processLookup(const char* alias)
{
	if ((alias == NULL) || (alias[0] == 0))
		return NULL;

	for (unsigned int ix = 0; ix < processMax; ix++)
	{
		if (processV[ix] == NULL)
			continue;

		if ((processV[ix]->alias == NULL) || (processV[ix]->alias[0] == 0))
			continue;

		if (strcmp(processV[ix]->alias, alias) == 0)
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
* spawnerLookup - 
*/
Process* spawnerLookup(const char* host)
{
	LM_T(LmtProcess, ("Looking for host '%s' (process 0-%d)", host, processMax));

	for (unsigned int ix = 0; ix < processMax; ix++)
	{
		if (processV[ix] == NULL)
			continue;

		LM_T(LmtProcess, ("Comparing hosts: '%s' and '%s' (types: '%s' & '%s')", processV[ix]->host, host, processTypeName(processV[ix]), processTypeName(PtSpawner)));

		if ((strcmp(processV[ix]->host, host) == 0) && (processV[ix]->type == PtSpawner))
		{
			LM_T(LmtProcess, ("Found spawner for host '%s'", host));
			return processV[ix];
		}
	}

	LM_T(LmtProcess, ("Cannot find spawner for host '%s'", host));
	return NULL;
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
	LM_T(LmtProcessListShow, ("---------- Process List: %s ----------", why));

	for (unsigned int ix = 0; ix < processMax; ix++)
	{
		if (processV[ix] == NULL)
			continue;

		LM_T(LmtProcessListShow, ("  %08p process %02d: %-20s %-20s   (endpoint: %p, starter at %p, spawner at %p)", 
								  processV[ix], ix, processV[ix]->name, processV[ix]->host,
								  processV[ix]->endpoint,
								  processV[ix]->starterP,
								  processV[ix]->spawnerP));
	}
	LM_T(LmtProcessListShow, ("------------------------------------"));
}



/* ****************************************************************************
*
* spawnerAdd - 
*/
Process* spawnerAdd(Process* spawnerP)
{
	LM_T(LmtSpawnerList, ("Adding spawner for host '%s'", spawnerP->host));

    if (processIx >= processMax)
		LM_X(1, ("No room for more Spawners/Processes (max index is %d) - change and recompile!", processMax));

	if (spawnerLookup(spawnerP->host) != NULL)
	{
		LM_T(LmtProcessList, ("spawner for host '%s' already in process list", spawnerP->host));
		return spawnerP;
	}

	processV[processIx++] = spawnerP;

	LM_T(LmtProcessList, ("Spawner for '%s' added", spawnerP->host));

	if (spawnerP->endpoint == NULL)
	{
		int fd;

		fd = iomConnect(spawnerP->host, spawnerP->port);
		spawnerP->endpoint = networkP->endpointAdd("Adding spawner", fd, fd, spawnerP->name, spawnerP->name, 0, ss::Endpoint::Temporal, spawnerP->host, spawnerP->port);
	}

	return spawnerP;
}



/* ****************************************************************************
*
* spawnerAdd - 
*/
Process* spawnerAdd(const char* nameP, const char* host, unsigned short port, ss::Endpoint* endpoint)
{
	Process*  spawnerP;
	char      name[128];

	spawnerP = (Process*) calloc(1, sizeof(Process));
	if (spawnerP == NULL)
		LM_X(1, ("calloc: %s", strerror(errno)));

	if (nameP == NULL)
	{
		snprintf(name, sizeof(name), "spawner@%s", spawnerP->host);
		nameP = name;
	}

	strncpy(spawnerP->name, nameP,      sizeof(spawnerP->name));
	strncpy(spawnerP->host, host,       sizeof(spawnerP->host));
	strncpy(spawnerP->alias, "Spawner", sizeof(spawnerP->alias));
	
	spawnerP->port      = port;
	spawnerP->endpoint  = endpoint;
	spawnerP->type      = PtSpawner;

	return spawnerAdd(spawnerP);
}
