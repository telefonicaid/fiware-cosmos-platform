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
#include <vector>				// ss::vector
#include <sys/types.h>          // pid_t
#include <unistd.h>             // fork, getpid
#include <sched.h>              // sched_setaffinity

#include "logMsg.h"             // LM_*
#include "networkTraceLevels.h" // LMT_*
#include "Alarm.h"              // ALARM

#include "Endpoint.h"			// Endpoint
#include "Message.h"            // ss::Message::MessageCode
#include "Packet.h"				// Packet
#include "iomInit.h"            // iomInit
#include "iomServerOpen.h"      // iomServerOpen
#include "iomConnect.h"         // iomConnect
#include "iomAccept.h"          // iomAccept
#include "iomMsgSend.h"         // iomMsgSend
#include "iomMsgRead.h"         // iomMsgRead
#include "iomMsgAwait.h"        // iomMsgAwait
#include "workerStatus.h"       // workerStatus, WorkerStatusData
#include "Network.h"			// Own interface



namespace ss
{



/* ****************************************************************************
*
* global vars ...
*/



/* ****************************************************************************
*
* endpointTypeName - 
*/
static const char* endpointTypeName(Endpoint::Type type)
{
	switch (type)
	{
	case Endpoint::Unknown:       return "Unknown";
	case Endpoint::Temporal:      return "Temporal";
	case Endpoint::Listener:      return "Listener";
	case Endpoint::Controller:    return "Controller";
	case Endpoint::Worker:        return "Worker";
	case Endpoint::CoreWorker:    return "CoreWorker";
	case Endpoint::Delilah:       return "Delilah";
	}

	return "UnknownEndpointType";
}



/* ****************************************************************************
*
* Constructor 
*/
Network::Network()
{
	Workers    = WORKERS;
	iAmReady   = false;

	receiver   = NULL;

	me         = NULL;
	listener   = NULL;
	controller = NULL;

	memset(endpoint, 0, sizeof(endpoint));
}



/* ****************************************************************************
*
* setPacketReceiver - 
*/
void Network::setPacketReceiver(PacketReceiverInterface* _receiver)
{
	LM_X(1, ("IN"));
	receiver = _receiver;
}

	

/* ****************************************************************************
*
* setPacketReceiverInterface - set the element to be notified when packages arrive
*/
void Network::setPacketReceiverInterface(PacketReceiverInterface* _receiver)
{
	LM_T(LMT_DELILAH, ("Setting receiver to %p", _receiver));
	receiver = _receiver;
}
	


/* ****************************************************************************
*
* init - open listen socket on port specifid by 'me' endpoint
*/
void Network::init(Endpoint::Type type, unsigned short port)
{
	endpoint[0] = new Endpoint(type, port);
	if (endpoint[0] == NULL)
		LM_XP(1, ("new Endpoint"));
	me = endpoint[0];

	if (me == NULL)
		LM_XP(1, ("unable to allocate room for Endpoint 'me'"));

	me->name     = progName;
	me->state    = Endpoint::Me;

	if (port != 0)
	{
		endpoint[1] = new Endpoint(*me);
		if (endpoint[1] == NULL)
			LM_XP(1, ("new Endpoint"));
		listener = endpoint[1];

		listener->fd       = iomServerOpen(listener->port);
		listener->state    = Endpoint::Listening;
		listener->type     = Endpoint::Listener;

		if (listener->fd == -1)
			LM_XP(1, ("unable to open port %d for listening", listener->port));

		LM_T(LMT_FDS, ("opened fd %d to accept incoming connections", listener->fd));
	}

	ipSet(NULL);
}



/* ****************************************************************************
*
* wordClean - 
*/
static char* wordClean(char* str)
{
	char* endP;

	while ((*str == ' ') || (*str == '\t'))
		++str;

	endP = str;
	while ((*endP != 0) && (*endP != ' ') && (*endP != '\t'))
		++endP;
	*endP = 0;

	return str;
}



/* ****************************************************************************
*
* ipSet - 
*/
void Network::ipSet(char* ip)
{
	char line[80];

	if (ip != NULL)
		me->ip = ip;
	else
	{
		FILE* fP;

		fP = popen("ifconfig | grep \"inet addr:\" | awk -F: '{ print $2 }' | awk '{ print $1 }'", "r");
		if (fgets(line, sizeof(line), fP) != NULL)
		{
			if (line[strlen(line) - 1] == '\n')
				line[strlen(line) - 1] = 0;
			me->ip = std::string(wordClean(line));
			LM_T(LMT_CONFIG, ("new IP: %s", me->ip.c_str()));
		}

		fclose(fP);
	}

	if (listener)
		listener->ip = me->ip;

	LM_T(LMT_CONFIG, ("me->ip:   '%s'", me->ip.c_str()));
	LM_T(LMT_CONFIG, ("me->name: '%s'", me->name.c_str()));
}

	

/* ****************************************************************************
*
* helloSend - 
*/
int Network::helloSend(Endpoint* ep, Message::MessageType type)
{
	ss::Message::HelloData hello;

	strncpy(hello.name, me->name.c_str(), sizeof(hello.name));
	strncpy(hello.ip,   me->ip.c_str(),   sizeof(hello.ip));

	hello.type    = me->type;
	hello.workers = me->workers;
	hello.port    = me->port;
	hello.coreNo  = me->coreNo;

	LM_T(LMT_WRITE, ("sending hello %s to '%s' (name: '%s', type: %s (%d))", messageType(type), ep->name.c_str(), hello.name, endpointTypeName(hello.type), hello.type));

	return iomMsgSend(ep->fd, ep->name.c_str(), me->name.c_str(), Message::Hello, type, &hello, sizeof(hello));
}



/* ****************************************************************************
*
* initAsSamsonController - 
*/
void Network::initAsSamsonController(int port, std::vector<std::string> peers)
{
	unsigned int ix;

	controller  = NULL;
	endpoint[2] = NULL;

	for (ix = 0; ix < peers.size(); ix++)
		endpointV.push_back(Endpoint(Endpoint::Worker, peers[ix])); 

	init(Endpoint::Controller, port);
	listener->name = "Listener";

	LM_F(("I am a '%s', my name: '%s', ip: %s", endpointTypeName(me->type), me->name.c_str(), me->ip.c_str()));

	ix = 0;
	if ((int) endpointV.size() != Workers)
		LM_X(1, ("bad size of worker vector (%d). Should be %d!", endpointV.size(), Workers));

	for (ix = 0; ix < endpointV.size(); ix++)
	{
		char name[32];

		LM_T(LMT_ENDPOINTS, ("endpointV.ip: '%s'", endpointV[ix].ip.c_str()));
		endpointV[ix].state = Endpoint::FutureWorker;

		snprintf(name, sizeof(name), "Worker %02d", ix);
		endpointV[ix].name = std::string(name);

		endpoint[3 + ix] = new Endpoint(endpointV[ix]);
	}
}



/* ****************************************************************************
*
* coreWorkerStart - 
*/
void Network::coreWorkerStart(int coreNo, char* fatherName, int port)
{
	Endpoint* ep;

	ep = endpointCoreWorkerLookup(coreNo);
	if (ep == NULL)
	{
		char cwName[16];

		sprintf(cwName, "Core %02d", coreNo);
		ep = endpointAdd(-1, cwName, 0, Endpoint::CoreWorker, "localhost", 0, coreNo);
	}
	if (ep == NULL)
		LM_X(1, ("NULL endpoint ..."));

	ep->startTime        = time(NULL);
	ep->coreNo           = coreNo;
	ep->coreWorkerState  = Message::NotBusy;

	// Using these two fields to improve debugging ...
	ep->ip              = "Core";
	ep->port            = coreNo;

	LM_T(LMT_COREWORKER, ("*********** Starting Core Worker %d", coreNo));
	if (fork() == 0)
	{
		cpu_set_t cpuSet;

		CPU_ZERO(&cpuSet);
		CPU_SET(coreNo, &cpuSet);
		if (sched_setaffinity(0, sizeof(cpuSet), &cpuSet) == -1)
			LM_XP(1, ("sched_setaffinity"));

		LM_T(LMT_COREWORKER, ("child %d running (pid: %d) on core %d", coreNo, (int) getpid(), coreNo));

		/* ************************************************************
		 *
		 * Core workers do not log to stdout
		 */
		lmFdUnregister(1);



		/* ************************************************************
		 *
		 * Set progName
		 */
		progName = (char*) malloc(strlen("samsonCoreWorker_") + 10);
		if (progName == NULL)
			LM_X(1, ("samsonCoreWorker_%d died allocating: %s", getpid(), strerror(errno)));
		sprintf(progName, (char*) "samsonCoreWorker_%d", (int) getpid());


		
		/* ************************************************************
		 *
		 * Setting auxiliar string for logMsg
		 */
		char auxString[16];

		sprintf(auxString, "core%02d", coreNo);
		lmAux(auxString);



		/* ************************************************************
		 *
		 * Clean out father processes endpoints
		 */
		me         = NULL;
		listener   = NULL;
		controller = NULL;

		unsigned int epIx;
		for (epIx = 0; epIx < sizeof(endpoint) / sizeof(endpoint[0]); epIx++)
		{
			if (endpoint[epIx] != NULL)
				close(endpoint[epIx]->fd);
			endpoint[epIx] = NULL;
		}



		/* ************************************************************
		 *
		 * Creating my 'me' endpoint
		 */
		endpoint[0] = new Endpoint(Endpoint::CoreWorker, -1);
		if (endpoint[0] == NULL)
			LM_XP(1, ("new Endpoint"));

		me          = endpoint[0];
		me->name    = progName;
		me->state   = Endpoint::Me;
		me->coreNo  = coreNo;



		LM_T(LMT_COREWORKER, ("new me created"));
		/* ************************************************************
		 *
		 * connect to samsonWorker (father process is like a controller, so I use index 2 ...)
		 */
		endpoint[2] = new Endpoint(Endpoint::Worker, fatherName);
		if (endpoint[2] == NULL)
			LM_XP(1, ("error allocating endpoint for father"));
		controller = endpoint[2];

		LM_T(LMT_COREWORKER, ("Connecting to father"));
		controller->fd = iomConnect("localhost", port);
		if (controller->fd == -1)
			LM_X(1, ("error connecting to controller at %s:%d", controller->ip.c_str(), controller->port));
		LM_T(LMT_COREWORKER, ("Connected to father"));
		controller->state = ss::Endpoint::Connected;

		LM_T(LMT_COREWORKER, ("Calling RUN"));
		run();
		LM_T(LMT_COREWORKER, ("Back from run - should not ever get here (coreWorker %d, pid: %d)", coreNo, (int) getpid()));
		exit(1);
	}
}



/* ****************************************************************************
*
* initAsSamsonWorker -
*
* NOTE
*/
void Network::initAsSamsonWorker(int port, std::string controllerName)
{
	Message::WorkerStatusData ws;

	init(Endpoint::Worker, port);

	LM_F(("I am a '%s', my name: '%s', ip: %s", endpointTypeName(me->type), me->name.c_str(), me->ip.c_str()));

	endpoint[2] = new Endpoint(Endpoint::Controller, controllerName);
	if (endpoint[2] == NULL)
		LM_XP(1, ("new Endpoint"));
	controller = endpoint[2];

	controller->fd = iomConnect((const char*) controller->ip.c_str(), (unsigned short) controller->port);
	if (controller->fd == -1)
		LM_X(1, ("error connecting to controller at %s:%d", controller->ip.c_str(), controller->port));

	controller->state = ss::Endpoint::Connected;

	workerStatus(&ws, this);

	char traceLevelV[256];
	lmTraceGet(traceLevelV);

	int coreNo;
	for (coreNo = 0; coreNo < ws.cpuInfo.cores; coreNo++)
		coreWorkerStart(coreNo, progName, port);
}



/* ****************************************************************************
*
* initAsDelilah - 
*/
void Network::initAsDelilah(std::string controllerName)
{
	init(Endpoint::Delilah);

	LM_F(("I am a '%s', my name: '%s', ip: %s", endpointTypeName(me->type), me->name.c_str(), me->ip.c_str()));

	endpoint[2] = new Endpoint(Endpoint::Controller, controllerName);
	if (endpoint[2] == NULL)
		LM_XP(1, ("new Endpoint"));
	controller = endpoint[2];

	controller->fd = iomConnect((const char*) controller->ip.c_str(), (unsigned short) controller->port);
	if (controller->fd == -1)
		LM_X(1, ("error connecting to controller at %s:%d", controller->ip.c_str(), controller->port));

	controller->state = ss::Endpoint::Connected;
}
	
	

/* ****************************************************************************
*
* controllerGetIdentifier - 
*/
int Network::controllerGetIdentifier(void)
{
	LM_T(LMT_DELILAH, ("Asking for controller id"));
	return 2;
}



/* ****************************************************************************
*
* workerGetIdentifier - 
*
* worker 0 is at index 3 in the local endpoint vector
*/
int Network::workerGetIdentifier(int nthWorker)
{
	LM_X(1, ("IN"));

	return nthWorker + 3;
}
	
	

/* ****************************************************************************
*
* getWorkerFromIdentifier - 
*
* worker 0 is at index 3 in the local endpoint vector
*/
int Network::getWorkerFromIdentifier(int identifier)
{
	return identifier - 3;
}



/* ****************************************************************************
*
* getNumWorkers - 
*
* Return number of connected workers
*/
int Network::getNumWorkers(void)
{
	int  ix;
	int  workers = 0;

	LM_T(LMT_DELILAH, ("IN"));

	for (ix = 3; ix < 3 + Workers; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix]->state == Endpoint::Connected)
			++workers;
	}

	return workers;
}
	
	

/* ****************************************************************************
*
* samsonWorkerEndpoints - 
*
* Return list of connected workers ?
*/
std::vector<Endpoint*> Network::samsonWorkerEndpoints()
{
	int                     ix;
	std::vector<Endpoint*>  v;

	LM_X(1, ("IN"));

	for (ix = 3; ix <  3 + Workers; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix]->state == Endpoint::Connected)
			v.push_back(endpoint[ix]);
	}

	return v;
}
	


/* ****************************************************************************
*
* send - 
*/
size_t Network::send(PacketSenderInterface* sender, int endpointId, ss::Message::MessageCode code, void* data, int dataLen, Packet* packetP)
{
	Endpoint* ep        = endpoint[endpointId];

	if (ep == NULL)
		LM_RE(-1, ("No endpoint at index %d", endpointId));
	if (ep->state != Endpoint::Connected)
		LM_RE(-1, ("Endpoint %d in state '%s'", endpointId, ep->stateName()));

	LM_T(LMT_DELILAH, ("sending a '%s' message to endpoint %d", messageCode(code), endpointId));

	int nb = iomMsgSend(ep->fd, ep->name.c_str(), me->name.c_str(), code, Message::Msg, data, dataLen, packetP, NULL, 0);

	if (sender)
		sender->notificationSent(0, true);

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
* endpointAdd - add an endpoint to the vector
*
* The first three slots in this vector are:
*  - 0: me
*  - 1: listener
*  - 2: controller
*
* From slot 3, the worker endpoints are stored. These endpoints have a fix
* position in the vector and these positions are defined by the response
* of the WorkerVector from thge controller. The controller itself gets the
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
Endpoint* Network::endpointAdd(int fd, char* name, int workers, Endpoint::Type type, std::string ip, unsigned short port, int coreNo = -1)
{
	int ix;

	LM_T(LMT_ENDPOINT, ("Adding endpoint '%s' of type '%s' for fd %d", name, endpointTypeName(type), fd));

	switch (type)
	{
	case Endpoint::Unknown:
	case Endpoint::Listener:
		LM_X(1, ("bad type: %d", type));
		return NULL;

	case Endpoint::Controller:
		endpoint[2]->name = std::string(name);
		endpoint[2]->ip   = std::string(ip);
		return controller;

	case Endpoint::Temporal:
		for (ix = sizeof(endpoint) / sizeof(endpoint[0]) - 1; ix >= 3 + Workers; ix--)
		{
			if (endpoint[ix] == NULL)
			{
				endpoint[ix] = new Endpoint();
				if (endpoint[ix] == NULL)
					LM_XP(1, ("allocating temporal Endpoint"));

				endpoint[ix]->name  = std::string(name);
				endpoint[ix]->fd    = fd;
				endpoint[ix]->state = Endpoint::Connected;
				endpoint[ix]->type  = Endpoint::Temporal;
				endpoint[ix]->ip    = ip;

				return endpoint[ix];
			}
		}

		if (endpoint[ix] == NULL)
			LM_X(1, ("No temporal endpoint slots available - redefine and recompile!"));
		break;


	case Endpoint::CoreWorker:
		Endpoint* ep;

		ep = endpointCoreWorkerLookup(coreNo);
		if (ep != NULL)
		{
			LM_T(LMT_EP, ("Found CoreWorker (core %d)", coreNo));
			ep->fd    = fd;
			ep->state = Endpoint::Connected;
			ep->name  = std::string(name);

			return ep;
		}
		/* no break here ! */
		/* The first time a CoreWorker Endpoint is created we pass thru */

	case Endpoint::Delilah:
		for (ix = 3 + Workers; ix < (int) (sizeof(endpoint) / sizeof(endpoint[0]) - 1); ix++)
		{
			if (endpoint[ix] == NULL)
			{
				endpoint[ix] = new Endpoint();
				if (endpoint[ix] == NULL)
					LM_XP(1, ("allocating Endpoint"));

				endpoint[ix]->name       = std::string(name);
				endpoint[ix]->fd         = fd;
				endpoint[ix]->state      = Endpoint::Connected;
				endpoint[ix]->type       = type;
				endpoint[ix]->ip         = ip;
				endpoint[ix]->port       = port;
				endpoint[ix]->coreNo     = coreNo;
				endpoint[ix]->restarts   = 0;
				endpoint[ix]->jobsDone   = 0;
				endpoint[ix]->startTime  = time(NULL);

				return endpoint[ix];
			}
		}

		if (endpoint[ix] == NULL)
			LM_X(1, ("No endpoint slots available - redefine and recompile!"));
		LM_X(1, ("ERROR ?"));
		break;

	case Endpoint::Worker:
		if (me->type == Endpoint::CoreWorker)
		{
			if (controller == NULL)
				LM_X(1, ("controller == NULL"));

			controller->fd   = fd;
			controller->name = std::string(name);
			
			return controller;
		}

		for (ix = 3; ix < 3 + Workers; ix++)
		{
			if (endpoint[ix] == NULL)
				LM_X(1, ("NULL worker endpoint at slot %d", ix));

			if (((strcmp(endpoint[ix]->ip.c_str(), ip.c_str()) == 0) && (endpoint[ix]->port == port)) ||
				((strcmp(endpoint[ix]->hostname.c_str(), ip.c_str()) == 0) && (endpoint[ix]->port == port)))
			{
				endpoint[ix]->fd    = fd;
				endpoint[ix]->name  = std::string(name);
				endpoint[ix]->state = Endpoint::Connected;
				endpoint[ix]->type  = Endpoint::Worker;

				return endpoint[ix];
			}
		}

		LM_X(1, ("Worker '%s:%d' not found!", ip.c_str(), port));
		break;
	}

	LM_X(1, ("BUG"));
	return NULL;
}



/* ****************************************************************************
*
* endpointRemove - 
*/
void Network::endpointRemove(Endpoint* ep)
{
	unsigned int ix;

	LM_T(LMT_EP, ("Removing '%s' endpoint '%s'", endpointTypeName(ep->type), ep->name.c_str()));

	for (ix = 0; ix < sizeof(endpoint) / sizeof(endpoint[0]); ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix] == ep)
		{
			if (ep->type == Endpoint::Worker)
			{
				ep->fd    = -1;
				ep->state = Endpoint::Disconnected;
				ep->name  = std::string("To be a worker");
			}
			else
			{
				delete ep;
				endpoint[ix] = NULL;
			}
		}
	}
}



/* ****************************************************************************
*
* endpointLookup - 
*/
Endpoint* Network::endpointLookup(int ix)
{
	return endpoint[ix];
}



/* ****************************************************************************
*
* endpointCoreWorkerLookup - 
*/
Endpoint* Network::endpointCoreWorkerLookup(int coreNo)
{
	unsigned int ix;

	for (ix = 3 + Workers; ix < sizeof(endpoint) / sizeof(endpoint[0]); ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix]->type != Endpoint::CoreWorker)
			continue;

		if (endpoint[ix]->coreNo != coreNo)
			continue;

		return endpoint[ix];
	}

	return NULL;
}



/* ****************************************************************************
*
* endpointLookup - 
*/
Endpoint* Network::endpointLookup(int fd, int* idP)
{
	unsigned int ix = 0;

	if (fd < 0)
		return NULL;

	for (ix = 0; ix < sizeof(endpoint) / sizeof(endpoint[0]); ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix]->fd == fd)
		{
			if (idP)
				*idP = ix;
			return endpoint[ix];
		}
	}

	LM_E(("endpoint (fd:%d) not found", fd));
	return NULL;
}



/* ****************************************************************************
*
* msgTreat - 
*/
void Network::msgTreat(int fd, char* name)
{
	Packet                packet;
	Packet                ack;
	Message::MessageCode  msgCode;
	Message::MessageType  msgType;
	int                   s;
	int                   endpointId;
	Endpoint*             ep = endpointLookup(fd, &endpointId);
	char                  data[1024];
	void*                 dataP   = data;
	int                   dataLen = sizeof(data);

	if (ep == NULL)
		LM_X(1, ("endpoint not found for fd %d", fd));

	LM_T(LMT_SELECT, ("treating incoming message from '%s' (ep at %p)", name, ep));
	s = iomMsgRead(fd, name, &msgCode, &msgType, &dataP, &dataLen, &packet, NULL, 0);
	LM_T(LMT_SELECT, ("iomMsgRead returned %d", s));

	if (s != 0)
	{
		LM_T(LMT_SELECT, ("iomMsgRead returned %d", s));

		if (s == -2) /* Connection closed */
		{
			LM_T(LMT_SELECT, ("Connection closed - ep at %p", ep));
			if (ep == controller)
			{
				if (me->type == Endpoint::CoreWorker)
					LM_X(1, ("My Father (samsonWorker) died, I cannot exist without father process"));

				LM_W(("controller died ... trying to reconnect !"));

				controller->fd    = -1;
				controller->state = ss::Endpoint::Disconnected;

				while (controller->fd == -1)
				{
					controller->fd = iomConnect((const char*) controller->ip.c_str(), (unsigned short) controller->port);
					sleep(1);
				}

				controller->state = ss::Endpoint::Connected;
				return;
			}
			else if (ep != NULL)
			{
				if (ep->type == Endpoint::Worker)
				{
					--me->workers;

					close(ep->fd);
					ep->state = ss::Endpoint::Closed;
					ep->fd    = -1;
					ep->name  = "-----";
				}
				else if (ep->type == Endpoint::CoreWorker)
				{
					time_t now = time(NULL);

					if (me->type != Endpoint::Worker)
						LM_X(1, ("BUG - only Worker should be connected to CoreWorker (I'm a '%s')", endpointTypeName(me->type)));

					close(ep->fd);
					ep->state = Endpoint::Dead;
					ep->name  = std::string("dead: ") + ep->name;
					ep->fd    = -1;

					if (now - ep->startTime > 5)
					{
						ALARM(Alarm::Error, Alarm::CoreWorkerDied, ("Core worker %d died", ep->coreNo));
						coreWorkerStart(ep->coreNo, progName, me->port);
						ep->startTime = now;
					}
					else
						ALARM(Alarm::Error, Alarm::CoreWorkerNotRestarted, ("Core worker %d died %d secs after restart",
																			ep->coreNo, now - ep->startTime));
				}
				else
					endpointRemove(ep);
			}
		}

		LM_RVE(("iomMsgRead: error reading message from '%s'", name));
	}


	LM_T(LMT_TREAT, ("Treating %s %s from %s", messageCode(msgCode), messageType(msgType), name));
	switch (msgCode)
	{
	case Message::Hello:
		Endpoint*            helloEp;
		Message::HelloData*  hello;

		hello = (Message::HelloData*) dataP;

		LM_T(LMT_HELLO, ("Got Hello %s from %s, type %s, %s:%d, workers: %d",
						 messageType(msgType), hello->name, endpointTypeName(hello->type), hello->ip, hello->port, hello->workers));

		helloEp = endpointAdd(fd, hello->name, hello->workers, hello->type, hello->ip, hello->port, hello->coreNo);

		if (helloEp == NULL)
			LM_X(1, ("helloEp == NULL"));

		if (ep && ep->type == Endpoint::Temporal)
			endpointRemove(ep);

		if (msgType == Message::Msg)
			helloSend(helloEp, Message::Ack);

		if ((ep != NULL) && (ep == controller))
		{
			if ((me->type != Endpoint::CoreWorker) && (me->type != Endpoint::Controller))
			{
				Packet packet;

				// Ask controller for list of workers
				iomMsgSend(controller->fd, (char*) controller->name.c_str(), (char*) me->name.c_str(),
						   Message::WorkerVector, Message::Msg, NULL, 0, NULL, NULL, 0);
			}
		}
		break;

	case Message::WorkerVector:
		if ((msgType == Message::Msg) && (me->type != Endpoint::Controller))
			LM_X(1, ("Got a WorkerVector request from '%s' but I'm not the controller ...", name));

		if ((me->type == Endpoint::Controller) && (msgType == Message::Msg))
		{
			ack.endpointVectorAdd(endpointV);

			LM_T(LMT_WRITE, ("sending ack with entire worker vector"));
			iomMsgSend(fd, name, (char*) me->name.c_str(), Message::WorkerVector, Message::Ack, NULL, 0, &ack, NULL, 0);
		}
		else if (msgType == Message::Ack)
		{
			LM_T(LMT_ENDPOINT, ("Got the worker vector from the Controller - now connect to them all ..."));

			if (packet.endpointVecSize() != Workers)
				LM_X(1, ("bad size of worker vector (%d). Should be %d!", packet.endpointVecSize(), Workers));

			int ix;
			for (ix = 0; ix < packet.endpointVecSize(); ix++)
			{
				Endpoint  ep = Endpoint(packet.endpointGet(ix));
				Endpoint* epP;

				if (endpoint[3 + ix] == NULL)
					endpoint[3 + ix] = new Endpoint(ep);

				epP = endpoint[3 + ix];

				LM_TODO(("compare endpoint[3 + ix] and endpoint - should be equal in everything (name, ip, port ...)!"));

				if (((strcmp(epP->ip.c_str(), me->ip.c_str())       == 0)  ||
					 (strcmp(epP->ip.c_str(), me->hostname.c_str()) == 0)) && (epP->port == me->port))
				{
					LM_T(LMT_WORKERS, ("NOT connecting to myself ..."));
					epP->name = std::string("me: ") + epP->ip;
					continue;
				}

				if (epP->fd == -1)
				{
					int workerFd;

					LM_T(LMT_WORKERS, ("Connect to worker %d: %s (host %s, port %d)",
									   ix, epP->name.c_str(), epP->ip.c_str(), epP->port));

					if ((workerFd = iomConnect(epP->ip.c_str(), epP->port)) == -1)
					{
						LM_T(LMT_WORKERS, ("worker %d: %s (host %s, port %d) not there - no problem, he'll connect to me",
									   ix, epP->name.c_str(), epP->ip.c_str(), epP->port));
					}
					else
					{
						Endpoint* ep;

						epP->state = Endpoint::Connected;

						ep = endpointAdd(workerFd, (char*) "to be worker", 0, Endpoint::Temporal, epP->ip, epP->port);
						if (ep != NULL)
							LM_T(LMT_ENDPOINT, ("Added ep with state '%s'", ep->stateName()));
						else
							LM_X(1, ("endpointAdd failed"));
					}
				}
			}
		}
		break;

	case Message::WorkerStatus:
		if ((me->type != Endpoint::Controller) && (msgType == Message::Msg))
			LM_X(1, ("Non-controller got a WorkerStatus message"));

		if (ep->status == NULL)
		{
			ep->status = (Message::WorkerStatusData*) malloc(sizeof(Message::WorkerStatusData));
			if (ep->status == NULL)
				LM_XP(1, ("malloc(WorkerStatusData"));
			memcpy(ep->status, dataP, sizeof(Message::WorkerStatusData));
			LM_T(LMT_STAT, ("endpoint '%s' has %d cores", ep->name.c_str(), ep->status->cpuInfo.cores));
		}
		break;

	case Message::Job:
		Message::JobData* jobP;

		jobP = (Message::JobData*) dataP;

		if (me->type == Endpoint::Worker)
		{
			if ((ep->type != Endpoint::Delilah) && (ep->type != Endpoint::Controller))
			{
				ALARM(Alarm::Error, Alarm::BadRequest, ("got a Job request from a '%s' endpoint - ignoring it", endpointTypeName(ep->type)));
			}
			else
			{
				Endpoint* cwP = endpointCoreWorkerLookup(jobP->coreNo);

				if (cwP == NULL)
					ALARM(Alarm::Error, Alarm::CoreWorkerNotFound, ("cannot find core worker %d", jobP->coreNo));
				else if (cwP->coreWorkerState != Message::NotBusy)
					ALARM(Alarm::Warning, Alarm::CoreWorkerBusy, ("core worker %d busy - try again later ...", jobP->coreNo));
				else
				{
					int s;

					s = iomMsgSend(cwP->fd, cwP->name.c_str(), me->name.c_str(), Message::Job, Message::Evt);
					if (s != 0)
						LM_E(("iomMsgSend error %d", s));
					else
						cwP->coreWorkerState = Message::Busy;
				}
			}
		}
		else if (me->type == Endpoint::CoreWorker)
		{
			if (ep != controller)
				LM_X(1, ("Got a job from != Controller"));

			// Instead of executing a job I'll just sleep 3 secs
			sleep(3);
			iomMsgSend(controller->fd, "Father", me->name.c_str(), Message::JobDone, Message::Evt);
		}
		else
			LM_X(1, ("got a Job message - I'm a '%s'", endpointTypeName(me->type)));
		break;

	case Message::JobDone:
		if (me->type != Endpoint::Worker)
			LM_X(1, ("got a JobDone message - I'm a '%s'", endpointTypeName(me->type)));
		if (ep->type != Endpoint::CoreWorker)
			LM_X(1, ("got a JobDone message from a '%s' endpoint!", endpointTypeName(ep->type)));

		ep->coreWorkerState  = Message::NotBusy;
		ep->jobsDone        += 1;
		break;

	case Message::Alarm:
		if (me->type == Endpoint::Worker)
		{
			if (ep->type != Endpoint::CoreWorker)
				LM_E(("Got an alarm event from %s endpoint '%s' - not supposed to happen!", endpointTypeName(ep->type), ep->name.c_str()));
			else
			{
				// Forward Alarm to controller
				iomMsgSend(controller->fd, controller->name.c_str(), me->name.c_str(), Message::Alarm, Message::Evt, dataP, dataLen);
			}
		}
		else if (me->type == Endpoint::Controller)
		{
			Alarm::AlarmData* alarmP = (Alarm::AlarmData*) dataP;

			LM_F(("Alarm from '%s': '%s'", ep->name.c_str(), alarmP->message));
			alarmSave(ep, alarmP);
		}
		else
			LM_X(1, ("Got an alarm event from %s endpoint '%s' - not supposed to happen!", endpointTypeName(ep->type), ep->name.c_str()));
		break;

	default:
		if (receiver == NULL)
			LM_X(1, ("no packet receiver and unknown message type: %d", msgType));

		LM_T(LMT_MSG, ("forwarding '%s' %s from %s to CoreWorkers", messageCode(msgCode), messageType(msgType), ep->name.c_str()));
		receiver->receive(endpointId, msgCode, dataP, dataLen, &packet);
		break;
	}

	if (dataP != data)
		free(dataP);
}



/* ****************************************************************************
*
* workerStatusToController - 
*/
void Network::workerStatusToController(void)
{
	Message::WorkerStatusData ws;
	int                       ix;

	workerStatus(&ws, this);

#ifdef DEBUG
	LM_T(LMT_STAT, ("CPU Load: %d%%  (%d cores)", ws.cpuInfo.load, ws.cpuInfo.cores));
	for (ix = 0; ix < ws.cpuInfo.cores; ix++)
		LM_T(LMT_STAT, ("Core %02d load: %d%%  (MHz: %d, bogomips: %d, cache size: %d)",
						ix,
						ws.cpuInfo.coreInfo[ix].load,
						ws.cpuInfo.coreInfo[ix].mhz,
						ws.cpuInfo.coreInfo[ix].bogomips,
						ws.cpuInfo.coreInfo[ix].cacheSize));

	for (ix = 0; ix < ws.netInfo.ifaces; ix++)
		LM_T(LMT_STAT, ("Net I/F %02d: %-10s Received: %12u / %8u,  Sent: %12u / %8u)",
						ix,
						ws.netInfo.iface[ix].name,
						ws.netInfo.iface[ix].rcvBytes,
						ws.netInfo.iface[ix].rcvPackets,
						ws.netInfo.iface[ix].sndBytes,
						ws.netInfo.iface[ix].sndPackets));
					
	for (ix = 0; ix < ws.netInfo.ifaces; ix++)
	{
		if (ws.netInfo.iface[ix].sndSpeed > 1024 * 1024)
			LM_T(LMT_STAT, ("Net I/F %02d %-10s: sending at %.2f Mbytes/s", ix,
							ws.netInfo.iface[ix].name,
							ws.netInfo.iface[ix].sndSpeed / 1024.0 / 1024.0));
		else if (ws.netInfo.iface[ix].sndSpeed > 1024)
			LM_T(LMT_STAT, ("Net I/F %02d %-10s: sending at %.2f kbytes/s", ix,
							ws.netInfo.iface[ix].name,
							ws.netInfo.iface[ix].sndSpeed / 1024.0));
		else
			LM_T(LMT_STAT, ("Net I/F %02d %-10s: sending at %.2f bytes/s", ix,
							ws.netInfo.iface[ix].name,
							ws.netInfo.iface[ix].sndSpeed));

		if (ws.netInfo.iface[ix].rcvSpeed > 1024 * 1024)
			LM_T(LMT_STAT, ("Net I/F %02d %-10s: receiving at %.2f Mbytes/s", ix,
							ws.netInfo.iface[ix].name,
							ws.netInfo.iface[ix].rcvSpeed / 1024.0 / 1024.0));
		else if (ws.netInfo.iface[ix].rcvSpeed > 1024)
			LM_T(LMT_STAT, ("Net I/F %02d %-10s: receiving at %.2f kbytes/s", ix,
							ws.netInfo.iface[ix].name,
							ws.netInfo.iface[ix].rcvSpeed / 1024.0));
		else
			LM_T(LMT_STAT, ("Net I/F %02d %-10s: receiving at %.2f bytes/s", ix,
							ws.netInfo.iface[ix].name,
							ws.netInfo.iface[ix].rcvSpeed));
	}

	LM_T(LMT_STAT, ("I have %d core workers", ws.coreWorkerInfo.workers));
	for (ix = 0; ix < ws.coreWorkerInfo.workers; ix++)
	{
		LM_T(LMT_STAT, ("Core %02d: %-20s %-20s  uptime:%-08d  jobsDone:%-05d  restarts:%-05d",
						ws.coreWorkerInfo.worker[ix].coreNo,
						ws.coreWorkerInfo.worker[ix].name,
						coreWorkerState(ws.coreWorkerInfo.worker[ix].state),
						ws.coreWorkerInfo.worker[ix].uptime,
						ws.coreWorkerInfo.worker[ix].jobsDone,
						ws.coreWorkerInfo.worker[ix].restarts));
	}

	LM_T(LMT_STAT, ("I have %d cores - sending %d bytes of workerStatus data to controller", ws.cpuInfo.cores, sizeof(ws)));
#endif

	iomMsgSend(controller->fd, controller->name.c_str(), me->name.c_str(), Message::WorkerStatus, Message::Msg, &ws, sizeof(ws));
}



/* ****************************************************************************
*
* coreWorkerRestart - 
*/
void Network::coreWorkerRestart(void)
{
	unsigned int epIx;

	for (epIx = 3 + Workers; epIx < sizeof(endpoint) / sizeof(endpoint[0]); epIx++)
	{
		if (endpoint[epIx] == NULL)
			continue;

		if (endpoint[epIx]->type != Endpoint::CoreWorker)
			continue;

		LM_T(LMT_COREWORKER, ("CoreWorker '%s' in state '%s'", endpoint[epIx]->name.c_str(), endpoint[epIx]->stateName()));
		if (endpoint[epIx]->state != Endpoint::Dead)
			continue;

		LM_T(LMT_RESTART, ("Trying to restart CoreWorker %d", endpoint[epIx]->coreNo));
		coreWorkerStart(endpoint[epIx]->coreNo, progName, me->port);
	}
}


#define PeriodForSendingWorkerStatusToController  10
#define PeriodForRestartingDeadCoreWorkers        60
/* ****************************************************************************
*
* run - 
*/
void Network::run()
{
	int             fds;
	fd_set          rFds;
	struct timeval  timeVal;
	time_t          now   = 0;
	time_t          then  = time(NULL);
	time_t          then2 = time(NULL);
	int             max;

	while (1)
	{
		int ix;

		do
		{
			if (me->type == Endpoint::Worker)
			{
				now = time(NULL);
				if (now - then > PeriodForSendingWorkerStatusToController)
				{
					workerStatusToController();
					then = now;
				}

				if (now - then2 > PeriodForRestartingDeadCoreWorkers)
				{
					LM_T(LMT_COREWORKER, ("calling coreWorkerRestart"));
					coreWorkerRestart();
					then2 = now;
				}
				else
					LM_T(LMT_COREWORKER, ("Not calling coreWorkerRestart (%d < 30)", now - then2));
			}


			timeVal.tv_sec  = 2;
			timeVal.tv_usec = 0;

			FD_ZERO(&rFds);
			max = 0;

			if (me->type == Endpoint::Delilah)  /* reconnect to dead workers */
			{
				for (ix = 3; ix < 3 + Workers; ix++)
				{
					int workerFd;

					if (endpoint[ix] == NULL)
						continue;

					if (endpoint[ix]->type != Endpoint::Worker)
						continue;

					if (endpoint[ix]->state == Endpoint::Closed)
					{
						LM_T(LMT_RECONNECT, ("delilah reconnecting to %s:%d (type: '%s', state: '%s')",
											 endpoint[ix]->ip.c_str(), endpoint[ix]->port, endpointTypeName(endpoint[ix]->type), endpoint[ix]->stateName()));
						workerFd = iomConnect(endpoint[ix]->ip.c_str(), endpoint[ix]->port);
						if (workerFd != -1)
						{
							endpointAdd(workerFd, (char*) "Reconnecting worker", 0, Endpoint::Temporal, endpoint[ix]->ip.c_str(), endpoint[ix]->port);
							endpoint[ix]->state = Endpoint::Reconnecting;
						}
					}
					else if (endpoint[ix]->state == Endpoint::Disconnected)
					{
						LM_T(LMT_ENDPOINT, ("Connect to %s:%d ?", endpoint[ix]->ip.c_str(), endpoint[ix]->port));

						workerFd = iomConnect(endpoint[ix]->ip.c_str(), endpoint[ix]->port);
						if (workerFd != -1)
						{
							Endpoint* ep;

							ep = endpointAdd(workerFd, (char*) "New Worker", 0, Endpoint::Temporal, endpoint[ix]->ip.c_str(), endpoint[ix]->port);
							if (ep != NULL)
								LM_T(LMT_ENDPOINT, ("Added ep with state '%s'", ep->stateName()));
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
			LM_T(LMT_SELECT, ("------------------------------------------------------------------------"));
			for (ix = 0; ix < (int) (sizeof(endpoint) / sizeof(endpoint[0])); ix++)
			{
				if (endpoint[ix] == NULL)
					continue;

				if ((endpoint[ix]->state == Endpoint::Connected || endpoint[ix]->state == Endpoint::Listening) && (endpoint[ix]->fd >= 0))
				{
					FD_SET(endpoint[ix]->fd, &rFds);
					max = MAX(max, endpoint[ix]->fd);
					
					LM_T(LMT_SELECT, ("+ endpoint %02d %-12s %-28s %20s:%05d %20s (fd: %d)",
									  ix,
									  endpointTypeName(endpoint[ix]->type),
									  endpoint[ix]->name.c_str(),
									  endpoint[ix]->ip.c_str(),
									  endpoint[ix]->port,
									  endpoint[ix]->stateName(),
									  endpoint[ix]->fd));
				}
				else
				{
					LM_T(LMT_SELECT, ("- endpoint %02d %-12s %-28s %20s:%05d %20s (fd: %d)",
									  ix,
									  endpointTypeName(endpoint[ix]->type),
									  endpoint[ix]->name.c_str(),
									  endpoint[ix]->ip.c_str(),
									  endpoint[ix]->port,
									  endpoint[ix]->stateName(),
									  endpoint[ix]->fd));
				}
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
			LM_D(("Timeout in network event loop"));
		}
		else
		{
			if (listener && (listener->state == Endpoint::Listening) && FD_ISSET(listener->fd, &rFds))
			{
				int        fd;
				char       hostName[128];

				LM_T(LMT_SELECT, ("incoming message from my listener - I will accept ..."));
				fd = iomAccept(listener, hostName, sizeof(hostName));
				if (fd == -1)
					LM_P(("iomAccept(%d)", listener->fd));
				else
				{
					std::string  s   = std::string("tmp:") + std::string(hostName);
					Endpoint*    ep  = endpointAdd(fd, (char*) s.c_str(), 0, Endpoint::Temporal, (char*) "ip", 0);

					helloSend(ep, Message::Msg);
				}
			}
			else
			{
				unsigned int ix;

				// Treat endpoint for endpoint vector - skipping the first three ... (me, listener, and controller)
				// For now, only temporal endpoints are in endpoint vector
				for (ix = 0; ix < sizeof(endpoint) / sizeof(endpoint[0]) ; ix++)
				{
					if ((endpoint[ix] == NULL) || (endpoint[ix]->fd < 0))
						continue;

					if (FD_ISSET(endpoint[ix]->fd, &rFds))
					{
						LM_T(LMT_SELECT, ("incoming message from '%s' endpoint %s", endpointTypeName(endpoint[ix]->type), endpoint[ix]->name.c_str()));
						msgTreat(endpoint[ix]->fd, (char*) endpoint[ix]->name.c_str());
						// FD_CLR(endpoint[ix]->fd, &rFds);  endpoint[ix] might have been set to NULL in msgTreat ...
					}
				}
			}
		}
	}
}



/* ****************************************************************************
*
* checkInitDone - 
*/
void Network::checkInitDone(void)
{
	int ix;

	for (ix = 3; ix < 3 + Workers; ix++)
	{
		if (endpoint[ix] == NULL)
		{
			iAmReady = false;
			return;
		}

		if (endpoint[ix]->state != Endpoint::Connected)
		{
			iAmReady = false;
			return;
		}
	}

	iAmReady = true;
}



/* ****************************************************************************
*
* quit - 
*/
void Network::quit()
{
}	

}
