/* ****************************************************************************
*
* FILE                     main_samsonSpawner.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <sys/time.h>           // gettimeofday
#include <sys/types.h>          // pid_t
#include <sys/wait.h>           // waitpid
#include <signal.h>             // kill, SIGINT, ...

#include "parseArgs.h"          // parseArgs
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace levels

#include "NetworkInterface.h"   // DataReceiverInterface
#include "Network.h"            // Network
#include "iomMsgSend.h"         // iomMsgSend
#include "iomMsgRead.h"         // iomMsgRead
#include "iomMsgAwait.h"        // iomMsgAwait
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
ss::ProcessVector*  procVec      = NULL;   // Should use Network's procVec, not a local copy ... 



/* ****************************************************************************
*
* SamsonSpawner - 
*/
class SamsonSpawner : public ss::DataReceiverInterface, public ss::TimeoutReceiverInterface, public ss::EndpointUpdateReceiverInterface
{
public:
	SamsonSpawner(ss::Network* nwP) { networkP = nwP; }

	virtual int  receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP);
	virtual void init(ss::ProcessVector* procVec);
	virtual int  endpointUpdate(ss::Endpoint* ep, ss::Endpoint::UpdateReason reason, const char* reasonText, void* info = NULL);
	virtual int  timeoutFunction(void);

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
	Host*                 controllerHostP;
	Host*                 localhostP;


	if ((reason == ss::Endpoint::HelloReceived) && (ep->type == ss::Endpoint::Spawner))
	{
		if (procVec == NULL)
			return 0;

		controllerHostP  = networkP->hostMgr->lookup(procVec->processV[0].host);
		localhostP       = networkP->hostMgr->lookup("localhost");

		//
		// samsonSpawner that runs in controller host distributes to the rest of spawners
		//
		if (controllerHostP != localhostP)
		{
			LM_M(("controllerHostP: %p (%s)", controllerHostP, procVec->processV[0].host));
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

			if (access(SAMSON_PLATFORM_PROCESSES, R_OK) != 0)
				LM_W(("'%s' died - NOT restarting it as the platform processes file isn't in place", processP->name));
			else
			{
				LM_W(("'%s' died - restarting it", processP->name));

				if (gettimeofday(&now, NULL) != 0)
					LM_X(1, ("gettimeofday failed (fatal error): %s", strerror(errno)));

				timeDiff(&processP->startTime, &now, &diff);

				LM_M(("Process %d '%s' died after %d.%06d seconds of uptime", processP->pid, processP->name, diff.tv_sec, diff.tv_usec));
				LM_TODO(("If process only been running for a few seconds, don't restart it - use this to initiate a Controller takeover"));
				newProcessP = processAdd(processP->type, processP->name, processP->alias, processP->controllerHost, pid, &now);
				processSpawn(newProcessP);
			}

			processRemove(processP);
		}
	}

	return 0;
}



/* ****************************************************************************
*
* processesStart - 
*/
static void processesStart(ss::ProcessVector* procVec)
{
	ss::Process*  processP;
	int           ix;
	Host*         localhostP;
	Host*         hostP;

	localhostP = networkP->hostMgr->lookup("localhost");

	LM_M(("Got %d processes", procVec->processes));
	for (ix = 0; ix < procVec->processes; ix++)
	{
		processP = &procVec->processV[ix];

		hostP = networkP->hostMgr->lookup(processP->host);
		if (hostP != localhostP)
		{
			LM_M(("Cannot start process in other host '%s'", processP->host));
			continue;
		}

		LM_M(("Spawning process '%s'", hostP->name));

		processP->verbose = lmVerbose;
		processP->debug   = lmDebug;
		processP->reads   = lmReads;
		processP->writes  = lmWrites;

		// processP->traceLevels

		processSpawn(processP);
	}	
}



/* ****************************************************************************
*
* spawnersConnect
*
* Disconnect from spawners that run in hosts not in 'procVec' and connect to added spawners
* But, for now, just connect to the spawners.
*/
void spawnersConnect(ss::ProcessVector* procVec)
{
	Host*          localhostP;
	Host*          hostP;
	int            ix;
	ss::Process*   processP;
	ss::Endpoint*  ep;
	int            s;
	int            procVecSize;

	localhostP = networkP->hostMgr->lookup("localhost");
	if (localhostP == NULL)
		LM_X(1, ("Error finding localhost in host manager"));

	for (ix = 0; ix < procVec->processes; ix++)
	{
		int fd;

		processP = &procVec->processV[ix];		
		hostP    = networkP->hostMgr->lookup(processP->host);

		if (hostP != NULL)
		{
			ss::Endpoint* ep;

			ep = networkP->endpointLookup(ss::Endpoint::Spawner, hostP->name);

			if ((ep != NULL) && (ep->state == ss::Endpoint::Connected))
			{
				LM_M(("Not connecting to spawner in '%s'", hostP->name));
				continue;
			}
		}

		LM_M(("ProcessVector: current host is '%s'", procVec->processV[ix].host));
		LM_M(("ProcessVector: localhost    is '%s'", localhostP->name));

		if (hostP == localhostP)
		{
			LM_M(("ProcessVector: not connecting to myself ..."));
			continue;
		}

		processP = &procVec->processV[ix];
		LM_M(("Inserting host for process %d ('%s')", ix, processP->host));
		hostP   = networkP->hostMgr->insert(processP->host, NULL);
		LM_M(("Inserted host for process %d ('%s')", ix, hostP->name));

		if ((ep = networkP->endpointLookup(ss::Endpoint::Spawner, hostP->name)) != NULL)
		{
			LM_M(("Already connected to spawner in '%s' - sending it the process vector", hostP->name));

			procVecSize = sizeof(ss::ProcessVector) + procVec->processes * sizeof(ss::Process);
			s = iomMsgSend(ep, networkP->endpoint[0], ss::Message::ProcessVector, ss::Message::Msg, procVec, procVecSize);
			if (s != 0)
				LM_E(("Error sending ProcessVector message to spawner in '%s'", ep->name.c_str()));
			continue;
		}

		LM_M(("ProcessVector: connecting to spawner in '%s'", hostP->name));
		fd = iomConnect(hostP->name, SPAWNER_PORT);
		if (fd == -1)
			LM_E(("iomConnect('%s', %d): %s", hostP->name, SPAWNER_PORT, strerror(errno)));
		else
			networkP->endpointAdd("New Spawner", fd, fd, "Spawner", "Spawner", procVec->processes - 1, ss::Endpoint::Spawner, hostP->name, SPAWNER_PORT);
	}
}




/* ****************************************************************************
*
* processVector - 
*/
static int processVector(ss::Endpoint* ep, ss::ProcessVector* pVec)
{
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

	processesStart(procVec);
	spawnersConnect(procVec);

	return error;
}



/* ****************************************************************************
*
* processesShutdown - 
*/
static void processesShutdown(void)
{
	ss::Process*  processP;
	ss::Process** processList;
	int           processes;
	int           ix;
	int           s;

	processes   = processMaxGet();
	processList = processListGet();

	for (ix = processes - 1; ix >= 0; ix--)
	{
		processP = processList[ix];

		if (processP == NULL)
			continue;

		if (processP->pid <= 0)
		{
			LM_M(("process '%s' has pid %d ...", processP->name, processP->pid));
			continue;
		}

		if (processP->endpoint != NULL)
		{
			LM_M(("Sending 'Die' to %s", processP->name));

			s = iomMsgSend(processP->endpoint, networkP->endpoint[0], ss::Message::Die, ss::Message::Msg);
			LM_M(("iomMsgSend returned %d", s));
			if (s != 0)
				LM_E(("Error sending 'Die' to %s", processP->endpoint->name.c_str()));

			usleep(50000);
		}

		LM_M(("Sending SIGINT to '%s'", processP->name));
		kill(processP->pid, SIGINT);
		usleep(50000);

		LM_M(("Sending SIGKILL to '%s'", processP->name));
		kill(processP->pid, SIGKILL);

		if (processP->endpoint)
			networkP->endpointRemove(processP->endpoint, "shutting down");

		// NOT removing process here - that will be done when the process is waited for
	}

	processListShow("After shutdown");
}



/* ****************************************************************************
*
* hello - 
*/
static int hello(ss::Endpoint* me, ss::Endpoint* ep, int* errP)
{
	int                             s;
	ss::Message::Header             header;
	ss::Message::MessageCode        msgCode;
	ss::Message::MessageType        msgType;
	char                            data[1];
	void*                           dataP   = data;
	int                             dataLen = sizeof(data);
	ss::Message::HelloData          hello;

	*errP = 0;

	LM_M(("Awaiting Hello message"));
	s = iomMsgAwait(ep->rFd, 5, 0);
	if (s != 1)
		LM_RE(s, ("error awaiting hello from '%s'", ep->name.c_str()));

	LM_M(("Reading Hello header"));
	s = iomMsgPartRead(ep, "hello header", (char*) &header, sizeof(header));
	if (s != sizeof(header))
		LM_RE(-1, ("error reading hello header from '%s'", ep->name.c_str()));

	LM_M(("Reading Hello data"));
	s = iomMsgRead(ep, &header, &msgCode, &msgType, &dataP, &dataLen);
	if (s != 0)
		LM_RE(-1, ("error reading hello data from '%s'", ep->name.c_str()));

	memset(&hello, 0, sizeof(hello));

	strncpy(hello.name,   "samsonSpawner", sizeof(hello.name));
	strncpy(hello.ip,     "myip",          sizeof(hello.ip));
	strncpy(hello.alias,  "samsonSetup",   sizeof(hello.alias));

	hello.type = ss::Endpoint::Spawner;

	s = iomMsgSend(ep, me, ss::Message::Hello, ss::Message::Ack, &hello, sizeof(hello));
	if (s != 0)
		LM_RE(-1, ("error sending Hello ack to '%s'", ep->name.c_str()));

	return 0;
}



/* ****************************************************************************
*
* dieSend - 
*/
static int dieSend(ss::Endpoint* me, ss::Endpoint* ep)
{
	int s;

	LM_M(("Sending Die message to endpoint '%s'", ep->name.c_str()));
	s = iomMsgSend(ep, me, ss::Message::Die, ss::Message::Msg);
	if (s != 0)
		LM_RE(1, ("error sending Die to endpoint '%s'", ep->name.c_str()));

	return 0;
}



/* ****************************************************************************
*
* processesSteal - 
*/
static void processesSteal(void)
{
	int            fd;
	int            err = 0;
	ss::Endpoint   ep;
	ss::Endpoint   me;



	//
	// Connecting to and sending 'Die' to worker
	//
	fd = iomConnect("localhost", WORKER_PORT);
	if (fd != -1)
	{
		LM_M(("Found an 'orphaned' worker - killing it"));
		ep.rFd   = fd;
		ep.wFd   = fd;
		ep.name  = "samsonWorkerToDie";
		ep.type  = ss::Endpoint::Worker;
		ep.ip    = (char*) "localhost";

		hello(networkP->endpoint[0], &ep, &err);
		if (err != 0)
			LM_E(("Error Helloing worker"));
		else
			dieSend(networkP->endpoint[0], &ep);
	}
	else
		LM_M(("Found NO 'orphaned' worker"));



	//
	// Connecting to and sending 'Die' to controller
	//
	fd = iomConnect("localhost", CONTROLLER_PORT);
	if (fd != -1)
	{
		LM_M(("Found an 'orphaned' controller - killing it"));
		ep.rFd   = fd;
		ep.wFd   = fd;
		ep.name  = "samsonControllerToDie";
		ep.type  = ss::Endpoint::Controller;
		ep.ip    = (char*) "localhost";
	
		hello(networkP->endpoint[0], &ep, &err);
		if (err != 0)
			LM_E(("Error Helloing controller"));
		else
			dieSend(networkP->endpoint[0], &ep);
	}
	else
		LM_M(("Found NO 'orphaned' controller"));
}



/* ****************************************************************************
*
* SamsonSpawner::init - 
*/
void SamsonSpawner::init(ss::ProcessVector* procVec)
{
	processesSteal();
	processesShutdown();

	if (procVec != NULL)
	{
		processesStart(procVec);
		spawnersConnect(procVec);
	}
}



/* ****************************************************************************
*
* spawnerForward - 
*/
void spawnerForward(ss::Message::MessageCode code)
{
	int    s;
	Host*  localHostP = networkP->hostMgr->lookup("localhost");
	Host*  hostP;

	if (localHostP == NULL)
		LM_X(1, ("cannot find 'localhost' in host manager host list"));

	for (int ix = 4; ix < networkP->Endpoints; ix++)
	{
		if (networkP->endpoint[ix] == NULL)
			continue;

		if (networkP->endpoint[ix]->type != ss::Endpoint::Spawner)
			continue;

		hostP = networkP->hostMgr->lookup(networkP->endpoint[ix]->ip);
		if (localHostP == NULL)
			LM_X(1, ("cannot find '%s' in host manager host list", networkP->endpoint[ix]->ip));
		else if (hostP == localHostP)
			LM_W(("How come I'm connected to myself ?"));
		else
		{
			s = iomMsgSend(networkP->endpoint[ix], networkP->endpoint[0], code, ss::Message::Msg);
			if (s != 0)
				LM_E(("error forwarding '%s' to spawner in '%s'", messageCode(code)));
		}
	}
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
	case ss::Message::Reset:
		if (headerP->type == ss::Message::Msg)
		{
			LM_M(("Got a Reset message from '%s'", ep->name.c_str()));
			LM_M(("unlinking '%s'", SAMSON_PLATFORM_PROCESSES));
			unlink(SAMSON_PLATFORM_PROCESSES);

			if (ep->type == ss::Endpoint::Setup) 
			{
				LM_M(("Got RESET from samsonSetup - forwarding RESET to all spawners"));
				spawnerForward(headerP->code);
			}
			else
				LM_M(("Got RESET from '%s' - NOT forwarding RESET to all spawners", ep->name.c_str()));
			
			LM_M(("killing local processes"));
			processesShutdown();
			LM_M(("Sending ack to RESET message to %s@%s", ep->name.c_str(), ep->ip));
			iomMsgSend(ep, networkP->endpoint[0], headerP->code, ss::Message::Ack);
		}
		break;

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
			LM_M(("Sending ProcessVector ack to '%s' in '%s'", ep->name.c_str(), ep->ip));
			if (s == 0)
				iomMsgSend(ep, networkP->endpoint[0], headerP->code, ss::Message::Ack);
			else
				iomMsgSend(ep, networkP->endpoint[0], headerP->code, ss::Message::Nak);
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

	processListInit(101);

	networkP = new ss::Network(ss::Endpoint::Spawner, "Spawner", SPAWNER_PORT, 80, 0);
	
	spawnerP = new SamsonSpawner(networkP);

	networkP->setDataReceiver(spawnerP);
	networkP->setEndpointUpdateReceiver(spawnerP);
	networkP->setTimeoutReceiver(spawnerP, 1, 0); // Timeout every second

	networkP->init(NULL);
	networkP->run();

	return 0;
}
