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

		++pIx;
	}
}



/* ****************************************************************************
*
* connectToSpawners - 
*/
void connectToSpawners()
{
	int            ix = 0;
	Process*       p;
	Spawner*       s;

	while (1)
	{
		int fd;

		p = processGet(ix++);
		if (p == NULL)
		{
			LM_M(("Connected to all spawners"));
			return;
		}
		
		if ((s = spawnerGet(p->host)) != NULL)
		{
			p->spawner = s;
			LM_M(("process %d - already connected to spawner in host '%s'", ix, p->host));
			continue;
		}

		LM_M(("connecting to spawner %d in %s on port %d", ix, p->host, SPAWNER_PORT));
		fd = iomConnect(p->host, SPAWNER_PORT);
		if (fd == -1)
			LM_X(1, ("error connecting to spawner in host '%s', port %d", p->host, SPAWNER_PORT));

		p->spawner = spawnerAdd(p->host, SPAWNER_PORT, fd);

		ss::Endpoint*  ep;

		LM_M(("Calling endpointAdd for spawner '%s'", p->host));
		ep = networkP->endpointAdd(fd,
								   fd,
								   "Spawner",
								   NULL,
								   0,
								   ss::Endpoint::Temporal,
								   std::string(p->host),
								   1233);

#if 0
		networkP->helloSend(ep, ss::Message::Msg);
#endif
	}
}



