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

#include "Message.h"            // ss::Message
#include "iomMsgSend.h"         // iomMsgSend
#include "iomConnect.h"         // iomConnect

#include "Spawner.h"            // Spawner, spawnerAdd, ...
#include "Process.h"            // Process, processAdd, ...
#include "ports.h"              // SPAWNER_PORT, ...
#include "globals.h"            // networkP
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
	spawnerList();

	LM_F(("----- Processs: -----"));
	processList();
}



/* ****************************************************************************
*
* start - 
*/
void start(void)
{
	int                     s;
	Process*                p;
	ss::Message::SpawnData  spawnData;
	int                     pIx = 0;

	LM_M(("Starting samson platform"));
	while (1)
	{
		int   ix;
		char* end;

		p = processGet(pIx);

		if (p == NULL)
		{
			LM_M(("All processes are started!"));
			return;
		}

		strcpy(spawnData.name, p->name);
		memset(spawnData.args, sizeof(spawnData.args), 0);

		end = spawnData.args;
		for (ix = 0; ix < p->argCount; ix++)
		{
			strcpy(end, p->arg[ix]);
			end += strlen(p->arg[ix]) + 1; // leave one ZERO character
		}

		LM_M(("starting process %d (%s in %s)", pIx, p->name, p->host));
		if (strcmp(spawnData.name, "Controller") == 0)
			s = iomMsgSend(p->spawner->fd, p->spawner->host, "samsonSupervisor", ss::Message::ControllerSpawn, ss::Message::Msg, &spawnData, sizeof(spawnData));
		else if (strcmp(spawnData.name, "Worker") == 0)
			s = iomMsgSend(p->spawner->fd, p->spawner->host, "samsonSupervisor", ss::Message::WorkerSpawn, ss::Message::Msg, &spawnData, sizeof(spawnData));
		if (s != 0)
			LM_E(("iomMsgSend: error %d", s));
		LM_M(("started process %d (%s in %s)", pIx, p->name, p->host));

		LM_M(("Connecting to newly started process ..."));
		if (strcmp(spawnData.name, "Controller") == 0)
			iomConnect(p->spawner->host, CONTROLLER_PORT);
		else if (strcmp(spawnData.name, "Worker") == 0)
			iomConnect(p->spawner->host, WORKER_PORT);

		++pIx;
	}
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
		LM_X(1, ("error connecting to spawner in host '%s', port %d", sP->host, sP->port));

	ss::Endpoint*  ep;

	LM_M(("Calling endpointAdd for spawner '%s'", sP->host));
	ep = networkP->endpointAdd(sP->fd,
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
* connectToAllSpawners - 
*/
void connectToAllSpawners(void)
{
	int ix = 0;

	for (ix = 0; ix < spawnersMax(); ix++)
	{
		Spawner* sP;

		if ((sP = spawnerGet(ix)) == NULL)
			continue;

		spawnerConnect(sP);
	}
}



/* ****************************************************************************
*
* spawnerDisconnect - disconnect from spawner
*/
void spawnerDisconnect(Spawner* spawnerP)
{
	close(spawnerP->fd);
	// select loop will do the rest ... ?
}



/* ****************************************************************************
*
* processStart - start a process
*/
void processStart(Process* processP)
{
	ss::Message::SpawnData  spawnData;
	int                     ix;
	char*                   end;
	int                     s;

	LM_M(("starting process '%s' in '%s' with %d parameters", processP->name, processP->host, processP->argCount));

	spawnData.argCount = processP->argCount;
	strcpy(spawnData.name, processP->name);
	memset(spawnData.args, sizeof(spawnData.args), 0);

	end = spawnData.args;
	for (ix = 0; ix < processP->argCount; ix++)
	{
		strcpy(end, processP->arg[ix]);
		LM_M(("parameter %d: '%s'", ix, end));
		end += strlen(processP->arg[ix]) + 1; // leave one ZERO character
	}
	*end = 0;

	if (strcmp(spawnData.name, "Controller") == 0)
		s = iomMsgSend(processP->spawner->fd, processP->spawner->host, "samsonSupervisor", ss::Message::ControllerSpawn, ss::Message::Msg, &spawnData, sizeof(spawnData));
	else if (strcmp(spawnData.name, "Worker") == 0)
		s = iomMsgSend(processP->spawner->fd, processP->spawner->host, "samsonSupervisor", ss::Message::WorkerSpawn, ss::Message::Msg, &spawnData, sizeof(spawnData));
	if (s != 0)
		LM_E(("iomMsgSend: error %d", s));

	LM_M(("started process '%s' in '%s')", processP->name, processP->host));

	LM_M(("Connecting to newly started process ..."));
	if (strcmp(spawnData.name, "Controller") == 0)
	{
		int fd;
		
		fd = iomConnect(processP->spawner->host, CONTROLLER_PORT);
		networkP->endpointAdd(fd, fd, "Controller", "Controller", 0, ss::Endpoint::Controller, processP->spawner->host, CONTROLLER_PORT);
		// WorkerVector ...
	}
	else if (strcmp(spawnData.name, "Worker") == 0)
	{
		int fd;

		fd = iomConnect(processP->spawner->host, WORKER_PORT);
		networkP->endpointAdd(fd, fd, "Worker", "Worker", 0, ss::Endpoint::Worker, processP->spawner->host, WORKER_PORT);
	}
}



/* ****************************************************************************
*
* startAllProcesses - 
*/
void startAllProcesses(void)
{
	int ix = 0;

	for (ix = 0; ix < processesMax(); ix++)
	{
		Process* sP;

		if ((sP = processGet(ix)) == NULL)
			continue;

		processStart(sP);
	}
}



/* ****************************************************************************
*
* processKill - kill a process
*/
void processKill(Process* processP)
{
	int s;

	s = iomMsgSend(processP->spawner->fd, processP->spawner->host, "samsonSupervisor", ss::Message::Die, ss::Message::Msg);
}
