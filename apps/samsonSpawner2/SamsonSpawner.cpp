/* ****************************************************************************
*
* FILE                     SamsonSpawner.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 29 2011
*
*/
#include <sys/time.h>           // gettimeofday
#include <sys/types.h>          // pid_t
#include <sys/wait.h>           // waitpid

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace levels

#include "NetworkInterface.h"   // DataReceiverInterface, ...
#include "Process.h"            // Process
#include "platformProcesses.h"  // platformProcessesSave
#include "ports.h"              // Samson platform ports
#include "processList.h"        // processListInit, Add, Remove and Lookup
#include "samsonDirectories.h"  // SAMSON_PLATFORM_PROCESSES
#include "Network2.h"           // Network2

#include "globals.h"            // Global variables for Spawner
#include "SamsonSpawner.h"      // Own interface



namespace ss
{



/* ****************************************************************************
*
* Global variables
*/
static bool  restartInProgress = false;



/* ****************************************************************************
*
* timeDiff - 
*/
static void timeDiff(struct timeval* from, struct timeval* to, struct timeval* diff)
{
	diff->tv_sec  = to->tv_sec  - from->tv_sec;
	diff->tv_usec = to->tv_usec - from->tv_usec;

	if (diff->tv_usec < 0)
	{
		diff->tv_sec  -= 1;
		diff->tv_usec += 1000000;
	}
}



/* ****************************************************************************
*
* SamsonSpawner -
*/
SamsonSpawner::SamsonSpawner()
{
	EndpointManager* epMgr = new EndpointManager(Endpoint2::Spawner);

	networkP = new Network2(epMgr);

	networkP->setDataReceiver(this);
	// networkP->setEndpointUpdateReceiver(this);
	// networkP->setTimeoutReceiver(this, 1, 0); // Timeout every second
}



/* ****************************************************************************
*
* ~SamsonSpawner -
*/
SamsonSpawner::~SamsonSpawner()
{
	if (networkP)
		delete networkP;

	processListDelete();
}



/* ****************************************************************************
*
* run - 
*/
void SamsonSpawner::run(void)
{
	networkP->run();
}



/* ****************************************************************************
*
* SamsonSpawner::init - 
*/
void SamsonSpawner::init()
{
	processListInit(101);

	restartInProgress    = true;
	localProcessesKill();
	restartInProgress    = false;

	processesStart(networkP->epMgr->procVecGet());
	spawnersConnect(networkP->epMgr->procVecGet());

	processListShow("INIT", true);
	networkP->endpointListShow("INIT", true);
}



/* ****************************************************************************
*
* init - 
*/
void SamsonSpawner::init(ProcessVector* procVec)
{
	LM_X(1, ("Don't want this method to be called!  (I think ...)"));
}



/* ****************************************************************************
*
* SamsonSpawner::receive - 
*
* This should be moved to SpawnerEndpoint
*/
int SamsonSpawner::receive(int fromId, int nb, Message::Header* headerP, void* dataP)
{
	int             s;
	Endpoint2*      ep      = networkP->epMgr->get(fromId);

	Process*        process = (Process*)       dataP;
	ProcessVector*  procVec = (ProcessVector*) dataP;


	if (ep == NULL)
		LM_X(1, ("Received a message for a NULL endpoint ..."));

	switch (headerP->code)
	{
	case Message::Reset:
		if (headerP->type == Message::Msg)
		{
			LM_V(("Got a Reset message from '%s'", ep->nameGet()));
			unlink(SAMSON_PLATFORM_PROCESSES);

			if (ep->typeGet() == Endpoint2::Setup) 
			{
				LM_T(LmtReset, ("Got RESET from samsonSetup - forwarding RESET to all spawners"));
				spawnerForward(Message::Reset);
			}
			else
				LM_T(LmtReset, ("Got RESET from '%s' - NOT forwarding RESET to all spawners", ep->nameGet()));
			
			LM_T(LmtReset, ("killing local processes"));
			restartInProgress    = true;
			localProcessesKill();
			restartInProgress    = false;
			LM_T(LmtReset, ("Sending ack to RESET message to %s@%s", ep->nameGet(), ep->hostname()));
			ep->send(Message::Ack, Message::Reset);
			processListShow("Got RESET", true);
			networkP->endpointListShow("Got RESET", true);
		}
		break;

	case Message::ProcessSpawn:
		spawn(process);
		break;

	case Message::ProcessVector:
		if (headerP->type == Message::Msg)
		{
			networkP->epMgr->procVecSet(procVec);
			s = procVecTreat(ep);
			LM_T(LmtProcessVector, ("Sending ProcessVector ack to '%s' in '%s'", ep->nameGet(), ep->hostname()));
			ep->send((s == 0)? Message::Ack : Message::Nak, headerP->code);
		}
		else if (headerP->type == Message::Ack)
			LM_T(LmtProcessVector, ("Received a ProcessVector Ack from '%s'", ep->nameGet()));
		else if (headerP->type == Message::Nak)
			LM_T(LmtProcessVector, ("Received a ProcessVector Nak from '%s'", ep->nameGet()));
		else
			LM_X(1, ("Bad message type in ProcessVector message (%d)", headerP->type));
		break;

	default:
		LM_X(1, ("Don't know how to treat '%s' message", Message::messageCode(headerP->code)));
	}

	return 0;
}


#if 0
/* ****************************************************************************
*
* endpointUpdate - 
*/
int SamsonSpawner::endpointUpdate(Endpoint* ep, Endpoint::UpdateReason reason, const char* reasonText, void* info)
{
	LM_M(("Endpoint '%s' updated as '%s', reason %d: %s", ep->nameGet(), reasonText, reason, info));
	return 0;
}
#endif



/* ****************************************************************************
*
* timeoutFunction - 
*/
int SamsonSpawner::timeoutFunction(void)
{
	pid_t         pid;
	int           status;
	Process*  processP;

	processListShow("periodic");

	pid = waitpid(-1, &status, WNOHANG);
	if (pid == 0)
	{
		LM_T(LmtWait, ("Children running, no one has exited since last sweep"));
		return 0;
	}

	if (pid == -1)
	{
		if (errno != ECHILD)
			LM_E(("waitpid error: %s", strerror(errno)));
		return 0;
	}

	processP = processLookup(pid);
	if (processP == NULL)
	{
		LM_W(("Unknown process %d died with status 0x%x", pid, status));
		return 0;
	}


	Process*    newProcessP;
	struct timeval  now;
	struct timeval  diff;
	
	if (access(SAMSON_PLATFORM_PROCESSES, R_OK) != 0)
		LM_W(("'%s' died - NOT restarting it as the platform processes file isn't in place", processP->name));
	else if (noRestarts == true)
		LM_W(("'%s' died - NOT restarting it (option '-noRestarts' used)", processP->name));
	else if (restartInProgress == true)
		LM_W(("'%s' died - NOT restarting it (Restart in progress)", processP->name));
	else
	{
		LM_W(("'%s' died - restarting it", processP->name));

		if (gettimeofday(&now, NULL) != 0)
			LM_X(1, ("gettimeofday failed (fatal error): %s", strerror(errno)));

		timeDiff(&processP->startTime, &now, &diff);

		LM_W(("Process %d '%s' died after only %d.%06d seconds of uptime", processP->pid, processP->name, diff.tv_sec, diff.tv_usec));
		if (diff.tv_sec < 5)
			LM_X(1, ("Error starting process '%s' - this bug must be fixed!", processP->name));

		newProcessP = processAdd(processP->type, processP->name, processP->alias, processP->controllerHost, 0, &now);
		spawn(newProcessP);
	}

	processRemove(processP);
	return 0;
}



/* ****************************************************************************
*
* processesStart - 
*/
void SamsonSpawner::processesStart(ProcessVector* procVec)
{
	Process*  processP;
	int       ix;
	Host*     hostP;

	for (ix = 0; ix < procVec->processes; ix++)
	{
		processP = &procVec->processV[ix];

		hostP = networkP->epMgr->hostMgr->lookup(processP->host);
		if (hostP != networkP->epMgr->hostMgr->localhostP)
			continue;

		LM_T(LmtProcess, ("Spawning process '%s'", processP->name));

		processP->verbose = lmVerbose;
		processP->debug   = lmDebug;
		processP->reads   = lmReads;
		processP->writes  = lmWrites;
		processP->pid     = 0;
		// processP->traceLevels ...

		Process* pP;
		pP = processAdd(processP->type, processP->name, processP->alias, processP->controllerHost, 0, NULL);
		spawn(pP);
	}
}



/* ****************************************************************************
*
* spawnersConnect
*
* Disconnect from spawners that run in hosts not in 'procVec' and connect to added spawners
* But, for now, just connect to the spawners.
*/
void SamsonSpawner::spawnersConnect(ProcessVector* procVec, bool force)
{
	Host*              hostP;
	int                ix;
	Process*           processP;
	Endpoint2*         ep;
	Endpoint2::Status  s;
	int                procVecSize;

	for (ix = 0; ix < procVec->processes; ix++)
	{
		processP = &procVec->processV[ix];		
		hostP    = networkP->epMgr->hostMgr->lookup(processP->host);

		if (hostP != NULL)
		{
			Endpoint2* ep;

			ep = networkP->epMgr->lookup(Endpoint2::Spawner, hostP->name);
			
			if ((ep != NULL) && (ep->stateGet() == Endpoint2::Ready))
			{
				LM_T(LmtProcessVector, ("Not connecting to spawner in '%s' - nor sending it the ProcessVector", hostP->name));
				continue;
			}
		}

		if (hostP == networkP->epMgr->hostMgr->localhostP)
		{
			LM_T(LmtProcessVector, ("ProcessVector: not connecting to myself ... (%s)", hostP->name));
			continue;
		}
		else if (hostP == NULL)
		{
			LM_T(LmtProcessVector, ("Inserting host for process %d ('%s')", ix, processP->host));
			hostP = networkP->epMgr->hostMgr->insert(processP->host, NULL);
			if (hostP == NULL)
				LM_X(1, ("error inserting host '%s'", processP->host));

			LM_T(LmtProcessVector, ("Inserted host for process %d ('%s')", ix, hostP->name));
		}

		LM_T(LmtProcessVector, ("ProcessVector: current host is '%s'", hostP->name));
		LM_T(LmtProcessVector, ("ProcessVector: localhost    is '%s'", networkP->epMgr->hostMgr->localhostP->name));

		//
		// Only connect to spawners whose host names comes before localhost alphabetically
		//
		if ((strcmp(hostP->name, networkP->epMgr->hostMgr->localhostP->name) > 0) && (force == false))
		{
			LM_T(LmtProcessVector, ("Not connecting to '%s' (I am '%s')", hostP->name, networkP->epMgr->hostMgr->localhostP->name));
			continue;
		}

		if ((ep = networkP->epMgr->lookup(Endpoint2::Spawner, hostP->name)) != NULL)
		{
			Host* controllerHostP;

			controllerHostP = networkP->epMgr->hostMgr->lookup(procVec->processV[0].host);

			if (controllerHostP == networkP->epMgr->hostMgr->localhostP)
			{
				LM_T(LmtProcessVector, ("Already connected to spawner in '%s' - sending it the process vector", hostP->name));

				procVecSize = sizeof(ProcessVector) + procVec->processes * sizeof(Process);
				s = ep->send(Message::Msg, Message::ProcessVector, procVec, procVecSize);
				if (s != 0)
					LM_E(("Error sending ProcessVector message to spawner in '%s': %s", ep->nameGet(), ep->status(s)));
			}
			else
				LM_T(LmtProcessVector, ("Already connected to spawner in '%s' - NOT sending it the process vector", hostP->name));

			continue;
		}

		Endpoint2* ep;
		
		ep = networkP->epMgr->add(Endpoint2::Spawner, 0, "Spawner", "Spawner", hostP, SPAWNER_PORT, -1, -1);
		LM_T(LmtProcessVector, ("ProcessVector: connecting to spawner in '%s'", hostP->name));
		ep->connect();
		if (ep->rFdGet() == -1)
			LM_E(("iomConnect('%s', %d): %s", hostP->name, SPAWNER_PORT, strerror(errno)));
	}
}



/* ****************************************************************************
*
* localProcessesKill - 
*/
void SamsonSpawner::localProcessesKill(void)
{
	int               s;
#if 0
	bool              oldRestartInProgress;
	Endpoint2*        ep;
	Endpoint2::Status status;

	oldRestartInProgress = restartInProgress;
	restartInProgress    = true;


	networkP->epMgr->show("Killing Local Processes", true);

	//
	// Connecting to and sending 'Die' to worker
	//
	LM_T(LmtInit, ("Connecting to and sending 'Die' to worker in localhost"));
	status = Endpoint2::OK;
	if ((ep = networkP->epMgr->lookup(Endpoint2::Worker, "localhost")) == NULL)
	{
		LM_T(LmtInit, ("Not connected to worker - creating endpoint"));
		ep = networkP->epMgr->add(Endpoint2::Worker, 0, "Worker", "Worker", networkP->epMgr->hostMgr->localhostP, WORKER_PORT);
	}
	else
		LM_T(LmtInit, ("Found worker endpoint"));

	if (ep->stateGet() != Endpoint2::Ready)
	{
		LM_T(LmtInit, ("Worker not READY - connecting"));
		if (ep->connect() == Endpoint2::OK)
		{
			LM_T(LmtInit, ("connect OK - sending Hello"));
			status = ep->hello(0, 50000);
		}
		else
			LM_T(LmtInit, ("connect failed: %s", strerror(errno)));
	}
	else
		LM_T(LmtInit, ("worker was READY"));

	if ((status == Endpoint2::OK) && (ep->stateGet() ==  Endpoint2::Ready))
	{
		LM_V(("Sending Die to controller"));
		status = ep->die(0, 200000);
	}
	else
		status = Endpoint2::Error;

	if (status != Endpoint2::OK)
	{
		usleep(200000);
		s = system("killall samsonWorker > /dev/null 2>&1");
		if (s != 0)
			LM_E((" system(\"killall samsonWorker\") returned %d (strerror: %s)", s, strerror(errno)));

		usleep(200000);
		s = system("killall -9 samsonWorker > /dev/null 2>&1");
		if (s != 0)
			LM_E((" system(\"killall -9 samsonWorker\") returned %d (strerror: %s)", s, strerror(errno)));
	}



	//
	// Connecting to and sending 'Die' to controller
	//
	status = Endpoint2::OK;
	if ((ep = networkP->epMgr->controller) == NULL)
		ep = networkP->epMgr->add(Endpoint2::Controller, 0, "Controller", "Controller to Die", networkP->epMgr->hostMgr->localhostP, CONTROLLER_PORT);

	if (ep->stateGet() != Endpoint2::Ready)
	{
		if (ep->connect() == Endpoint2::OK)
			status = ep->hello(0, 200000);
	}

	if ((status == Endpoint2::OK) && (ep->stateGet() == Endpoint2::Ready))
	{
		LM_V(("Sending Die to controller"));
		status = ep->die(0, 200000);
	}
	else
		status = Endpoint2::Error;

	if (status != Endpoint2::OK)
	{
		usleep(200000);
		s = system("killall samsonController > /dev/null 2>&1");
		if (s != 0)
			LM_E((" system(\"killall samsonController\") returned %d (strerror: %s)", s, strerror(errno)));

		usleep(200000);
		s = system("killall -9 samsonController > /dev/null 2>&1");
		if (s != 0)
			LM_E((" system(\"killall -9 samsonController\") returned %d (strerror: %s)", s, strerror(errno)));
	}

	// Await the processes in the timeout function
	timeoutFunction();
	timeoutFunction();

	restartInProgress = oldRestartInProgress;
#else
	s = system("killall samsonWorker > /dev/null 2>&1");
	if (s != 0)
		LM_E((" system(\"killall samsonWorker\") returned %d (strerror: %s)", s, strerror(errno)));
	usleep(200000);
	s = system("killall -9 samsonWorker > /dev/null 2>&1");
	if (s != 0)
		LM_E((" system(\"killall -9 samsonWorker\") returned %d (strerror: %s)", s, strerror(errno)));


	s = system("killall samsonController > /dev/null 2>&1");
	if (s != 0)
		LM_E((" system(\"killall samsonController\") returned %d (strerror: %s)", s, strerror(errno)));
	usleep(200000);
	s = system("killall -9 samsonController > /dev/null 2>&1");
	if (s != 0)
		LM_E((" system(\"killall -9 samsonController\") returned %d (strerror: %s)", s, strerror(errno)));
#endif
}



/* ****************************************************************************
*
* procVecTreat - 
*/
int SamsonSpawner::procVecTreat(Endpoint2* ep)
{
	ProcessVector* procVec = networkP->epMgr->procVecGet();

	LM_M(("Received a procVec with %d processes from %s@%s", procVec->processes, ep->nameGet(), ep->hostname()));

	restartInProgress = true;
	localProcessesKill();
	restartInProgress = false;
	processesStart(procVec);



	//
	// The spawner that gets the message from 'samsonSetup' connects to all spawners
	//
	if (ep->typeGet() == Endpoint2::Setup) 
		spawnersConnect(procVec, true);
	else
		spawnersConnect(procVec, false);

	return 0;
}



/* ****************************************************************************
*
* spawnerForward - 
*/
void SamsonSpawner::spawnerForward(Message::MessageCode code, void* dataP, int dataLen)
{
	int s;

	s = networkP->epMgr->multiSend(Endpoint2::Spawner, code, dataP, dataLen);
	LM_M(("Sent '%s' Msg to %d spawners", Message::messageCode(code), s));
}


/* ****************************************************************************
*
* spawn - 
*/
void SamsonSpawner::spawn(Process* process)
{
	pid_t  pid;
	char*  argV[50];
	int    argC = 0;

	LM_T(LmtProcessList, ("spawning process '%s' (incoming pid: %d)", process->name, process->pid));

	if (process->type == PtWorker)
	{
		argV[argC++] = (char*) "samsonWorker";

		argV[argC++] = (char*) "-alias";
		argV[argC++] = process->alias;
		argV[argC++] = (char*) "-controller";
		argV[argC++] = (char*) process->controllerHost;
	}
	else if (process->type == PtController)
	{
		argV[argC++] = (char*) "samsonController";
	}
	else
		LM_X(1, ("Will only start workers and controllers - bad process type %d", process->type));

#if 1
	LM_W(("Turning on VERBOSE and ALL TRACE LEVELS for process '%s'", process->name));
	process->verbose = true;
	strcpy(process->traceLevels, "0-255");
#endif

	if (process->verbose == true)   argV[argC++] = (char*) "-v";
	if (process->debug   == true)   argV[argC++] = (char*) "-d";
	if (process->reads   == true)   argV[argC++] = (char*) "-r";
	if (process->writes  == true)   argV[argC++] = (char*) "-w";
	if (process->toDo    == true)   argV[argC++] = (char*) "-toDo";


	char traceLevels[512];
	memset(traceLevels, 0, sizeof(traceLevels));
	lmTraceGet(traceLevels, sizeof(traceLevels), process->traceLevels);
	if (traceLevels[0] != 0)
	{
		argV[argC++] = (char*) "-t";
		argV[argC++] = traceLevels;
	}

	argV[argC] = NULL;

	LM_T(LmtSpawn, ("Spawning process '%s'", argV[0]));
	for (int ix = 0; ix < argC; ix++)
	   LM_T(LmtSpawn, ("  argV[%d]: '%s'", ix, argV[ix]));

	pid = fork();
	if (pid == 0)
	{
		int ix;
		int s;

		if (logFd != -1)
		{
			close(logFd);
			logFd = -1;
		}

		s = execvp(argV[0], argV);
		if (s == -1)
			LM_E(("Back from EXEC: %s", strerror(errno)));
		else
			LM_E(("Back from EXEC"));

		LM_E(("Tried to start '%s' with the following parameters:", argV[0]));
		for (ix = 0; ix < argC + 1; ix++)
			LM_E(("%02d: %s", ix, argV[ix]));

		LM_X(1, ("Back from EXEC !!!"));
	}

	process->pid = pid;
}

}
