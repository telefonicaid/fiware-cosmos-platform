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
#include "paConfig.h"           // paConfigCleanup
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
bool  fg;
bool  noRestarts;



/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-fg",          &fg,          "FOREGROUND",   PaBool,   PaOpt,  false,  false,   true, "don't start as daemon"   },
	{ "-noRestarts",  &noRestarts,  "NO_RESTARTS",  PaBool,   PaOpt,  false,  false,   true, "don't restart processes" },

	PA_END_OF_ARGS
};



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
* Global variables
*/
int                 logFd             = -1;
ss::Network*        networkP          = NULL;
ss::ProcessVector*  procVec           = NULL;   // Should use Network's procVec, not a local copy ... 
SamsonSpawner*      spawnerP          = NULL;
static bool         restartInProgress = false;



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
			LM_T(LmtProcessVector, ("Controller does not run in this machine - not distributing ProcessVector"));
			return 0;
		}

		LM_T(LmtProcessVector, ("ProcessVector: got Hello Msg/Ack from %s@%s", ep->name.c_str(), ep->ip));
		if (headerP->type == ss::Message::Msg)
		{
			LM_T(LmtProcessVector, ("ProcessVector: got Hello Msg from %s@%s", ep->name.c_str(), ep->ip));
			procVecSize = sizeof(ss::ProcessVector) + procVec->processes * sizeof(ss::Process);

			LM_T(LmtProcessVector, ("Sending ProcessVector to spawner in '%s' (%d processes)", ep->ip, procVec->processes));
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


	ss::Process*    newProcessP;
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
		processSpawn(newProcessP);
	}

	processRemove(processP);
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

	for (ix = 0; ix < procVec->processes; ix++)
	{
		processP = &procVec->processV[ix];

		hostP = networkP->hostMgr->lookup(processP->host);
		if (hostP != localhostP)
			continue;

		LM_T(LmtProcess, ("Spawning process '%s'", processP->name));

		processP->verbose = lmVerbose;
		processP->debug   = lmDebug;
		processP->reads   = lmReads;
		processP->writes  = lmWrites;
		processP->pid     = 0;
		// processP->traceLevels ...

		ss::Process* pP;
		pP = processAdd(processP->type, processP->name, processP->alias, processP->controllerHost, 0, NULL);
		processSpawn(pP);
	}
}



/* ****************************************************************************
*
* spawnersConnect
*
* Disconnect from spawners that run in hosts not in 'procVec' and connect to added spawners
* But, for now, just connect to the spawners.
*/
void spawnersConnect(ss::ProcessVector* procVec, bool force = false)
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

#if 1
		if (hostP != NULL)
		{
			ss::Endpoint* ep;

			ep = networkP->endpointLookup(ss::Endpoint::Spawner, hostP->name);
			
			if ((ep != NULL) && (ep->state == ss::Endpoint::Connected))
			{
				LM_T(LmtProcessVector, ("Not connecting to spawner in '%s' - nor sending it the ProcessVector", hostP->name));
				continue;
			}
		}
#endif

		if (hostP == localhostP)
		{
			LM_T(LmtProcessVector, ("ProcessVector: not connecting to myself ..."));
			continue;
		}
		else if (hostP == NULL)
		{
			LM_T(LmtProcessVector, ("Inserting host for process %d ('%s')", ix, processP->host));
			hostP = networkP->hostMgr->insert(processP->host, NULL);
			if (hostP == NULL)
				LM_X(1, ("error inserting host '%s'", processP->host));

			LM_T(LmtProcessVector, ("Inserted host for process %d ('%s')", ix, hostP->name));
		}

		LM_T(LmtProcessVector, ("ProcessVector: current host is '%s'", hostP->name));
		LM_T(LmtProcessVector, ("ProcessVector: localhost    is '%s'", localhostP->name));

		//
		// Only connect to spawners whose host names comes before localhost alphabetically
		//
		if ((strcmp(hostP->name, localhostP->name) > 0) && (force == false))
		{
			LM_T(LmtProcessVector, ("Not connecting to '%s' (I am '%s')", hostP->name, localhostP->name));
			continue;
		}

		if ((ep = networkP->endpointLookup(ss::Endpoint::Spawner, hostP)) != NULL)
		{
			Host* controllerHostP;

			controllerHostP = networkP->hostMgr->lookup(procVec->processV[0].host);

			if (controllerHostP == localhostP)
			{
				LM_T(LmtProcessVector, ("Already connected to spawner in '%s' - sending it the process vector", hostP->name));

				procVecSize = sizeof(ss::ProcessVector) + procVec->processes * sizeof(ss::Process);
				s = iomMsgSend(ep, networkP->endpoint[0], ss::Message::ProcessVector, ss::Message::Msg, procVec, procVecSize);
				if (s != 0)
					LM_E(("Error sending ProcessVector message to spawner in '%s'", ep->name.c_str()));
			}
			else
				LM_T(LmtProcessVector, ("Already connected to spawner in '%s' - NOT sending it the process vector", hostP->name));

			continue;
		}

		LM_T(LmtProcessVector, ("ProcessVector: connecting to spawner in '%s'", hostP->name));
		fd = iomConnect(hostP->name, SPAWNER_PORT);
		if (fd == -1)
			LM_E(("iomConnect('%s', %d): %s", hostP->name, SPAWNER_PORT, strerror(errno)));
		else
			networkP->endpointAdd("New Spawner", fd, fd, "Spawner", "Spawner", procVec->processes - 1, ss::Endpoint::Spawner, hostP->name, SPAWNER_PORT);
	}
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
	ss::Message::HelloData          helloIn;
	ss::Message::HelloData          helloOut;
	void*                           dataP     = (void*) &helloIn;
	int                             dataLen   = sizeof(helloIn);
	Host*                           localhost = networkP->hostMgr->lookup("localhost");
	char*                           hostname;

	if (localhost)
		hostname = localhost->name;
	else
		hostname = (char*) "localhost";

	*errP = 0;

	LM_T(LmtHello, ("Awaiting Hello message"));
	s = iomMsgAwait(ep->rFd, 5, 0);
	if (s != 1)
		LM_RE(s, ("error awaiting hello from '%s'", ep->name.c_str()));

	LM_T(LmtHello, ("Reading Hello header"));
	s = iomMsgPartRead(ep, "hello header", (char*) &header, sizeof(header));
	if (s != sizeof(header))
		LM_RE(-1, ("error reading hello header from '%s'", ep->name.c_str()));

	LM_T(LmtHello, ("Reading Hello data"));
	s = iomMsgRead(ep, &header, &msgCode, &msgType, &dataP, &dataLen);
	if (s != 0)
		LM_RE(-1, ("error reading hello data from '%s'", ep->name.c_str()));

	memset(&helloOut, 0, sizeof(helloOut));

	strncpy(helloOut.name,   "samsonSpawner", sizeof(helloOut.name));
	strncpy(helloOut.ip,     hostname,        sizeof(helloOut.ip));
	strncpy(helloOut.alias,  "samsonSetup",   sizeof(helloOut.alias));

	helloOut.type = ss::Endpoint::Spawner;

	s = iomMsgSend(ep, me, ss::Message::Hello, ss::Message::Ack, &helloOut, sizeof(helloOut));
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

	LM_T(LmtDie, ("Sending Die message to endpoint '%s'", ep->name.c_str()));
	s = iomMsgSend(ep, me, ss::Message::Die, ss::Message::Msg);
	if (s != 0)
		LM_RE(1, ("error sending Die to endpoint '%s'", ep->name.c_str()));

	return 0;
}



/* ****************************************************************************
*
* localProcessesKill - 
*/
static void localProcessesKill(void)
{
	int            fd;
	int            err = 0;
	ss::Endpoint   me;
	int            s;
	bool           oldRestartInProgress;

	oldRestartInProgress = restartInProgress;
	restartInProgress    = true;

	//
	// Connecting to and sending 'Die' to worker
	//
	fd = iomConnect("localhost", WORKER_PORT);
	if (fd != -1)
	{
		ss::Endpoint   ep;

		ep.rFd   = fd;
		ep.wFd   = fd;
		ep.name  = "samsonWorkerToDie";
		ep.type  = ss::Endpoint::Worker;
		ep.ip    = strdup("localhost");

		hello(networkP->endpoint[0], &ep, &err);
		if (err != 0)
			LM_E(("Error Helloing worker"));
		else
		{
			LM_V(("Sending Die to worker"));
			dieSend(networkP->endpoint[0], &ep);
		}
	}

	usleep(200000);
	s = system("killall samsonWorker > /dev/null 2>&1");
	if (s != 0)
		LM_E((" system(\"killall samsonWorker\"): %s", strerror(errno)));

	usleep(200000);
	s = system("killall -9 samsonWorker > /dev/null 2>&1");
	if (s != 0)
		LM_E((" system(\"killall -9 samsonWorker\"): %s", strerror(errno)));



	//
	// Connecting to and sending 'Die' to controller
	//
	fd = iomConnect("localhost", CONTROLLER_PORT);
	if (fd != -1)
	{
		ss::Endpoint   ep;

		ep.rFd   = fd;
		ep.wFd   = fd;
		ep.name  = "samsonControllerToDie";
		ep.type  = ss::Endpoint::Controller;
		ep.ip    = strdup("localhost");
	
		hello(networkP->endpoint[0], &ep, &err);
		if (err != 0)
			LM_E(("Error Helloing controller"));
		else
		{
			LM_V(("Sending Die to controller"));
			dieSend(networkP->endpoint[0], &ep);
		}
	}

	usleep(200000);
	s = system("killall samsonController > /dev/null 2>&1");
	if (s != 0)
		LM_E((" system(\"killall samsonController\"): %s", strerror(errno)));

	usleep(200000);
	s = system("killall -9 samsonController > /dev/null 2>&1");
	if (s != 0)
		LM_E((" system(\"killall -9 samsonController\"): %s", strerror(errno)));

	spawnerP->timeoutFunction();
	spawnerP->timeoutFunction();
	restartInProgress = oldRestartInProgress;
}



/* ****************************************************************************
*
* processVector - 
*/
static int processVector(ss::Endpoint* ep, ss::ProcessVector* pVec)
{
	int           procVecSize;
	int           error = 0;

	LM_T(LmtProcessVector, ("Received a procVec with %d processes from %s@%s", pVec->processes, ep->name.c_str(), ep->ip));

	localProcessesKill();

	if (procVec != NULL)
	   free(procVec);

	procVecSize = sizeof(ss::ProcessVector) + pVec->processes * sizeof(ss::Process);
	procVec     = (ss::ProcessVector*) malloc(procVecSize);
	if (procVec == NULL)
		LM_X(1, ("error allocating space for proc vector (%d bytes): %s", procVecSize, strerror(errno)));

	memcpy(procVec, pVec, procVecSize);

	if (networkP->hostMgr->lookup("localhost") == NULL)
		LM_X(1, ("Cannot find 'localhost' in my host manager"));

	
	platformProcessesSave(procVec);

	processesStart(procVec);


	//
	// The spawner that gets the message from 'samsonSetup' connects to all spawners
	//
	if (ep->type == ss::Endpoint::Setup) 
		spawnersConnect(procVec, true);
	else
		spawnersConnect(procVec, false);

	return error;
}



/* ****************************************************************************
*
* SamsonSpawner::init - 
*/
void SamsonSpawner::init(ss::ProcessVector* procVec)
{
	localProcessesKill();

	if (procVec != NULL)
	{
		processesStart(procVec);
		spawnersConnect(procVec);
	}

	processListShow("INIT", true);
	networkP->endpointListShow("INIT", true);
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
			LM_V(("Got a Reset message from '%s'", ep->name.c_str()));
			unlink(SAMSON_PLATFORM_PROCESSES);

			if (ep->type == ss::Endpoint::Setup) 
			{
				LM_T(LmtReset, ("Got RESET from samsonSetup - forwarding RESET to all spawners"));
				spawnerForward(headerP->code);
			}
			else
				LM_T(LmtReset, ("Got RESET from '%s' - NOT forwarding RESET to all spawners", ep->name.c_str()));
			
			LM_T(LmtReset, ("killing local processes"));
			localProcessesKill();
			LM_T(LmtReset, ("Sending ack to RESET message to %s@%s", ep->name.c_str(), ep->ip));
			iomMsgSend(ep, networkP->endpoint[0], headerP->code, ss::Message::Ack);

			processListShow("Got RESET", true);
			networkP->endpointListShow("Got RESET", true);
		}
		break;

	case ss::Message::ProcessSpawn:
		processSpawn(process);
		break;

	case ss::Message::ProcessVector:
		if (headerP->type == ss::Message::Msg)
		{
			LM_V(("Got a ProcessVector message"));
			if ((procVec->processes <= 0) || (procVec->processes > 10))
				LM_X(1, ("Bad number of processes in process vector: %d", procVec->processes));

			LM_T(LmtProcessVector, ("--------------- Process Vector ---------------"));
			for (int ix = 0; ix < procVec->processes; ix++)
				LM_T(LmtProcessVector, ("  %02d: %-20s %-20s %-20s %d", ix, procVec->processV[ix].name, procVec->processV[ix].host, procVec->processV[ix].alias, procVec->processV[ix].port));
			LM_T(LmtProcessVector, ("----------------------------------------------"));

			s = processVector(ep, procVec);
			LM_T(LmtProcessVector, ("Sending ProcessVector ack to '%s' in '%s'", ep->name.c_str(), ep->ip));
			if (s == 0)
			{
				iomMsgSend(ep, networkP->endpoint[0], headerP->code, ss::Message::Ack);
				platformProcessesSave(procVec);
			}
			else
				iomMsgSend(ep, networkP->endpoint[0], headerP->code, ss::Message::Nak);
		}
		else if (headerP->type == ss::Message::Ack)
			LM_T(LmtProcessVector, ("Received a ProcessVector Ack from '%s'", ep->name.c_str()));
		else if (headerP->type == ss::Message::Nak)
			LM_T(LmtProcessVector, ("Received a ProcessVector Nak from '%s'", ep->name.c_str()));
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
* sigHandler - 
*/
void sigHandler(int sigNo)
{
	printf("Caught signal %d\n", sigNo);

	if (sigNo == SIGINT)
	{
		printf("Got SIGINT\n");
	}

	exit(1);
}



/* ****************************************************************************
*
* exitFunction - 
*/
void exitFunction(void)
{
	if (networkP)
		delete networkP;

	if (spawnerP)
		delete spawnerP;

	if (procVec)
		free(procVec);

	processListDelete();

	google::protobuf::ShutdownProtobufLibrary();

	if (progName)
		free(progName);
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	signal(SIGINT, sigHandler);

	paConfig("prefix",                        (void*) "SSS_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE@TIME  EXEC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);
	paConfigCleanup();

	LM_T(LmtInit, ("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtInit, ("  %02d: '%s'", ix, argV[ix]));

	if (fg == false)
		daemonize();

	atexit(exitFunction);

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
