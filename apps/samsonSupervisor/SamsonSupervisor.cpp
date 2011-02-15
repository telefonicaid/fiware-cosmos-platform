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
* Global variables
*/
ss::Message::WorkerVectorData* workerVec = NULL;



/* ****************************************************************************
*
* workerLookup - 
*/
ss::Message::Worker* workerLookup(const char* alias)
{
	if (workerVec == NULL)
		return NULL;

	for (int ix = 0; ix < workerVec->workers; ix++)
	{
		if (strcmp(workerVec->workerV[ix].alias, alias) == 0)
			return &workerVec->workerV[ix];
	}

	return NULL;
}



/* ****************************************************************************
*
* workerUpdate - 
*/
void workerUpdate(ss::Message::Worker* workerDataP)
{
	ss::Message::Worker* worker = workerLookup(workerDataP->alias);

	if (worker == NULL)
		LM_RVE(("Cannot find worker '%s'", workerDataP->alias));

	memcpy(worker, workerDataP, sizeof(ss::Message::Worker));
	LM_M(("Updated worker '%s' in local worker vec (host: '%s')", worker->alias, worker->ip));
}



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



#if 0
/* ****************************************************************************
*
* hostForWorker - 
*/
static char* hostForWorker(ss::Message::Worker* workerP, int* argsP, char** argV)
{
	char   xhost[64];
	char   processName[64];
	char*  host;
	char   eText[256];

	host = workerP->ip;

	memset(argV, 0, sizeof(argV));
	*argsP = 0;

	if (strcmp(host, "II.PP") == 0)
	{
		LM_M(("Worker '%s' has no valid IP - thus has never connected to Controller.", workerP->alias));
		LM_M(("Must consult Config File (platformProcesses) to get its IP"));
		if (configFileParseByAlias(workerP->alias, xhost, processName, argsP, argV) != 0)
		{
			snprintf(eText, sizeof(eText), "Controller reports an unknown future worker, aliased '%s'\n"
					 "Cannot find process in config files.\n"
					 "No possible way to spawn this process.\n"
					 "\nCannot continue, sorry."
					 "You need no check the configuration files ...", workerP->alias);

			new Popup("Unknown worker", eText, true);
			LM_X(1, (eText));
		}

		host = xhost;
	}

	return strdup(host);
}
#endif



/* ****************************************************************************
*
* workerVectorReceived - 
*/
static void workerVectorReceived(ss::Message::WorkerVectorData*  wvDataP)
{
	char*                 host;
	int                   fd;
	Process*              spawner;
	Process*              process;
	Starter*              starter;
	ss::Message::Worker*  worker;
	ss::Endpoint*         ep;
	int                   size;

	LM_M(("Got Worker Vector from Controller (with %d workers)", wvDataP->workers));
	size      = sizeof(ss::Message::WorkerVectorData) + wvDataP->workers * sizeof(ss::Message::Worker);
	workerVec = (ss::Message::WorkerVectorData*) malloc(size);
	memset(workerVec, 0, size);
	memcpy(workerVec, wvDataP, size);

	for (int ix = 0; ix < wvDataP->workers; ix++)
	{
		worker = &wvDataP->workerV[ix];
		LM_M(("Worker %d (name: '%s', alias: '%s') in host %s", ix, worker->name, worker->alias, worker->ip));
	}

	for (int ix = 0; ix < wvDataP->workers; ix++)
	{
		LM_M(("Create a Starter for endpoint '%s@%s' (unless it already exists ...)", worker->alias, worker->ip));
		worker = &wvDataP->workerV[ix];
		host   = worker->ip;

		if ((host == NULL) || (host[0] == 0) || (strcmp(host, "ip") == 0))
		{
			Process* processP;

			if ((processP = processLookup(worker->alias)) != NULL)
			{
				LM_W(("Unconfigured worker '%s' seems to already exist - not adding process nor starter", worker->alias));
				if (processP->starterP)
				{
					if (processP->starterP->logButton)
						processP->starterP->logButton->setDisabled(true);
					processP->starterP->check("Unconfigured worker seems to already exist");
				}

				continue;
			}

			LM_M(("Worker %d is totally unconfigured - adding it a process and starter", ix));
			process = processAdd("Worker", "ip", WORKER_PORT, worker->alias, NULL, NULL, 0);

			starter = starterAdd(process);
			if (starter == NULL)
                LM_X(1, ("NULL starter for Worker@%s", host));

			if ((tabManager != NULL) && (tabManager->processListTab != NULL))
				tabManager->processListTab->starterInclude(starter);

			continue;
		}

		LM_M(("Now I really have the IP for the worker (and its Spawner ...): '%s'", host));
		LM_M(("Lets lookup spawner, process, and starter for this worker (aliased '%s')", worker->alias));
		LM_M(("But we start with looking up the network endpoint for the spawner in '%s'", host));

		ep = networkP->endpointLookup(ss::Endpoint::Spawner, host);
		if (ep == NULL)
		{
			LM_W(("Not even connected to Spawner in '%s' - lets connect !", host));
			fd = iomConnect(host, SPAWNER_PORT);
			ep = networkP->endpointAdd("unconnected spawner for workerVector", fd, fd, "Spawner", "Spawner", 0, ss::Endpoint::Spawner, host, SPAWNER_PORT);

			LM_M(("Now, adding this spawner as a process in our lists"));
			spawner = spawnerAdd("Spawner", (char*) host, SPAWNER_PORT, ep);
		}
		else
		{
			LM_M(("We are connected to the spawner, now lets see if we have it in our process list"));
			spawner = spawnerLookup(host);
			if (spawner == NULL)
			{
				LM_M(("Nope, not there, I better add it ..."));
				spawner = spawnerAdd("Spawner", host, SPAWNER_PORT, ep);
			}
		}

		if (spawner == NULL)
			LM_X(1, ("NULL spawner - this is a SW bug. Probably Ken's first bug ever!"));

		LM_M(("Now, we're connected to the spawner and it is added to out process list - does it have a starter ?"));
		starter = starterLookup(ep);
		if (starter == NULL)
		{
			LM_M(("Creating starter for spawner in '%s'", host));
			
			starter = starterAdd(spawner);
			if (starter == NULL)
				LM_X(1, ("NULL starter for Spawner@%s", host));
			
			if ((tabManager != NULL) && (tabManager->processListTab != NULL))
				tabManager->processListTab->starterInclude(starter);
		}

		LM_M(("OK, spawner complete - now the worker itself."));



		LM_M(("Let's see first if we're already connected to it"));
		ep = networkP->endpointLookup(ss::Endpoint::Worker, host);
		if (ep == NULL)
		{
			LM_M(("Nope, not connected. Let's try to connect"));
			fd = iomConnect(host, WORKER_PORT);
			ep = networkP->endpointAdd("unconnected worker in workerVector", fd, fd, "Worker", worker->alias, 0, ss::Endpoint::Worker, host, WORKER_PORT);
		}
		else
			LM_M(("Yes, found the endpoint"));

		LM_M(("Now, is this process in our process list ?"));
		process = processLookup("Worker", host);
		if (process == NULL)
		{
			LM_M(("Nope, not in the list - let's add it"));
			process = processAdd("Worker", host, WORKER_PORT, worker->alias, ep);
		}
		else
		{
			LM_M(("Process already existed, is its endpoint OK?"));
			if (process->endpoint != ep)
			{
				LM_W(("Strange, another endpoint ...  I change it to the new one!"));
				LM_W(("If this happens, I will probably get an extra starter for this Worker ..."));
				process->endpoint = ep;
			}
		}

		LM_M(("Now, how is it started? Pues, via its spawner!"));
		if (process->spawnInfo == NULL)
			LM_X(1, ("No spawn info for a starter - a bug!"));
		process->spawnInfo->spawnerP = spawner;
		LM_M(("process complete - with spawner and everything!"));

		LM_M(("OK, process there, now its starter ..."));
		starter = starterLookup(ep);
		if (starter == NULL)
		{
			LM_M(("No starter, let's create it ..."));

			starter = starterAdd(process);
			if (starter == NULL)
                LM_X(1, ("NULL starter for Worker@%s", host));

			if ((tabManager != NULL) && (tabManager->processListTab != NULL))
				tabManager->processListTab->starterInclude(starter);
		}
		else
			LM_M(("The starter for Worker@%s already existed ...", host));
	}

	LM_T(LmtWorkerVector, ("Treated worker vector with %d workers", wvDataP->workers));
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
			if ((tabManager != NULL) && (tabManager->processListTab != NULL))
				starter->check("endpointUpdate");
		}
		else
		{
			Process* processP;

			if (ep->type == ss::Endpoint::Temporal)
				LM_W(("Just a temporal endpoint - never mind ...   ?"));
			else
			{
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
		LM_X(1, ("NULL controller - try to connect to it, asbefore ?"));

	return 0;
}
