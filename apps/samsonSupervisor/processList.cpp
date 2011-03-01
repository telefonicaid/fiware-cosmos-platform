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
static ss::Process**  processV    = NULL;
static unsigned int   processMax  = 0;



/* ****************************************************************************
*
* processListInit - 
*/
void processListInit(unsigned int pMax)
{
	processMax = pMax;
	processV   = (ss::Process**) calloc(processMax, sizeof(ss::Process*));
}



/* ****************************************************************************
*
* processTypeName - 
*/
const char* processTypeName(ss::ProcessType type)
{
	switch (type)
	{
	case ss::PtWorker:       return "WorkerStarter";
	case ss::PtController:   return "ControllerStarter";
	case ss::PtSpawner:      return "Spawner";
	}

	return "Unknown Process Type";
}



/* ****************************************************************************
*
* processTypeName - 
*/
const char* processTypeName(ss::Process* processP)
{
	return processTypeName(processP->type);
}



/* ****************************************************************************
*
* processAdd - 
*/
ss::Process* processAdd(ss::Process* processP)
{
	ss::Process*  pP;
	unsigned int  ix;

	LM_T(LmtProcessList, ("Adding process '%s' in host '%s'. type '%s'", processP->name, processP->host, processTypeName(processP)));

	if ((strcmp(processP->host, "ip") != 0) && (pP = processLookup(processP->name, processP->host)) != NULL)
	{
		LM_W(("process '%s' in host '%s' already in process list", processP->name, processP->host));
		free(processP);
		return pP;
	}

	for (ix = 0; ix < processMax; ix++)
	{
		if (processV[ix] == NULL)
			break;
	}

	if (ix >= processMax)
		LM_X(1, ("No room for more Processes (max index is %d) - change and recompile!", processMax));

	processV[ix] = processP;
	LM_T(LmtProcess, ("Added process '%s@%s' with endpoint at %p", processP->name, processP->host, processP->endpoint));
	processListShow("Process added");

	return processP;
}



extern char* controllerHostP;
/* ****************************************************************************
*
* processAdd - 
*/
ss::Process* processAdd
(
	ss::ProcessType  type,
	const char*      name,
	const char*      host,
	unsigned short   port,
	const char*      alias,
	ss::Endpoint*    endpoint
)
{
	ss::Process* processP;

	if (type == ss::PtWorker)
		LM_T(LmtProcess, ("Adding a Worker process"));
	else if (type == ss::PtController)
		LM_T(LmtProcess, ("Adding a Controller process"));
	else if (type == ss::PtSpawner)
		LM_T(LmtProcess, ("Adding a Spawner process"));
	else
		LM_X(1, ("Bad process type"));

	if ((strcmp(host, "ip") != 0) && (processP = processLookup(name, host)) != NULL)
	{
		LM_W(("There is already a '%s' process in host '%s'", name, host));
		return processP;
	}

	processP = (ss::Process*) calloc(1, sizeof(ss::Process));
	if (processP == NULL)
		LM_X(1, ("calloc: %s", strerror(errno)));

	if (name == NULL)
		name = "noProcessName";

	strncpy(processP->name, name, sizeof(processP->name));
	strncpy(processP->host, host, sizeof(processP->host));
		
	processP->type       = type;
	processP->port       = port;
	processP->endpoint   = endpoint;
	processP->workers    = networkP->Workers;
	processP->sendsLogs  = false;

	if (alias != NULL)
		strncpy(processP->alias, alias, sizeof(processP->alias));

	strncpy(processP->controllerHost, controllerHostP, sizeof(processP->controllerHost));

	return processAdd(processP);
}



/* ****************************************************************************
*
* spawnerAdd - 
*/
ss::Process* spawnerAdd(ss::Process* spawnerP)
{
	LM_T(LmtSpawnerList, ("Adding spawner for host '%s'", spawnerP->host));

	if (spawnerLookup(spawnerP->host) != NULL)
	{
		LM_T(LmtProcessList, ("spawner for host '%s' already in process list", spawnerP->host));
		return spawnerP;
	}

	spawnerP->type = ss::PtSpawner;
	processAdd(spawnerP);

	if (spawnerP->endpoint == NULL)
	{
		int fd;

		fd = iomConnect(spawnerP->host, spawnerP->port);
		LM_TODO(("Why Temporal?  Its as Spawner, I could use endpoint[3] directly ... ?"));
		spawnerP->endpoint = networkP->endpointAdd("Adding spawner", fd, fd, spawnerP->name, spawnerP->name, 0, ss::Endpoint::Spawner, spawnerP->host, spawnerP->port);
		LM_T(LmtSpawnerList, ("Set spawner endpoint to %p", spawnerP->endpoint));
	}

	processListShow("Spawner added");
	return spawnerP;
}



/* ****************************************************************************
*
* spawnerAdd - 
*/
ss::Process* spawnerAdd(const char* nameP, const char* host, unsigned short port, ss::Endpoint* endpoint)
{
	ss::Process*  spawnerP;
	char          name[128];

	LM_T(LmtSpawnerList, ("** Adding spawner '%s@%s' endpoint %p", nameP, host, endpoint));

	if ((spawnerP = spawnerLookup(host)) != NULL)
	{
		LM_T(LmtProcessList, ("spawner for host '%s' already in process list", host));
		return spawnerP;
	}

	spawnerP = (ss::Process*) calloc(1, sizeof(ss::Process));
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
	spawnerP->type      = ss::PtSpawner;

	return spawnerAdd(spawnerP);
}



/* ****************************************************************************
*
* processLookup - 
*/
ss::Process* processLookup(const char* name, const char* host)
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
ss::Process* processLookup(const char* alias)
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
ss::Process* processLookup(unsigned int ix)
{
	if (ix > processMax)
		LM_X(1, ("cannot return process %d - max process id is %d", processMax));

	return processV[ix];
}



/* ****************************************************************************
*
* processLookup - 
*/
ss::Process* processLookup(ss::Endpoint* ep)
{
	if (ep == NULL)
		return NULL;

	for (unsigned int ix = 0; ix < processMax; ix++)
	{
		if (processV[ix] == NULL)
			continue;

		if (processV[ix]->endpoint == ep)
			return processV[ix];
	}

	return NULL;
}



/* ****************************************************************************
*
* spawnerLookup - 
*/
ss::Process* spawnerLookup(const char* host)
{
	LM_T(LmtProcess, ("Looking for host '%s' (process 0-%d)", host, processMax));

	for (unsigned int ix = 0; ix < processMax; ix++)
	{
		if (processV[ix] == NULL)
			continue;

		if (processV[ix]->type != ss::PtSpawner)
			continue;

		LM_T(LmtProcess, ("Comparing hosts: '%s' and '%s'", processV[ix]->host, host));

		if (strcmp(processV[ix]->host, host) == 0)
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
ss::Process** processListGet(void)
{
	return processV;
}



/* ****************************************************************************
*
* processListShow - 
*/
void processListShow(const char* why, bool forcedOn)
{
	if (forcedOn)
		LM_F(("---------- Process List: %s ----------", why));
	else
		LM_T(LmtProcessListShow, ("---------- Process List: %s ----------", why));

	for (unsigned int ix = 0; ix < processMax; ix++)
	{
		if (processV[ix] == NULL)
			continue;

		if (forcedOn)
			LM_F(("  %08p process %02d: %-20s %-20s   (endpoint: %p, starter at %p, spawner at %p)", 
				  processV[ix], ix, processV[ix]->name, processV[ix]->host,
				  processV[ix]->endpoint,
				  processV[ix]->starterP,
				  processV[ix]->spawnerP));
		else
			LM_T(LmtProcessListShow, ("  %08p process %02d: %-20s %-20s   (endpoint: %p, starter at %p, spawner at %p)", 
									  processV[ix], ix, processV[ix]->name, processV[ix]->host,
									  processV[ix]->endpoint,
									  processV[ix]->starterP,
									  processV[ix]->spawnerP));
	}

	if (forcedOn)
		LM_F(("------------------------------------"));
	else
		LM_T(LmtProcessListShow, ("------------------------------------"));
}



/* ****************************************************************************
*
* processRemove - 
*/
void processRemove(ss::Process* processP)
{
	for (unsigned int ix = 0; ix < processMax; ix++)
	{
		if (processV[ix] == processP)
		{
			free(processP);
			processV[ix] = NULL;
		}
	}
}
