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
#include "traceLevels.h"        // Trace Levels

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
#include "Process.h"            // Process
#include "starterList.h"        // starterLookup
#include "spawnerList.h"        // spawnerListGet, ...
#include "processList.h"        // processListGet, ...
#include "configFile.h"         // configFileParseByAlias
#include "Popup.h"              // Popup
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
	LM_T(LmtWorkers, ("Got %d endpoints", epV.size()));
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

	if (ep->type != ss::Endpoint::Temporal)
		LM_X(1, ("BUG - endpoint not temporal"));

	if ((newEp->type != ss::Endpoint::Worker) && (newEp->type != ss::Endpoint::Spawner))
		LM_X(1, ("BUG - new endpoint should be either Worker or Spawner - is '%s'", newEp->typeName()));

	if (starter != NULL)
	{
		LM_T(LmtTemporalEndpoint, ("Changing temporal endpoint %p for '%s' endpoint %p", ep, newEp->typeName(), newEp));
		starter->process->endpoint = newEp;
		starter->check("noLongerTemporal");
		return;
	}

	LM_T(LmtStarterLookup, ("starter not found for '%s' endpoint '%s' at '%s'", ep->typeName(), ep->name.c_str(), ep->ip.c_str()));
	LM_T(LmtProcessLookup, ("Lookup spawner/process instead!"));

	processP = processLookup((char*) ep->name.c_str(), (char*) ep->ip.c_str());
	if (processP == NULL)
		LM_T(LmtProcessLookup, ("Cannot find process %s@%s!", ep->name.c_str(), ep->ip.c_str()));
	else
		processP->endpoint = ep;
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

	LM_T(LmtWorkers, ("Controller disconnected - I should now disconnect from all workers ..."));
	LM_T(LmtWorkers, ("... to reconnect to workers when controller is back"));

	starterV   = starterListGet();
	starterMax = starterMaxGet();

	for (ix = 0; ix < starterMax; ix++)
	{
		if (starterV[ix] == NULL)
			continue;

		if (starterV[ix]->process == NULL)
			continue;

		if (starterV[ix]->process->endpoint == NULL)
			continue;

		if (starterV[ix]->process->endpoint->type != ss::Endpoint::Worker)
			continue;

		LM_W(("Removing endpoint for worker in %s", starterV[ix]->process->endpoint->ip.c_str()));
		networkP->endpointRemove(starterV[ix]->process->endpoint, "Controller disconnected");
	}
}



/* ****************************************************************************
*
* workerVectorReceived - 
*/
static void workerVectorReceived(ss::Message::WorkerVectorData*  wvDataP)
{
	Process*  processP;
	Starter*  starterP;
	char      host[64];
	char      processName[64];
	int       args  = 20;
	char*     argV[20];
	char      eText[256];
	int       fd;

	for (int ix = 0; ix < wvDataP->workers; ix++)
	{
		LM_T(LmtWorkerVector, ("Create a Starter for endpoint '%s@%s'", wvDataP->workerV[ix].alias, wvDataP->workerV[ix].ip));

		if (strcmp(wvDataP->workerV[ix].ip, "II.PP") == 0)
		{
			if (configFileParseByAlias(wvDataP->workerV[ix].alias, host, processName, &args, argV) != 0)
			{
				snprintf(eText, sizeof(eText), "Controller reports an unknown future worker, aliased '%s'\nCannot find process in config files.\nNo possible way to spawn this process - skipping it",
						 wvDataP->workerV[ix].alias);
				LM_E(("Cannot find worker process with alias '%s' in platformProcesses - skipping it", wvDataP->workerV[ix].alias));
				new Popup("Unknown worker", eText);
				continue;
			}

			processP = processAdd("Worker", host, 0, NULL, argV, args);
			if (processP == NULL)
				LM_X(1, ("NULL processP for Worker@%s", host));
			if (processP->spawnInfo == NULL)
				LM_X(1, ("How come spawn-info is NULL for Worker (alias: '%s') in host '%s'", wvDataP->workerV[ix].alias, host));
			if ((starterP = starterLookup(processP)) != NULL)
				LM_W(("Strange - starter already there - restarted process ?"));
			else
			{
				starterP = starterAdd(processP);
				if (starterP == NULL)
					LM_X(1, ("NULL starterP for Worker@%s", host));

				if ((tabManager != NULL) && (tabManager->processListTab != NULL))
					tabManager->processListTab->starterInclude(starterP);
			}

			processP->spawnInfo->spawnerP = spawnerLookup(host);
			if (processP->spawnInfo->spawnerP == NULL)
			{
				fd = iomConnect(host, SPAWNER_PORT);
				if (fd != -1)
					processP->spawnInfo->spawnerP = spawnerAdd("Spawner", host, SPAWNER_PORT, NULL);
			}

			if (processP->spawnInfo->spawnerP == NULL)
			{
				snprintf(eText, sizeof(eText),
						 "In order to spawn worker (with alias '%s') in host '%s',\na samsonSpawner must be running in that host.\nPlease make sure that a samsonSpawner is running in '%s'",
						 wvDataP->workerV[ix].alias, host, host);
				
				new Popup("Spawner not running", eText);
				LM_X(1, ("Sorry, no spawner found in host '%s'", host));
			}
		}
		else
		{
			LM_T(LmtWorkerVector, ("Connecting to worker in '%s'", wvDataP->workerV[ix].ip));
			fd = iomConnect(wvDataP->workerV[ix].ip, wvDataP->workerV[ix].port);
			if (fd == -1)
				LM_W(("Error connecting to worker in '%s' (port %d) - ***** Here I should create a starter", wvDataP->workerV[ix].ip, wvDataP->workerV[ix].port));
			else
				LM_T(LmtStarter, ("Connected to worker in '%s' (no need to create Starter)", wvDataP->workerV[ix].ip));

			LM_TODO(("Perhaps I should create the endpoint here - so I find this process later when Hello arrives"));
			processP = processAdd("Worker", wvDataP->workerV[ix].ip, wvDataP->workerV[ix].port, NULL, NULL, 0);
			LM_TODO(("When Hello arrives - lookup this process"));
			if (processP == NULL)
				LM_X(1, ("processAdd returned NULL for Worker at %s", wvDataP->workerV[ix].ip));

			starterP = starterAdd(processP);

			if (processP->spawnInfo != NULL)
			{
				processP->spawnInfo->spawnerP = spawnerLookup(wvDataP->workerV[ix].ip);
				if (processP->spawnInfo->spawnerP == NULL)
					LM_X(1, ("Sorry, no spawner found in host '%s'", wvDataP->workerV[ix].ip));
			}
			else
				LM_W(("processAdd returned NULL for spawnInfo for Worker at %s", wvDataP->workerV[ix].ip));

			if ((tabManager != NULL) && (tabManager->processListTab != NULL))
				tabManager->processListTab->starterInclude(starterP);
		}
	}

	LM_T(LmtWorkerVector, ("Got worker vector with %d workers", wvDataP->workers));
}



/* ****************************************************************************
*
* SamsonSupervisor::endpointUpdate - 
*/
int SamsonSupervisor::endpointUpdate(ss::Endpoint* ep, ss::Endpoint::UpdateReason reason, const char* reasonText, void* info)
{
	ss::Endpoint*                   newEp     = (ss::Endpoint*) info;
	ss::Message::WorkerVectorData*  wvDataP   = (ss::Message::WorkerVectorData*) info;
	Starter*                        starter   = NULL;

	if (reason == ss::Endpoint::SelectToBeCalled)
	{
		starterListShow("periodic");
		return 0;
	}

	if (ep != NULL)
		LM_T(LmtEndpointUpdate, ("Got an Update Notification ('%s') for endpoint %p '%s' at '%s'", reasonText, ep, ep->name.c_str(), ep->ip.c_str()));
	else
		LM_T(LmtEndpointUpdate, ("Got an Update Notification ('%s') for NULL endpoint", reasonText));

	if (ep != NULL)
	{
		LM_T(LmtEndpointUpdate, ("looking for starter with endpoint %p", ep));
		starterListShow("Before starterLookup");
		starter = starterLookup(ep);
		starterListShow("After starterLookup");
		LM_T(LmtEndpointUpdate, ("starterLookup(%p) returned %p", ep, starter));

		if (starter != NULL)
		{
			LM_T(LmtEndpointUpdate, ("found %s-starter '%s'", processTypeName(starter->process), starter->process->name));
			if (starter->process->endpoint != ep)
				LM_E(("********* Should have the same endpoint ...  %p vs %p", starter->process->endpoint, ep));
			starter->check("endpointUpdate");
		}
		else
		{
			Process* processP;

			LM_T(LmtStarter, ("starter == NULL - looking up process %s@%d", ep->name.c_str(), ep->ip.c_str()));
			processP = processLookup(ep->name.c_str(), ep->ip.c_str());
			if (processP != NULL)
			{
				if (processP->endpoint == NULL)
					processP->endpoint = ep;
				else if (processP->endpoint != ep)
					LM_E(("********* Should have the same endpoint ...  %p vs %p", processP->endpoint, ep));
			}
			else
				LM_W(("NULL process for endpoint '%s@%s' - create one ?", ep->name.c_str(), ep->ip.c_str()));

			LM_W(("Here I should probably create starter for '%s@%s'", ep->name.c_str(), ep->ip.c_str()));
		}
	}

	switch (reason)
	{
	case ss::Endpoint::SupervisorAdded:
		LM_X(1, ("Supervisor Added ..."));
		break;

	case ss::Endpoint::WorkerVectorReceived:
		workerVectorReceived(wvDataP);
		break;

	case ss::Endpoint::NoLongerTemporal:
		noLongerTemporal(ep, newEp, starter);
		break;

	case ss::Endpoint::WorkerDisconnected:
	case ss::Endpoint::WorkerAdded:
		break;

	case ss::Endpoint::ControllerDisconnected:
		disconnectWorkers();
		break;

	case ss::Endpoint::EndpointRemoved:
	case ss::Endpoint::ControllerRemoved:
	case ss::Endpoint::WorkerRemoved:
		LM_W(("Some endpoint closed connection"));
		break;

	case ss::Endpoint::HelloReceived:
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
	LM_T(LmtNetworkReady, ("---- Network READY - %s --------------------------", info));
	networkP->endpointListShow("Network READY");
	processListShow("ready");

	if (networkP->controllerGet() == NULL)
	{
		LM_T(LmtNetworkReady, ("Connecting to controller and I return - will come back to this function and then we'll connect to the workers"));
		connectToController();
	}

	return 0;
}
