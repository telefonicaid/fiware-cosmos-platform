/* ****************************************************************************
*
* FILE                     Network.cpp - Definition for the network interface
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Oct 11 2010
*
*/
#include <sys/select.h>         // select
#include <string>               // string
#include <vector>               // ss::vector
#include <sys/types.h>          // pid_t
#include <unistd.h>             // fork, getpid
#include <sched.h>              // sched_setaffinity
#include <pthread.h>            // pthread_t
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>         // sockaddr_in

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels
#include "Alarm.h"              // ALARM
#include "ports.h"              // Port numbers for samson processes

#include "Misc.h"               // ipGet 
#include "Endpoint.h"           // Endpoint
#include "Message.h"            // Message::MessageCode
#include "Packet.h"             // Packet
#include "MemoryManager.h"      // MemoryManager
#include "iomInit.h"            // iomInit
#include "iomServerOpen.h"      // iomServerOpen
#include "iomConnect.h"         // iomConnect
#include "iomAccept.h"          // iomAccept
#include "iomMsgSend.h"         // iomMsgSend
#include "iomMsgRead.h"         // iomMsgRead
#include "iomMsgAwait.h"        // iomMsgAwait
#include "workerStatus.h"       // workerStatus, WorkerStatusData
#include "Host.h"               // Host
#include "HostMgr.h"            // HostMgr
#include "Network.h"            // Own interface



/* ****************************************************************************
*
* Predecided indices in the endpoint vector
*
* FIRST_WORKER - the index where the workers start
* CONTROLLER   - the index where the controller reside
* SUPERVISOR   - the index where the superviros recide
*/
#define ME              0
#define LISTENER        1
#define CONTROLLER      2
#define SUPERVISOR      3
#define FIRST_WORKER   10



/* ****************************************************************************
*
* KILO - 
* MEGA - 
*/
#define KILO(x)                      (x * 1024)
#define MEGA(x)                      (KILO(x) * 1024)



/* ****************************************************************************
*
* THREAD_BUF_SIZE_THRESHOLD
* SEND_SIZE_TO_USE_THREAD
*/
#define THREAD_BUF_SIZE_THRESHOLD    (MEGA(10))
#define SEND_SIZE_TO_USE_THREAD      (MEGA(10))



namespace ss
{


static int                 logSocket = -1;
static struct sockaddr_in  logAddr;
/* ****************************************************************************
*
* logHookInit - 
*/
static void logHookInit(struct sockaddr_in* sinP)
{
	if (logSocket != -1)
		close(logSocket);
	logSocket = -1;

	if (sinP == NULL)
		return;

	LM_T(LmtLogServer, ("Setting up log hook function to send logs to port %d", LOG_MESSAGE_PORT));

	logSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (logSocket == -1)
		LM_RVE(("socket: %s", strerror(errno)));

	memset((char*) &logAddr, 0, sizeof(logAddr));
	logAddr.sin_family = AF_INET;
	logAddr.sin_port   = htons(LOG_MESSAGE_PORT);
	memcpy(&logAddr.sin_addr, sinP, sizeof(logAddr.sin_addr));

#if 0
	if (inet_aton(ip, &logAddr.sin_addr) == 0)
	{
		close(logSocket);
		logSocket = -1;
		LM_RVE(("inet_aton failed"));
	}
#endif
}



/* ****************************************************************************
*
* logHookFunction - 
*/
static void logHookFunction(void* vP, char* text, char type, const char* date, const char* file, int lineNo, const char* fName, int tLev, const char* stre)
{
	Message::Header       header;
	Message::LogLineData  logLine;
	Network*              networkP = (Network*) vP;
	Endpoint*             supervisor;
	Endpoint*             me;
	int                   tot;
	int                   nb;
	char*                 buf;
	int                   bufLen;
	int                   flags;
	int                   logAddrLen = sizeof(logAddr);
	int                   writes     = 0;

	supervisor = networkP->endpoint[SUPERVISOR];
	me         = networkP->endpoint[ME];

	if ((logSocket == -1) || (supervisor == NULL) || (supervisor->wFd == -1) || (supervisor->state != Endpoint::Connected) || (supervisor->helloReceived != true))
		return;


	//
	// Writing header
	//
	flags  = 0;
	buf    = (char*) &header;
	bufLen = sizeof(header);
	tot    = 0;
	memset(buf, 0, bufLen);
	
	header.code    = ss::Message::LogLine;
	header.type    = ss::Message::Msg;
	header.dataLen = sizeof(logLine);
	header.magic   = 0xFEEDC0DE;

	while (tot < bufLen)
	{
		++writes;

		nb = sendto(logSocket, &buf[tot], bufLen - tot, flags, (struct sockaddr*) &logAddr, logAddrLen);
		if (nb == -1)
		{
			printf("sendto: %s\n", strerror(errno));
			return;
		}
		else if (nb == 0)
		{
			printf("sendto returned 0 bytes (wanted %d) ... (%s)\n", bufLen - tot, strerror(errno));
			return;
		}

		tot += nb;
	}



	//
	// Writing data
	//
	flags  = 0;
	buf    = (char*) &logLine;
	bufLen = sizeof(logLine);
	tot    = 0;
	memset(buf, 0, bufLen);

	logLine.type   = type;
	logLine.lineNo = lineNo;
	logLine.tLev   = tLev;

	strncpy(logLine.processName, progName, sizeof(logLine.processName) - 1);

	if (text == NULL)
		LM_X(1, ("NULL text"));

	if (date  != NULL)  strncpy(logLine.date,  date,  sizeof(logLine.date)  - 1);
	if (text  != NULL)  strncpy(logLine.text,  text,  sizeof(logLine.text)  - 1);
	if (file  != NULL)  strncpy(logLine.file,  file,  sizeof(logLine.file)  - 1);
	if (fName != NULL)  strncpy(logLine.fName, fName, sizeof(logLine.fName) - 1);
	if (stre  != NULL)  strncpy(logLine.stre,  stre,  sizeof(logLine.stre));

	while (tot < bufLen)
	{
		++writes;

		nb = sendto(logSocket, &buf[tot], bufLen - tot, flags, (struct sockaddr*) &logAddr, logAddrLen);
		if (nb == -1)
		{
			printf("sendto: %s\n", strerror(errno));
			return;
		}
		else if (nb == 0)
		{
			printf("sendto returned 0 bytes (wanted %d) ... (%s)\n", bufLen - tot, strerror(errno));
			return;
		}
		
		tot += nb;
	}
}



/* ****************************************************************************
*
* Network::reset - 
*/
void Network::reset(Endpoint::Type type, const char* alias, unsigned short port, int endpoints, int workers)
{
	if ((endpoints > 200) || (endpoints < 2))
		LM_X(1, ("bad number of endpoints (%d)", endpoints));
	if ((workers > 20) || (workers < 0))
		LM_X(1, ("bad number of workers (%d)", workers));
	if (endpoints < 2 * workers + 5)
		LM_X(1, ("bad number of workers (%d) and endpoints (%d)", workers, endpoints));

	packetReceiver         = NULL;
	dataReceiver           = NULL;
	endpointUpdateReceiver = NULL;
	readyReceiver          = NULL;

	iAmReady               = false;

	tmoSecs                = 0;
	tmoUsecs               = 50000;
	this->port             = port;
	Endpoints              = endpoints;
	Workers                = workers;


	// This is just used in samsonController ...
	workerVec              = NULL;
	workerVecSize          = 0;
	workerVecSaveCallback  = NULL;

	//
	// Endpoint vector
	//
	endpoint = (Endpoint**) calloc(Endpoints, sizeof(Endpoint*));
	if (endpoint == NULL)
		LM_XP(1, ("calloc(%d, %d)", Endpoints, sizeof(Endpoint*)));

	LM_T(LmtInit, ("Allocated room for %d endpoints (%d workers)", endpoints, workers));



	//
	// me
	//
	endpoint[ME] = new Endpoint(type, port);
	if (endpoint[ME] == NULL)
		LM_XP(1, ("new Endpoint"));

	if (endpoint[ME] == NULL)
		LM_XP(1, ("unable to allocate room for Endpoint 'me'"));

	endpoint[ME]->name     = progName;
	endpoint[ME]->state    = Endpoint::Me;
	endpoint[ME]->ip       = ipGet();

	if (alias != NULL)
		endpoint[ME]->alias = alias;
	else
		endpoint[ME]->alias = "NO ALIAS";

	if ((alias != NULL) && (strncmp(&alias[1], "orker", 5) == 0))
		endpoint[ME]->workerId = atoi(&alias[6]);
	else
		endpoint[ME]->workerId = -2;
}



/* ****************************************************************************
*
* Constructor
*/
Network::Network(Endpoint::Type type, const char* alias, unsigned short port, int endpoints, int workers)
{
	reset(type, alias, port, endpoints, workers);

	hostMgr = new HostMgr(20);
}



/* ****************************************************************************
*
* setPacketReceiver - set the element to be notified when packages arrive
*/
void Network::setPacketReceiver(PacketReceiverInterface* receiver)
{
	LM_T(LmtDelilah, ("Setting packet receiver to %p", receiver));
	packetReceiver = receiver;
}
	


/* ****************************************************************************
*
* setDataReceiver - set the element to be notified when packages arrive
*/
void Network::setDataReceiver(DataReceiverInterface* receiver)
{
	LM_T(LmtDelilah, ("Setting data receiver to %p", receiver));
	dataReceiver = receiver;
}



/* ****************************************************************************
*
* setEndpointUpdateReceiver - 
*/
void Network::setEndpointUpdateReceiver(EndpointUpdateReceiverInterface* receiver)
{
	LM_T(LmtDelilah, ("Setting endpoint update receiver to %p", receiver));
	endpointUpdateReceiver = receiver;
}
	


/* ****************************************************************************
*
* setReadyReceiver - 
*/
void Network::setReadyReceiver(ReadyReceiverInterface* receiver)
{
	LM_T(LmtDelilah, ("Setting Ready receiver to %p", receiver));
	readyReceiver = receiver;
}



/* ****************************************************************************
*
* controllerConnect - 
*/
Endpoint* Network::controllerConnect(const char* controllerName)
{
	if ((endpoint[ME]->type != Endpoint::Worker) && (endpoint[ME]->type != Endpoint::Delilah) && (endpoint[ME]->type != Endpoint::Supervisor))
	{
		endpoint[CONTROLLER] = NULL;   // Probably not necessary ...
		return endpoint[CONTROLLER];
	}


	endpoint[CONTROLLER] = new Endpoint(Endpoint::Controller, controllerName);
	if (endpoint[CONTROLLER] == NULL)
		LM_XP(1, ("new Endpoint"));

	LM_TODO(("Why do I not call endpointAdd here ... ?"));

	LM_T(LmtControllerConnect, ("connecting to controller in %s, port %d", endpoint[CONTROLLER]->ip.c_str(), endpoint[CONTROLLER]->port));
	endpoint[CONTROLLER]->rFd = iomConnect((const char*) endpoint[CONTROLLER]->ip.c_str(), (unsigned short) endpoint[CONTROLLER]->port);
	if (endpoint[CONTROLLER]->rFd == -1)
	{
		if (endpoint[ME]->type != Endpoint::Supervisor)
			LM_X(1, ("error connecting to controller at %s:%d", endpoint[CONTROLLER]->ip.c_str(), endpoint[CONTROLLER]->port));
		else
		{
			iAmReady = true;
			if (readyReceiver)
				readyReceiver->ready("unable to connect to controller");
		}
	}
	else
	{
		LM_T(LmtControllerConnect, ("connected to controller - calling ready after trying to connect to all workers"));
		endpoint[CONTROLLER]->wFd   = endpoint[CONTROLLER]->rFd;
		endpoint[CONTROLLER]->state = Endpoint::Connected;
	}

	return endpoint[CONTROLLER];
}



/* ****************************************************************************
*
* init - create endpoints 0 and 1
*/
void Network::init(const char* controllerName)
{
	if (this->port != 0)
	{
		endpoint[LISTENER] = new Endpoint(*endpoint[ME]);
		if (endpoint[LISTENER] == NULL)
			LM_XP(1, ("new Endpoint"));
		endpoint[LISTENER] = endpoint[LISTENER];

		endpoint[LISTENER]->rFd      = iomServerOpen(endpoint[LISTENER]->port);
		if (endpoint[LISTENER]->rFd == -1)
			LM_XP(1, ("unable to open port %d for listening", endpoint[LISTENER]->port));

		endpoint[LISTENER]->state    = Endpoint::Listening;
		endpoint[LISTENER]->type     = Endpoint::Listener;
		endpoint[LISTENER]->ip       = endpoint[ME]->ip;
		endpoint[LISTENER]->name     = "Listener";
		endpoint[LISTENER]->wFd      = endpoint[LISTENER]->rFd;

		LM_T(LmtFds, ("opened fd %d to accept incoming connections", endpoint[LISTENER]->rFd));
	}

	if (endpoint[CONTROLLER] == NULL)
		controllerConnect(controllerName);

	LM_T(LmtInit, ("I am a '%s', my name: '%s', ip: %s", endpoint[ME]->typeName(), endpoint[ME]->nam(), endpoint[ME]->ip.c_str()));
}



/* ****************************************************************************
*
* helloSend
*/
int Network::helloSend(Endpoint* ep, Message::MessageType type)
{
	Message::HelloData hello;

	strncpy(hello.name,   endpoint[ME]->name.c_str(),   sizeof(hello.name));
	strncpy(hello.ip,     endpoint[ME]->ip.c_str(),     sizeof(hello.ip));
	strncpy(hello.alias,  endpoint[ME]->alias.c_str(),  sizeof(hello.alias));

	hello.type     = endpoint[ME]->type;
	hello.workers  = endpoint[ME]->workers;
	hello.port     = endpoint[ME]->port;
	hello.coreNo   = endpoint[ME]->coreNo;
	hello.workerId = endpoint[ME]->workerId;

	LM_T(LmtWrite, ("sending hello %s to '%s' (name: '%s', type: '%s')", messageType(type), ep->name.c_str(), hello.name, endpoint[ME]->typeName()));

	return iomMsgSend(ep, endpoint[ME], Message::Hello, type, &hello, sizeof(hello));
}



/* ****************************************************************************
*
* workerNew - 
*/
Endpoint* workerNew(int ix)
{
	char       alias[16];
	char       name[32];
	Endpoint*  ep;

	snprintf(alias, sizeof(alias), "Worker%02d",  ix);
	snprintf(name,  sizeof(name),  "Worker %02d", ix);

	ep = new Endpoint();
	if (ep == NULL)
		LM_XP(1, ("new Endpoint"));

	LM_T(LmtWorker, ("*** New worker endpoint at %p", ep));

	ep->rFd     = -1;
	ep->wFd     = -1;
	ep->name    = name;
	ep->alias   = alias;
	ep->workers = 0;
	ep->state   = Endpoint::FutureWorker;
	ep->type    = Endpoint::Worker;
	ep->ip      = "II.PP";
	ep->port    = 0;
	ep->coreNo  = -1;
	
	LM_T(LmtEndpoint, ("Created endpoint %d, worker %d (%s)", FIRST_WORKER + ix, ix, ep->alias.c_str()));

	return ep;
}



#define WEB_SERVICE_PORT (unsigned short) 9898
/* ****************************************************************************
*
* initAsSamsonController - 
*/
void Network::initAsSamsonController(void)
{
	init();

	for (int ix = 0; ix < Workers; ix++)
		endpoint[FIRST_WORKER + ix] = workerNew(ix);

	int fd = iomServerOpen(WEB_SERVICE_PORT);
	if (fd == -1)
		LM_XP(1, ("error opening web service listen socket"));

	endpointAdd("Controller adding web listener", fd, fd, "Web Listener", "Weblistener", 0, Endpoint::WebListener, "localhost", WEB_SERVICE_PORT);
}



/* ****************************************************************************
*
* controllerGetIdentifier - 
*/
int Network::controllerGetIdentifier(void)
{
	LM_T(LmtDelilah, ("Asking for controller id"));
	return CONTROLLER;
}



/* ****************************************************************************
*
* workerGetIdentifier - 
*
* worker 0 is at index FIRST_WORKER in the local endpoint vector
*/
int Network::workerGetIdentifier(int nthWorker)
{
	return nthWorker + FIRST_WORKER;
}
	
	

/* ****************************************************************************
*
* getWorkerFromIdentifier - 
*
* worker 0 is at index FIRST_WORKER in the local endpoint vector
*/
int Network::getWorkerFromIdentifier(int identifier)
{
	if (identifier == 0)
		return endpoint[ME]->workerId;

	if ((identifier < FIRST_WORKER) || (identifier >= FIRST_WORKER + Workers))
		LM_RE(-1, ("invalid worker identifier '%d'  (only have %d workers)", identifier, Workers));

	return identifier - FIRST_WORKER;
}



/* ****************************************************************************
*
* getNumWorkers - 
*
* Return number of workers in the system
*/
int Network::getNumWorkers(void)
{
	return Workers;
}
	
	

/* ****************************************************************************
*
* samsonEndpoints - return list of ALL endpoints!
*/
std::vector<Endpoint*> Network::samsonEndpoints(void)
{
	int                     ix;
	std::vector<Endpoint*>  v;

	LM_T(LmtWorkers, ("%d workers", Workers));

	for (ix = 0; ix <=  Endpoints; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		v.push_back(endpoint[ix]);
	}

	return v;
}
	


/* ****************************************************************************
*
* samsonWorkerEndpoints - return list of Worker endpoints
*/
std::vector<Endpoint*> Network::samsonWorkerEndpoints(void)
{
	int                     ix;
	std::vector<Endpoint*>  v;

	LM_T(LmtWorkers, ("%d workers", Workers));

	for (ix = FIRST_WORKER; ix <  FIRST_WORKER + Workers; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;
		if (endpoint[ix]->type != Endpoint::Worker)
			continue;

		v.push_back(endpoint[ix]);
	}

	return v;
}
	


/* ****************************************************************************
*
* samsonEndpoints - return list of typed endpoints!
*/
std::vector<Endpoint*> Network::samsonEndpoints(Endpoint::Type type)
{
	int                     ix;
	std::vector<Endpoint*>  v;

	LM_T(LmtWorkers, ("%d workers", Workers));

	for (ix = 0; ix <=  Endpoints; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix]->type != type)
			continue;

		v.push_back(endpoint[ix]);
	}

	return v;
}
	


/* ****************************************************************************
*
* senderThread - 
*/
static void* senderThread(void* vP)
{
	Endpoint* ep    = (Endpoint*) vP;
	
	LM_T(LmtSenderThread, ("Sender Thread for '%s' running - wFd: %d (reading from fd %d) (process id: %d)", ep->name.c_str(), ep->wFd, ep->senderReadFd, (int) getpid()));

	while (1)
	{
		SendJob  job;
		int      s;

		s = iomMsgAwait(ep->senderReadFd, -1);
		LM_T(LmtSenderThread, ("Got something to read on fd %d", ep->senderReadFd));
		s = read(ep->senderReadFd, &job, sizeof(job));
		LM_T(LmtSenderThread, ("read %d bytes from fd %d", s, ep->senderReadFd));
		if (s != sizeof(job))
		{
			if (s == -1)
				LM_E(("read error: %s", strerror(errno)));
			else
				LM_E(("read %d bytes - expected %d", s, sizeof(job)));

			continue;
		}

		LM_T(LmtSenderThread, ("Received and read '%s' job from '%s' father (fd %d) - the job is forwarded to fd %d (packetP at %p)",
				   messageCode(job.msgCode),
				   ep->name.c_str(),
				   ep->senderReadFd,
				   ep->wFd,
				   job.packetP));
		LM_T(LmtSenderThread, ("google protocol 'message': %p (packet at %p)", &job.packetP->message, job.packetP));
		LM_T(LmtSenderThread, ("google protocol buffer data len: %d", job.packetP->message.ByteSize()));

		// To be improved ...
		if (ep->alias == job.me->alias)
		{
			if (job.network->packetReceiver)
				job.network->packetReceiver->_receive(0, job.msgCode, job.packetP);
		}
		else
		{
			s = iomMsgSend(ep, NULL, job.msgCode, job.msgType, job.dataP, job.dataLen, job.packetP);
		
			LM_T(LmtSenderThread, ("iomMsgSend returned %d", s));
			if (s != 0)
			{
				LM_E(("iomMsgSend error"));
				if (ep->packetSender)
					ep->packetSender->notificationSent(-1, false);
			}
			else
			{
				ep->msgsOut += 1;
				LM_T(LmtSenderThread, ("iomMsgSend OK"));
				if (ep->packetSender)
					ep->packetSender->notificationSent(0, true);
			}

			LM_T(LmtSenderThread, ("iomMsgSend ok"));
		}

		if (job.dataP)
			free(job.dataP);
	}

	// Cannot really get here ... !!!

	LM_E(("******************* after while TRUE ..."));

	close(ep->senderReadFd);
	close(ep->senderWriteFd);
	ep->sender = false;

	return NULL;
}



/* ****************************************************************************
*
* send - 
*/
size_t Network::_send(PacketSenderInterface* packetSender, int endpointId, Message::MessageCode code, Packet* packetP)
{
	Endpoint* ep        = endpoint[endpointId];
	int       nb;

	if (packetP != NULL)
		LM_T(LmtSend, ("Request to send '%s' package with %d packet size (to endpoint '%s')", messageCode(code), packetP->message.ByteSize(), ep->name.c_str()));
	else
		LM_T(LmtSend, ("Request to send '%s' package without data (to endpoint '%s')", messageCode(code), ep->name.c_str()));

	if (ep == NULL)
		LM_X(1, ("No endpoint at index %d", endpointId));

	if (packetP != NULL)
	{
		if (packetP->message.ByteSize() == 0)
			LM_W(("packet not NULL but its data len is 0 ..."));
	}


	// To be improved ...
	if (ep->alias == endpoint[ME]->alias)
	{
		LM_T(LmtMsgLoopBack, ("NOT looping back the '%s' message to myself", messageCode(code)));
		ep->state           = Endpoint::Connected;
		ep->useSenderThread = true;
	}
	else if (( ep->state != Endpoint::Connected ) && (ep->state != Endpoint::Threaded))
	{

		if (ep->useSenderThread == false)
		{
			// LM_X(1, ("cannot send to an unconnected peer '%s' if not using sender threads, sorry ...", ep->name.c_str()));
		   LM_RE(0, ("cannot send to non connected (%s) peer '%s' if not using sender threads, sorry ...", ep->stateName(), ep->name.c_str()));
		}

		SendJob* jobP = new SendJob();
		jobP->ep      = ep;
		jobP->me      = endpoint[ME];
		jobP->msgCode = code;
		jobP->msgType = Message::Msg;
		jobP->dataP   = NULL;
		jobP->dataLen = 0;
		jobP->packetP = packetP;
		jobP->network = this;

		LM_T(LmtJob, ("pushing a job for endpoint '%s'", ep->name.c_str()));
		ep->jobPush(jobP);

		return 0;
	}

	ep->packetSender  = packetSender;

	if (ep->useSenderThread == true)
	{
		LM_T(LmtSenderThread, ("using sender thread to send the '%s' message", messageCode(code)));
		if (ep->sender == false)
		{
			LM_T(LmtSenderThread, ("Creating a new sender thread for endpoint '%s'", ep->name.c_str()));
			int tunnelPipe[2];

			if (pipe(tunnelPipe) == -1)
				LM_X(1, ("pipe: %s", strerror(errno)));

			ep->senderWriteFd = tunnelPipe[1];  // father writes to this fd
			ep->senderReadFd  = tunnelPipe[0];  // child reads from this fd
			ep->sender        = true;

			LM_T(LmtSenderThread, ("msgs for endpoint '%s' to fd %d instead of fd %d", ep->name.c_str(), ep->senderWriteFd, ep->wFd));
			LM_T(LmtSenderThread, ("'%s' sender thread to read from fd %d and send to fd %d", ep->name.c_str(), ep->senderReadFd, ep->wFd));

			//
			// Create sender thread
			//
			char alias[64];

			snprintf(alias, sizeof(alias), "%sSender", ep->name.c_str());
			LM_W(("Don't forget to remove this fictive endpoint when a real endpoint restarts ..."));
			endpointAdd("fictive Sender endpoint", -1, -1, "Sender", alias, 0, Endpoint::ThreadedSender, "", 0);

			pthread_create(&ep->senderTid, NULL, senderThread, ep);
			LM_W(("This usleep to be removed some day ..."));
			usleep(1000);


			//
			// Flush job queue on sender pipe
			//
			SendJob* jobP;

			LM_T(LmtJob, ("sender thread created - flushing job queue"));
			while ((jobP = ep->jobPop()) != NULL)
			{
				LM_T(LmtJob, ("sending a queued job to job-sender"));
				nb = write(ep->senderWriteFd, jobP, sizeof(SendJob));
				if (nb == -1)
					LM_P(("write(SendJob)"));
				else if (nb != sizeof(SendJob))
					LM_E(("error writing SendJob. Written %d bytes and not %d", nb, sizeof(SendJob)));
				free(jobP);
			}

			LM_T(LmtJob, ("sender thread created - job queue flushed"));
		}

		SendJob job;

		job.ep      = ep;
		job.me      = endpoint[ME];
		job.msgCode = code;
		job.msgType = Message::Msg;
		job.dataP   = NULL;
		job.dataLen = 0;
		job.packetP = packetP;
		job.network = this;

		LM_T(LmtSenderThread, ("Sending '%s' job to '%s' sender (real destiny fd: %d) with %d packet size - the job is tunneled over fd %d (packet pointer: %p)",
						   messageCode(job.msgCode), ep->name.c_str(), ep->wFd, job.packetP->message.ByteSize(), ep->senderWriteFd, job.packetP));
		
		nb = write(ep->senderWriteFd, &job, sizeof(job));
		if (nb != (sizeof(job)))
		{
			LM_E(("write(written only %d bytes (of %d) to sender thread)", nb, sizeof(job)));
			return -1;
		}

		return 0;
	}

	LM_T(LmtSenderThread, ("Sending message directly (%d bytes)", packetP->message.ByteSize()));
	nb = iomMsgSend(ep, endpoint[ME], code, Message::Msg, NULL, 0, packetP);

	return nb;
}



/* ****************************************************************************
*
* ready - 
*/
bool Network::ready()
{
	return iAmReady;
}



/* ****************************************************************************
*
* Network::endpointListShow - 
*/
void Network::endpointListShow(const char* why)
{
	int ix;

	LM_V((""));
	LM_V(("----------- Endpoint List (%s) -----------", why));

	if (lmVerbose)      LM_F(("Verbose mode is on"));
	if (lmDebug)        LM_D(("Debug mode is on"));
	if (lmReads)        LM_F(("Reads mode is on"));
	if (lmWrites)       LM_F(("Writes mode is on"));
	if (lmToDo)         LM_F(("ToDo mode is on"));

	for (ix = 0; ix < Endpoints; ix++)
	{
		char sign = '-';

		if (endpoint[ix] == NULL)
			continue;

		if ((endpoint[ix]->state == Endpoint::Connected || endpoint[ix]->state == Endpoint::Listening) && (endpoint[ix]->rFd >= 0))
		{
			if (endpoint[ix]->helloReceived)
				sign = '*';
			else
				sign = '+';
		}

		LM_V(("%c %08p  Endpoint %02d: %-15s %-20s %-12s %15s:%05d %16s  fd: %02d  (in: %03d/%09d, out: %03d/%09d) r:%d (acc %d) - w:%d (acc: %d))",
			  sign,
			  endpoint[ix],
			  ix,
			  endpoint[ix]->typeName(),
			  endpoint[ix]->name.c_str(),
			  endpoint[ix]->alias.c_str(),
			  endpoint[ix]->ip.c_str(),
			  endpoint[ix]->port,
			  endpoint[ix]->stateName(),
			  endpoint[ix]->rFd,
			  endpoint[ix]->msgsIn,
			  endpoint[ix]->bytesIn,
			  endpoint[ix]->msgsOut,
			  endpoint[ix]->bytesOut,
			  endpoint[ix]->rMbps,
			  endpoint[ix]->rAccMbps,
			  endpoint[ix]->wMbps,
			  endpoint[ix]->wAccMbps));
	}

    LM_V(("--------------------------------"));
}



/* ****************************************************************************
*
* inheritFrom - 
*/
static void inheritFrom(Endpoint* to, Endpoint* from)
{
	to->msgsIn         = from->msgsIn;
	to->msgsOut        = from->msgsOut;
	to->msgsInErrors   = from->msgsInErrors;
	to->msgsOutErrors  = from->msgsOutErrors;
	to->bytesIn        = from->bytesIn;
	to->bytesOut       = from->bytesOut;
	to->ip             = from->ip;
	to->startTime      = from->startTime;
}



/* ****************************************************************************
*
* endpointFill - 
*/
static void endpointFill(Endpoint* ep, Endpoint* inheritedFrom, int rFd, int wFd, const char* name, const char* alias, int workers, std::string ip, Endpoint::Type type, unsigned short port, int coreNo)
{
	ep->state = Endpoint::Unconnected;

	if (inheritedFrom)
		inheritFrom(ep, inheritedFrom);

	ep->rFd        = rFd;
	ep->wFd        = wFd;
	ep->name       = (name  != NULL)? std::string(name)  : std::string("noname");
	ep->alias      = (alias != NULL)? std::string(alias) : std::string("noalias");
	ep->workers    = workers;
	ep->type       = type;
	ep->port       = port;
	ep->coreNo     = coreNo;

	ep->startTime  = time(NULL);
	ep->restarts   = 0;
	ep->jobsDone   = 0;	

	if ((ep->ip == "") || (ip != "II.PP"))
		ep->ip = ip;

	//
	// If state not changed by 'inheritedFrom', and the socket seems connected,
	// set the state to 'Connected'
	//
	if ((ep->state == Endpoint::Unconnected) && ((rFd != -1) || (wFd != -1)))
		ep->state = Endpoint::Connected;
}



/* ****************************************************************************
*
* endpointAddController - 
*/
Endpoint* Network::endpointAddController(int rFd, int wFd, const char* name, const char* alias, int workers, std::string ip, unsigned short port, int coreNo, Endpoint* inheritedFrom)
{
	if (endpoint[CONTROLLER] == NULL)
	{
		LM_T(LmtInit, ("Allocating room for Controller endpoint"));
		endpoint[CONTROLLER] = new Endpoint();
		LM_T(LmtInit, ("*** Controller Endpoint at %p", endpoint[CONTROLLER]));
	}

	endpointFill(endpoint[CONTROLLER], inheritedFrom, rFd, wFd, name, alias, workers, ip, Endpoint::Controller, port, coreNo);

	if (endpointUpdateReceiver != NULL)
		endpointUpdateReceiver->endpointUpdate(endpoint[CONTROLLER], Endpoint::ControllerAdded, "Controller Added");

	return endpoint[CONTROLLER];
}



/* ****************************************************************************
*
* endpointAddSupervisor - 
*/
Endpoint* Network::endpointAddSupervisor(int rFd, int wFd, const char* name, const char* alias, int workers, std::string ip, unsigned short port, int coreNo, Endpoint* inheritedFrom)
{
	if (endpoint[SUPERVISOR] == NULL)
		endpoint[SUPERVISOR] = new Endpoint();

	endpointFill(endpoint[SUPERVISOR], inheritedFrom, rFd, wFd, name, alias, workers, ip, Endpoint::Supervisor, port, coreNo);

	if (endpointUpdateReceiver != NULL)
		endpointUpdateReceiver->endpointUpdate(endpoint[SUPERVISOR], Endpoint::SupervisorAdded, "Supervisor Added");

	return endpoint[SUPERVISOR];
}



/* ****************************************************************************
*
* Network::endpointAddTemporal - 
*/
Endpoint* Network::endpointAddTemporal(int rFd, int wFd, const char* name, const char* alias, std::string ip, Endpoint* inheritedFrom)
{
	int ix;

	for (ix = Endpoints - 1; ix >= FIRST_WORKER + Workers; ix--)
	{
		if (endpoint[ix] != NULL)
			continue;

		endpoint[ix] = new Endpoint();
		if (endpoint[ix] == NULL)
			LM_XP(1, ("allocating temporal Endpoint"));

		LM_T(LmtTemporalEndpoint, ("*** Temporal Endpoint '%s' at %p", name, endpoint[ix]));

		endpointFill(endpoint[ix], inheritedFrom, rFd, wFd, name, alias, 0, ip, Endpoint::Temporal, 0, -1);

		return endpoint[ix];
	}

	LM_X(1, ("No temporal endpoint slots available - redefine and recompile!"));
	return NULL;
}



/* ****************************************************************************
*
* Network::endpointAddDefault - 
*/
Endpoint* Network::endpointAddDefault(int rFd, int wFd, const char* name, const char* alias, int workers, Endpoint::Type type, std::string ip, unsigned short port, int coreNo, Endpoint* inheritedFrom)
{
	int ix;

	for (ix = FIRST_WORKER + Workers; ix < (int) (Endpoints - 1); ix++)
	{
		if (endpoint[ix] != NULL)
			continue;

		endpoint[ix] = new Endpoint();
		if (endpoint[ix] == NULL)
			LM_XP(1, ("allocating Endpoint"));

		LM_T(LmtEndpoint, ("*** New Endpoint '%s' at %p", name, endpoint[ix]));

		endpointFill(endpoint[ix], inheritedFrom, rFd, wFd, name, alias, workers, ip, type, port, coreNo);

		if (endpoint[ix]->type == Endpoint::WebListener)
			endpoint[ix]->state = Endpoint::Listening;

		endpointListShow("Added default endpoint");
		return endpoint[ix];
	}

	LM_X(1, ("No endpoint slots available - redefine and recompile!"));
	return NULL;
}



/* ****************************************************************************
*
* Network::coreWorkerEndpointAdd - 
*/
Endpoint* Network::coreWorkerEndpointAdd(int rFd, int wFd, const char* name, const char* alias)
{
	if (endpoint[CONTROLLER] == NULL)
		LM_X(1, ("controller == NULL"));

	endpoint[CONTROLLER]->rFd    = rFd;
	endpoint[CONTROLLER]->wFd    = wFd;
	endpoint[CONTROLLER]->name   = std::string(name);
	endpoint[CONTROLLER]->alias  = (alias != NULL)? alias : "NO ALIAS" ;
			
	return endpoint[CONTROLLER];
}



/* ****************************************************************************
*
* Network::endpointAddWorker - 
*/
Endpoint* Network::endpointAddWorker(int rFd, int wFd, const char* name, const char* alias, int workers, std::string ip, unsigned short port, int coreNo, Endpoint* inheritedFrom)
{
	int ix;

	if (endpoint[ME]->type == Endpoint::CoreWorker)
		return coreWorkerEndpointAdd(rFd, wFd, name, alias);

	LM_T(LmtWorkers, ("%d workers", Workers));
	for (ix = FIRST_WORKER; ix < FIRST_WORKER + Workers; ix++)
	{
		Endpoint* ep;

		if (endpoint[ix] == NULL)
			LM_X(1, ("NULL worker endpoint at slot %d", ix));

		if (((ep = endpointLookup((char*) alias)) != NULL) && (ep->state == Endpoint::Connected))
		{
			if (ep->wFd != wFd)
				LM_E(("write file descriptors don't coincide for endpoint '%s' (%d vs %d)", alias, wFd, ep->wFd));

			if ((ep->type == Endpoint::Temporal) && (ep->wFd == wFd) && (ep->rFd == rFd))
				inheritedFrom = ep;
			else
			{
				LM_E(("Intent to connect a second Worker with alias '%s' - rejecting connection", alias));
				iomMsgSend(wFd, name, progName, Message::Die, Message::Evt);

				close(rFd);
				if (wFd != rFd)
					close(wFd);

				return NULL;
			}
		}

		if ((endpoint[ME]->type == Endpoint::Delilah) || (endpoint[ME]->type == Endpoint::Worker))
		{
			endpoint[ix]->useSenderThread = true;
			LM_T(LmtSenderThread, ("Delilah worker endpoint uses SenderThread"));
		}

		if (strcmp(endpoint[ix]->alias.c_str(), alias) == 0)
		{
			endpointFill(endpoint[ix], inheritedFrom, rFd, wFd, name, alias, workers, ip, Endpoint::Worker, port, coreNo);

			// LM_W(("What state should I use here ... Unconnected (never been connected), Disconnected or just the inherited state ..."));
			// endpoint[ix]->state    = (rFd > 0)? Endpoint::Connected : Endpoint::Disconnected;   /* XXX */

			LM_T(LmtJob, ("worker '%s' connected - any pending messages for him? (jobQueueHead at %p)", endpoint[ix]->alias.c_str(),  endpoint[ix]->jobQueueHead));
			
			if (endpointUpdateReceiver != NULL)
				endpointUpdateReceiver->endpointUpdate(endpoint[ix], Endpoint::WorkerAdded, "Worker Added");

			return endpoint[ix];
		}
	}

	LM_E(("Worker '%s:%d' (alias '%s') not found - rejecting connection with a Die message", ip.c_str(), port, alias));
	iomMsgSend(wFd, name, progName, Message::Die, Message::Evt);

	return NULL;
}



/* ****************************************************************************
*
* endpointAdd - add an endpoint to the vector
*
* The first three slots in this vector are:
*  - 0: me
*  - 1: listener
*  - 2: controller
*
* From slot FIRST_WORKER, the worker endpoints are stored. These endpoints have a fix
* position in the vector and these positions are defined by the response
* of the WorkerVector from the controller. The controller itself gets the
* positions from the workers in the configuration file 'setup.txt'.
* The worker positions in the endpoint vector may never change.
*
* After the workers, we have instances of delilah, which are considered
* temporal, i.e. they're removed and forgotten if a delilah is disconnected.
*
* The method 'endpointAdd' is called when a Hello is received, as we don't really
* know who is on the other side until we receive the Hello package, so at the
* end of this vector, temporal endpoints are stored, these endpoints have not
* yet received the Hello package. These temporal endpoints grow downwards in
* the vector.
*
*/
Endpoint* Network::endpointAdd
(
	const char*      why,
	int              rFd,
	int              wFd,
	const char*      name,
	const char*      alias,
	int              workers,
	Endpoint::Type   type,
	std::string      ip,
	unsigned short   port,
	int              coreNo,
	Endpoint*        inheritedFrom
)
{
	LM_T(LmtEndpoint, ("%s: adding endpoint '%s' of type '%s' for fd %d (alias: '%s')", why, name, endpoint[ME]->typeName(type), rFd, alias));

	switch (type)
	{
	case Endpoint::Controller:
		return endpointAddController(rFd, wFd, name, alias, workers, ip, port, coreNo, inheritedFrom);
		break;

	case Endpoint::Worker:
		return endpointAddWorker(rFd, wFd, name, alias, workers, ip, port, coreNo, inheritedFrom);
		break;

	case Endpoint::Temporal:
		return endpointAddTemporal(rFd, wFd, name, alias, ip, inheritedFrom);
		break;

	case Endpoint::Supervisor:
		return endpointAddSupervisor(rFd, wFd, name, alias, workers, ip, port, coreNo, inheritedFrom);
		break;

	case Endpoint::ThreadedReader:
	case Endpoint::ThreadedSender:
	case Endpoint::Fd:
	case Endpoint::Spawner:
	case Endpoint::Delilah:
	case Endpoint::WebListener:
	case Endpoint::WebWorker:
		return endpointAddDefault(rFd, wFd, name, alias, workers, type, ip, port, coreNo, inheritedFrom);
		break;

	case Endpoint::Sender:
	case Endpoint::CoreWorker:
	case Endpoint::Unknown:
	case Endpoint::Listener:
		LM_X(1, ("bad type: %d (%s)", type, endpoint[ME]->typeName(type)));
		break;
	}

	LM_X(1, ("BUG"));
	return NULL;
}



/* ****************************************************************************
*
* endpointRemove
*/
void Network::endpointRemove(Endpoint* ep, const char* why)
{
	int ix;

	LM_T(LmtEndpoint, ("Removing '%s' endpoint '%s' (at %p) - %s", ep->typeName(), ep->name.c_str(), ep, why));

	for (ix = 0; ix < Endpoints; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix] == ep)
		{
			if (ep->type == Endpoint::Worker)
			{
				close(ep->rFd);
				if (ep->wFd != ep->rFd)
					close(ep->wFd);

				ep->rFd   = -1;
				ep->wFd   = -1;
				ep->state = Endpoint::Disconnected;
				ep->name  = std::string("To be a worker");

				if (endpointUpdateReceiver != NULL)
					endpointUpdateReceiver->endpointUpdate(ep, Endpoint::WorkerRemoved, "Worker Removed");
			}
			else if (ep->type == Endpoint::Controller)
			{
                ep->state = Endpoint::Disconnected;
				LM_W(("NOT removing Controller"));
				if (endpointUpdateReceiver != NULL)
					endpointUpdateReceiver->endpointUpdate(ep, Endpoint::ControllerRemoved, "Controller Removed");
			}
			else
			{
				if (ep->type == Endpoint::Supervisor)
				{
					LM_T(LmtLogServer, ("Removing supervisor - calling lmOutHookSet(NULL) to disable completely the log server functionality"));
					lmOutHookSet(NULL, NULL);
				}

				LM_T(LmtEndpoint, ("Closing down endpoint '%s'", ep->name.c_str()));
				ep->state = Endpoint::Closed;
				if ((endpointUpdateReceiver != NULL) && (ep->type != Endpoint::Temporal))
					endpointUpdateReceiver->endpointUpdate(ep, Endpoint::EndpointRemoved, "Endpoint Removed");

				if (ep->type != Endpoint::Temporal)
				{
					LM_W(("Closing fd %d for endpoint '%s'", ep->rFd, ep->name.c_str()));
					close(ep->rFd);
					if (ep->wFd != ep->rFd)
						close(ep->wFd);
				}

				delete ep;
				endpoint[ix] = NULL;
			}
		}
	}
}



/* ****************************************************************************
*
* endpointLookup
*/
Endpoint* Network::endpointLookup(int ix)
{
	return endpoint[ix];
}



/* ****************************************************************************
*
* endpointLookup
*/
Endpoint* Network::endpointLookup(int rFd, int* idP)
{
	int ix = 0;

	if (rFd < 0)
		return NULL;

	for (ix = 0; ix < Endpoints; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix]->rFd == rFd)
		{
			if (idP)
				*idP = ix;
			return endpoint[ix];
		}
	}

	return NULL;
}



/* ****************************************************************************
*
* endpointLookup
*/
Endpoint* Network::endpointLookup(char* alias)
{
	int ix = 0;

	for (ix = 0; ix < Endpoints; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (strcmp(endpoint[ix]->alias.c_str(), alias) == 0)
			return endpoint[ix];
	}

	return NULL;
}



/* ****************************************************************************
*
* endpointLookup
*/
Endpoint* Network::endpointLookup(Endpoint::Type type, char* ip)
{
	int ix = 0;

	LM_T(LmtEndpointLookup, ("Looking for '%s' endpoint with IP '%s'", endpoint[ME]->typeName(type), ip));
	for (ix = 0; ix < Endpoints; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		LM_TODO(("Remove Temporal from Endpoint::Type - make it a separate field in Endpoint"));
		LM_T(LmtEndpointLookup, ("Comparing types: '%s' to '%s'", endpoint[ME]->typeName(type), endpoint[ix]->typeName()));
		if (endpoint[ix]->type != type)
		{
			if ((endpoint[ix]->type == Endpoint::Temporal) && (type == Endpoint::Spawner) && (strcmp(endpoint[ix]->name.c_str(), "Spawner") == 0))
				;
			else
				continue;
		}

		LM_T(LmtEndpointLookup, ("Comparing IPs: '%s' to '%s'", endpoint[ix]->ip.c_str(), ip));
		if (strcmp(endpoint[ix]->ip.c_str(), ip) == 0)
		{
			LM_T(LmtEndpointLookup, ("FOUND Endpoint at %s", ip));
			return endpoint[ix];
		}
	}

	LM_T(LmtEndpointLookup, ("'%s' Endpoint at %s not found", endpoint[ME]->typeName(type), ip));
	return NULL;
}



/* ****************************************************************************
*
* endpointLookup
*/
Endpoint* Network::endpointLookup(Endpoint::Type type, Host* hostP)
{
	int ix = 0;

	LM_T(LmtEndpointLookup, ("Looking for '%s' endpoint on host '%s'", endpoint[ME]->typeName(type), hostP->name));
	for (ix = 0; ix < Endpoints; ix++)
	{
		Host* epHostP;

		if (endpoint[ix] == NULL)
			continue;

		LM_T(LmtEndpointLookup, ("Comparing types: '%s' to '%s'", endpoint[ME]->typeName(type), endpoint[ix]->typeName()));
		if (endpoint[ix]->type != type)
			continue;

		epHostP = hostMgr->lookup(endpoint[ix]->ip.c_str());
		if (epHostP == hostP)
			return endpoint[ix];
	}

	LM_T(LmtEndpointLookup, ("'%s' Endpoint on %s not found", endpoint[ME]->typeName(type), hostP->name));
	return NULL;
}



/* ****************************************************************************
*
* MsgTreatParams - 
*/
typedef struct MsgTreatParams
{
	Network*          diss;
	Endpoint*         ep;
	int               endpointId;
	Message::Header   header;
	Endpoint::State   state;
	int               fd;
} MsgTreatParams;



/* ****************************************************************************
*
* msgTreatThreadFunction - 
*/
static void* msgTreatThreadFunction(void* vP)
{
	MsgTreatParams*  paramP = (MsgTreatParams*) vP;
	Endpoint*        ep     = paramP->ep;

	paramP->diss->msgTreat(paramP);

	ep->state = paramP->state;

	LM_T(LmtThreadedMsgTreat, ("back after msgTreat - setting back state for '%s' to %d (was in 'Threaded' state while msgTreat ran)", ep->name.c_str(), ep->state));
	
	free(vP);
	close(paramP->fd);

	return NULL;
}



/* ****************************************************************************
*
* webServiceAccept - 
*/
void Network::webServiceAccept(Endpoint* ep)
{
	int   fd;
	char  hostName[128];
	char  ip[128];

	fd = iomAccept(ep->rFd, &ep->sockin, hostName, sizeof(hostName), ip, sizeof(ip));
	if (endpoint[ME]->type != Endpoint::Controller)
	{
		LM_E(("got incoming WebListener connection but I'm not the controller ..."));
		if (fd != -1)
			close(fd);

		return;
	}

	if (fd == -1)
	{
		LM_P(("iomAccept(%d)", ep->rFd));
		ep->msgsInErrors += 1;
	}
	else
	{
		endpointAdd("Accepted incoming web service request", fd, fd, "Web Worker", "Webworker", 0, Endpoint::WebWorker, hostName, 0);
		ep->msgsIn += 1;
	}
}



/* ****************************************************************************
*
* webServiceTreat - 
*/
void Network::webServiceTreat(Endpoint* ep)
{
	char buf[1024];
	int  nb;

	if (endpoint[ME]->type != Endpoint::Controller)
		LM_X(1, ("Got a request from a WebWorker and I'm not a controller !"));

	nb = read(ep->rFd, buf, sizeof(buf));
	if (nb == -1)
	{
		ep->msgsInErrors += 1;
		LM_E(("error reading web service request: %s", strerror(errno)));
	}
	else if (nb == 0)
	{
		ep->msgsInErrors += 1;
		LM_E(("read ZERO bytes of web service request"));
	}
	else
	{
		std::string command     = packetReceiver->getJSONStatus(std::string(buf));
		int         commandLen  = command.size();

		ep->msgsIn  += 1;
		ep->msgsOut += 1;
		write(ep->wFd, command.c_str(), commandLen);
	}

	close(ep->wFd);
	endpointRemove(ep, "Done servicing Web Service connection");
}



/* ****************************************************************************
*
* msgPreTreat - 
*/
void Network::msgPreTreat(Endpoint* ep, int endpointId)
{
	Message::Header header;
	int             nb;

	if (ep->type == Endpoint::WebListener)
	{
		webServiceAccept(ep);
		return;
	}		

	if (ep->type == Endpoint::WebWorker)
	{
		webServiceTreat(ep);
        return;
	}



	//
	// Special case - FD endpoint type (stdin) which does not use Message
	//
	if (ep->type == Endpoint::Fd)
	{
		char buffer[1024];
		int  nb;

		LM_T(LmtRead, ("reading data from fd %d", ep->rFd));

		nb = read(ep->rFd, buffer, sizeof(buffer));
		if (nb == -1)
			LM_RVE(("iomMsgRead: error reading message from '%s': %s", ep->name.c_str(), strerror(errno)));
		else if (nb == 0) /* Connection closed */
			LM_RVE(("iomMsgRead: connection closed from fd %d of type Fd ... Error ?", ep->rFd));

		LM_T(LmtRead, ("read %d bytes of data from fd %d - calling dataReceiver->receive", nb, ep->rFd));
		if (dataReceiver)
			dataReceiver->receive(endpointId, nb, NULL, buffer);
		return;
	}



	//
	// Reading header of the message
	//
	nb = iomMsgPartRead(ep, "header", (char*) &header, sizeof(header));
	
	if (nb == -1)
		LM_RVE(("iomMsgRead: error reading message from '%s': %s", ep->name.c_str(), strerror(errno)));
	else if ((nb == 0) || (nb == -2)) /* Connection closed */
	{
		LM_T(LmtSelect, ("Connection closed - ep at %p", ep));
		ep->msgsInErrors += 1;
		ep->state = Endpoint::Disconnected;

		if (ep->type == Endpoint::Worker)
		{
			LM_W(("Worker %d just died !", ep->workerId));
			if (packetReceiver)
				packetReceiver->notifyWorkerDied(ep->workerId);
			
			if (endpointUpdateReceiver != NULL)
				endpointUpdateReceiver->endpointUpdate(ep, Endpoint::WorkerDisconnected, "Worker Disconnected");

			if (endpoint[ME]->type == Endpoint::Delilah)
			{
				LM_T(LmtTimeout, ("Lower select timeout to ONE second to poll restarting worker"));
				tmoSecs = 1;
			}
		}

		if (ep == endpoint[CONTROLLER])
		{
			if (endpoint[ME]->type == Endpoint::CoreWorker)
				LM_X(1, ("My Father (samsonWorker) died, I cannot exist without father process"));

			endpoint[CONTROLLER]->rFd    = -1;
			endpoint[CONTROLLER]->state = Endpoint::Disconnected;

			if (endpointUpdateReceiver != NULL)
				endpointUpdateReceiver->endpointUpdate(endpoint[CONTROLLER], Endpoint::ControllerDisconnected, "Controller Disconnected");

			if (endpoint[ME]->type != Endpoint::Supervisor)
			{
				LM_W(("controller died ... trying to reconnect!"));

				while (endpoint[CONTROLLER]->rFd == -1)
				{
					LM_T(LmtControllerConnect, ("Reconnecting to Controller"));
					endpoint[CONTROLLER]->rFd = iomConnect((const char*) endpoint[CONTROLLER]->ip.c_str(), (unsigned short) endpoint[CONTROLLER]->port);
					sleep(1); // sleep one second before reintenting connection to controller
				}

				endpoint[CONTROLLER]->state = Endpoint::Connected;
				endpoint[CONTROLLER]->wFd   = endpoint[CONTROLLER]->rFd;

				if (endpointUpdateReceiver != NULL)
					endpointUpdateReceiver->endpointUpdate(endpoint[CONTROLLER], Endpoint::ControllerReconnected, "Controller Reconnected");
			}
			else
				LM_W(("controller died ... NOT trying to reconnect!"));

		}
		else if (ep != NULL)
		{
			if (ep->type == Endpoint::Worker)
			{
				--endpoint[ME]->workers;

				close(ep->rFd);
				if (ep->wFd == ep->rFd)
					close(ep->wFd);

				ep->state = Endpoint::Closed;
				ep->rFd   = -1;
				ep->wFd   = -1;
				ep->name  = "-----";

				if (endpointUpdateReceiver != NULL)
					endpointUpdateReceiver->endpointUpdate(ep, Endpoint::WorkerDisconnected, "Worker Disconnected");
			}
			else if (ep->type == Endpoint::CoreWorker)
				LM_X(1, ("should get no messages from core worker ..."));
			else
				endpointRemove(ep, "connection closed");
		}

		return;
	}
	else if (nb != sizeof(header))
	{
		LM_RVE(("iomMsgRead: error reading header from '%s' (read %d, wanted %d bytes", ep->name.c_str(), nb, sizeof(header)));
		assert(false);
	}

	if (header.magic != 0xFEEDC0DE)
		LM_X(1, ("Bad magic number in header (0x%x)", header.magic));

	LM_T(LmtMsgTreat, ("Read header of '%s' message with dataLens %d, %d, %d", messageCode(header.code), header.dataLen, header.gbufLen, header.kvDataLen));



	//
	// calling msgTreat ...
	//
	if (header.dataLen + header.gbufLen + header.kvDataLen > THREAD_BUF_SIZE_THRESHOLD)
	{
		pthread_t        tid;
		MsgTreatParams*  paramsP = (MsgTreatParams*) malloc(sizeof(MsgTreatParams));
		int              fdPair[2];
		Endpoint*        newEpP;
		char             alias[64];

		if (pipe(fdPair) == -1)
		   LM_X(1, ("pipe: %s", strerror(errno)));
		
		snprintf(alias, sizeof(alias), "%sTreater", ep->name.c_str());
		newEpP = endpointAdd("starting treater thread", fdPair[0], -1, "Reader/Treater", alias, 0, Endpoint::ThreadedReader, "", 0);
		newEpP->state = Endpoint::Connected;

		paramsP->diss        = this;
		paramsP->ep          = ep;
		paramsP->endpointId  = endpointId;
		paramsP->header      = header;
		paramsP->state       = ep->state;
		paramsP->fd          = fdPair[1];   // This "write part" of the pipe is closed when the thread termnates.

		ep->state = Endpoint::Threaded;
		LM_T(LmtMsgTreat, ("setting state of '%s' to Threaded (%d)  old state: %d", ep->name.c_str(), ep->state, paramsP->state));

		LM_T(LmtMsgTreat, ("calling msgTreatThreadFunction via pthread_create (params at %p) (dataLens: %d, %d, %d)", paramsP, header.dataLen, header.gbufLen, header.kvDataLen));
		pthread_create(&tid, NULL, msgTreatThreadFunction, (void*) paramsP);
		LM_T(LmtMsgTreat, ("after pthread_create of msgTreatThreadFunction"));
	}
	else
	{
		MsgTreatParams  params;

		params.diss        = this;
		params.ep          = ep;
		params.endpointId  = endpointId;
		params.header      = header;

		msgTreat(&params);
	}
}



/* ****************************************************************************
*
* checkAllWorkersConnected - 
*/
void Network::checkAllWorkersConnected(void)
{
	int ix;

	for (ix = 0; ix < Workers; ix++)
	{
		if (endpoint[FIRST_WORKER + ix] == NULL)
			return;
		if (endpoint[FIRST_WORKER + ix]->state != Endpoint::Connected)
			return;
	}	

	//
	// All workers are connected!
	//

	LM_T(LmtWorkers, ("All workers are connected!"));
	if (endpoint[ME]->type == Endpoint::Delilah)
	{
		LM_T(LmtTimeout, ("All workers are connected! - select timeout set to 60 secs"));
		tmoSecs = 60;
	}
}



/* ****************************************************************************
*
* workerVecLookup - 
*/
Message::Worker* Network::workerVecLookup(const char* alias)
{
	if (workerVec == NULL)
	{
		LM_E(("NULL worker vector!"));
		return NULL;
	}

	LM_M(("Comparing %d workers with alias '%s'", workerVec->workers, alias));
	for (int ix = 0; ix < workerVec->workers; ix++)
	{
		LM_M(("Comparing '%s' to '%s'", workerVec->workerV[ix].alias, alias));
		if (strcmp(workerVec->workerV[ix].alias, alias) == 0)
			return &workerVec->workerV[ix];
	}

	return NULL;
}



/* ****************************************************************************
*
* controllerMsgTreat - 
*
* This whole function should move to apps/samsonController ...
*/
void Network::controllerMsgTreat
(
	Endpoint*             ep,
	Message::MessageCode  msgCode,
	Message::MessageType  msgType,
	void*                 dataP,
	int                   dataLen,
	Packet*               packetP
)
{
	const char*           name   = ep->name.c_str();
	Message::ConfigData*  config = (Message::ConfigData*) dataP;
	Message::Worker*      worker;
	char*                 alias  = (char*) dataP;

	LM_T(LmtMsgTreat, ("Treating %s %s from %s", messageCode(msgCode), messageType(msgType), name));
	switch (msgCode)
	{
	case Message::LogLine:
		LM_X(1, ("Got a LogLine from '%s' (%s) - I die", name, ep->typeName()));
		break;

	case Message::WorkerConfigGet:
		LM_M(("Asked for worker config for '%s'", alias));
		worker = workerVecLookup(alias);
		if (worker != NULL)
		{
			LM_T(LmtWrite, ("sending ack with worker '%s' to '%s'", alias, name));
			iomMsgSend(ep, endpoint[ME], Message::WorkerVector, Message::Ack, worker, sizeof(Message::Worker));
		}
		else
			iomMsgSend(ep, endpoint[ME], Message::WorkerVector, Message::Nak, NULL, 0);
		break;

	case Message::ConfigChange:
		LM_M(("Got Configuration change for '%s' (host: '%s')", config->alias, config->host));
		worker = workerVecLookup(config->alias);
		if (worker != NULL)
		{
			LM_M(("found worker with alias '%s'", config->alias));
			worker->verbose = config->verbose;
			worker->debug   = config->debug;
			worker->reads   = config->reads;
			worker->writes  = config->writes;
			worker->toDo    = config->toDo;
			memcpy(worker->traceV, config->traceLevels, sizeof(worker->traceV));

			LM_TODO(("If host was something else and the process is running ..."));
			strncpy(worker->ip,   config->host, sizeof(worker->ip));
			strncpy(worker->name, config->name, sizeof(worker->name));

			if (workerVecSaveCallback)
				workerVecSaveCallback();
		}
		else
			LM_X(1, ("Worker '%s' not found", config->alias));
		break;

	case Message::WorkerVector:
		if (msgType != Message::Msg)
			LM_X(1, ("Controller got an ACK for WorkerVector message"));
		LM_T(LmtWrite, ("sending ack with entire worker vector to '%s'", name));
		iomMsgSend(ep, endpoint[ME], Message::WorkerVector, Message::Ack, workerVec, workerVecSize);
		break;

	default:
		LM_X(1, ("message code '%s' not treated in this method", messageCode(msgCode)));
	}
}



/* ****************************************************************************
*
* workerVecSet - 
*/
void Network::workerVecSet(Message::WorkerVectorData* wvData, int size, WorkerVecSave saveCallback)
{
	workerVec             = wvData;
	workerVecSize         = size;
	workerVecSaveCallback = saveCallback;
}



/* ****************************************************************************
*
* msgTreat - 
*/
void Network::msgTreat(void* vP)
{
	char                  data[1024];
	void*                 dataP        = data;
	int                   dataLen      = sizeof(data);
	Message::ConfigData   configData;
	Message::ConfigData*  configDataP;

	MsgTreatParams*       paramsP      = (MsgTreatParams*) vP;
	Endpoint*             ep           = paramsP->ep;
	int                   endpointId   = paramsP->endpointId;
	Message::Header*      headerP      = &paramsP->header;

	char*                 name         = (char*) ep->name.c_str();
	Packet                packet;
	Packet                ack;
	Message::MessageCode  msgCode;
	Message::MessageType  msgType;
	int                   s;

	LM_T(LmtRead, ("treating incoming message from '%s' (ep at %p) (dataLens: %d, %d, %d)", name, ep, headerP->dataLen, headerP->gbufLen, headerP->kvDataLen));
	s = iomMsgRead(ep, headerP, &msgCode, &msgType, &dataP, &dataLen, &packet, NULL, 0);
	LM_T(LmtRead, ("iomMsgRead returned %d (dataLens: %d, %d, %d)", s, headerP->dataLen, headerP->gbufLen, headerP->kvDataLen));

	if (s != 0)
	{
		LM_T(LmtSelect, ("iomMsgRead returned %d", s));

		if (s == -2) /* Connection closed */
		{
			if (ep->type == Endpoint::Worker)
			{
				LM_W(("Worker %d just died !", ep->workerId));
				packetReceiver->notifyWorkerDied(ep->workerId);

				if (endpoint[ME]->type == Endpoint::Delilah)
				{
					LM_T(LmtTimeout, ("Lower select timeout to ONE second to poll restarting worker"));
					tmoSecs = 1;
				}
			}

			LM_T(LmtSelect, ("Connection closed - ep at %p", ep));
			if (ep == endpoint[CONTROLLER])
			{
				if (endpoint[ME]->type == Endpoint::CoreWorker)
					LM_X(1, ("My Father (samsonWorker) died, I cannot exist without father process"));

				LM_W(("controller died ... trying to reconnect !"));

				endpoint[CONTROLLER]->rFd    = -1;
				endpoint[CONTROLLER]->state = Endpoint::Disconnected;

				if (endpointUpdateReceiver != NULL)
					endpointUpdateReceiver->endpointUpdate(endpoint[CONTROLLER], Endpoint::ControllerDisconnected, "Controller Disconnected");

				if (endpoint[ME]->type == Endpoint::Supervisor)
					return;

				while (endpoint[CONTROLLER]->rFd == -1)
				{
					LM_T(LmtControllerConnect, ("Reconnecting to Controller"));
					endpoint[CONTROLLER]->rFd = iomConnect((const char*) endpoint[CONTROLLER]->ip.c_str(), (unsigned short) endpoint[CONTROLLER]->port);
					sleep(1); // sleep one second before reintenting connection to controller
				}

				endpoint[CONTROLLER]->state = Endpoint::Connected;
				endpoint[CONTROLLER]->wFd   = endpoint[CONTROLLER]->rFd;

				if (endpointUpdateReceiver != NULL)
					endpointUpdateReceiver->endpointUpdate(endpoint[CONTROLLER], Endpoint::ControllerReconnected, "Controller Reconnected");

				return;
			}
			else if (ep != NULL)
			{
				if (ep->type == Endpoint::Worker)
				{
					--endpoint[ME]->workers;

					close(ep->rFd);
					if (ep->wFd == ep->rFd)
						close(ep->wFd);

					ep->state = Endpoint::Closed;
					ep->rFd   = -1;
					ep->wFd   = -1;
					ep->name  = "-----";
				}
				else if (ep->type == Endpoint::CoreWorker)
					LM_X(1, ("should get no messages from core worker ..."));
				else
					endpointRemove(ep, "connection closed");
			}
		}

		LM_RVE(("iomMsgRead: error reading message from '%s'", name));
	}


	LM_T(LmtMsgTreat, ("Treating %s %s from %s", messageCode(msgCode), messageType(msgType), name));
	switch (msgCode)
	{
	case Message::LogSendingOn:
		if (ep->type != Endpoint::Supervisor)
			LM_E(("Got a LogSendingOn message from %s", ep->name.c_str()));
		else
		{
			LM_T(LmtLogServer, ("Setting up LM hook function"));
			logHookInit(&ep->sockin);
			lmOutHookSet(logHookFunction, (void*) this);
		}
		break;

	case Message::LogSendingOff:
		if (ep->type != Endpoint::Supervisor)
			LM_E(("Got a LogSendingOff message from %s", ep->name.c_str()));
		else
		{
			LM_T(LmtLogServer, ("Removing LM hook function"));
			lmOutHookSet(NULL, NULL);
		}
		break;

	case Message::WorkerSpawn:
	case Message::ControllerSpawn:
		if (dataReceiver == NULL)
			LM_X(1, ("no data receiver ... Please implement one!"));
		if (dataReceiver)
			dataReceiver->receive(endpointId, 0, headerP, dataP);
		break;

	case Message::LogLine:
		if (endpoint[ME]->type != ss::Endpoint::Supervisor)
			LM_X(1, ("Got a LogLine from '%s' (type %s) and I'm not the supervisor ... That is a platform bug, so I die", name, ep->typeName()));

		if (dataReceiver)
			dataReceiver->receive(endpointId, 0, headerP, dataP);
		else
			LM_X(1, ("LogServer without data receiver ..."));
		break;

	case Message::Die:
		LM_X(1, ("Got a Die message from '%s' (%s) - I die", name, ep->typeName()));
		break;

	case Message::IDie:
		LM_W(("Got an IDie message from '%s' (%s) - that endpoint is about to die", name, ep->typeName()));
		endpointRemove(ep, "Got an IDie message");
		break;

	case Message::ConfigGet:
		configData.verbose = lmVerbose;
		configData.debug   = lmDebug;
		configData.reads   = lmReads;
		configData.writes  = lmWrites;
		configData.toDo    = lmToDo;
		for (int ix = 0; ix < 256; ix++)
			configData.traceLevels[ix] = lmTraceIsSet(ix);

		iomMsgSend(ep, endpoint[ME], Message::ConfigGet, Message::Ack, &configData, sizeof(configData));
		break;

	case Message::ConfigSet:
		configDataP = (Message::ConfigData*) dataP;

		lmVerbose = configDataP->verbose;
		lmDebug   = configDataP->debug;
		lmReads   = configDataP->reads;
		lmWrites  = configDataP->writes;
		lmToDo    = configDataP->toDo;

		for (int ix = 0; ix < 256; ix++)
			lmTraceLevelSet(ix, configDataP->traceLevels[ix]);
		break;

	case Message::WorkerConfigGet:
	case Message::ConfigChange:
		if (endpoint[ME]->type == Endpoint::Controller)
			controllerMsgTreat(ep, msgCode, msgType, dataP, dataLen, &packet);
		else
			LM_X(1, ("Got a %s message and I'm not the controller ...", messageCode(msgCode)));
		break;

	case Message::Hello:
		Endpoint*            helloEp;
		Message::HelloData*  hello;

		hello = (Message::HelloData*) dataP;

		LM_T(LmtHello, ("Got 'Hello' from ep '%s', fd %d", ep->name.c_str(), ep->rFd));

		if (msgType == Message::Msg)
			helloEp = endpointAdd("Got a Hello Message", ep->rFd, ep->wFd, hello->name, hello->alias, hello->workers, (Endpoint::Type) hello->type, hello->ip, hello->port, hello->coreNo, ep);
		else if (msgType == Message::Ack)
			helloEp = endpointAdd("Got a Hello Ack", ep->rFd, ep->wFd, hello->name, hello->alias, hello->workers, (Endpoint::Type) hello->type, hello->ip, hello->port, hello->coreNo, ep);
		else
			LM_X(1, ("message isn't a Msg nor an Ack ..."));

		if (helloEp == NULL)
		{
			LM_W(("Error adding endpoint '%s'", hello->name));
			endpointRemove(ep, "endpointAdd failed");
			return;
		}

		LM_T(LmtHello, ("Set helloReceived to true for endpoint '%s', fd %d", helloEp->name.c_str(), helloEp->rFd));
		helloEp->helloReceived = true;
		endpointListShow("helloEp->helloReceived = true");

		helloEp->workerId = hello->workerId;

		if (endpointUpdateReceiver != NULL)
			endpointUpdateReceiver->endpointUpdate(ep, Endpoint::HelloReceived, "Hello Received", helloEp);

		LM_T(LmtHello, ("Got Hello %s from %s, type %s, %s:%d, workers: %d",
			  messageType(msgType), helloEp->name.c_str(), helloEp->typeName(), helloEp->ip.c_str(), helloEp->port, helloEp->workers));

		if (ep && ep->type == Endpoint::Temporal)
		{
			if (endpointUpdateReceiver != NULL)
				endpointUpdateReceiver->endpointUpdate(ep, Endpoint::NoLongerTemporal, "no longer temporal", helloEp);
			endpointRemove(ep, "No longer temporal");
		}

		LM_T(LmtHello, ("helloEp->helloReceived = true"));
		helloEp->helloReceived = true;

		if (msgType == Message::Msg)
		{
			LM_T(LmtHello, ("Acking HELLO"));
			helloSend(helloEp, Message::Ack);
		}
		else
			LM_T(LmtHello, ("HELLO was an ACK"));

		if (helloEp->jobQueueHead != NULL)
		{
			SendJob*  jobP;
			int       ix  = 1;

			LM_T(LmtJob, ("Sending pending JOBs to '%s' (job queue head at %p)", helloEp->name.c_str(), helloEp->jobQueueHead));
			while ((jobP = helloEp->jobPop()) != NULL)
			{
				int nb;

				LM_T(LmtJob, ("Sending pending JOB %d (job at %p)", ix, jobP));
				nb = write(helloEp->senderWriteFd, jobP, sizeof(SendJob));
				if (nb != sizeof(SendJob))
				{
					if (nb == -1)
						LM_P(("write"));
					else
						LM_E(("written only %d bytes, instead of %d", nb, sizeof(SendJob)));
				}
				++ix;
			}
		}

		if (helloEp == endpoint[CONTROLLER])
		{
			LM_T(LmtWorkerVector, ("Asking Controller for the WorkerVector"));
			if ((endpoint[ME]->type != Endpoint::CoreWorker) && (endpoint[ME]->type != Endpoint::Controller))
				iomMsgSend(endpoint[CONTROLLER], endpoint[ME], Message::WorkerVector, Message::Msg, NULL, 0, NULL);
		}
		else if ((helloEp->type == Endpoint::Controller) && (endpoint[CONTROLLER] == NULL))
			LM_T(LmtWorkerVector, ("NOT Asking Controller for the WorkerVector - controller == NULL"));
		else
			LM_T(LmtWorkerVector, ("NOT Asking '%s/%s' for the WorkerVector!", ep->typeName(), helloEp->typeName()));

		checkAllWorkersConnected();
		break;

	case Message::WorkerVector:
		if ((msgType == Message::Msg) && (endpoint[ME]->type != Endpoint::Controller))
			LM_X(1, ("Got a WorkerVector request from '%s' but I'm not the controller ...", name));

		if (endpoint[ME]->type == Endpoint::Controller)
			controllerMsgTreat(ep, msgCode, msgType, dataP, dataLen, &packet);
		else
		{
			if (msgType != Message::Ack)
				LM_X(1, ("Got a WorkerVector request, not being controller ..."));

			if (ep->type != Endpoint::Controller)
				LM_X(1, ("Got a WorkerVector ack NOT from Controller ... (endpoint type: %d)", ep->type));

			LM_T(LmtWorkerVector, ("Got the worker vector from the Controller - now connect to them all ..."));

			int                         ix;
			Message::WorkerVectorData*  workerVec = (Message::WorkerVectorData*) dataP;

			for (int ix = 0; ix < workerVec->workers; ix++)
				LM_T(LmtWorkerVector, ("Controller gave me a worker in %s:%d", workerVec->workerV[ix].ip, workerVec->workerV[ix].port));

			if (Workers < workerVec->workers)
			{
				LM_T(LmtWorkerVector, ("Got %d workers from Controller - I thought there were %d workers - changing to %d workers",
									   workerVec->workers, Workers, workerVec->workers));

				// Adding workers in Endpoint vector
				for (int ix = Workers; ix < workerVec->workers; ix++)
				{
					endpoint[FIRST_WORKER + ix]       = workerNew(ix);
					endpoint[FIRST_WORKER + ix]->port = workerVec->workerV[ix].port;
					
					if (workerVec->workerV[ix].ip[0] != 0)
						endpoint[FIRST_WORKER + ix]->ip   = workerVec->workerV[ix].ip;
					else
						LM_W(("Empty IP for worker %d", ix));
				}
			}
			else if (Workers > workerVec->workers)
			{
				// Removing workers from Endpoint vector
				LM_T(LmtWorkerVector, ("I had %d workers - Controller tells me I should have %d - deleting the rest (%d - %d)",
									   Workers,
									   workerVec->workers,
									   workerVec->workers,
									   Workers));

				for (unsigned int ix = workerVec->workers; ix < (unsigned int) Workers; ix++)
				{
					if (endpoint[FIRST_WORKER + ix] != NULL)
					{
						LM_T(LmtWorkerVector, ("deleting obsolete worker endpoint %d", FIRST_WORKER + ix));
						delete endpoint[FIRST_WORKER + ix];
						endpoint[FIRST_WORKER + ix] = NULL;
					}
				}
			}

			Workers = workerVec->workers;


			for (ix = 0; ix < Workers; ix++)
			{
				if (endpoint[FIRST_WORKER + ix] == NULL)
				{
					endpoint[FIRST_WORKER + ix]        = new Endpoint(Endpoint::Worker, workerVec->workerV[ix].name, workerVec->workerV[ix].ip, workerVec->workerV[ix].port, -1, -1);
					endpoint[FIRST_WORKER + ix]->state = Endpoint::Unconnected;
					endpoint[FIRST_WORKER + ix]->alias = workerVec->workerV[ix].alias;

					LM_T(LmtWorker, ("*** New Worker Endpoint '%s' at %p", workerVec->workerV[ix].alias, endpoint[ix]));
				}
				else
					LM_T(LmtWorker, ("Should I fill worker %02d with the info the controller gave me ?", ix));
			}

			if (endpointUpdateReceiver != NULL)
				endpointUpdateReceiver->endpointUpdate(NULL, Endpoint::WorkerVectorReceived, "Got Worker Vector", workerVec);

			for (ix = 0; ix < Workers; ix++)
			{
				Endpoint* epP;

				epP = endpoint[FIRST_WORKER + ix];

				if (strcmp(epP->alias.c_str(), endpoint[ME]->alias.c_str()) == 0)
				{
					LM_T(LmtWorker, ("NOT connecting to myself ..."));
					epP->name = std::string("me: ") + epP->ip;
					continue;
				}

				// if ((epP->rFd == -1) && (epP->port != 0))
				{
					int workerFd;

					LM_T(LmtWorker, ("Connect to worker %d: %s (host %s, port %d, alias '%s')",
									   ix, epP->name.c_str(), epP->ip.c_str(), epP->port, epP->alias.c_str()));

					if ((workerFd = iomConnect(epP->ip.c_str(), epP->port)) == -1)
					{
						if (endpoint[ME]->type == Endpoint::Delilah)
						{
							LM_X(1, ("Unable to connect to worker %d (%s) - delilah must connect to all workers", ix, workerVec->workerV[ix].name));
						}

						LM_T(LmtWorker, ("worker %d: %s (host %s, port %d) not there - no problem, he'll connect to me",
									   ix, epP->name.c_str(), epP->ip.c_str(), epP->port));
					}
					else
					{
						Endpoint* ep;

						ep = endpointAdd("Just connected to a Worker", workerFd, workerFd, (char*) "to be worker", NULL, 0, Endpoint::Temporal, epP->ip, epP->port);
						if (ep != NULL)
						{
							ep->state = Endpoint::Connected;
							LM_T(LmtEndpoint, ("Added ep with state '%s'", ep->stateName()));
						}
						else
							LM_X(1, ("endpointAdd failed"));
					}
				}
			}
		}

		iAmReady = true;
		if (readyReceiver != NULL)
			readyReceiver->ready("All endpoints added to endpoint vector");

		break;

	case Message::Alarm:
		if (endpoint[ME]->type == Endpoint::Worker)
		{
			if (ep->type != Endpoint::CoreWorker)
				LM_E(("Got an alarm event from %s endpoint '%s' - not supposed to happen!", ep->typeName(), ep->name.c_str()));
			else
			{
				// Forward Alarm to controller
				iomMsgSend(endpoint[CONTROLLER], endpoint[ME], Message::Alarm, Message::Evt, dataP, dataLen);
			}
		}
		else if (endpoint[ME]->type == Endpoint::Controller)
		{
			Alarm::AlarmData* alarmP = (Alarm::AlarmData*) dataP;

			LM_T(LmtAlarm, ("Alarm from '%s': '%s'", ep->name.c_str(), alarmP->message));
			alarmSave(ep, alarmP);
		}
		else
			LM_X(1, ("Got an alarm event from %s endpoint '%s' - not supposed to happen!", ep->typeName(), ep->name.c_str()));
		break;

	default:
		LM_T(LmtSenderThread, ("calling receiver->receive for message code '%s'", messageCode(msgCode)));
		if (packetReceiver)
		{
			packetReceiver->_receive(endpointId, msgCode, &packet);
			LM_T(LmtSenderThread, ("back from receiver->receive for message code '%s'", messageCode(msgCode)));
		}
		else
		{
			if (packet.buffer)
				MemoryManager::shared()->destroyBuffer(packet.buffer);
		}
		break;
	}

	if (dataP != data)
		free(dataP);
}



static bool stopWhenReady = false;
/* ****************************************************************************
*
* Network::runUntilReady - 
*/
void Network::runUntilReady(void)
{
	stopWhenReady = true;
	run();
}



#define PeriodForSendingWorkerStatusToController  10
/* ****************************************************************************
*
* run
*/
void Network::run(void)
{
	int             fds;
	fd_set          rFds;
	struct timeval  timeVal;
	time_t          now   = 0;
	time_t          then  = time(NULL);
	int             max;

	if (iAmReady && stopWhenReady)
		return;

	while (1)
	{
		int ix;

		do
		{
			if (endpoint[ME]->type == Endpoint::Worker)
			{
				now = time(NULL);
				if (now - then > PeriodForSendingWorkerStatusToController)
				{
					// workerStatusToController();
					then = now;
				}
			}

			timeVal.tv_sec  =  this->tmoSecs;
			timeVal.tv_usec =  this->tmoUsecs;

			FD_ZERO(&rFds);
			max = 0;

			if (endpoint[ME]->type == Endpoint::Delilah)  /* reconnect to dead workers */
			{
				for (ix = FIRST_WORKER; ix < FIRST_WORKER + Workers; ix++)
				{
					int workerFd;

					if (endpoint[ix] == NULL)
						continue;

					if (endpoint[ix]->type != Endpoint::Worker)
						continue;

					if (endpoint[ix]->state == Endpoint::Closed)
					{
						LM_T(LmtReconnect, ("delilah reconnecting to %s:%d (type: '%s', state: '%s')",
											endpoint[ix]->ip.c_str(), endpoint[ix]->port, endpoint[ix]->typeName(),
											endpoint[ix]->stateName()));
						workerFd = iomConnect(endpoint[ix]->ip.c_str(), endpoint[ix]->port);
						if (workerFd != -1)
						{
							endpointAdd("reconnecting to dead worker", workerFd, workerFd, (char*) "Reconnecting worker", NULL, 0, Endpoint::Temporal,
										endpoint[ix]->ip.c_str(),
										endpoint[ix]->port);
							endpoint[ix]->state = Endpoint::Reconnecting;
						}
					}
					else if (endpoint[ix]->state == Endpoint::Disconnected)
					{
						LM_T(LmtEndpoint, ("Connect to %s:%d ?", endpoint[ix]->ip.c_str(), endpoint[ix]->port));

						workerFd = iomConnect(endpoint[ix]->ip.c_str(), endpoint[ix]->port);
						if (workerFd != -1)
						{
							Endpoint* ep;

							ep = endpointAdd("reconnecting to Disconnected worker", workerFd, workerFd, (char*) "New Worker", NULL, 0, Endpoint::Temporal,
											 endpoint[ix]->ip.c_str(),
											 endpoint[ix]->port);
							if (ep != NULL)
								LM_T(LmtEndpoint, ("Added ep with state '%s'", ep->stateName()));
							else
								LM_X(1, ("endpointAdd failed"));

							endpoint[ix]->state = Endpoint::Connected;
						}
					}
				}
			}


			//
			// Adding fds to the read-set
			//

			time_t         now;
			static time_t  lastTime       = 0;
			bool           showSelectList = false;

			now = time(NULL);
			if (now - lastTime > 3)  // Show list each three seconds ...
			{
				showSelectList = true;
				lastTime       = now;
			}
			else
				showSelectList = false;

			if (showSelectList)
			{
				LM_V((""));
				LM_V(("------------ %.5f secs timeout, %d endpoints -------------------------------------", ((double) tmoSecs + ((double) tmoUsecs) / 1000000), Endpoints));
			}

			for (ix = 0; ix < Endpoints; ix++)
			{
				if (endpoint[ix] == NULL)
					continue;

				if ((endpoint[ix]->state == Endpoint::Connected || endpoint[ix]->state == Endpoint::Listening) && (endpoint[ix]->rFd >= 0))
				{
					FD_SET(endpoint[ix]->rFd, &rFds);
					max = MAX(max, endpoint[ix]->rFd);
				}
			}


			if (showSelectList)
			{
				endpointListShow("periodic");

				if (endpointUpdateReceiver != NULL)
					endpointUpdateReceiver->endpointUpdate(NULL, Endpoint::SelectToBeCalled, "Select to be called");
			}

			fds = select(max + 1, &rFds, NULL, NULL, &timeVal);
		} while ((fds == -1) && (errno == EINTR));

		if (fds == -1)
		{
			if (errno != EINTR)
				LM_XP(1, ("select"));
		}
		else if (fds == 0)
		{
			// LM_D(("Timeout in network event loop"));
		}
		else
		{
			int ix;

			if (endpoint[LISTENER] && (endpoint[LISTENER]->state == Endpoint::Listening) && FD_ISSET(endpoint[LISTENER]->rFd, &rFds))
			{
				int  fd;
				char hostName[128];
				char ip[128];
				struct sockaddr_in sin;

				LM_T(LmtSelect, ("incoming message from my listener - I will accept ..."));
				--fds;
				fd = iomAccept(endpoint[LISTENER]->rFd, &sin, hostName, sizeof(hostName), ip, sizeof(ip));
				if (fd == -1)
				{
					LM_P(("iomAccept(%d)", endpoint[LISTENER]->rFd));
					endpoint[LISTENER]->msgsInErrors += 1;
				}
				else
				{
					std::string  s   = std::string("tmp:") + std::string(hostName);
					Endpoint*    ep;

					ep = endpointAdd("'run' just accepted an incoming connection",
									 fd, fd, (char*) s.c_str(), NULL, 0, Endpoint::Temporal, hostName, 0);

					memcpy(&ep->sockin, &sin, sizeof(sin));

					hostMgr->insert(hostName, ip);
					endpoint[LISTENER]->msgsIn += 1;
					LM_T(LmtHello, ("sending hello to newly accepted endpoint"));
					helloSend(ep, Message::Msg);
				}
			}

			if (fds > 0)
			{
				// Treat endpoint for endpoint vector - skipping the first two ... (me & listener)
				// For now, only temporal endpoints are in endpoint vector
				LM_T(LmtSelect, ("looping from %d to %d", CONTROLLER, Endpoints));
				for (ix = CONTROLLER; ix < Endpoints; ix++)
				{
					if ((endpoint[ix] == NULL) || (endpoint[ix]->rFd < 0))
						continue;

					if (FD_ISSET(endpoint[ix]->rFd, &rFds))
					{
						--fds;
						LM_T(LmtSelect, ("incoming message from '%s' endpoint %s (fd %d)", endpoint[ix]->typeName(), endpoint[ix]->name.c_str(), endpoint[ix]->rFd));
						msgPreTreat(endpoint[ix], ix);
						if (endpoint[ix])
							FD_CLR(endpoint[ix]->rFd, &rFds);
					}
				}
			}
		}

		if (iAmReady && stopWhenReady)
			return;
	}
}



/* ****************************************************************************
*
* poll
*/
int Network::poll(void)
{
	int             fds;
	fd_set          rFds;
	struct timeval  timeVal;
	int             max;
	int             ix;

	do
	{
		timeVal.tv_sec  =  0;
		timeVal.tv_usec =  0;

		//
		// Adding fds to the read-set
		//
		FD_ZERO(&rFds);
		max = 0;

		for (ix = 0; ix < Endpoints; ix++)
		{
			if (endpoint[ix] == NULL)
				continue;

			if ((endpoint[ix]->state == Endpoint::Connected || endpoint[ix]->state == Endpoint::Listening) && (endpoint[ix]->rFd >= 0))
			{
				FD_SET(endpoint[ix]->rFd, &rFds);
				max = MAX(max, endpoint[ix]->rFd);
			}
		}

		fds = select(max + 1, &rFds, NULL, NULL, &timeVal);
	} while ((fds == -1) && (errno == EINTR));

	if (fds == -1)
		LM_RE(-1, ("select: %s", strerror(errno)));
	if (fds == 0)
		return -2;

	if (endpoint[LISTENER] && (endpoint[LISTENER]->state == Endpoint::Listening) && FD_ISSET(endpoint[LISTENER]->rFd, &rFds))
	{
		int   fd;
		char  hostName[128];
		char  ip[128];

		LM_T(LmtSelect, ("incoming message from my listener - I will accept ..."));
		--fds;
		fd = iomAccept(endpoint[LISTENER]->rFd, &endpoint[LISTENER]->sockin, hostName, sizeof(hostName), ip, sizeof(ip));
		if (fd == -1)
		{
			LM_P(("iomAccept(%d)", endpoint[LISTENER]->rFd));
			endpoint[LISTENER]->msgsInErrors += 1;
		}
		else
		{
			std::string  s   = std::string("tmp:") + std::string(hostName);
			Endpoint*    ep;

			ep = endpointAdd("'poll' just accepted an incoming connection",
							 fd, fd, (char*) s.c_str(), NULL, 0, Endpoint::Temporal, hostName, 0);
			hostMgr->insert(hostName, ip);

			endpoint[LISTENER]->msgsIn += 1;
			LM_T(LmtHello, ("sending hello to newly accepted endpoint"));
			helloSend(ep, Message::Msg);
		}
	}

	while (fds > 0)
	{
		// Treat endpoint for endpoint vector - skipping the first two ... (me & listener)
		// For now, only temporal endpoints are in endpoint vector
		// LM_T(LmtSelect, ("looping from %d to %d", CONTROLLER, Endpoints));
		for (ix = CONTROLLER; ix < Endpoints; ix++)
		{
			if ((endpoint[ix] == NULL) || (endpoint[ix]->rFd < 0))
				continue;

			if (FD_ISSET(endpoint[ix]->rFd, &rFds))
			{
				FD_CLR(endpoint[ix]->rFd, &rFds);
				--fds;

				LM_T(LmtSelect, ("incoming message from '%s' endpoint %s (fd %d)", endpoint[ix]->typeName(), endpoint[ix]->name.c_str(), endpoint[ix]->rFd));
				msgPreTreat(endpoint[ix], ix);
			}
		}
	}

	return 0;
}



/* ****************************************************************************
*
* getState - 
*/
std::string Network::getState(std::string selector)
{
	int          ix;
	int          eps = 0;
	std::string  output;
	char         partString[256];

	output = "";

	for (ix = CONTROLLER; ix < Endpoints; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if ((endpoint[ix]->state == Endpoint::Connected || endpoint[ix]->state == Endpoint::Listening) && (endpoint[ix]->rFd >= 0))
		{
			++eps;
			snprintf(partString, sizeof(partString), "+ %02d: %-15s %-20s %-12s %15s:%05d %16s  fd: %02d  (in: %03d, out: %03d)\n",
					 ix,
					 endpoint[ix]->typeName(),
					 endpoint[ix]->name.c_str(),
					 endpoint[ix]->alias.c_str(),
					 endpoint[ix]->ip.c_str(),
					 endpoint[ix]->port,
					 endpoint[ix]->stateName(),
					 endpoint[ix]->rFd,
					 endpoint[ix]->msgsIn,
					 endpoint[ix]->msgsOut);
		}
		else
		{
			snprintf(partString, sizeof(partString), "- %02d: %-15s %-20s %-12s %15s:%05d %16s  fd: %02d  (in: %03d, out: %03d)\n",
					 ix,
					 endpoint[ix]->typeName(),
					 endpoint[ix]->name.c_str(),
					 endpoint[ix]->alias.c_str(),
					 endpoint[ix]->ip.c_str(),
					 endpoint[ix]->port,
					 endpoint[ix]->stateName(),
					 endpoint[ix]->rFd,
					 endpoint[ix]->msgsIn,
					 endpoint[ix]->msgsOut);
		}

		output += partString;
	}

	snprintf(partString, sizeof(partString), "Connected to %d endpoints:\n", eps);
	return std::string(partString) + output;
}



/* ****************************************************************************
*
* fdSet - 
*/
void Network::fdSet(int fd, const char* name, const char* alias)
{
	Endpoint* ep;

	ep = endpointAdd("setting a file descriptor", fd, -1, name, alias, 0, Endpoint::Fd, "FD", 0);
	LM_T(LmtFds, ("setting state to Endpoint::Connected for fd %d", fd));
	ep->state = Endpoint::Connected;
}	



/* ****************************************************************************
*
* quit - 
*/
void Network::quit()
{
}	

/* ****************************************************************************
*
* controllerGet -
*/
Endpoint* Network::controllerGet(void)
{
	return endpoint[CONTROLLER];
}



/* ****************************************************************************
*
* controllerGet -
*/
bool Network::isConnected(unsigned int identifier)
{
	if (identifier >= (unsigned int) Endpoints)
		return false;

	if (endpoint[identifier] == NULL)
		return false;

	if (endpoint[identifier]->state == Endpoint::Connected)
		return true;

	return false;
}

}
