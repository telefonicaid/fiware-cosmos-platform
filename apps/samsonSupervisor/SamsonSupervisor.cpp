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
		LM_X(1, ("Don't want to accept commands from stdin - please remove this now!"));

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
* noLongerTemporal - 
*/
static void noLongerTemporal(ss::Endpoint* ep, ss::Endpoint* newEp, Starter* starter)
{
	Process* processP = NULL;
	Spawner* spawnerP = NULL;

	if (ep->type != ss::Endpoint::Temporal)
		LM_X(1, ("BUG - endpoint not temporal"));

	if (newEp->type == ss::Endpoint::LogServer)
	{
		LM_M(("endpoint is LogServer - nothing to be done ..."));
		return;
	}

	if ((newEp->type != ss::Endpoint::Worker) && (newEp->type != ss::Endpoint::Spawner))
		LM_X(1, ("BUG - new endpoint should be either Worker or Spawner - is '%s'", newEp->typeName()));

	if (starter != NULL)
	{
		LM_M(("Changing temporal endpoint %p for '%s' endpoint %p", ep, newEp->typeName(), newEp));
		starter->endpoint = newEp;
		starter->check();
		return;
	}

	LM_W(("starter not found for '%s' endpoint '%s' at '%s'", ep->typeName(), ep->name.c_str(), ep->ip.c_str()));
	LM_W(("Lookup spawner/process instead!"));
	processP = processLookup((char*) ep->name.c_str(), (char*) ep->ip.c_str());
	if (processP != NULL)
		LM_M(("Found process!  Setting its endpoint to this one ..."));
	else
	{
		LM_W(("Cannot find process '%s' at '%s' - trying spawner", ep->name.c_str(), ep->ip.c_str()));
		spawnerP = spawnerLookup((char*) ep->ip.c_str());
		if (spawnerP != NULL)
			LM_M(("Found spawner! Setting its endpoint to this one ... ?"));
		else
			LM_W(("Nothing found ..."));
	}
}



/* ****************************************************************************
*
* disconnectWorkers - 
*/
static void disconnectWorkers(void)
{
	Starter**     starterV;
	unsigned int  starterMax;
	unsigned int  ix;

	LM_W(("Controller disconnected - I should now disconnect from all workers ..."));
	LM_W(("... to reconnect to workers when controller is back"));

	starterV   = starterListGet();
	starterMax = starterMaxGet();

	for (ix = 0; ix < starterMax; ix++)
	{
		if (starterV[ix] == NULL)
			continue;

		if (starterV[ix]->endpoint == NULL)
			continue;

		if (starterV[ix]->endpoint->type != ss::Endpoint::Worker)
			continue;

		LM_W(("Removing endpoint for worker in %s", starterV[ix]->endpoint->ip.c_str()));
		networkP->endpointRemove(starterV[ix]->endpoint, "Controller disconnected");
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

	if (ep->type != ss::Endpoint::LogServer)
	{
		LM_M(("looking for starter with endpoint %p", ep));
		starterListShow("Before starterLookup");
		starter = starterLookup(ep);
		starterListShow("After starterLookup");
		LM_M(("starterLookup(%p) returned %p", ep, starter));

		if (starter != NULL)
			LM_M(("found %s-starter '%s'", starter->typeName(), starter->name));
	}

	switch (reason)
	{
	case ss::Endpoint::NoLongerTemporal:
		noLongerTemporal(ep, newEp, starter);
		break;

	case ss::Endpoint::WorkerDisconnected:
	case ss::Endpoint::WorkerAdded:
		if (starter == NULL)
			LM_W(("NULL starter for '%s'", reasonText));
		else
			starter->check();
		break;

	case ss::Endpoint::ControllerDisconnected:
		disconnectWorkers();
		break;

	case ss::Endpoint::EndpointRemoved:
		LM_W(("Some endpoint closed connection"));
		if (ep->type == ss::Endpoint::LogServer)
		{
			logServerEndpoint = NULL;
			logServerFd       = -1;
			LM_W(("Log Server closed connection"));

			tabManager->processListTab->logServerRunningLabel->hide();
			tabManager->processListTab->logServerStartButton->show();

			return -1;
		}
		else
			LM_M(("Endpoint that is not Log Server was removed"));

	case ss::Endpoint::ControllerRemoved:
	case ss::Endpoint::WorkerRemoved:
		if (starter == NULL)
			LM_RE(-1, ("NULL starter for '%s'", reasonText));
		starter->check();
		break;

	case ss::Endpoint::HelloReceived:
		if (ep == logServerEndpoint)
		{
			LM_W(("Got Hello from Log Server (name: '%s'/'%s', ip: '%s'/'%s') - here I should notify Qt thread to update logServer push-button", ep->name.c_str(), newEp->name.c_str(), ep->ip.c_str(), newEp->ip.c_str()));
			LM_TODO(("Add a QT timeout handler and connect a socket between threads to send a LogServer started message"));
			LM_TODO(("Even better would be to create a Network::Poll function and have only one thread"));

			
			tabManager->processListTab->logServerRunningLabel->show();
			tabManager->processListTab->logServerStartButton->hide();
		}
		else
			LM_W(("Got a '%s' endpoint-update-reason and I take no action ...", reasonText));
		break;

	case ss::Endpoint::ControllerAdded:
	case ss::Endpoint::ControllerReconnected:
		LM_W(("Got a '%s' endpoint-update-reason and I take no action ...", reasonText));
		break;

	case ss::Endpoint::SelectToBeCalled:
		starterListShow("periodic");
		break;
	}

	return 0;
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
		connectToAllSpawners();

	networkReady = true;    // Is this true if Controller not running ?
	firstTime    = false;

	return 0;
}
