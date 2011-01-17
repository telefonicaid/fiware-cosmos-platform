/* ****************************************************************************
*
* FILE                     SamsonSupervisor.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 15 2010
*
*/
#include "logMsg.h"             // LM_*

#include "baTerm.h"             // baTermSetup
#include "globals.h"            // tabManager, ...
#include "ports.h"              // WORKER_PORT, ...
#include "NetworkInterface.h"   // DataReceiverInterface, EndpointUpdateInterface
#include "Endpoint.h"           // Endpoint
#include "Network.h"            // Network
#include "iomConnect.h"         // iomConnect
#include "Message.h"            // ss::Message::Header
#include "qt.h"                 // qtRun, ...
#include "actions.h"            // help, list, start, ...
#include "Starter.h"            // Starter
#include "Spawner.h"            // Spawner
#include "Process.h"            // Process
#include "configFile.h"         // cfParse, cfPresent
#include "SamsonSupervisor.h"   // Own interface



/* ****************************************************************************
*
* SamsonSupervisor::receive - 
*/
int SamsonSupervisor::receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP)
{
	ss::Endpoint* ep = networkP->endpointLookup(fromId);

	if (ep == NULL)
		LM_RE(0, ("Cannot find endpoint with id %d", fromId));

	if (ep->type == ss::Endpoint::Fd)
	{
		char* msg = (char*) dataP;

		printf("\n");
		switch (*msg)
		{
		case 'h':
			help();
			break;

		case 'c':
			connectToAllSpawners();
			break;

		case 'p':
			startAllProcesses();
			break;

		case 's':
			start();
			break;

		case 'l':
			list();
			break;

		case 3:
			LM_X(0, ("'Ctrl-C' pressed - I quit!"));

		case 'q':
			LM_X(0, ("'q' pressed - I quit!"));

		case ' ':
		case '\n':
			printf("\n");
			break;

		default:
			LM_E(("Key '%c' has no function", *msg));
			help();
		}

		printf("\n");
		return 0;
	}

	switch (headerP->code)
	{
	case ss::Message::WorkerSpawn:
	case ss::Message::ControllerSpawn:

	default:
		LM_X(1, ("Don't know how to treat '%s' message", ss::Message::messageCode(headerP->code)));
	}

	return 0;
}



/* ****************************************************************************
*
* SamsonSupervisor::endpointUpdate - 
*/
int SamsonSupervisor::endpointUpdate(ss::Endpoint* ep, const char* reason, void* info)
{
	Starter* starter;

	LM_M(("********************* Got an Update Notification ('%s') for endpoint '%s' at '%s'", reason, ep->name.c_str(), ep->ip.c_str()));

	if (networkP->ready() == false)
	{
		LM_M(("Not treating Endpoint Update Notification as Network module isn't ready ..."));
		return 0;
	}

	if (tabManager == NULL)
	{
		LM_M(("tabManager not created yet ..."));
		return 0;
	}

	if (tabManager->processListTab == NULL)
	{
		LM_M(("processListTab not created yet ..."));
		return 0;
	}

	starter = tabManager->processListTab->starterLookup(ep);
	if ((starter == NULL) && (info != NULL))
	{
		starter = tabManager->processListTab->starterLookup((ss::Endpoint*) info);
		if (starter)
			starter->endpoint = (ss::Endpoint*) info;
		ep = (ss::Endpoint*) info;
	}

	if (starter == NULL)
	{
		LM_M(("********************* Cannot find starter for endpoint at %p (%p)", ep, info));
		return -1;
	}

	LM_M(("Found starter '%s'", starter->name));

	if (ep->state == ss::Endpoint::Connected)
		starter->checkState = Qt::Checked;
	else
		starter->checkState = Qt::Unchecked;

	LM_M(("********************* Setting state to '%s' for endpoint '%s' at '%s'",
		  (starter->checkState == Qt::Checked)? "Checked" : "Unchecked",
		  ep->name.c_str(),
		  ep->ip.c_str()));

	if (starter->checkbox)
		starter->checkbox->setCheckState(starter->checkState);

	return 0;
}



/* ****************************************************************************
*
* runQtAsThread - 
*/
void* runQtAsThread(void* nP)
{
	int          argC    = 1;
	const char*  argV[2] = { "samsonSupervisor", NULL };

	qtRun(argC, argV);
	LM_X(1, ("Back from qtRun !"));

	return NULL;
}



/* ****************************************************************************
*
* SamsonSupervisor::ready - 
*/
int SamsonSupervisor::ready(const char* info)
{
	unsigned int                ix;
	std::vector<ss::Endpoint*>  epV;

	LM_M(("---- Network READY - %s --------------------------", info));

	epV = networkP->samsonWorkerEndpoints();
	LM_M(("Got %d endpoints", epV.size()));
	for (ix = 0; ix < epV.size(); ix++)
	{
		ss::Endpoint* ep;

		ep = epV[ix];

		LM_M(("%02d: %-20s %-20s   %s", ix, ep->name.c_str(), ep->ip.c_str(), ep->stateName()));
	}


	// Connecting to all spawners
	Spawner**     spawnerV;
	unsigned int  spawners;

	spawnerInit();
	processInit();

	cfParse(cfPath);
	cfPresent();

	spawnerV = spawnerListGet(&spawners);
	LM_M(("Connecting to all %d spawners", spawners));
	for (ix = 0; ix < spawners; ix++)
	{
		int s;

		LM_M(("Connecting to spawner in host '%s'", spawnerV[ix]->host));
		s = iomConnect(spawnerV[ix]->host, SPAWNER_PORT);
		if (s == -1)
			LM_E(("Error connecting to spawner in %s (port %d)", spawnerV[ix]->host, SPAWNER_PORT));
		else
			networkP->endpointAdd(s, s, (char*) "Spawner", "Spawner", 0, ss::Endpoint::Temporal, spawnerV[ix]->host, SPAWNER_PORT);
	}

#if 1
	pthread_t t;
	pthread_create(&t, NULL, runQtAsThread, networkP);
#else
	runQtAsThread(networkP);
#endif

	return 0;
}
