/* ****************************************************************************
*
* FILE                     actions.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 13 2011
*
*/
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "Message.h"            // ss::Message
#include "iomMsgSend.h"         // iomMsgSend
#include "iomConnect.h"         // iomConnect
#include "globals.h"            // global vars
#include "ports.h"              // SPAWNER_PORT, ...

#include "Popup.h"              // Popup
#include "Starter.h"            // Starter
#include "Process.h"            // Process, processAdd, ...
#include "processList.h"        // processListGet
#include "starterList.h"        // starterLookup
#include "actions.h"            // Own interface



/* ****************************************************************************
*
* help - 
*/
void help(void)
{
	printf("c - connect to samson spawners\n");
	printf("s - start samson\n");
	printf("h - print this help message\n");
	printf("q - quit\n");
}



/* ****************************************************************************
*
* list - 
*/
void list(void)
{
	LM_T(LmtProcessListShow, ("----- Processes: -----"));
	processListShow("'l' pressed");
}



/* ****************************************************************************
*
* processConnect - connect to process
*/
void processConnect(Process* processP)
{
	int fd;

	LM_T(LmtProcessConnect, ("connecting to process in %s on port %d", processP->host, processP->port));

	fd = iomConnect(processP->host, processP->port);
	if (fd == -1)
	{
		char errorText[256];

		snprintf(errorText, sizeof(errorText), "error connecting to process in host '%s', port %d", processP->host, processP->port);
		new Popup("Connect Error", errorText);
		
		return;
	}

	LM_T(LmtProcessConnect, ("Calling endpointAdd for process '%s'", processP->host));
	processP->endpoint = networkP->endpointAdd("connected to process",
										 fd,
										 fd,
										 "Process",
										 NULL,
										 0,
										 ss::Endpoint::Temporal,
										 std::string(processP->host),
										 processP->port);
}



/* ****************************************************************************
*
* connectToAllSpawners - 
*/
void connectToAllSpawners(void)
{
	LM_T(LmtSpawnerConnect, ("Connecting to all %d spawners", processMaxGet()));

	for (unsigned int ix = 0; ix < processMaxGet(); ix++)
	{
		Process* spawnerP;
		Starter* starterP;

		if ((spawnerP = processLookup(ix)) == NULL)
			continue;

		if (spawnerP->spawnInfo != NULL)  // Not a Spawner - a normal process ...
			continue;

		starterP = starterLookup(spawnerP);

		if (starterP == NULL)
			LM_W(("Cannot find starter for spawner '%s'", spawnerP->host));
		else
			processConnect(spawnerP);
	}
}



/* ****************************************************************************
*
* connectToController - 
*/
void connectToController(void)
{
	LM_T(LmtControllerConnect, ("Connecting to controller"));

	for (unsigned int ix = 0; ix < processMaxGet(); ix++)
	{
		Process* processP;

		if ((processP = processLookup(ix)) == NULL)
			continue;

		if (strcmp(processP->name, "Controller") == 0)
			processConnect(processP);
	}
}



/* ****************************************************************************
*
* connectToAllProcesses - 
*/
void connectToAllProcesses(void)
{
	LM_T(LmtProcessConnect, ("Connecting to all (possibly %d) processes", processMaxGet()));

	for (unsigned int ix = 0; ix < processMaxGet(); ix++)
	{
		Process* processP;

		if ((processP = processLookup(ix)) == NULL)
			continue;

		processConnect(processP);
	}
}



/* ****************************************************************************
*
* processStart - start a process
*/
void processStart(Process* processP, Starter* starter)
{
	ss::Message::SpawnData  spawnData;
	ss::Endpoint*           logServer;
	int                     ix;
	char*                   end;
	int                     s;
	char*                   alias = (char*) "no_alias";

	LM_T(LmtProcessStart, ("starting process '%s' in '%s' with %d parameters", processP->name, processP->host, processP->spawnInfo->argCount));
	processListShow("starting process");

	LM_TODO(("Lookup starter and don't start if already started!"));

	spawnData.argCount = processP->spawnInfo->argCount;
	strcpy(spawnData.name, processP->name);
	memset(spawnData.args, sizeof(spawnData.args), 0);

	end = spawnData.args;

	networkP->endpointListShow("Looking up logServer");
	
	if ((logServer = networkP->logServerLookup()) != NULL)
	{
		strcpy(end, "-logServer");
		LM_T(LmtProcessStart, ("parameter: '%s'", end));
		end += strlen("-logServer") + 1;  // leave one ZERO character
		strcpy(end, logServer->ip.c_str());
		LM_T(LmtProcessStart, ("parameter: '%s'", end));
		end += strlen(logServer->ip.c_str()) + 1; // leave one ZERO character
	}
	else
		LM_T(LmtProcessStart, ("no log server found!"));

	for (ix = 0; ix < processP->spawnInfo->argCount; ix++)
	{
		strcpy(end, processP->spawnInfo->arg[ix]);
		LM_T(LmtProcessStart, ("parameter %d: '%s'", ix, end));
		end += strlen(processP->spawnInfo->arg[ix]) + 1; // leave one ZERO character
		if (strcmp(processP->spawnInfo->arg[ix], "-alias") == 0)
			alias = end;
	}
	*end = 0;

	LM_T(LmtProcessStart, ("starting %s via spawner %p (host: '%s', fd: %d)",
						   spawnData.name,
						   processP->spawnInfo->spawnerP,
						   processP->spawnInfo->spawnerP->host,
						   processP->spawnInfo->spawnerP->endpoint->rFd));

	if (strcmp(spawnData.name, "Controller") == 0)
		s = iomMsgSend(processP->spawnInfo->spawnerP->endpoint->wFd, processP->spawnInfo->spawnerP->host, "samsonSupervisor", ss::Message::ControllerSpawn, ss::Message::Msg, &spawnData, sizeof(spawnData));
	else if (strcmp(spawnData.name, "Worker") == 0)
		s = iomMsgSend(processP->spawnInfo->spawnerP->endpoint->wFd, processP->spawnInfo->spawnerP->host, "samsonSupervisor", ss::Message::WorkerSpawn, ss::Message::Msg, &spawnData, sizeof(spawnData));
	if (s != 0)
		LM_E(("iomMsgSend: error %d", s));

	LM_T(LmtProcessStart, ("started process '%s' in '%s')", processP->name, processP->host));
	LM_T(LmtProcessStart, ("Connecting to newly started process (%s) ...", spawnData.name));

	int tries = 0;
	while (1)
	{
		int fd;

		if (strcmp(spawnData.name, "Controller") == 0)
		{
			fd = iomConnect(processP->spawnInfo->spawnerP->host, CONTROLLER_PORT);
			if (fd != -1)
			{
				starter->process->endpoint = networkP->endpointAdd("connected to spawner", fd, fd, "Controller", "Controller", 0, ss::Endpoint::Controller, processP->spawnInfo->spawnerP->host, CONTROLLER_PORT);
				break;
			}			
		}
		else if (strcmp(spawnData.name, "Worker") == 0)
		{
			fd = iomConnect(processP->spawnInfo->spawnerP->host, WORKER_PORT);

			if (fd != -1)
			{
				processP->endpoint = networkP->endpointAdd("connected to worker", fd, fd, spawnData.name, alias, 0, ss::Endpoint::Temporal, processP->host, WORKER_PORT);
				break;
			}

			LM_TODO(("This endpoint is TEMPORAL and will be changed when the Hello is received - fix this problem!"));
		}
		
		if (++tries > 20)
		{
			char errorText[256];

			if (strcmp(spawnData.name, "Worker") == 0)
				snprintf(errorText, sizeof(errorText), "Error connecting to Samson Worker in '%s', port %d", processP->spawnInfo->spawnerP->host, WORKER_PORT);
			else if (strcmp(spawnData.name, "Controller") == 0)
				snprintf(errorText, sizeof(errorText), "Error connecting to Samson Controller in '%s', port %d", processP->spawnInfo->spawnerP->host, CONTROLLER_PORT);
			else
				snprintf(errorText, sizeof(errorText), "Error connecting ");

			new Popup("Connect Error", errorText);
		}

		usleep(50000);
	}

	starter->check();
}



/* ****************************************************************************
*
* startAllProcesses - 
*/
void startAllProcesses(void)
{
	for (unsigned ix = 0; ix < processMaxGet(); ix++)
	{
		Process* processP;
		Starter* starterP;

		if ((processP = processLookup(ix)) == NULL)
			continue;

		starterP = starterLookup(processP);
		processStart(processP, starterP);
	}
}



/* ****************************************************************************
*
* processKill - kill a process
*/
void processKill(Process* processP, Starter* starterP)
{
	Starter*  starter;
	int       s;
	
	LM_T(LmtProcessKill, ("killing process '%s' in host '%s'", processP->name, processP->host));
	starter = starterLookup(processP);
	if (starter == NULL)
		LM_E(("can't find starter for process '%s' at %s", processP->name, processP->host));
	else if (starter != starterP)
		LM_E(("This is very strange - the starter pointer seems erroneous (%p vs %p)", starterP, starter));
	else
	{
		if (processP->endpoint == NULL)
			LM_E(("can't kill starter for process '%s' at %s as its endpoint is NULL", processP->name, processP->host));
		else if (processP->endpoint->state != ss::Endpoint::Connected)
			LM_E(("can't kill starter for process '%s' at %s as its endpoint is in state '%s'", processP->name, processP->host, processP->endpoint->stateName()));
		else
		{
			LM_T(LmtDie, ("Now really sending 'Die' to '%s' at '%s' (name: '%s')", processP->endpoint->typeName(), processP->endpoint->ip.c_str(), processP->endpoint->name.c_str()));
			s = iomMsgSend(processP->endpoint->wFd, processP->endpoint->ip.c_str(), "samsonSupervisor", ss::Message::Die, ss::Message::Msg);
		}
	}
}
