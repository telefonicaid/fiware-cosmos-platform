/* ****************************************************************************
*
* FILE                     SamsonSpawner.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 29 2011
*
*/
#include <sys/time.h>                         // gettimeofday
#include <sys/types.h>                        // pid_t
#include <sys/wait.h>                         // waitpid

#include "logMsg/logMsg.h"                    // LM_*
#include "logMsg/traceLevels.h"               // Trace levels

#include "samson/common/Process.h"            // Process
#include "samson/common/platformProcesses.h"  // platformProcessesSave
#include "samson/common/ports.h"              // Samson platform ports
#include "samson/common/samsonDirectories.h"  // SAMSON_PLATFORM_PROCESSES
#include "samson/network/NetworkInterface.h"  // DataReceiverInterface, ...
#include "samson/network/Packet.h"            // Packet
#include "samson/network/Network2.h"          // Network2

#include "processList.h"                      // processListInit, Add, Remove and Lookup
#include "globals.h"                          // Global variables for Spawner
#include "SamsonSpawner.h"                    // Own interface



namespace samson
{



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
* periodic - 
*/
static void periodic(void* nada, void* vP)
{
	SamsonSpawner* spawnerP = (SamsonSpawner*) vP;
	
	spawnerP->timeoutFunction();
	nada = NULL;
}



/* ****************************************************************************
*
* SamsonSpawner -
*/
SamsonSpawner::SamsonSpawner()
{
	EndpointManager* epMgr = new EndpointManager(Endpoint2::Spawner);
	networkP               = new Network2(epMgr);
	restartInProgress      = false;

	networkP->setDataReceiver(this);
	epMgr->callbackSet(samson::EndpointManager::Periodic, periodic, this);
	networkP->tmoSet(1, 500000);
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
* SamsonSpawner::init - 
*/
void SamsonSpawner::init()
{
	processListInit(101);

	restartInProgress    = true;
	localProcessesKill();
	restartInProgress    = false;

	processesStart(networkP->epMgr->procVecGet());
	processListShow("INIT", true);
	networkP->endpointListShow("INIT", true);
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
void SamsonSpawner::init(samson::ProcessVector* pv)
{
	LM_W(("Got initialized with a process vector ..."));
}



/* ****************************************************************************
*
* SamsonSpawner::receive - 
*/
int SamsonSpawner::receive(int fromId, int nb, samson::Message::Header* headerP, void* dataP)
{
	Process*        process  = (Process*)       dataP;
	ProcessVector*  procVec  = (ProcessVector*) dataP;
	samson::Endpoint2*  ep       = networkP->epMgr->indexedGet((unsigned int) fromId);

	if (ep == NULL)
		LM_X(1, ("Got a message from endpoint %d, but endpoint manager has no endpoint at that index ..."));

	switch (headerP->code)
	{
	case Message::Reset:
		if (headerP->type == Message::Ack)
			LM_X(1, ("Spawner cannot receive Ack for Reset"));
		reset(ep);
		break;

#if 0
	case Message::ProcessList:
		if (headerP->type == Message::Ack)
			LM_X(1, ("Spawner cannot receive Ack for ProcessList"));
		processList();
		break;
#endif

	case Message::ProcessVector:
		if (headerP->type == Message::Ack)
			LM_X(1, ("Spawner cannot receive Ack for ProcessVector"));
		processVector(ep, procVec);
		break;

	case Message::ProcessSpawn:
		if (headerP->type == Message::Ack)
			LM_X(1, ("Spawner cannot receive Ack for ProcessSpawn"));
		LM_W(("Spawning a process without adding it to process vector ..."));
		spawn(process);
		break;

	default:
		LM_X(1, ("No messages treated - got a '%s'", messageCode(headerP->code)));
		return 1;
	}

	return 0;
}



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

	LM_W(("caught death of process %d", pid));
	processListShow("child died", true);
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
* reset - 
*/
void SamsonSpawner::reset(Endpoint2* ep)
{
	LM_W(("Got a Reset message from '%s%s'", ep->nameGet(), ep->hostname()));
	unlink(SAMSON_PLATFORM_PROCESSES);

	LM_T(LmtReset, ("killing local processes"));
	restartInProgress    = true;
	localProcessesKill();
	restartInProgress    = false;

	LM_T(LmtReset, ("Sending ack to RESET message to %s@%s", ep->nameGet(), ep->hostname()));
	Packet* packetP = new Packet(Message::Ack, Message::Reset);
	ep->send(NULL, packetP);
	networkP->epMgr->show("Got RESET", true);
}



/* ****************************************************************************
*
* processVector - 
*/
void SamsonSpawner::processVector(Endpoint2* ep, ProcessVector* procVec)
{
	networkP->epMgr->procVecSet(procVec);

	LM_T(LmtProcessVector, ("Got Process Vector with %d processes from %s@%s", procVec->processes, ep->nameGet(), ep->hostname()));

#if 0
	// Supposedly, a RESET was sent before ...
	restartInProgress = true;
	localProcessesKill();
	restartInProgress = false;
#endif

	processesStart(procVec);

	Packet* packetP = new Packet(Message::Ack, Message::ProcessVector);
	ep->send(NULL, packetP);
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
		argV[argC++] = (char*) "samsonWorker2";
	else if (process->type == PtController)
		argV[argC++] = (char*) "samsonController2";
	else
		LM_X(1, ("Will only start workers and controllers - bad process type %d", process->type));

	if (getenv("SAMSON_DEBUG") != NULL)
		process->debug = true;
	if (getenv("SAMSON_VERBOSE") != NULL)
		process->verbose = true;
	if (getenv("SAMSON_READS") != NULL)
		process->reads = true;
	if (getenv("SAMSON_WRITES") != NULL)
		process->writes = true;

	if (process->verbose == true)   argV[argC++] = (char*) "-v";
	if (process->debug   == true)   argV[argC++] = (char*) "-d";
	if (process->reads   == true)   argV[argC++] = (char*) "-r";
	if (process->writes  == true)   argV[argC++] = (char*) "-w";
	if (process->toDo    == true)   argV[argC++] = (char*) "-toDo";


	// Inheriting the trace levels from Spawner process
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

		LM_TODO(("Use close-on-exec!"));
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
	processListShow("Process Spawned", true);
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
	int       startedProcesses = 0;

	for (ix = 0; ix < procVec->processes; ix++)
	{
		processP = &procVec->processV[ix];

		hostP = networkP->epMgr->hostMgr->lookup(processP->host);
		if (hostP != networkP->epMgr->hostMgr->localhostP)
		{
			LM_W(("Host '%s' is not ME (%s) - NOT starting process %d", processP->host, networkP->epMgr->hostMgr->localhostP->name, ix));
			continue;
		}

		LM_T(LmtProcess, ("Spawning process '%s'", processP->name));

		processP->verbose = lmVerbose;
		processP->debug   = lmDebug;
		processP->reads   = lmReads;
		processP->writes  = lmWrites;
		processP->pid     = 0;

		struct timeval  now;
		if (gettimeofday(&now, NULL) != 0)
			LM_X(1, ("gettimeofday failed (fatal error): %s", strerror(errno)));
		Process* pP = processAdd(processP->type, processP->name, processP->alias, processP->controllerHost, 0, &now);
		spawn(pP);
		++startedProcesses;
	}

	if (startedProcesses == 0)
	{
		networkP->epMgr->hostMgr->list("ZERO processes for me");
		LM_X(1, ("No processes for me (%s) in the Process Vector - am I not in the cluster ?", networkP->epMgr->hostMgr->localhostP->name));
	}
}


/* ****************************************************************************
*
* localProcessesKill - 
*/
void SamsonSpawner::localProcessesKill(void)
{
	int s;

	s = system("killall samsonWorker > /dev/null 2>&1");
	if (s != 0)
		LM_E((" system(\"killall samsonWorker\") returned %d (strerror: %s)", s, strerror(errno)));
	usleep(200000);
	s = system("killall -9 samsonWorker > /dev/null 2>&1");
	if (s != 0)
		LM_E((" system(\"killall -9 samsonWorker\") returned %d (strerror: %s)", s, strerror(errno)));


	s = system("killall samsonController2 > /dev/null 2>&1");
	if (s != 0)
		LM_E((" system(\"killall samsonController2\") returned %d (strerror: %s)", s, strerror(errno)));
	usleep(200000);
	s = system("killall -9 samsonController2 > /dev/null 2>&1");
	if (s != 0)
		LM_E((" system(\"killall -9 samsonController2\") returned %d (strerror: %s)", s, strerror(errno)));
}

}
