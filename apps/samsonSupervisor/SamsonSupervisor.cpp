/* ****************************************************************************
*
* FILE                     SamsonSupervisor.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 15 2010
*
*/
#include <sys/time.h>           // getimeofday

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
#include "Process.h"            // ss::Process
#include "starterList.h"        // starterLookup
#include "spawnerList.h"        // spawnerListGet, ...
#include "processList.h"        // processListGet, ...
#include "Popup.h"              // Popup
#include "LogFileWindow.h"      // LogFileWindow
#include "SamsonSupervisor.h"   // Own interface



/* ****************************************************************************
*
* Global variables
*/
ss::Message::WorkerVectorData* workerVec = NULL;



/* ****************************************************************************
*
* SamsonSupervisor constructor - 
*/
SamsonSupervisor::SamsonSupervisor(ss::Network* netP) : ss::Delilah(netP, false)
{
	networkP = netP; 
		
	logReceiverInit(LOG_MESSAGE_PORT);
	
	LM_T(LmtQtTimer, ("Starting timer for Network polling"));
	startTimer(50);  // 50 millisecond timer
}



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
	LM_T(LmtWorker, ("Updated worker '%s' in local worker vec (host: '%s')", worker->alias, worker->ip));
}



extern ss::Process*  processToBeConfigured;
extern QGridLayout*  gridForProcessToBeConfigured;
/* ****************************************************************************
*
* SamsonSupervisor::receive - 
*/
int SamsonSupervisor::receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP)
{
	ss::Endpoint*             ep = networkP->endpointLookup(fromId);
	ss::Message::ConfigData*  configDataP;
	ss::Message::Worker*      workerP;
	
	if (ep == NULL)
		LM_RE(0, ("Cannot find endpoint with id %d", fromId));

	if (ep->type == ss::Endpoint::Fd)
		LM_X(1, ("Don't want to accept commands from stdin - please remove this now!"));

	switch (headerP->code)
	{
	case ss::Message::ConfigGet:
		configDataP = (ss::Message::ConfigData*) dataP;

		if (headerP->type != ss::Message::Ack)
			LM_X(1, ("Bad msg type '%d'", headerP->type));

		tabManager->processListTab->configView = new ProcessConfigView(gridForProcessToBeConfigured, processToBeConfigured, configDataP);
		break;

	case ss::Message::WorkerConfigGet:
		workerP = (ss::Message::Worker*) dataP;

		if (headerP->type != ss::Message::Ack)
			LM_X(1, ("Bad msg type '%d'", headerP->type));

		tabManager->processListTab->configView = new ProcessConfigView(gridForProcessToBeConfigured, processToBeConfigured, workerP);
		break;

	case ss::Message::EntireLogFile:
		new LogFileWindow(ep, (char*) dataP);
		break;

	case ss::Message::EntireOldLogFile:
		new LogFileWindow(ep, (char*) dataP, true);
		break;

	default:
		LM_X(1, ("Don't know how to treat '%s' message", ss::Message::messageCode(headerP->code)));
	}

	return 0;
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
		{
			LM_W(("Should I really remove this starter with a NULL process (is it even possible to reach this point) ?"));
			starterRemove(starterV[ix]);
			continue;
		}

		if (starterV[ix]->process->type != ss::PtWorker)
			continue;

		if (starterV[ix]->process->endpoint == NULL)
		{
			LM_W(("Should I really remove this starter with a NULL endpoint?"));
			processRemove(starterV[ix]->process);
			starterRemove(starterV[ix]);
			continue;
		}

		if (starterV[ix]->process->endpoint->type != ss::Endpoint::Worker)
			continue;

		LM_W(("Removing endpoint for worker in %s", starterV[ix]->process->endpoint->ip));
		networkP->endpointRemove(starterV[ix]->process->endpoint, "Controller disconnected");
		processRemove(starterV[ix]->process);
		starterRemove(starterV[ix]);
	}
}



/* ****************************************************************************
*
* hostValid - 
*/
static bool hostValid(const char* host)
{
	if (host == NULL)
		return false;

	if (host[0] == 0)
		return false;

	if (strcmp(host, "ip") == 0)
		return false;

	return true;
}



/* ****************************************************************************
*
* emptyStarter - 
*/
static void emptyStarter(ss::Message::Worker* worker, int workerId)
{
	ss::Process* processP;
	Starter*     starter;

	if ((processP = processLookup(worker->alias)) != NULL)
	{
		LM_W(("Unconfigured worker '%s' seems to already exist - not adding process nor starter", worker->alias));
		if (processP->starterP)
		{
			if (processP->starterP->logButton)
				processP->starterP->logButton->setDisabled(true);
			processP->starterP->check("Unconfigured worker seems to already exist");
		}

		return;
	}

	LM_T(LmtWorkerVector, ("Worker %d is totally unconfigured - adding it a process and starter", workerId));
	processP = processAdd(ss::PtWorker, "Worker", "ip", WORKER_PORT, worker->alias, NULL);

	starter = starterAdd("workerVectorReceived - Starter for Worker without valid IP address", processP);
	if (starter == NULL)
		LM_X(1, ("NULL starter for Worker@%s", worker->ip));

	if ((tabManager != NULL) && (tabManager->processListTab != NULL))
		tabManager->processListTab->starterInclude(starter);
}




/* ****************************************************************************
*
* workerVectorReceived - 
*/
static void workerVectorReceived(ss::Message::WorkerVectorData*  wvDataP)
{
	Host*                 hostP;
	int                   fd;
	ss::Process*          spawner;
	ss::Process*          process;
	Starter*              starter;
	ss::Message::Worker*  worker;
	ss::Endpoint*         ep;
	int                   size;

	LM_T(LmtWorkerVector, ("Got Worker Vector from Controller (with %d workers)", wvDataP->workers));

	size      = sizeof(ss::Message::WorkerVectorData) + wvDataP->workers * sizeof(ss::Message::Worker);
	workerVec = (ss::Message::WorkerVectorData*) malloc(size);

	if (workerVec == NULL)
		LM_X(1, ("malloc(%d): %s", size, strerror(errno)));

	memset(workerVec, 0, size);
	memcpy(workerVec, wvDataP, size);

	networkP->endpointListShow("Got Worker Vector");
	processListShow("Got Worker Vector");

	LM_T(LmtWorkerVector, ("------------------ Worker Vector ------------------"));
	for (int ix = 0; ix < wvDataP->workers; ix++)
	{
		worker = &wvDataP->workerV[ix];
		LM_T(LmtWorkerVector, ("Worker %d (name: '%s', alias: '%s') in host %s", ix, worker->name, worker->alias, worker->ip));
	}
	LM_T(LmtWorkerVector, ("-------------------------------------------------------"));

	for (int ix = 0; ix < wvDataP->workers; ix++)
	{
		// process Lookup/Add
		// starter
		// endpoint?
		// show?
		

		LM_T(LmtWorkerVector, ("Create a Starter for endpoint '%s@%s' (unless it already exists ...)", worker->alias, worker->ip));
		worker = &wvDataP->workerV[ix];

		if (hostValid(worker->ip) == false)
		{
			emptyStarter(worker, ix);
			continue;
		}

		LM_T(LmtWorkerVector, ("Now I have a valid IP for the worker (and its Spawner ...): '%s'", worker->ip));
		LM_T(LmtWorkerVector, ("Lets lookup spawner, process, and starter for this worker (aliased '%s')", worker->alias));
		LM_T(LmtWorkerVector, ("But we start with looking up the network endpoint for the spawner in '%s'", worker->ip));

		hostP = networkP->hostMgr->lookup(worker->ip);
		if (hostP == NULL)
		{
			networkP->hostMgr->insert(worker->ip, NULL);
			networkP->hostMgr->list("Got Worker Vector");

			hostP = networkP->hostMgr->lookup(worker->ip);
			if (hostP == NULL)
				LM_X(1, ("Host Manager cannot find host '%s'", worker->ip));
		}

		ep = networkP->endpointLookup(ss::Endpoint::Spawner, hostP);
		if (ep == NULL)
		{
			LM_W(("Not even connected to Spawner in '%s' - lets connect !", hostP->name));
			fd = iomConnect(hostP->name, SPAWNER_PORT);
			ep = networkP->endpointAdd("unconnected spawner for workerVector", fd, fd, "Spawner", "Spawner", 0, ss::Endpoint::Spawner, hostP->name, SPAWNER_PORT);

			LM_T(LmtWorkerVector, ("Now, adding this spawner as a process in our lists"));
			spawner = spawnerAdd("Spawner", hostP->name, SPAWNER_PORT, ep);
		}
		else
		{
			LM_T(LmtWorkerVector, ("We are connected to the spawner, now lets see if we have it in our process list"));
			spawner = spawnerLookup(hostP->name);
			if (spawner == NULL)
			{
				LM_T(LmtWorkerVector, ("Nope, not there, I better add it ..."));
				spawner = spawnerAdd("Spawner", hostP->name, SPAWNER_PORT, ep);
			}
		}

		if (spawner == NULL)
			LM_X(1, ("NULL spawner - this is a SW bug. Probably Ken's first bug ever!"));

		LM_T(LmtWorkerVector, ("Now, we're connected to the spawner and it is added to out process list - does it have a starter ?"));
		starter = starterLookup(ep);
		if (starter == NULL)
		{
			LM_T(LmtWorkerVector, ("Creating starter for spawner in '%s'", hostP->name));
			
			starter = starterAdd("workerVectorReceived - Starter for Spawner", spawner);
			if (starter == NULL)
				LM_X(1, ("NULL starter for Spawner@%s", hostP->name));
			
			if ((tabManager != NULL) && (tabManager->processListTab != NULL))
				tabManager->processListTab->starterInclude(starter);
		}

		LM_T(LmtWorkerVector, ("OK, spawner complete - now the worker itself."));



		LM_T(LmtWorkerVector, ("Let's see first if we're already connected to it"));
		ep = networkP->endpointLookup(ss::Endpoint::Worker, hostP);
		if (ep == NULL)
		{
			LM_T(LmtWorkerVector, ("Nope, not connected. Let's try to connect"));
			fd = iomConnect(hostP->name, WORKER_PORT);
			ep = networkP->endpointAdd("unconnected worker in workerVector", fd, fd, "Worker", worker->alias, 0, ss::Endpoint::Worker, hostP->name, WORKER_PORT);
		}
		else
			LM_T(LmtWorkerVector, ("Yes, found the endpoint"));

		LM_T(LmtWorkerVector, ("Now, is this process in our process list ?"));
		process = processLookup("Worker", hostP->name);
		if (process == NULL)
		{
			LM_T(LmtWorkerVector, ("Nope, not in the list - let's add it"));
			process = processAdd(ss::PtWorker, "Worker", hostP->name, WORKER_PORT, worker->alias, ep);
		}
		else
		{
			LM_T(LmtWorkerVector, ("Process already existed, is its endpoint OK?"));
			if (process->endpoint != ep)
			{
				LM_W(("Strange, another endpoint ...  I change it to the new one!"));
				LM_W(("If this happens, I will probably get an extra starter for this Worker ..."));
				process->endpoint = ep;
			}
		}

		LM_T(LmtWorkerVector, ("Now, how is it started? Pues, via its spawner!"));

		process->spawnerP = spawner;
		LM_T(LmtWorkerVector, ("process complete - with spawner and everything!"));

		LM_T(LmtWorkerVector, ("OK, process there, now its starter ..."));
		starter = starterLookup(ep);
		if (starter == NULL)
		{
			LM_T(LmtWorkerVector, ("No starter, let's create it ..."));

			starter = starterAdd("workerVectorReceived - Starter for Worker", process);
			if (starter == NULL)
                LM_X(1, ("NULL starter for Worker@%s", hostP->name));

			if ((tabManager != NULL) && (tabManager->processListTab != NULL))
				tabManager->processListTab->starterInclude(starter);
		}
		else
			starter->check("workerVectorReceived - just created a Worker");
	}

	LM_T(LmtWorkerVector, ("Treated worker vector with %d workers", wvDataP->workers));
}



/* ****************************************************************************
*
* SamsonSupervisor::endpointUpdate - 
*/
int SamsonSupervisor::endpointUpdate(ss::Endpoint* ep, ss::Endpoint::UpdateReason reason, const char* reasonText, void* info)
{
	ss::Message::WorkerVectorData*  wvDataP   = (ss::Message::WorkerVectorData*) info;
	Starter*                        starter   = NULL;
	ss::Process*                    processP  = NULL;
	char                            eText[256];

	if (reason == ss::Endpoint::SelectToBeCalled)
	{
		starterListShow("periodic");
		return 0;
	}

	if (reason == ss::Endpoint::HelloReceived)
	{
		if (ep->type == ss::Endpoint::Worker)
		{
			starter = starterLookup(ep);
			if (starter)
				starter->check("Worker Helloed");
		}

		return 0;
	}

	if (ep != NULL)
		LM_T(LmtEndpointUpdate, ("Got an Update Notification ('%s') for endpoint %p '%s' at '%s'", reasonText, ep, ep->name.c_str(), ep->ip));
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
			processP = starter->process;

			LM_T(LmtEndpointUpdate, ("found %s-starter '%s'", processTypeName(processP), processP->name));
			if (processP->endpoint != ep)
				LM_E(("********* Should have the same endpoint ...  %p vs %p", processP->endpoint, ep));
			if ((tabManager != NULL) && (tabManager->processListTab != NULL))
				starter->check("endpointUpdate");
		}
		else
		{
			if (ep->type == ss::Endpoint::Temporal)
				LM_X(1, ("Temporal endpoint (%p) - don't want any!", ep));
			else
			{
				LM_T(LmtStarter, ("starter == NULL - looking up process %s@%d", ep->name.c_str(), ep->ip));
				processP = processLookup(ep->name.c_str(), ep->ip);
				if (processP != NULL)
				{
					if (processP->endpoint == NULL)
						processP->endpoint = ep;
					else if (processP->endpoint != ep)
						LM_E(("********* Should have the same endpoint ...  %p vs %p", processP->endpoint, ep));
				}
			}
		}
	}


	if (starter == NULL)
		starter = starterLookup(ep);

	if (processP == NULL)
	{
		if (starter)
			processP = starter->process;
	}

	if (processP == NULL)
		processP = processLookup(ep);

	switch (reason)
	{
	case ss::Endpoint::SupervisorAdded:
		LM_X(1, ("Supervisor Added ..."));
		break;

	case ss::Endpoint::WorkerVectorReceived:
		workerVectorReceived(wvDataP);
		break;

	case ss::Endpoint::WorkerAdded:
		if (processP != NULL)
			LM_W(("WorkerAdded and I take no action (%s@%s) ...", processP->alias, processP->host));
		else
			LM_W(("WorkerAdded and I take no action ..."));
		break;

	case ss::Endpoint::ControllerDisconnected:
		disconnectWorkers();
		if (starter && starter->process)
		{
			snprintf(eText, sizeof(eText), "Lost connection to samsonController in host '%s'.\n"
					 "This process is a vital part of the samson platform,\n"
					 "so please restart the process as soon as possible.",
					 starter->process->host);
		
			new Popup("Lost Connection to Controller", eText);
		}
		break;

	case ss::Endpoint::WorkerDisconnected:
		LM_W(("Worker %s@%s (%s) disconnected and I do nothing ...", processP->name, processP->host, processP->alias));
		snprintf(eText, sizeof(eText), "Lost connection to samsonWorker in host '%s'.\n"
				 "This process is a vital part of the samson platform,\n"
				 "so please restart the process as soon as possible.",
				 starter->process->host);

		new Popup("Lost Connection to Worker", eText);
		break;

	case ss::Endpoint::EndpointRemoved:
	case ss::Endpoint::ControllerRemoved:
	case ss::Endpoint::WorkerRemoved:
		if (processP != NULL)
			LM_W(("Endpoint %s@%s closed connection", processP->name, processP->host));
		else
			LM_W(("Some endpoint closed connection"));

		if (starter == NULL)
		{
			LM_W(("No starter found for endpoint %p", ep));
			return 0;
		}

		starter->check("Some endpoint closed connection");
		if (starter->process->type == ss::PtSpawner)
		{
			snprintf(eText, sizeof(eText),
					 "Lost connection to samsonSpawner in host '%s'.\n"
					 "This process is a vital part of the samson platform,\n"
					 "so please restart the process as soon as possible.",
					 starter->process->host);

			new Popup("Lost Connection to Spawner", eText);
		}
		else if (starter->process->type == ss::PtController)
		{
			snprintf(eText, sizeof(eText),
					 "Lost connection to samsonController in host '%s'.\n"
					 "This process is a vital part of the samson platform,\n"
					 "so please restart the process as soon as possible.",
					 starter->process->host);

			new Popup("Lost Connection to Controller", eText);
		}
#if 0
		else if (starter->process->type == ss::PtWorker)
		{
			snprintf(eText, sizeof(eText),
					 "Lost connection to samsonWorker in host '%s'.\n"
					 "This process is a vital part of the samson platform,\n"
					 "so please restart the process as soon as possible.",
					 starter->process->host);

			new Popup("Lost Connection to Worker", eText);
		}
#endif
		break;

	case ss::Endpoint::HelloReceived:
		if (starter != NULL)
			starter->check("Hello Received");

		if (ep != NULL)
		{
			if (processP != NULL)
			{
				LM_T(LmtProcess, ("Changing endpoint for '%s@%s' from %p to %p", processP->name, processP->host, processP->endpoint, info));
				processP->endpoint = (ss::Endpoint*) info;
				processListShow("Changed endpoint for Spawner");
			}
			else
			{
				LM_T(LmtProcess, ("No process found for endpoint %p - but perhaps I find one for the new endpoint %p ?", ep, info));
				processP = processLookup((ss::Endpoint*) info);
				if (processP == NULL)
					LM_T(LmtProcess, ("... NO. didn't find a process for endpoint %p either", info));
				else
					LM_T(LmtProcess, ("... YES. Found the process for endpoint %p: %s@%s", info, processP->name, processP->host));

				LM_W(("Got a '%s' endpoint-update-reason from %s@%s (ep %p) and I take no action (processP == NULL)", reasonText, ep->name.c_str(), ep->ip, ep));
			}
		}
		else
			LM_W(("Got a '%s' endpoint-update-reason and I take no action (ep == NULL)", reasonText));
		break;

	case ss::Endpoint::ControllerAdded:
	case ss::Endpoint::ControllerReconnected:
		LM_TODO(("Got a '%s' endpoint-update-reason and I take no action ...", reasonText));
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
		LM_X(1, ("NULL controller - try to connect to it, as before ?"));

	return 0;
}



/* ****************************************************************************
*
* logReceiverInit - 
*/
void SamsonSupervisor::logReceiverInit(unsigned short port)
{
	struct sockaddr_in  sAddr;

	logSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (logSocket == -1)
	{
		new Popup("Internal Error", "No log will be available.\nSee log file for more info");
		LM_E(("socket: %s", strerror(errno)));
		return;
	}

	memset((char*) &sAddr, 0, sizeof(sAddr));
	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(port);
	sAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(logSocket, (struct sockaddr*) &sAddr, sizeof(sAddr)) == -1)
	{
		new Popup("Internal Error", "No log will be available.\nSee log file for more info");
		LM_E(("bind: %s", strerror(errno)));
		::close(logSocket);
		logSocket = -1;
        return;
	}
}



/* ****************************************************************************
*
* logReceive - 
*/
void SamsonSupervisor::logReceive(void)
{
	struct sockaddr_in        sAddr;
	socklen_t                 sAddrLen = sizeof(sAddr);
	int                       flags;
	ssize_t                   nb;
	unsigned int              tot;
	ss::LogLineData           logLine;
	ss::Message::Header       header;
	char*                     buf;
	unsigned int              bufLen;

	flags = MSG_DONTWAIT;

	while (1)
	{
		//
		// 1. Read header
		//
		tot    = 0;
		bufLen = sizeof(header);
		buf    = (char*) &header;
		memset(buf, 0, bufLen);

		while (tot < bufLen)
		{
			nb = recvfrom(logSocket, &buf[tot], bufLen - tot, flags, (struct sockaddr*) &sAddr, &sAddrLen);
			if (nb == -1)
			{
				if ((errno == EAGAIN) && (tot == 0))
					return;

				new Popup("Internal Error", "No log lines from other processes will be available.\nSee local samsonSupervisor log file for more info.");
				LM_E(("recvfrom: %s", strerror(errno)));
				logSocket = -1;
				LM_TODO(("Inform processes that the logging mechanism has stopped"));
				return;
			}
			else if (nb == 0)
				LM_E(("recvfrom returned 0 bytes ..."));

			tot += nb;
		}



		//
		// Read data
		//
		tot    = 0;
		bufLen = header.dataLen;
		buf    = (char*) &logLine;
		memset(buf, 0, bufLen);
		
		while (tot < bufLen)
		{
			nb = recvfrom(logSocket, &buf[tot], bufLen - tot, flags, (struct sockaddr*) &sAddr, &sAddrLen);
			if (nb == -1)
			{
				if ((errno == EAGAIN) && (tot == 0))
				{
					LM_E(("No data ..."));
					return;
				}

				new Popup("Internal Error", "No log lines from other processes will be available.\nSee local samsonSupervisor log file for more info.");
				LM_E(("recvfrom: %s", strerror(errno)));
				logSocket = -1;
				LM_TODO(("Inform processes that the logging mechanism has stopped"));
				return;
			}
			else if (nb == 0)
				LM_E(("recvfrom returned 0 bytes ..."));

			tot += nb;
		}

		if ((tot == bufLen) && (header.magic == 0xFEEDC0DE))
		{
			tabManager->logTab->logLineInsert(&sAddr, &header, &logLine);
		}
		else
			LM_W(("skipping log line as its garbage ..."));
	}
}



/* ****************************************************************************
*
* SamsonSupervisor::timerEvent - 
*/
void SamsonSupervisor::timerEvent(QTimerEvent* e)
{
	static struct timeval  lastShow = { 0, 0 };
	struct timeval         now;

	gettimeofday(&now, NULL);
	if (now.tv_sec - lastShow.tv_sec > 10)
	{
		if (networkP != NULL)
			networkP->endpointListShow("periodic");

		starterListShow("periodic");
		processListShow("periodic");

		lastShow = now;
	}



	//
	// Poll Samson Network modules
	//
	if (networkP != NULL)
		networkP->poll();


	//
	// Try to connect to unconnected Controller, Worker and Spawner
	//
	ss::Process** processV = processListGet();
	for (unsigned int ix = 0; ix < processMaxGet(); ix++)
	{
		if (processV[ix] == NULL)
			continue;

		if (processV[ix]->endpoint == NULL)
			continue;

		if (processV[ix]->endpoint->state != ss::Endpoint::Connected)
		{
			int fd;

			fd = iomConnect(processV[ix]->host, processV[ix]->port);
			if (fd >= 0)
			{
				processV[ix]->endpoint->rFd = fd;
				processV[ix]->endpoint->wFd = fd;
				processV[ix]->endpoint->state = ss::Endpoint::Connected;
				processV[ix]->starterP->check("Reconnected");
			}
		}
	}


	//
	// Read Log messages from other Samson processes
	//
	if (logSocket != -1)
		logReceive();


	//
	// Any InfoWin that needs to be killed ?
	//
	if (infoWin != NULL)
	{
		if ((now.tv_sec > infoWin->dieAt.tv_sec) || ((now.tv_sec == infoWin->dieAt.tv_sec) && (now.tv_usec >= infoWin->dieAt.tv_usec)))
		{
			delete infoWin;
			infoWin = NULL;
		}
	}
}
