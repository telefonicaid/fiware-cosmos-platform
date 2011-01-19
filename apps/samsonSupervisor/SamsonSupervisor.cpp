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
#include "traceLevels.h"        // LMT_*

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
#include "starterList.h"        // starterLookup
#include "spawnerList.h"        // spawnerListGet, ...
#include "processList.h"        // processListGet, ...
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
* disconnectAllWorkers - 
*/
void disconnectAllWorkers(void)
{
	unsigned int                ix;
	std::vector<ss::Endpoint*>  epV;

	epV = networkP->samsonWorkerEndpoints();
	LM_M(("Got %d endpoints", epV.size()));
	for (ix = 0; ix < epV.size(); ix++)
	{
		ss::Endpoint* ep;

		ep = epV[ix];

		if (ep->type == ss::Endpoint::Worker)
		{
			LM_W(("Closing connection to Worker %02d: %-20s %-20s   %s", ix, ep->name.c_str(), ep->ip.c_str(), ep->stateName()));
			close(ep->wFd);
			networkP->endpointRemove(ep, "Disconnecting all workers");
		}
	}
}



/* ****************************************************************************
*
* SamsonSupervisor::endpointUpdate - 
*/
int SamsonSupervisor::endpointUpdate(ss::Endpoint* ep, ss::Endpoint::UpdateReason reason, const char* reasonText, void* info)
{
	Starter*       starter;
	ss::Endpoint*  newEp = (ss::Endpoint*) info;

	if (reason == ss::Endpoint::SelectToBeCalled)
	{
		starterListShow("periodic");
		return 0;
	}

	if (ep != NULL)
		LM_M(("********************* Got an Update Notification ('%s') for endpoint %p '%s' at '%s'", reasonText, ep, ep->name.c_str(), ep->ip.c_str()));
	else
		LM_M(("********************* Got an Update Notification ('%s') for NULL endpoint", reasonText));

	LM_M(("looking for starter with endpoint %p", ep));
	starterListShow("Before starterLookup");
	starter = starterLookup(ep);
	starterListShow("After starterLookup");
	LM_M(("starterLookup(%p) returned %p", ep, starter));

	if (starter != NULL)
		LM_M(("found %s-starter '%s'", starter->typeName(), starter->name));

	switch (reason)
	{
	case ss::Endpoint::NoLongerTemporal:
		if (ep->type != ss::Endpoint::Temporal)
			LM_X(1, ("BUG - endpoint not temporal"));

		if ((newEp->type != ss::Endpoint::Worker) && (newEp->type != ss::Endpoint::Spawner))
			LM_X(1, ("BUG - new endpoint should be either Worker or Spawner - is '%s'", newEp->typeName()));

		if (starter != NULL)
		{
			LM_M(("fds for temporal endpoint: r:%d w:%d", ep->rFd, ep->wFd));
			LM_M(("fds for new endpoint:      r:%d w:%d", newEp->rFd, newEp->wFd));

			LM_M(("Changing temporal endpoint %p for '%s' endpoint %p", ep, newEp->typeName(), newEp));
			starter->endpoint = newEp;
			starter->check();
			return 0;
		}
		else
		{
			Process* processP = NULL;
			Spawner* spawnerP = NULL;

			LM_W(("%s: starter not found for '%s' endpoint '%s' at '%s'", reasonText, ep->typeName(), ep->name.c_str(), ep->ip.c_str()));
			LM_W(("Lookup spawner/process instead!"));
			processP = processLookup((char*) ep->name.c_str(), (char*) ep->ip.c_str());
			if (processP != NULL)
				LM_M(("Found process!  Setting its endpoint to this one ..."));
			else
			{
				LM_W(("Cannot find process '%s' at '%s' - trying spawner", ep->name.c_str(), ep->ip.c_str()));
				spawnerP = spawnerLookup((char*) ep->ip.c_str());
				if (spawnerP != NULL)
					LM_M(("Found spawner! Setting its endpoint to this one ..."));
				else
					LM_W(("Nothing found ..."));
			}
		}
		break;

	case ss::Endpoint::WorkerDisconnected:
		if (starter == NULL)
			LM_RE(-1, ("NULL starter for '%s'", reasonText));
		starter->check();
		break;

	case ss::Endpoint::ControllerDisconnected:
		LM_W(("Controller disconnected - I should now disconnect from all workers ..."));
		LM_W(("... to reconnect to workers when controller is back"));
		break;

	case ss::Endpoint::ControllerRemoved:
	case ss::Endpoint::WorkerRemoved:
	case ss::Endpoint::EndpointRemoved:
		LM_M(("Endpoint removed"));
		if (starter == NULL)
			LM_RE(-1, ("NULL starter for '%s'", reasonText));
		starter->check();
		break;

	case ss::Endpoint::ControllerAdded:
	case ss::Endpoint::ControllerReconnected:
	case ss::Endpoint::HelloReceived:
	case ss::Endpoint::WorkerAdded:
		LM_W(("Got a '%s' endpoint-update-reason and I take no action ...", reasonText));
		break;

	case ss::Endpoint::SelectToBeCalled:
		starterListShow("periodic");
		break;
	}

#if 0
	if ((networkP->controller != NULL) && (networkP->controller->state != ss::Endpoint::Connected))
	{
		if (networkP->controller->state != ss::Endpoint::Unconnected)
		{
			LM_W(("Seems like the controller died (controller in state '%s') - disconnecting all workers", networkP->controller->stateName()));
			disconnectAllWorkers();
			return 0;
		}
		else
			LM_M(("controller is unconnected - no action"));
	}

	if (strcmp(reason, "no longer temporal") == 0)
	{
		ss::Endpoint* newEp = (ss::Endpoint*) info;

		if (ep->type != ss::Endpoint::Temporal)
			LM_X(1, ("BUG - endpoint not temporal"));

		if ((newEp->type != ss::Endpoint::Worker) && (newEp->type != ss::Endpoint::Spawner))
			LM_X(1, ("BUG - new endpoint not ss::Endpoint::Worker nor ss::Endpoint::Spawner"));

	}

	if ((starter != NULL) && (info != NULL))
	{
		ss::Endpoint* newEp = (ss::Endpoint*) info;

		if ((newEp->type == ss::Endpoint::Worker) && (ep->type == ss::Endpoint::Temporal))
		{
			LM_M(("Probably a 'Worker No longer temporal' - changing endpoint pointer for starter '%s' type '%s'", starter->name, starter->type));
			starter->endpoint = newEp;
			starter->check();
			return 0;
		}

		LM_RE(-1, ("Don't know how I got here ... (starter: %s, type: %s", starter->name, starter->type));
	}

	if ((starter == NULL) && (info != NULL))
	{
		starter = starterLookup((ss::Endpoint*) info);
		ep      = (ss::Endpoint*) info;
	}

	if (starter == NULL)
	{
		LM_M(("********************* Cannot find starter for endpoint at %p (%p)", ep, info));
		return -1;
	}

	LM_T(LMT_STARTER, ("Found starter '%s'", starter->name));

	starter->endpoint = ep;
	starter->check();
#endif

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
	static bool                 firstTime = true;

	LM_M(("---- Network READY - %s --------------------------", info));

	epV = networkP->samsonWorkerEndpoints();
	LM_M(("Got %d endpoints", epV.size()));
	for (ix = 0; ix < epV.size(); ix++)
	{
		ss::Endpoint* ep;

		ep = epV[ix];

		LM_M(("%02d: %-20s %-20s   %s", ix, ep->name.c_str(), ep->ip.c_str(), ep->stateName()));
	}

	if (firstTime == true)
	{
		// Connecting to all spawners
		Spawner**     spawnerV;
		unsigned int  spawners;

		spawners = spawnerMaxGet();
		spawnerV = spawnerListGet();

		LM_M(("Connecting to all %d spawners", spawners));
		for (ix = 0; ix < spawners; ix++)
		{
			int s;

			if (spawnerV[ix] == NULL)
				continue;

			LM_M(("Connecting to spawner in host '%s'", spawnerV[ix]->host));
			s = iomConnect(spawnerV[ix]->host, SPAWNER_PORT);
			spawnerV[ix]->fd = s;
			if (s == -1)
				LM_E(("Error connecting to spawner in %s (port %d)", spawnerV[ix]->host, SPAWNER_PORT));
			else
				networkP->endpointAdd(s, s, (char*) "Spawner", "Spawner", 0, ss::Endpoint::Temporal, spawnerV[ix]->host, SPAWNER_PORT);
		}
	}

	networkReady = true;    // Is this true if Controller not running ?
	firstTime    = false;

	return 0;
}
