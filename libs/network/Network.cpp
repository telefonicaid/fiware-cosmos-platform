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
#include <pthread.h>            // pthread_t

#include "logMsg.h"             // LM_*
#include "networkTraceLevels.h" // LMT_*
#include "Alarm.h"              // ALARM

#include "Misc.h"               // ipGet 
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


#define THREAD_BUF_SIZE_THRESHOLD (20 * 1024)
namespace ss
{



/* ****************************************************************************
*
* Constructor 
*/
Network::Network()
{
	iAmReady   = false;
	receiver   = NULL;
	me         = NULL;
	listener   = NULL;
	controller = NULL;
	Workers    = WORKERS;
	Endpoints  = 3 + WORKERS + DELILAHS + CORE_WORKERS + TEMPORALS;

	LM_M(("Allocating room for %d endpoint pointers", Endpoints));

	endpoint = (Endpoint**) calloc(Endpoints, sizeof(Endpoint*));
	if (endpoint == NULL)
		LM_XP(1, ("calloc(%d, %d)", Endpoints, sizeof(Endpoint*)));
}



/* ****************************************************************************
*
* Constructor
*/
Network::Network(int endpoints, int workers)
{
	iAmReady   = false;
	receiver   = NULL;
	me         = NULL;
	listener   = NULL;
	controller = NULL;
	Workers    = workers;
	Endpoints  = endpoints;

	LM_M(("Allocating room for %d endpoint pointers", Endpoints));

	endpoint = (Endpoint**) calloc(Endpoints, sizeof(Endpoint*));
	if (endpoint == NULL)
		LM_XP(1, ("calloc(%d, %d)", Endpoints, sizeof(Endpoint*)));

	LM_M(("Endpoints: %d", Endpoints));
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
* init - create endpoints 0 and 1
*/
void Network::init(Endpoint::Type type, const char* alias, unsigned short port, const char* controllerName)
{
	endpoint[0] = new Endpoint(type, port);
	if (endpoint[0] == NULL)
		LM_XP(1, ("new Endpoint"));
	me = endpoint[0];

	if (me == NULL)
		LM_XP(1, ("unable to allocate room for Endpoint 'me'"));

	me->name     = progName;
	me->state    = Endpoint::Me;
	me->ip       = ipGet();
	me->alias    = alias;

	if (strncmp(&alias[1], "orker", 5) == 0)
		me->workerId = atoi(&alias[6]);
	else
		me->workerId = -2;

	if (port != 0)
	{
		endpoint[1] = new Endpoint(*me);
		if (endpoint[1] == NULL)
			LM_XP(1, ("new Endpoint"));
		listener = endpoint[1];

		listener->rFd      = iomServerOpen(listener->port);
		if (listener->rFd == -1)
			LM_XP(1, ("unable to open port %d for listening", listener->port));

		listener->state    = Endpoint::Listening;
		listener->type     = Endpoint::Listener;
		listener->ip       = me->ip;
		listener->name     = "Listener";
		listener->wFd      = listener->rFd;

		LM_T(LMT_FDS, ("opened fd %d to accept incoming connections", listener->rFd));
	}

	if ((type == Endpoint::Worker) || (type == Endpoint::Delilah))
	{
		endpoint[2] = new Endpoint(Endpoint::Controller, controllerName);
		if (endpoint[2] == NULL)
			LM_XP(1, ("new Endpoint"));
		controller = endpoint[2];

		controller->rFd = iomConnect((const char*) controller->ip.c_str(), (unsigned short) controller->port);
		if (controller->rFd == -1)
			LM_X(1, ("error connecting to controller at %s:%d", controller->ip.c_str(), controller->port));

		controller->wFd   = controller->rFd;
		controller->state = ss::Endpoint::Connected;
	}
	else
	{
		endpoint[2] = NULL;
		controller  = NULL;
	}

	LM_F(("I am a '%s', my name: '%s', ip: %s", me->typeName(), me->nam(), me->ip.c_str()));
}



/* ****************************************************************************
*
* helloSend
*/
int Network::helloSend(Endpoint* ep, Message::MessageType type)
{
	ss::Message::HelloData hello;

	strncpy(hello.name,   me->name.c_str(),   sizeof(hello.name));
	strncpy(hello.ip,     me->ip.c_str(),     sizeof(hello.ip));
	strncpy(hello.alias,  me->alias.c_str(),  sizeof(hello.alias));

	hello.type    = me->type;
	hello.workers = me->workers;
	hello.port    = me->port;
	hello.coreNo  = me->coreNo;

	LM_T(LMT_WRITE, ("sending hello %s to '%s' (name: '%s', type: '%s')", messageType(type), ep->name.c_str(), hello.name, me->typeName()));

	return iomMsgSend(ep->rFd, ep->name.c_str(), me->name.c_str(), Message::Hello, type, &hello, sizeof(hello));
}



#define WEB_SERVICE_PORT (unsigned short) 9898
/* ****************************************************************************
*
* initAsSamsonController - 
*/
void Network::initAsSamsonController(int port, int workers)
{
	init(Endpoint::Controller, "Controller", port);


	int   ix;
	char  alias[16];
	char  name[32];

	Workers = workers;
	LM_M(("%d workers", Workers));
	for (ix = 0; ix < Workers; ix++)
	{
		snprintf(alias, sizeof(alias), "Worker%02d",  ix);
		snprintf(name,  sizeof(name),  "Worker %02d", ix);

		endpoint[3 + ix] = new Endpoint();
		if (endpoint[3 + ix] == NULL)
			LM_XP(1, ("new Endpoint"));

		endpoint[3 + ix]->rFd     = -1;
		endpoint[3 + ix]->wFd     = -1;
		endpoint[3 + ix]->name    = name;
		endpoint[3 + ix]->alias   = alias;
		endpoint[3 + ix]->workers = 0;
		endpoint[3 + ix]->state   = Endpoint::FutureWorker;
		endpoint[3 + ix]->type    = Endpoint::Worker;
		endpoint[3 + ix]->ip      = "II.PP";
		endpoint[3 + ix]->port    = 0;
		endpoint[3 + ix]->coreNo  = -1;

		LM_M(("Created endpoint %d, worker %d (%s)", 3 + ix, ix, endpoint[3 + ix]->alias.c_str()));
	}


	int fd = iomServerOpen(WEB_SERVICE_PORT);
	if (fd == -1)
		LM_XP(1, ("error opening web service listen socket"));

	endpointAdd(fd, fd, "Web Listener", "Weblistener", 0, Endpoint::WebListener, "localhost", WEB_SERVICE_PORT, 0);
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
	if (identifier >= Workers)
		LM_RE(-1, ("invalid worker identifier '%d'  (only have %d workers)", identifier, Workers));

	if (identifier == 0)
		return me->workerId;

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
size_t Network::send(PacketSenderInterface* sender, int endpointId, ss::Message::MessageCode code, Packet* packetP)
{
	Endpoint* ep        = endpoint[endpointId];

	if (ep == NULL)
		LM_RE(-1, ("No endpoint at index %d", endpointId));
	if (ep->state != Endpoint::Connected)
		LM_RE(-1, ("Endpoint %d in state '%s'", endpointId, ep->stateName()));

	LM_T(LMT_DELILAH, ("sending a '%s' message to endpoint %d", messageCode(code), endpointId));

	int nb = iomMsgSend(ep->wFd, ep->name.c_str(), me->name.c_str(), code, Message::Msg, NULL, 0, packetP);

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
Endpoint* Network::endpointAdd
(
	int              rFd,
	int              wFd,
	const char*      name,
	const char*      alias,
	int              workers,
	Endpoint::Type   type,
	std::string      ip,
	unsigned short   port,
	int              coreNo = -1
)
{
	int ix;

	LM_T(LMT_ENDPOINT, ("Adding endpoint '%s' of type '%s' for fd %d", name, me->typeName(type), rFd));

	switch (type)
	{
	case Endpoint::CoreWorker:
	case Endpoint::Unknown:
	case Endpoint::Listener:
		LM_X(1, ("bad type: %d", type));
		return NULL;

	case Endpoint::Controller:
		endpoint[2]->name  = std::string(name);
		endpoint[2]->ip    = std::string(ip);
		endpoint[2]->alias = (alias != NULL)? alias : "NO ALIAS" ;
		return controller;

	case Endpoint::Temporal:
		LM_M(("Endpoints: %d", Endpoints));
		LM_M(("Workers:   %d", Workers));
		for (ix = Endpoints - 1; ix >= 3 + Workers; ix--)
		{
			if (endpoint[ix] == NULL)
			{
				endpoint[ix] = new Endpoint();
				if (endpoint[ix] == NULL)
					LM_XP(1, ("allocating temporal Endpoint"));

				endpoint[ix]->name   = std::string(name);
				endpoint[ix]->rFd    = rFd;
				endpoint[ix]->wFd    = wFd;
				endpoint[ix]->type   = Endpoint::Temporal;
				endpoint[ix]->ip     = ip;
				endpoint[ix]->alias  = (alias != NULL)? alias : "NO ALIAS" ;
				endpoint[ix]->state  = (rFd > 0)? Endpoint::Connected : Endpoint::Unconnected;

				return endpoint[ix];
			}
		}

		if (endpoint[ix] == NULL)
			LM_X(1, ("No temporal endpoint slots available - redefine and recompile!"));
		break;

	case Endpoint::Delilah:
	case Endpoint::WebListener:
	case Endpoint::WebWorker:
		for (ix = 3 + Workers; ix < (int) (Endpoints - 1); ix++)
		{
			if (endpoint[ix] == NULL)
			{
				endpoint[ix] = new Endpoint();
				if (endpoint[ix] == NULL)
					LM_XP(1, ("allocating Endpoint"));

				endpoint[ix]->name       = std::string(name);
				endpoint[ix]->alias      = (alias != NULL)? alias : "NO ALIAS" ;
				endpoint[ix]->rFd        = rFd;
				endpoint[ix]->wFd        = wFd;
				endpoint[ix]->state      = (rFd > 0)? Endpoint::Connected : Endpoint::Unconnected;   /* XXX */
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

			controller->rFd    = rFd;
			controller->wFd    = wFd;
			controller->name   = std::string(name);
			controller->alias  = (alias != NULL)? alias : "NO ALIAS" ;
			
			return controller;
		}

		for (ix = 3; ix < 3 + Workers; ix++)
		{
			if (endpoint[ix] == NULL)
				LM_X(1, ("NULL worker endpoint at slot %d", ix));

			if (endpointLookup((char*) alias) != NULL)
			{
				LM_E(("Intent to connect a second Worker with alias '%s' - rejecting connection", alias));
				iomMsgSend(wFd, name, progName, Message::Die, Message::Evt);
				close(rFd);
				if (wFd != rFd)
					close(wFd);
				return NULL;
			}

			LM_M(("comparing '%s' to '%s'", endpoint[ix]->alias.c_str(), alias));
			if (strcmp(endpoint[ix]->alias.c_str(), alias) == 0)
			{
				LM_M(("Got endpoint '%s'", alias));
				endpoint[ix]->rFd      = rFd;
				endpoint[ix]->wFd      = wFd;
				endpoint[ix]->name     = std::string(name);
				endpoint[ix]->alias    = std::string(alias);
				endpoint[ix]->workers  = workers;
				endpoint[ix]->type     = Endpoint::Worker;
				endpoint[ix]->ip       = ip;
				endpoint[ix]->port     = port;
				endpoint[ix]->state    = (rFd > 0)? Endpoint::Connected : Endpoint::Unconnected;   /* XXX */

				return endpoint[ix];
			}
		}

		LM_E(("Worker '%s:%d' not found!", ip.c_str(), port));

		LM_E(("alias '%s' - rejecting connection", alias));
		iomMsgSend(wFd, name, progName, Message::Die, Message::Evt);

		return NULL;
	}

	LM_X(1, ("BUG"));
	return NULL;
}



/* ****************************************************************************
*
* endpointRemove
*/
void Network::endpointRemove(Endpoint* ep)
{
	int ix;

	LM_T(LMT_EP, ("Removing '%s' endpoint '%s'", ep->typeName(), ep->name.c_str()));

	for (ix = 0; ix < Endpoints; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix] == ep)
		{
			if (ep->type == Endpoint::Worker)
			{
				ep->rFd   = -1;
				ep->wFd   = -1;
				ep->state = Endpoint::Disconnected;
				ep->name  = std::string("To be a worker");
			}
			else if (ep->type == Endpoint::Controller)
			{
				LM_W(("NOT removing Controller"));
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

	LM_E(("endpoint (rFd:%d) not found", rFd));
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

		LM_M(("comparing ep %02d '%s' to '%s' (state: '%s')", ix, endpoint[ix]->alias.c_str(), alias, endpoint[ix]->stateName()));
		if ((strcmp(endpoint[ix]->alias.c_str(), alias) == 0) && (endpoint[ix]->state == Endpoint::Connected))
		{
			LM_M(("found occupied (state: '%s') endpoint with alias '%s'", endpoint[ix]->stateName(), alias));
			return endpoint[ix];
		}
	}

	LM_E(("endpoint (alias:%s) not found", alias));
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
} MsgTreatParams;



/* ****************************************************************************
*
* msgTreatThreadFunction - 
*/
static void* msgTreatThreadFunction(void* vP)
{
	LM_M(("IN: vP at %p", vP));

	MsgTreatParams*  paramP = (MsgTreatParams*) vP;
	Endpoint*        ep     = paramP->ep;

	LM_M(("calling msgTreat function in thread (ep at %p, vP at %p)", paramP->ep, paramP));
	paramP->diss->msgTreat(paramP);

	ep->state = paramP->state;

	LM_E(("back after msgTreat set state for '%s' to %d", ep->name.c_str(), ep->state));

	free(vP);
	return NULL;
}



/* ****************************************************************************
*
* msgPreTreat - 
*/
void Network::msgPreTreat(Endpoint* ep, int endpointId)
{
	Message::Header header;
	int             nb;

	//
	// Special case - incoming connection on WebListener interface
	//
	if (ep->type == Endpoint::WebListener)
	{
		int   fd;
		char  hostName[128];

		fd = iomAccept(ep->rFd, hostName, sizeof(hostName));
		if (fd == -1)
			LM_P(("iomAccept(%d)", ep->rFd));
		else
			endpointAdd(fd, fd, "Web Worker", "Webworker", 0, Endpoint::WebWorker, hostName, 0);

		return;
	}



	//
	// Special case - controller reads from Web Service connection
	//
	if (ep->type == Endpoint::WebWorker)
	{
		char buf[1024];
		int  nb;

		if (me->type != Endpoint::Controller)
			LM_X(1, ("Got a request from a WebWorker and I'm not a controller !"));

		nb = read(ep->rFd, buf, sizeof(buf));
		if (nb == -1)
			LM_E(("error reading web service request: %s", strerror(errno)));
		else if (nb == 0)
			LM_E(("read ZERO bytes of web service request"));
		else
		{
			std::string command     = receiver->getJSONStatus(std::string(buf));
			int         commandLen  = command.size();

			write(ep->wFd, command.c_str(), commandLen);
		}

		close(ep->wFd);
		endpointRemove(ep);
		return;
	}



	//
	// Reading header of the message
	//
	nb = read(ep->rFd, &header, sizeof(header));
	LM_M(("nb == %d", nb));
	if (nb == 0) /* Connection closed */
	{
		LM_T(LMT_SELECT, ("Connection closed - ep at %p", ep));
		if (ep == controller)
		{
			if (me->type == Endpoint::CoreWorker)
				LM_X(1, ("My Father (samsonWorker) died, I cannot exist without father process"));

			LM_W(("controller died ... trying to reconnect !"));

			controller->rFd    = -1;
			controller->state = ss::Endpoint::Disconnected;

			while (controller->rFd == -1)
			{
				controller->rFd = iomConnect((const char*) controller->ip.c_str(), (unsigned short) controller->port);
				sleep(1); // sleep one second before reintenting connection to coltroller
			}

			controller->state = ss::Endpoint::Connected;
			controller->wFd   = controller->rFd;
		}
		else if (ep != NULL)
		{
			if (ep->type == Endpoint::Worker)
			{
				--me->workers;

				close(ep->rFd);
				if (ep->wFd == ep->rFd)
					close(ep->wFd);

				ep->state = ss::Endpoint::Closed;
				ep->rFd   = -1;
				ep->wFd   = -1;
				ep->name  = "-----";
			}
			else if (ep->type == Endpoint::CoreWorker)
				LM_X(1, ("should get no messages from core worker ..."));
			else
				endpointRemove(ep);
		}

		return;
	}
	else if (nb == -1)
		LM_RVE(("iomMsgRead: error reading message from '%s': %s", ep->name.c_str(), strerror(errno)));
	else if (nb != sizeof(header))
		LM_RVE(("iomMsgRead: error reading header from '%s' (read %d, wanted %d bytes", ep->name.c_str(), nb, sizeof(header)));

	LM_M(("Read header of '%s' message with dataLens %d, %d, %d", messageCode(header.code), header.dataLen, header.gbufLen, header.kvDataLen));

	//
	// calling msgTreat ...
	//
	if (header.dataLen + header.gbufLen + header.kvDataLen > THREAD_BUF_SIZE_THRESHOLD)
	{
		pthread_t        tid;
		MsgTreatParams*  paramsP = (MsgTreatParams*) malloc(sizeof(MsgTreatParams));

		paramsP->diss        = this;
		paramsP->ep          = ep;
		paramsP->endpointId  = endpointId;
		paramsP->header      = header;
		paramsP->state       = ep->state;

		ep->state = Endpoint::Threaded;
		LM_M(("setting state of '%s' to Threaded (%d)  old state: %d", ep->name.c_str(), ep->state, paramsP->state));

		LM_M(("calling msgTreatThreadFunction via pthread_create (params at %p) (dataLens: %d, %d, %d)", paramsP, header.dataLen, header.gbufLen, header.kvDataLen));
		pthread_create(&tid, NULL, msgTreatThreadFunction, (void*) paramsP);
		LM_M(("after pthread_create of msgTreatThreadFunction"));
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
* msgTreat - 
*/
void Network::msgTreat(void* vP)
{
	MsgTreatParams*       paramsP      = (MsgTreatParams*) vP;
	Endpoint*             ep           = paramsP->ep;
	int                   endpointId   = paramsP->endpointId;
	Message::Header*      headerP      = &paramsP->header;
	Packet                packet;
	Packet                ack;
	Message::MessageCode  msgCode;
	Message::MessageType  msgType;
	int                   s;
	char                  data[1024];
	void*                 dataP      = data;
	int                   dataLen    = sizeof(data);
	int                   wFd        = ep->rFd;     // This function will not work for pipe pairs ...
	char*                 name       = (char*) ep->name.c_str();

	LM_T(LMT_READ, ("treating incoming message from '%s' (ep at %p) (dataLens: %d, %d, %d)", name, ep, headerP->dataLen, headerP->gbufLen, headerP->kvDataLen));
	s = iomMsgRead2(ep->rFd, headerP, name, &msgCode, &msgType, &dataP, &dataLen, &packet, NULL, 0);
	LM_T(LMT_READ, ("iomMsgRead returned %d (dataLens: %d, %d, %d)", s, headerP->dataLen, headerP->gbufLen, headerP->kvDataLen));

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

				controller->rFd    = -1;
				controller->state = ss::Endpoint::Disconnected;

				while (controller->rFd == -1)
				{
					controller->rFd = iomConnect((const char*) controller->ip.c_str(), (unsigned short) controller->port);
					sleep(1); // sleep one second before reintenting connection to coltroller
				}

				controller->state = ss::Endpoint::Connected;
				controller->wFd   = controller->rFd;
				return;
			}
			else if (ep != NULL)
			{
				if (ep->type == Endpoint::Worker)
				{
					--me->workers;

					close(ep->rFd);
					if (ep->wFd == ep->rFd)
						close(ep->wFd);

					ep->state = ss::Endpoint::Closed;
					ep->rFd   = -1;
					ep->wFd   = -1;
					ep->name  = "-----";
				}
				else if (ep->type == Endpoint::CoreWorker)
					LM_X(1, ("should get no messages from core worker ..."));
				else
					endpointRemove(ep);
			}
		}

		LM_RVE(("iomMsgRead: error reading message from '%s'", name));
	}


	LM_T(LMT_TREAT, ("Treating %s %s from %s", messageCode(msgCode), messageType(msgType), name));
	switch (msgCode)
	{
	case Message::Die:
		LM_X(1, ("Got a DIE message from '%s' - I die", name));
		break;

	case Message::Hello:
		Endpoint*            helloEp;
		Message::HelloData*  hello;

		hello   = (Message::HelloData*) dataP;
		helloEp = endpointAdd(ep->rFd, ep->wFd, hello->name, hello->alias, hello->workers, hello->type, hello->ip, hello->port, hello->coreNo);

		if (helloEp == NULL)
		{
			endpointRemove(ep);
			return;
		}

		LM_T(LMT_HELLO, ("Got Hello %s from %s, type %s, %s:%d, workers: %d",
						 messageType(msgType), helloEp->name.c_str(), helloEp->typeName(), helloEp->ip.c_str(), helloEp->port, helloEp->workers));

		if (ep && ep->type == Endpoint::Temporal)
			endpointRemove(ep);

		if (msgType == Message::Msg)
			helloSend(helloEp, Message::Ack);

		if ((ep != NULL) && (ep == controller))
		{
			if ((me->type != Endpoint::CoreWorker) && (me->type != Endpoint::Controller))
			{
				LM_M(("Asking controller for list of workers"));
				iomMsgSend(controller->wFd, (char*) controller->name.c_str(), (char*) me->name.c_str(),
						   Message::WorkerVector, Message::Msg, NULL, 0, NULL );
			}
		}
		break;

	case Message::WorkerVector:
		if ((msgType == Message::Msg) && (me->type != Endpoint::Controller))
			LM_X(1, ("Got a WorkerVector request from '%s' but I'm not the controller ...", name));

		if ((me->type == Endpoint::Controller) && (msgType == Message::Msg))
		{
			Message::Worker* workerV;
			int       ix;

			LM_M(("allocating room for a worker vector of %d workers", Workers));
			workerV = (Message::Worker*) calloc(Workers, sizeof(Message::Worker));
			if (workerV == NULL)
				LM_XP(1, ("calloc(%d, %d)", Workers, sizeof(Endpoint)));

			LM_M(("filling worker vector of %d workers", Workers));
			for (ix = 3; ix < 3 + Workers; ix++)
			{
				if (endpoint[ix] != NULL)
				{
					LM_M(("Adding worker %d to ep-vector (alias: '%s', ip: '%s', port: %d)",
						  ix - 3,
						  endpoint[ix]->alias.c_str(),
						  endpoint[ix]->ip.c_str(),
						  endpoint[ix]->port));

					strncpy(workerV[ix - 3].name,  endpoint[ix]->name.c_str(),  sizeof(workerV[ix - 3].name));
					strncpy(workerV[ix - 3].alias, endpoint[ix]->alias.c_str(), sizeof(workerV[ix - 3].alias));
					strncpy(workerV[ix - 3].ip,    endpoint[ix]->ip.c_str(),    sizeof(workerV[ix - 3].ip));

					workerV[ix - 3].port   = endpoint[ix]->port;
					workerV[ix - 3].state  = endpoint[ix]->state;
				}
				else
					LM_M(("worker %d empty", ix - 3));
			}

			LM_T(LMT_WRITE, ("sending ack with entire worker vector to '%s'", name));
			iomMsgSend(wFd, name, (char*) me->name.c_str(), Message::WorkerVector, Message::Ack, workerV, Workers * sizeof(Message::Worker));
			free(workerV);
		}
		else if (msgType == Message::Ack)
		{
			LM_T(LMT_ENDPOINT, ("Got the worker vector from the Controller - now connect to them all ..."));

			unsigned int        ix;
			Message::Worker*    workerV = (Message::Worker*) dataP;

			LM_M(("Got %d bytes of WorkerVector msg data (%d workers)", dataLen, dataLen / sizeof(Message::Worker)));

			if ((unsigned int) Workers != dataLen / sizeof(Message::Worker))
				LM_X(1, ("Got %d workers from Controller - I thought there were %d workers", dataLen / sizeof(Message::Worker), Workers));

			for (ix = 0; ix < dataLen / sizeof(Message::Worker); ix++)
			{
				Endpoint* epP;

				LM_M(("Filling worker %d", ix));

				if (endpoint[3 + ix] == NULL)
				{
					endpoint[3 + ix] = new Endpoint(Endpoint::Worker, workerV[ix].name, workerV[ix].ip, workerV[ix].port, -1, -1);
					endpoint[3 + ix]->state = Endpoint::Unconnected;
					endpoint[3 + ix]->alias = workerV[ix].alias;
				}

				epP = endpoint[3 + ix];

				if (strcmp(epP->alias.c_str(), me->alias.c_str()) == 0)
				{
					LM_T(LMT_WORKERS, ("NOT connecting to myself ..."));
					epP->name = std::string("me: ") + epP->ip;
					continue;
				}

				if (epP->rFd == -1)
				{
					int workerFd;

					LM_T(LMT_WORKERS, ("Connect to worker %d: %s (host %s, port %d, alias '%s')",
									   ix, epP->name.c_str(), epP->ip.c_str(), epP->port, epP->alias.c_str()));

					if ((workerFd = iomConnect(epP->ip.c_str(), epP->port)) == -1)
					{
						LM_T(LMT_WORKERS, ("worker %d: %s (host %s, port %d) not there - no problem, he'll connect to me",
									   ix, epP->name.c_str(), epP->ip.c_str(), epP->port));
					}
					else
					{
						Endpoint* ep;

						ep = endpointAdd(workerFd, workerFd, (char*) "to be worker", NULL, 0, Endpoint::Temporal, epP->ip, epP->port);
						if (ep != NULL)
						{
							ep->state = Endpoint::Connected;
							LM_T(LMT_ENDPOINT, ("Added ep with state '%s'", ep->stateName()));
						}
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

	case Message::Alarm:
		if (me->type == Endpoint::Worker)
		{
			if (ep->type != Endpoint::CoreWorker)
				LM_E(("Got an alarm event from %s endpoint '%s' - not supposed to happen!", ep->typeName(), ep->name.c_str()));
			else
			{
				// Forward Alarm to controller
				iomMsgSend(controller->wFd, controller->name.c_str(), me->name.c_str(), Message::Alarm, Message::Evt, dataP, dataLen);
			}
		}
		else if (me->type == Endpoint::Controller)
		{
			Alarm::AlarmData* alarmP = (Alarm::AlarmData*) dataP;

			LM_F(("Alarm from '%s': '%s'", ep->name.c_str(), alarmP->message));
			alarmSave(ep, alarmP);
		}
		else
			LM_X(1, ("Got an alarm event from %s endpoint '%s' - not supposed to happen!", ep->typeName(), ep->name.c_str()));
		break;

	default:
		LM_M(("DEFAULT"));
		if (receiver == NULL)
			LM_X(1, ("no packet receiver and unknown message type: %d", msgType));

		LM_M(("forwarding '%s' %s from %s to Endpoint %d", messageCode(msgCode), messageType(msgType), ep->name.c_str(), endpointId));
		receiver->receive(endpointId, msgCode, &packet);
		break;
	}

	if (dataP != data)
		free(dataP);
}



#define PeriodForSendingWorkerStatusToController  10
/* ****************************************************************************
*
* run
*/
void Network::run()
{
	int             fds;
	fd_set          rFds;
	struct timeval  timeVal;
	time_t          now   = 0;
	time_t          then  = time(NULL);
	int             max;

	LM_M(("endpoints: %d", Endpoints));

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
					// workerStatusToController();
					then = now;
				}
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
											 endpoint[ix]->ip.c_str(), endpoint[ix]->port, endpoint[ix]->typeName(), endpoint[ix]->stateName()));
						workerFd = iomConnect(endpoint[ix]->ip.c_str(), endpoint[ix]->port);
						if (workerFd != -1)
						{
							endpointAdd(workerFd, workerFd, (char*) "Reconnecting worker", NULL, 0, Endpoint::Temporal, endpoint[ix]->ip.c_str(), endpoint[ix]->port);
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

							ep = endpointAdd(workerFd, workerFd, (char*) "New Worker", NULL, 0, Endpoint::Temporal, endpoint[ix]->ip.c_str(), endpoint[ix]->port);
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
			LM_T(LMT_SELECT, ("------------ %d endpoints ------------------------------------------------------------", Endpoints));
			for (ix = 0; ix < Endpoints; ix++)
			{
				if (endpoint[ix] == NULL)
					continue;

				if ((endpoint[ix]->state == Endpoint::Connected || endpoint[ix]->state == Endpoint::Listening) && (endpoint[ix]->rFd >= 0))
				{
					FD_SET(endpoint[ix]->rFd, &rFds);
					max = MAX(max, endpoint[ix]->rFd);
					
					LM_T(LMT_SELECT, ("+ %02d: %-12s %-22s %-15s %15s:%05d %18s  fd: %d",
									  ix,
									  endpoint[ix]->typeName(),
									  endpoint[ix]->name.c_str(),
									  endpoint[ix]->alias.c_str(),
									  endpoint[ix]->ip.c_str(),
									  endpoint[ix]->port,
									  endpoint[ix]->stateName(),
									  endpoint[ix]->rFd));
				}
				else
				{
					LM_T(LMT_SELECT, ("- %02d: %-12s %-22s %-15s %15s:%05d %18s  fd: %d",
									  ix,
									  endpoint[ix]->typeName(),
									  endpoint[ix]->name.c_str(),
									  endpoint[ix]->alias.c_str(),
									  endpoint[ix]->ip.c_str(),
									  endpoint[ix]->port,
									  endpoint[ix]->stateName(),
									  endpoint[ix]->rFd));
				}
			}

			fds = select(max + 1, &rFds, NULL, NULL, &timeVal);
		} while ((fds == -1) && (errno == EINTR));

		LM_T(LMT_SELECT, ("select returned %d", fds));
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
			int ix;

			if (listener && (listener->state == Endpoint::Listening) && FD_ISSET(listener->rFd, &rFds))
			{
				int  fd;
				char hostName[128];

				LM_T(LMT_SELECT, ("incoming message from my listener - I will accept ..."));
				--fds;
				fd = iomAccept(listener->rFd, hostName, sizeof(hostName));
				if (fd == -1)
					LM_P(("iomAccept(%d)", listener->rFd));
				else
				{
					std::string  s   = std::string("tmp:") + std::string(hostName);
					Endpoint*    ep  = endpointAdd(fd, fd, (char*) s.c_str(), NULL, 0, Endpoint::Temporal, (char*) "ip", 0);

					helloSend(ep, Message::Msg);
				}
			}

			if (fds > 0)
			{
				// Treat endpoint for endpoint vector - skipping the first two ... (me & listener)
				// For now, only temporal endpoints are in endpoint vector
				LM_T(LMT_SELECT, ("looping from %d to %d", 3, Endpoints));
				for (ix = 2; ix < Endpoints; ix++)
				{
					if ((endpoint[ix] == NULL) || (endpoint[ix]->rFd < 0))
						continue;

					if (FD_ISSET(endpoint[ix]->rFd, &rFds))
					{
						--fds;
						LM_T(LMT_SELECT, ("incoming message from '%s' endpoint %s (fd %d)", endpoint[ix]->typeName(), endpoint[ix]->name.c_str(), endpoint[ix]->rFd));
						msgPreTreat(endpoint[ix], ix);
						if (endpoint[ix])
							FD_CLR(endpoint[ix]->rFd, &rFds);
					}
				}
			}
		}
	}
}



/* ****************************************************************************
*
* quit - 
*/
void Network::quit()
{
}	

}
