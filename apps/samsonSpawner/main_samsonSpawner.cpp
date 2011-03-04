/* ****************************************************************************
*
* FILE                     main_samsonSpawner.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <unistd.h>             // fork & exec
#include <sys/types.h>          // pid_t
#include <sys/wait.h>           // waitpid
#include <sys/time.h>           // gettimeofday

#include "parseArgs.h"          // parseArgs
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace levels

#include "NetworkInterface.h"   // DataReceiverInterface
#include "Network.h"            // Network
#include "Process.h"            // Process
#include "platformProcesses.h"  // platformProcessesSave
#include "iomMsgSend.h"         // iomMsgSend
#include "iomConnect.h"         // iomConnect
#include "ports.h"              // LOG_SERVER_PORT
#include "daemonize.h"          // daemonize
#include "processList.h"        // processListInit, Add, Remove and Lookup


/* ****************************************************************************
*
* Option variables
*/
bool notdaemon;



#define NOLS (long int) "no log server"
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-notdaemon",  &notdaemon,   "NOT_DAEMON",  PaBool,   PaOpt,  false,  false,   true, "don't start as daemon" },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* Global variables
*/
int                 logFd        = -1;
ss::Network*        networkP     = NULL;
ss::ProcessVector*  procVec      = NULL;



/* ****************************************************************************
*
* SamsonSpawner - 
*/
class SamsonSpawner : public ss::DataReceiverInterface, public ss::TimeoutReceiverInterface, public ss::EndpointUpdateReceiverInterface
{
public:
	SamsonSpawner(ss::Network* nwP) { networkP = nwP; }
	void processSpawn(ss::Process* processP);

	virtual int receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP);
	virtual int endpointUpdate(ss::Endpoint* ep, ss::Endpoint::UpdateReason reason, const char* reasonText, void* info = NULL);
	virtual int timeoutFunction(void);

private:
	ss::Network*    networkP;
};



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
* endpointUpdate - 
*/
int SamsonSpawner::endpointUpdate(ss::Endpoint* ep, ss::Endpoint::UpdateReason reason, const char* reasonText, void* info)
{
	int                   s;
	int                   procVecSize;
	ss::Message::Header*  headerP      = (ss::Message::Header*) info;
	Host*                 hostP;
	Host*                 localhostP;

	if (reason != ss::Endpoint::HelloReceived)
		return 0;

	if (ep->type != ss::Endpoint::Spawner)
		return 0;

	if (procVec == NULL)
		return 0;

	//
	// samsonSpawner that runs in controller host distributes to the rest of spawners
	//
	hostP      = networkP->hostMgr->lookup(procVec->processV[0].host);
	localhostP = networkP->hostMgr->lookup("localhost");

	if (hostP != localhostP)
	{
		LM_M(("hostP: %p (%s)",      hostP, procVec->processV[0].host));
		LM_M(("localhostP: %p", localhostP));
		LM_M(("ProcessVector: not controller host - I do nothing"));
		return 0;
	}

	LM_M(("ProcessVector: got Hello Msg/Ack from %s@%s", ep->name.c_str(), ep->ip));
	if (headerP->type == ss::Message::Msg)
	{
		procVecSize = sizeof(ss::ProcessVector) + procVec->processes * sizeof(ss::Process);

		LM_M(("Sending ProcessVector to spawner in '%s' (%d processes)", ep->ip, procVec->processes));
		s = iomMsgSend(ep, networkP->endpoint[0], ss::Message::ProcessVector, ss::Message::Msg, procVec, procVecSize);
		if (s != 0)
			LM_E(("iomMsgSend(%s, ProcessVector)", ep->ip));
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
	ss::Process*  processP;

	processListShow("periodic");

	pid = waitpid(-1, &status, WNOHANG);
	if (pid == 0)
		LM_T(LmtWait, ("Children running, no one has exited since last sweep"));
	else if (pid == -1)
	{
		if (errno != ECHILD)
			LM_E(("waitpid error: %s", strerror(errno)));
	}
	else
	{
		processP = processLookup(pid);
		if (processP == NULL)
			LM_W(("Unknown process %d died with status 0x%x", pid, status));
		else
		{
			ss::Process*    newProcessP;
			struct timeval  now;
			struct timeval  diff;

			LM_W(("'%s' died - restarting it?", processP->name));

			if (gettimeofday(&now, NULL) != 0)
				LM_X(1, ("gettimeofday failed (fatal error): %s", strerror(errno)));

			timeDiff(&processP->startTime, &now, &diff);

			LM_M(("Process %d '%s' died after %d.%06d seconds of uptime", processP->pid, processP->name, diff.tv_sec, diff.tv_usec));
			LM_TODO(("If process only been running for a few seconds, don't restart it - use this to initiate a Controller takeover"));
			newProcessP = processAdd(processP->type, processP->name, processP->alias, processP->controllerHost, pid, &now);
			processSpawn(newProcessP);
			processRemove(processP);
		}
	}

	return 0;
}



/* ****************************************************************************
*
* processSpawn - 
*/
void SamsonSpawner::processSpawn(ss::Process* processP)
{
	pid_t  pid;
	char*  argV[50];
	int    argC = 0;

	if (processP->type == ss::PtWorker)
	{
		argV[argC++] = (char*) "samsonWorker";

		argV[argC++] = (char*) "-alias";
		argV[argC++] = processP->alias;
		argV[argC++] = (char*) "-controller";
		argV[argC++] = (char*) processP->controllerHost;
	}
	else if (processP->type == ss::PtController)
	{
		argV[argC++] = (char*) "samsonController";
	}
	else
		LM_X(1, ("Will only start workers and controllers - bad process type %d", processP->type));

	if (processP->verbose == true)   argV[argC++] = (char*) "-v";
	if (processP->debug   == true)   argV[argC++] = (char*) "-d";
	if (processP->reads   == true)   argV[argC++] = (char*) "-r";
	if (processP->writes  == true)   argV[argC++] = (char*) "-w";
	if (processP->toDo    == true)   argV[argC++] = (char*) "-toDo";

	char traceLevels[512];
	lmTraceGet(traceLevels, sizeof(traceLevels), processP->traceLevels);
	if (traceLevels[0] != 0)
	{
		argV[argC++] = (char*) "-t";
		argV[argC++] = traceLevels;
	}

	argV[argC] = NULL;

	LM_M(("Spawning process '%s'", argV[0]));
	for (int ix = 0; ix < argC; ix++)
		LM_M(("  argV[%d]: '%s'", ix, argV[ix]));

	pid = fork();
	if (pid == 0)
	{
		int ix;
		int s;

		LM_V(("Spawning a '%s' with %d parameters", argV[0], argC));

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

	processAdd(processP->type, argV[0], processP->alias, processP->controllerHost, pid, NULL);
}



/* ****************************************************************************
*
* processVector - 
*/
static int processVector(ss::Endpoint* ep, ss::ProcessVector* pVec)
{
	int           ix;
	ss::Process*  process;
	Host*         hostP;
	Host*         localhostP;
	int           procVecSize;
	int           error = 0;

	LM_M(("Received a procVec with %d processes from '%s'", pVec->processes, ep->name.c_str()));

	procVecSize = sizeof(ss::ProcessVector) + pVec->processes * sizeof(ss::Process);
	procVec     = (ss::ProcessVector*) malloc(procVecSize);
	if (procVec == NULL)
		LM_X(1, ("error allocating space for proc vector (%d bytes): %s", procVecSize, strerror(errno)));

	memcpy(procVec, pVec, procVecSize);

	if (networkP->hostMgr->lookup("localhost") == NULL)
		LM_X(1, ("Cannot find 'localhost' in my host manager"));

	platformProcessesSave(procVec);

	LM_TODO(("Send 'Die' to Controller"));
	LM_TODO(("Send 'Die' to Worker"));

	localhostP = networkP->hostMgr->lookup("localhost");
	if (localhostP == NULL)
		LM_X(1, ("Cannot find 'localhost' in host manager ..."));

	LM_M(("Got a Process Vector with %d processes", procVec->processes));
	for (ix = 0; ix < procVec->processes; ix++)
	{
		int fd;

		hostP = networkP->hostMgr->lookup(procVec->processV[ix].host);
		if (hostP != NULL)
		{
			LM_TODO(("Check if we're really connected to the spawner in '%s' before continuing", procVec->processV[ix].host));
			LM_M(("Not connecting to spawner in '%s'", hostP->name));
			continue;
		}

		LM_M(("ProcessVector: current host is '%s'", procVec->processV[ix].host));
		LM_M(("ProcessVector: localhost    is '%s'", localhostP->name));

		if (hostP == localhostP)
		{
			LM_M(("ProcessVector: not connecting to myself ..."));
			continue;
		}

		process = &procVec->processV[ix];
		LM_M(("Inserting host for process %d ('%s')", ix, process->host));
		hostP   = networkP->hostMgr->insert(process->host, NULL);
		LM_M(("Inserted host for process %d ('%s')", ix, hostP->name));

		if (networkP->endpointLookup(ss::Endpoint::Spawner, hostP->name) != NULL)
		{
			LM_M(("Already connected to spawner in '%s'", hostP->name));
			LM_TODO(("What about sending the ProcessVector ..."));
			continue;
		}

		LM_M(("ProcessVector: connecting to spawner in '%s'", hostP->name));
		fd = iomConnect(hostP->name, SPAWNER_PORT);
		if (fd == -1)
		{
			LM_E(("iomConnect('%s', %d): %s", hostP->name, SPAWNER_PORT, strerror(errno)));
			error = 1;
		}
		else
			networkP->endpointAdd("New Spawner", fd, fd, "Spawner", "Spawner", procVec->processes - 1, ss::Endpoint::Spawner, hostP->name, SPAWNER_PORT);
	}

	LM_M(("Done connecting to spawners, now I await the callbacks to 'endpointUpdate'"));

	return error;
}



/* ****************************************************************************
*
* SamsonSpawner::receive - 
*/
int SamsonSpawner::receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP)
{
	int                 s;
	ss::Endpoint*       ep      = networkP->endpoint[fromId];

	ss::Process*        process = (ss::Process*)       dataP;
	ss::ProcessVector*  procVec = (ss::ProcessVector*) dataP;


	if (ep == NULL)
		LM_X(1, ("Received a message for a NULL endpoint ..."));

	switch (headerP->code)
	{
	case ss::Message::ProcessSpawn:
		processSpawn(process);
		break;

	case ss::Message::ProcessVector:
		if (headerP->type == ss::Message::Msg)
		{
			LM_M(("Received a ProcessVector from '%s'. dataLen: %d", ep->name.c_str(), headerP->dataLen));
			if ((procVec->processes <= 0) || (procVec->processes > 10))
				LM_X(1, ("Bad number of processes in process vector: %d", procVec->processes));

			LM_M(("--------------- Process Vector ---------------"));
			for (int ix = 0; ix < procVec->processes; ix++)
				LM_M(("  %02d: %-20s %-20s %-20s %d", ix, procVec->processV[ix].name, procVec->processV[ix].host, procVec->processV[ix].alias, procVec->processV[ix].port));
			LM_M(("----------------------------------------------"));

			s = processVector(ep, procVec);
			LM_M(("Sending ProcessVector ack to spawner in '%s'", ep->name.c_str()));
			if (s == 0)
				iomMsgSend(ep, networkP->endpoint[0], ss::Message::ProcessVector, ss::Message::Ack);
			else
				iomMsgSend(ep, networkP->endpoint[0], ss::Message::ProcessVector, ss::Message::Nak);
		}
		else if (headerP->type == ss::Message::Ack)
			LM_M(("Received a ProcessVector Ack from '%s'", ep->name.c_str()));
		else if (headerP->type == ss::Message::Nak)
			LM_M(("Received a ProcessVector Nak from '%s'", ep->name.c_str()));
		else
			LM_X(1, ("Bad message type in ProcessVector message (%d)", headerP->type));
		break;

	default:
		LM_X(1, ("Don't know how to treat '%s' message", ss::Message::messageCode(headerP->code)));
	}

	return 0;
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	SamsonSpawner* spawnerP;

#if 0
	ss::Process    process;
    printf("sizeof(ss::Process): %d\n", (int) sizeof(ss::Process));
    printf("state at offset   %d\n", (int) ((long) &process.state   - (long) &process));
    printf("verbose at offset %d\n", (int) ((long) &process.verbose - (long) &process));
    printf("workers at offset %d\n", (int) ((long) &process.workers - (long) &process));
    exit(1);
#endif

	paConfig("prefix",                        (void*) "SSS_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	LM_T(LmtInit, ("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtInit, ("  %02d: '%s'", ix, argV[ix]));

	if (notdaemon == false)
		daemonize();

	processListInit(10);

	networkP = new ss::Network(ss::Endpoint::Spawner, "Spawner", SPAWNER_PORT, 80, 0);
	networkP->init(NULL);

	spawnerP = new SamsonSpawner(networkP);

	networkP->setDataReceiver(spawnerP);
	networkP->setEndpointUpdateReceiver(spawnerP);
	networkP->setTimeoutReceiver(spawnerP, 1, 0); // Timeout every second

	networkP->run();

	return 0;
}
