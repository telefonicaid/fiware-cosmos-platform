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
#include "traceLevels.h"        // LMT_*

#include "Message.h"            // ss::Message
#include "iomMsgSend.h"         // iomMsgSend
#include "iomConnect.h"         // iomConnect
#include "globals.h"            // global vars
#include "ports.h"              // SPAWNER_PORT, ...

#include "Popup.h"              // Popup
#include "Starter.h"            // Starter
#include "Spawner.h"            // Spawner, spawnerAdd, ...
#include "Process.h"            // Process, processAdd, ...
#include "spawnerList.h"        // spawnerListGet
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
	LM_F(("----- Spawners: -----"));
	spawnerListShow("'l' pressed");

	LM_F(("----- Processs: -----"));
	processListShow("'l' pressed");
}



/* ****************************************************************************
*
* spawnerConnect - connect to spawner
*/
void spawnerConnect(Spawner* sP)
{
	LM_M(("connecting to spawner in %s on port %d", sP->host, sP->port));

	sP->fd = iomConnect(sP->host, sP->port);
	if (sP->fd == -1)
	{
		char errorText[256];

		snprintf(errorText, sizeof(errorText), "error connecting to spawner in host '%s', port %d", sP->host, sP->port);
		new Popup("Connect Error", errorText);
		
		return;
	}

	ss::Endpoint*  ep;

	LM_M(("Calling endpointAdd for spawner '%s'", sP->host));
	ep = networkP->endpointAdd("connected to spawner",
							   sP->fd,
							   sP->fd,
							   "Spawner",
							   NULL,
							   0,
							   ss::Endpoint::Temporal,
							   std::string(sP->host),
							   sP->port);
}



/* ****************************************************************************
*
* processConnect - connect to process
*/
void processConnect(Process* sP)
{
	LM_M(("connecting to process in %s on port %d", sP->host, sP->port));

	sP->fd = iomConnect(sP->host, sP->port);
	if (sP->fd == -1)
	{
		char errorText[256];

		snprintf(errorText, sizeof(errorText), "error connecting to process in host '%s', port %d", sP->host, sP->port);
		new Popup("Connect Error", errorText);
		
		return;
	}

	ss::Endpoint*  ep;

	LM_M(("Calling endpointAdd for process '%s'", sP->host));
	ep = networkP->endpointAdd("connected to process",
							   sP->fd,
							   sP->fd,
							   "Process",
							   NULL,
							   0,
							   ss::Endpoint::Temporal,
							   std::string(sP->host),
							   sP->port);
}



/* ****************************************************************************
*
* connectToAllSpawners - 
*/
void connectToAllSpawners(void)
{
	LM_M(("Connecting to all %d spawners", spawnerMaxGet()));

	for (unsigned int ix = 0; ix < spawnerMaxGet(); ix++)
	{
		Spawner* sP;

		if ((sP = spawnerLookup(ix)) == NULL)
			continue;

		spawnerConnect(sP);
	}
}



/* ****************************************************************************
*
* connectToController - 
*/
void connectToController(void)
{
	LM_M(("Connecting to controller"));

	for (unsigned int ix = 0; ix < processMaxGet(); ix++)
	{
		Process* sP;

		if ((sP = processLookup(ix)) == NULL)
			continue;

		if (strcmp(sP->name, "Controller") == 0)
			processConnect(sP);
	}
}



/* ****************************************************************************
*
* connectToAllProcesses - 
*/
void connectToAllProcesses(void)
{
	LM_M(("Connecting to all %d processes", processMaxGet()));

	for (unsigned int ix = 0; ix < processMaxGet(); ix++)
	{
		Process* sP;

		if ((sP = processLookup(ix)) == NULL)
			continue;

		processConnect(sP);
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

	if (starter->checked())
		LM_RVE(("Not starting process '%s' in '%s' - already started", processP->name, processP->host));

	LM_M(("starting process '%s' in '%s' with %d parameters", processP->name, processP->host, processP->argCount));

	LM_TODO(("Lookup starter and don't start if already started!"));

	spawnData.argCount = processP->argCount;
	strcpy(spawnData.name, processP->name);
	memset(spawnData.args, sizeof(spawnData.args), 0);

	end = spawnData.args;

	networkP->endpointListShow("Looking up logServer");
	if ((logServer = networkP->logServerLookup()) != NULL)
	{
		strcpy(end, "-logServer");
		LM_M(("parameter: '%s'", end));
		end += strlen("-logServer") + 1;  // leave one ZERO character
		strcpy(end, logServer->ip.c_str());
		LM_M(("parameter: '%s'", end));
		end += strlen(logServer->ip.c_str()) + 1; // leave one ZERO character
	}
	else
		LM_W(("no log server found!"));

	for (ix = 0; ix < processP->argCount; ix++)
	{
		strcpy(end, processP->arg[ix]);
		LM_M(("parameter %d: '%s'", ix, end));
		end += strlen(processP->arg[ix]) + 1; // leave one ZERO character
		if (strcmp(processP->arg[ix], "-alias") == 0)
			alias = end;
	}
	*end = 0;

	LM_M(("starting %s via spawner %p (host: '%s', fd: %d)", spawnData.name, processP->spawnerP, processP->spawnerP->host, processP->spawnerP->fd));
	if (strcmp(spawnData.name, "Controller") == 0)
		s = iomMsgSend(processP->spawnerP->fd, processP->spawnerP->host, "samsonSupervisor", ss::Message::ControllerSpawn, ss::Message::Msg, &spawnData, sizeof(spawnData));
	else if (strcmp(spawnData.name, "Worker") == 0)
		s = iomMsgSend(processP->spawnerP->fd, processP->spawnerP->host, "samsonSupervisor", ss::Message::WorkerSpawn, ss::Message::Msg, &spawnData, sizeof(spawnData));
	if (s != 0)
		LM_E(("iomMsgSend: error %d", s));

	LM_M(("started process '%s' in '%s')", processP->name, processP->host));
	usleep(50000);
	LM_M(("Connecting to newly started process (%s) ...", spawnData.name));
	if (strcmp(spawnData.name, "Controller") == 0)
	{
		int  fd;

		fd = iomConnect(processP->spawnerP->host, CONTROLLER_PORT);

		if (fd == -1)
		{
			char errorText[256];

			snprintf(errorText, sizeof(errorText), "Error connecting to Samson Controller in '%s', port %d", processP->spawnerP->host, CONTROLLER_PORT);
			new Popup("Connect Error", errorText);
		}
		else
			starter->endpoint = networkP->endpointAdd("connected to spawner", fd, fd, "Controller", "Controller", 0, ss::Endpoint::Controller, processP->spawnerP->host, CONTROLLER_PORT);
	}
	else if (strcmp(spawnData.name, "Worker") == 0)
	{
		int fd;

		fd = iomConnect(processP->spawnerP->host, WORKER_PORT);

		if (fd == -1)
		{
			char errorText[256];

			snprintf(errorText, sizeof(errorText), "Error connecting to Samson Worker in '%s', port %d", processP->spawnerP->host, WORKER_PORT);
			new Popup("Connect Error", errorText);
		}
		else
			starter->endpoint = networkP->endpointAdd("connected to worker", fd, fd, spawnData.name, alias, 0, ss::Endpoint::Temporal, processP->spawnerP->host, WORKER_PORT);

		LM_TODO(("This endpoint is TEMPORAL and will be changed when the Hello is received - fix this problem!"));
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
	
	LM_M(("killing process '%s' in host '%s'", processP->name, processP->host));
	starter = starterLookup(processP);
	if (starter == NULL)
		LM_E(("can't find starter for process '%s' at %s", processP->name, processP->host));
	else if (starter != starterP)
		LM_E(("This is very strange - the starter pointer seems erroneous (%p vs %p)", starterP, starter));
	else
	{
		if (starter->endpoint == NULL)
			LM_E(("can't kill starter for process '%s' at %s as its endpoint is NULL", processP->name, processP->host));
		else if (starter->endpoint->state != ss::Endpoint::Connected)
			LM_E(("can't kill starter for process '%s' at %s as its endpoint is in state '%s'", processP->name, processP->host, starter->endpoint->stateName()));
		else
		{
			LM_W(("Now really sending 'Die' to '%s' at '%s' (name: '%s')", starter->endpoint->typeName(), starter->endpoint->ip.c_str(), starter->endpoint->name.c_str()));
			s = iomMsgSend(starter->endpoint->wFd, starter->endpoint->ip.c_str(), "samsonSupervisor", ss::Message::Die, ss::Message::Msg);
		}
	}
}
