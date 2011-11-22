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

#include "samson/common/Process.h"
#include "samson/common/platformProcesses.h"  // platformProcessesSave
#include "samson/common/ports.h"              // Samson platform ports
#include "samson/common/samsonVars.h"         // SAMSON_EXTERNAL_VARS
#include "samson/network/NetworkInterface.h"
#include "samson/network/Packet.h"
#include "samson/network/Network2.h"

#include "processList.h"                      // processListInit, Add, Remove and Lookup
#include "globals.h"                          // Global variables for Spawner
#include "SamsonSpawner.h"                    // Own interface



/* ****************************************************************************
*
* External declaration of common Samson variables
*/
SAMSON_EXTERNAL_VARS;



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
	networkP               = new Network2(Endpoint2::Spawner);
	restartInProgress      = false;

	networkP->setPacketReceiver(this);
	networkP->epMgr->callbackSet(samson::EndpointManager::Periodic, periodic, this);
	networkP->tmoSet(0, 500000);
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
*  localProcVecGenerate - 
*/ 
void SamsonSpawner::localProcVecGenerate(void)
{
	samson::ProcessVector* pVec;
	samson::Process*       p;
	Host*                  hostP;

	hostP = networkP->epMgr->hostMgr->lookup("localhost");

	pVec = (samson::ProcessVector*) malloc(sizeof(samson::ProcessVector) + 2 * sizeof(samson::Process));
	if (pVec == NULL)
		LM_X(1, ("error allocating room for two processes: %s", strerror(errno)));

	pVec->processes      = 2;
	pVec->processVecSize = 2 * sizeof(samson::Process);

	// Controller
	p = &pVec->processV[0];
	p->type = samson::PtController;
	p->id   = 1;
	strcpy(p->name, "samsonController");
	strcpy(p->alias, "NO ALIAS");
	strcpy(p->host, hostP->name);
	p->port = CONTROLLER_PORT;

	// Worker
	p = &pVec->processV[1];
	p->type = samson::PtWorker;
	p->id   = 0;
	strcpy(p->name, "samsonWorker");
	strcpy(p->alias, "NO ALIAS");
	strcpy(p->host, hostP->name);
	p->port = WORKER_PORT;

	networkP->epMgr->procVecSet(pVec);
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

	if (local)
		localProcVecGenerate();

	networkP->epMgr->me->portSet(SPAWNER_PORT);
	networkP->epMgr->listener = (ListenerEndpoint*) networkP->epMgr->add(Endpoint2::Listener, 0, networkP->epMgr->me->hostGet(), networkP->epMgr->me->portGet(), -1, -1);

	ProcessVector* pv = platformProcessesGet();
	if (pv != NULL)
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
void SamsonSpawner::receive(Packet* packetP)
{
	Process*            process  = (Process*)       packetP->dataP;
	ProcessVector*      procVec  = (ProcessVector*) packetP->dataP;
	samson::Endpoint2*  ep       = networkP->epMgr->indexedGet((unsigned int) packetP->fromId);

	LM_T(LmtMsg, ("Treating message %s from endpoint %d", messageCode(packetP->msgCode), packetP->fromId));

	switch (packetP->msgCode)
	{
	case Message::Reset:
		LM_T(LmtMsg, ("Got a RESET message - resetting platform"));
		if (packetP->msgType == Message::Ack)
			LM_X(1, ("Spawner cannot receive Ack for Reset"));
		reset(ep);
		break;

	case Message::ProcessVector:
		if (packetP->msgType == Message::Ack)
			LM_X(1, ("Spawner cannot receive Ack for ProcessVector"));
		processVector(ep, procVec);
		break;

	case Message::ProcessSpawn:
		if (packetP->msgType == Message::Ack)
			LM_X(1, ("Spawner cannot receive Ack for ProcessSpawn"));
		LM_W(("Spawning a process without adding it to process vector ..."));
		spawn(process);
		break;

#if 0
	case Message::ProcessList:
		if (packetP->msgType == Message::Ack)
			LM_X(1, ("Spawner cannot receive Ack for ProcessList"));
		processList();
		break;
#endif

	default:
		if (ep)
			LM_X(1, ("No messages treated - got a '%s' from %s", messageCode(packetP->msgCode), ep->name()));
		else
			LM_X(1, ("No messages treated - got a '%s'", messageCode(packetP->msgCode)));
	}
}


#if 0
Status EndpointManager::starterAwait(void)
{
	Status             s;
	Message::Header    header;
	void*              dataP    = NULL;
	long               dataLen  = 0;
	Packet*            packetP  = new Packet(Message::Unknown);

	LM_TODO(("Instead of all this, perhaps I can just start the spawner and treat the messages in SamsonSpawner::receive ... ?"));

	if (listener == NULL)
		LM_RE(Error, ("Cannot await the starter to arrive if I have no Listener ..."));

	LM_T(LmtStarter, ("Awaiting samsonStarter to connect and pass the Process Vector"));
	while (1)
	{
		UnhelloedEndpoint* ep;

		LM_T(LmtStarter, ("Await FOREVER for an incoming connection"));
		if (listener->msgAwait(-1, -1, "Incoming Connection") != 0)
			LM_X(1, ("Endpoint2::msgAwait error"));

		if ((ep = listener->accept()) == NULL)
		{
			LM_E(("error accepting an incoming connection"));
			continue;
		}

		if ((s = ep->helloExchange(60, 0)) != OK)
		{
			LM_E(("Hello Exchange error: %s", status(s)));
			delete ep;
			continue;
		}

		
		// Is the newly connected peer a 'samsonStarter' ?
		if (ep->type != Endpoint2::Starter)
		{
			LM_E(("The incoming connection was from a '%s' (only Starter allowed)", ep->typeName()));
			remove(ep);
			continue;
		}


		// Reading the message (supposedly a ProcessVector)
		while (1)
		{
			// Hello exchanged, now the endpoint will send a ProcessVector message
			// Awaiting the message to arrive 
			if ((s = ep->msgAwait(60, 0, "ProcessVector Message")) != 0)
				LM_X(1, ("msgAwait(ProcessVector): %s", status(s)));

			if ((s = ep->receive(&header, &dataP, &dataLen, packetP)) != 0)
			{
				LM_E(("Endpoint2::receive error"));
				remove(ep);
				ep = NULL;
				break;
			}

			// All OK ?
			if (header.type != Message::Msg)
			{
				LM_W(("Read an unexpected '%s' Ack from '%s' - throwing it away!",  messageCode(header.code), ep->name()));
				if (dataP != NULL)
                    free(dataP);
				remove(ep);
				ep = NULL;
				break;
			}
			else if (header.code == Message::ProcessVector)
				break;
			else if (header.code == Message::Reset)
            {
				LM_T(LmtSpawner,("Got a reset, and that's OK but I don't need to do anything, I have nothing started ... Let's just keep reading ..."));
            }
			else if (header.code == Message::ProcessList)
			{
				LM_T(LmtStarter, ("Got a ProcessList, and that's OK, but I have nothing ... Let's Ack with NO DATA and continue to wait for a Process Vector ..."));
				ep->ack(Message::ProcessList);
			}
			else
			{
				LM_E(("Unexpected Message (%s %s)", messageCode(header.code), messageType(header.type)));
				if (dataP != NULL)
					free(dataP);
				remove(ep);
				continue;
			}
		}

		if (ep == NULL)
			continue;

		// Copying the process vector
		this->procVec = (ProcessVector*) malloc(dataLen);
		if (this->procVec == NULL)
			LM_X(1, ("Error allocating %d bytes for the Process Vector", dataLen));

		memcpy(this->procVec, dataP, dataLen);
		free(dataP);
		ep->ack(header.code);

		if ((s = ep->msgAwait(10, 0, "Connection Closed")) != OK)
			LM_W(("All OK, except that samsonStarter didn't close connection in time. msgAwait(): %s", status(s)));
		else if ((s = ep->receive(&header, &dataP, &dataLen, packetP)) != ConnectionClosed)
			LM_W(("All OK, except that samsonStarter didn't close connection when it was supposed to. receive(): %s", status(s)));

		show("Before removing samsonStarter");
		remove(ep);
		show("After removing samsonStarter", true);
		return OK;
	}
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
	Process*      processP;

	processListShow("periodic");

	pid = waitpid(-1, &status, WNOHANG);
	if (pid == 0)
	{
		LM_T(LmtWait, ("Children running, no one has exited since last sweep"));
		return 0;
	}

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


	//
	// Show all possible ionfo on death cause
	//
	LM_W(("caught death of process %d", pid));
	if (WIFEXITED(status))
		LM_W(("Supervised process '%s' died with exit code %d", processP->name, WEXITSTATUS(status)));
	else if (WIFSIGNALED(status))
		LM_W(("Supervised process '%s' died on signal %d", processP->name, WTERMSIG(status)));

	if (WCOREDUMP(status))
		LM_W(("Supervised process '%s' produced a core dump", processP->name));
	if (WIFSTOPPED(status))
		LM_W(("Supervised process '%s' stopped on signal %d", processP->name, WSTOPSIG(status)));

	if (WIFCONTINUED(status))
	{
		LM_W(("Supervised process '%s' continues", processP->name));
		return 0;
	}



	//
	// Restart process
	//
	Process*    newProcessP;
	struct timeval  now;
	struct timeval  diff;
	
	if (access(processListFilename.c_str(), R_OK) != 0)
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
	if (ep)
		LM_W(("Got a Reset message from '%s'", ep->name()));

	unlink(processListFilename.c_str());

	LM_T(LmtReset, ("killing local processes"));
	restartInProgress    = true;
	localProcessesKill();
	restartInProgress    = false;

	if (ep)
	{
		LM_T(LmtReset, ("Sending ack to RESET message to %s", ep->name()));
		Packet* packetP = new Packet(Message::Ack, Message::Reset);
		ep->send(packetP);
		networkP->epMgr->show("Got RESET", true);
	}
}



/* ****************************************************************************
*
* processVector - 
*/
void SamsonSpawner::processVector(Endpoint2* ep, ProcessVector* procVec)
{
	networkP->epMgr->procVecSet(procVec);

	if (ep)
		LM_T(LmtProcessVector, ("Got Process Vector with %d processes from %s", procVec->processes, ep->name()));
	else
		LM_T(LmtProcessVector, ("Got Process Vector with %d processes", procVec->processes));

	processesStart(procVec);

	if (ep)
	{
		Packet* packetP = new Packet(Message::Ack, Message::ProcessVector);
		ep->send(packetP);
	}
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
		argV[argC++] = (char*) "samsonWorker";
	else if (process->type == PtController)
		argV[argC++] = (char*) "samsonController";
	else
		LM_X(1, ("Will only start workers and controllers - bad process type %d", process->type));

	argV[argC++] = (char*) "-home";
	argV[argC++] = samsonHome;
	argV[argC++] = (char*) "-working";
	argV[argC++] = samsonWorking;
	argV[argC]   = NULL;

	LM_T(LmtSpawn, ("Spawning process '%s'", argV[0]));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtSpawn, ("  argV[%d]: '%s'", ix, argV[ix]));

	pid = fork();
	if (pid == 0)
	{
		int ix;
		int s;
		char absPath[512];

		LM_TODO(("Use close-on-exec!"));
		snprintf(absPath, sizeof(absPath), "%s/bin/%s", samsonHome, argV[0]);
		argV[0] = absPath;
		s = execvp(absPath, argV);
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



void SamsonSpawner::processesTest(ProcessVector* procVec)
{
	Process* p0 = &procVec->processV[0];

	LM_T(LmtStructPadding, ("sizeof(Process):           %d", sizeof(Process)));

	LM_T(LmtStructPadding, ("Offset for type:           %d", (long) (&p0->type) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for id:             %d", (long) (&p0->id) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for name:           %d", (long) (p0->name) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for alias:          %d", (long) (p0->alias) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for host:           %d", (long) (p0->host) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for port:           %d", (long) (&p0->port) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for pad01:          %d", (long) (&p0->pad01) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for state:          %d", (long) (&p0->state) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for pad02:          %d", (long) (&p0->pad02) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for verbose:        %d", (long) (&p0->verbose) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for debug:          %d", (long) (&p0->debug) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for reads:          %d", (long) (&p0->reads) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for writes:         %d", (long) (&p0->writes) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for hidden:         %d", (long) (&p0->hidden) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for toDo:           %d", (long) (&p0->toDo) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for traceLevels:    %d", (long) (p0->traceLevels) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for sendsLogs:      %d", (long) (&p0->sendsLogs) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for workers:        %d", (long) (&p0->workers) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for controllerHost: %d", (long) (p0->controllerHost) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for endpoint:       %d", (long) (&p0->endpoint) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for starterP:       %d", (long) (&p0->starterP) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for spawnerP:       %d", (long) (&p0->spawnerP) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for pid:            %d", (long) (&p0->pid) - (long) p0));
	LM_T(LmtStructPadding, ("Offset for startTime:      %d", (long) (&p0->startTime) - (long) p0));
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

	networkP->epMgr->hostMgr->list("trying to start my processes");
	processListShow("trying to start my processes", true);
	processesTest(procVec);

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
		LM_T(LmtSpawn, (" system(\"killall samsonWorker\") returned %d (strerror: %s)", s, strerror(errno)));
	usleep(200000);
	s = system("killall -9 samsonWorker > /dev/null 2>&1");
	if (s != 0)
		LM_T(LmtSpawn, (" system(\"killall -9 samsonWorker\") returned %d (strerror: %s)", s, strerror(errno)));


	s = system("killall samsonController > /dev/null 2>&1");
	if (s != 0)
		LM_T(LmtSpawn, (" system(\"killall samsonController\") returned %d (strerror: %s)", s, strerror(errno)));
	usleep(200000);
	s = system("killall -9 samsonController > /dev/null 2>&1");
	if (s != 0)
		LM_T(LmtSpawn, (" system(\"killall -9 samsonController\") returned %d (strerror: %s)", s, strerror(errno)));
}

}
