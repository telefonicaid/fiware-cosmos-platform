/* ****************************************************************************
*
* FILE                      EndpointMgr.cpp
*
* DESCRIPTION               Class for endpoint manager
*
*/
#include "logMsg.h"             // LM_*
#include "networkTraceLevels.h" // LMT_*
#include "Alarm.h"              // ALARM

#include "Misc.h"               // ipGet
#include "Message.h"            // MessageType
#include "iomServerOpen.h"      // iomServerOpen
#include "iomConnect.h"         // iomConnect
#include "iomAccept.h"          // iomAccept
#include "iomMsgRead.h"         // iomMsgRead
#include "iomMsgSend.h"         // iomMsgSend
#include "Endpoint.h"           // Endpoint
#include "EndpointMgr.h"        // Own interface


namespace ss
{



/* ****************************************************************************
*
* Constructor
*/
EndpointMgr::EndpointMgr(NetworkInterface* network, int endpoints, int workers)
{
	this->network = network;

	iAmReady   = false;
	me         = NULL;
	listener   = NULL;
	controller = NULL;
	Workers    = workers;
	Endpoints  = endpoints;

	endpoint = (Endpoint**) calloc(Endpoints, sizeof(Endpoint*));
	if (endpoint == NULL)
		LM_XP(1, ("calloc(%d, %d)", Endpoints, sizeof(Endpoint*)));
}



/* ****************************************************************************
*
* init - create endpoints 0-2
*/
void EndpointMgr::init(Endpoint::Type type, const char* alias, unsigned short port, const char* controllerName)
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

	if (port != 0)
	{
		endpoint[1] = new Endpoint(*me);
		if (endpoint[1] == NULL)
			LM_XP(1, ("new Endpoint"));
		listener = endpoint[1];

		listener->rFd      = iomServerOpen(listener->port);
		listener->state    = Endpoint::Listening;
		listener->type     = Endpoint::Listener;
		listener->ip       = me->ip;
		listener->name     = "Listener";
		listener->wFd      = listener->rFd;

		if (listener->rFd == -1)
			LM_XP(1, ("unable to open port %d for listening", listener->port));

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
* helloSend - 
*/
int EndpointMgr::helloSend(Endpoint* ep, Message::MessageType type)
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

	return iomMsgSend(ep->wFd, ep->name.c_str(), me->name.c_str(), Message::Hello, type, &hello, sizeof(hello));
}



/* ****************************************************************************
*
* endpointAdd - add an endpoint to the vector - MOVE to EndpointMgr
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
Endpoint* EndpointMgr::endpointAdd
(
	int             rFd,
	int             wFd,
	char*           name,
	char*           alias,
	int             workers,
	Endpoint::Type  type,
	std::string     ip,
	unsigned short  port,
	int             coreNo
)
{
	int ix;

	LM_T(LMT_ENDPOINT, ("Adding endpoint '%s' of type '%s' for fd %d", name, me->typeName(type), rFd));

	switch (type)
	{
	case Endpoint::Fd:
	case Endpoint::Spawner:
	case Endpoint::Supervisor:
	case Endpoint::Sender:
	case Endpoint::Unknown:
	case Endpoint::Listener:
		LM_X(1, ("bad type: %d", type));
		return NULL;

	case Endpoint::Controller:
		endpoint[2]->name   = std::string(name);
		endpoint[2]->ip     = std::string(ip);
		endpoint[2]->alias  = (alias != NULL)? alias : "NO ALIAS" ;
		return controller;

	case Endpoint::Temporal:
		for (ix = Endpoints - 1; ix >= 3 + Workers; ix--)
		{
			if (endpoint[ix] == NULL)
			{
				endpoint[ix] = new Endpoint();
				if (endpoint[ix] == NULL)
					LM_XP(1, ("allocating temporal Endpoint"));

				endpoint[ix]->name  = std::string(name);
				endpoint[ix]->rFd   = rFd;
				endpoint[ix]->wFd   = wFd;
				endpoint[ix]->type  = Endpoint::Temporal;
				endpoint[ix]->ip    = ip;
                endpoint[ix]->alias = (alias != NULL)? alias : "NO ALIAS" ;
                endpoint[ix]->state = (rFd > 0)? Endpoint::Connected : Endpoint::Unconnected;

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
			ep->rFd   = rFd;
			ep->wFd   = wFd;
			ep->name  = std::string(name);

			return ep;
		}
		/* no break here ! */
		/* The first time a CoreWorker Endpoint is created we pass thru */

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
				endpoint[ix]->state      = (wFd > 0)? Endpoint::Connected : Endpoint::Unconnected;
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

			if (endpointLookup(alias) != NULL)
			{
				LM_E(("Intent to connect a second Worker with alias '%s' - rejecting connection", alias));
				iomMsgSend(wFd, name, progName, Message::Die, Message::Evt);
				close(wFd);
				return NULL;
			}

			if (strcmp(endpoint[ix]->alias.c_str(), alias) == 0)
			{
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
* endpointRemove - MOVE to EndpointMgr
*/
void EndpointMgr::endpointRemove(Endpoint* ep)
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
				ep->rFd    = -1;
				ep->wFd    = -1;
				ep->state  = Endpoint::Disconnected;
				ep->name   = std::string("To be a worker");
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
* endpointLookup - MOVE to EndpointMgr
*/
Endpoint* EndpointMgr::endpointLookup(int ix)
{
	return endpoint[ix];
}



/* ****************************************************************************
*
* endpointCoreWorkerLookup - MOVE to EndpointMgr
*/
Endpoint* EndpointMgr::endpointCoreWorkerLookup(int coreNo)
{
	int ix;

	for (ix = 3 + Workers; ix < Endpoints; ix++)
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
* endpointLookup - MOVE to EndpointMgr
*/
Endpoint* EndpointMgr::endpointLookup(char* alias)
{
	int ix = 0;

	for (ix = 0; ix < Endpoints; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if ((strcmp(endpoint[ix]->alias.c_str(), alias) == 0) && (endpoint[ix]->state == Endpoint::Connected))
		{
			return endpoint[ix];
		}
	}

	LM_E(("endpoint (alias:%s) not found", alias));
	return NULL;
}



/* ****************************************************************************
*
* endpointLookup - MOVE to EndpointMgr
*/
Endpoint* EndpointMgr::endpointLookup(int rFd, int* idP)
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
* workerStatusToController - MOVE to SamsonWorker
*/
void EndpointMgr::workerStatusToController(void)
{
	Message::WorkerStatusData ws;
	int                       ix;

	// workerStatus(&ws, network);    To fix when ALL change is done (cannot include EndpointMgr in workerStatus.[h|cpp] ...

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

	iomMsgSend(controller->wFd, controller->name.c_str(), me->name.c_str(), Message::WorkerStatus, Message::Msg, &ws, sizeof(ws));
}



/* ****************************************************************************
*
* msgTreat - MOVE to EndpointMgr
*/
void EndpointMgr::msgTreat(int rFd, char* name)
{
	Packet                packet;
	Packet                ack;
	Message::MessageCode  msgCode;
	Message::MessageType  msgType;
	int                   s;
	int                   endpointId;
	Endpoint*             ep = endpointLookup(rFd, &endpointId);
	char                  data[1024];
	void*                 dataP   = data;
	int                   dataLen = sizeof(data);
	int                   wFd     = rFd;   // if pipe pair, this function will nmot work ...

	if (ep == NULL)
		LM_X(1, ("endpoint not found for rFd %d", rFd));

	LM_T(LMT_READ, ("treating incoming message from '%s' (ep at %p)", name, ep));
	s = iomMsgRead(rFd, name, &msgCode, &msgType, &dataP, &dataLen, &packet, NULL, 0);
	LM_T(LMT_READ, ("iomMsgRead returned %d", s));

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
				controller->wFd    = -1;
				controller->state  = ss::Endpoint::Disconnected;

				while (controller->rFd == -1)
				{
					controller->rFd = iomConnect((const char*) controller->ip.c_str(), (unsigned short) controller->port);
					sleep(1); // sleep one second before reintenting connection to coltroller
				}

				controller->wFd   = controller->rFd;
				controller->state = ss::Endpoint::Connected;
				return;
			}
			else if (ep != NULL)
			{
				if (ep->type == Endpoint::Worker)
				{
					--me->workers;

					close(ep->rFd);
					if (ep->wFd != ep->rFd)
						close(ep->wFd);

					ep->state = ss::Endpoint::Closed;
					ep->rFd    = -1;
					ep->wFd    = -1;
					ep->name  = "-----";
				}
				else if (ep->type == Endpoint::CoreWorker)
					LM_X(1, ("BUG - should NOT get any messages from CoreWorker (I'm a '%s')", me->typeName()));
				else
					endpointRemove(ep);
			}
		}

		LM_RVE(("iomMsgRead: error reading message from '%s'", name));
	}


	LM_T(LMT_TREAT, ("Treating %s %s from %s", messageCode(msgCode), messageType(msgType), name));
	switch (msgCode)
	{
	case Message::ThroughPutTest:
	case Message::WorkerSpawn:
	case Message::ControllerSpawn:	   
	case Message::Die:
		LM_X(1, ("Got a DIE message from '%s' - I die", name));
		break;

	case Message::Hello:
		Endpoint*            helloEp;
		Message::HelloData*  hello;

		hello   = (Message::HelloData*) dataP;
		helloEp = endpointAdd(rFd, rFd, hello->name, hello->alias, hello->workers, (ss::Endpoint::Type) hello->type, hello->ip, hello->port, hello->coreNo);

		if (helloEp == NULL)
			LM_X(1, ("helloEp == NULL"));

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
				Packet packet;

				// Ask controller for list of workers
				iomMsgSend(controller->wFd, (char*) controller->name.c_str(), (char*) me->name.c_str(),
						   Message::WorkerVector, Message::Msg, NULL, 0, NULL);
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

			workerV = (Message::Worker*) calloc(Workers, sizeof(Message::Worker));
			if (workerV == NULL)
				LM_XP(1, ("calloc(%d, %d)", Workers, sizeof(Endpoint)));

			for (ix = 3; ix < 3 + Workers; ix++)
			{
				if (endpoint[ix] != NULL)
				{
					strncpy(workerV[ix - 3].name,  endpoint[ix]->name.c_str(),  sizeof(workerV[ix - 3].name));
					strncpy(workerV[ix - 3].alias, endpoint[ix]->alias.c_str(), sizeof(workerV[ix - 3].alias));
					strncpy(workerV[ix - 3].ip,    endpoint[ix]->ip.c_str(),    sizeof(workerV[ix - 3].ip));

					workerV[ix - 3].port   = endpoint[ix]->port;
					workerV[ix - 3].state  = endpoint[ix]->state;
				}
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

			for (ix = 0; ix < dataLen / sizeof(Message::Worker); ix++)
			{
				Endpoint* epP;

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

	case Message::WorkerTask:
	case Message::WorkerTaskFinish:
	case Message::Command:
	case Message::CommandResponse:
	case Message::Help:
	case Message::HelpResponse:
	case Message::StatusRequest:
	case Message::StatusResponse:
	case Message::DownloadData:
	case Message::DownloadDataResponse:
	case Message::UploadData:
	case Message::UploadDataResponse:
	case Message::UploadDataConfirmation:
	case Message::UploadDataConfirmationResponse:
	case Message::WorkerTaskConfirmation:
	case Message::WorkerDataExchange:
	case Message::WorkerDataExchangeClose:
		if (receiver == NULL)
			LM_X(1, ("no packet receiver and unknown message type: %d", msgType));

		LM_T(LMT_MSG, ("forwarding '%s' %s from %s to CoreWorkers", messageCode(msgCode), messageType(msgType), ep->name.c_str()));
		receiver->_receive(endpointId, msgCode, &packet);
		break;
	}

	if (dataP != data)
		free(dataP);
}



#define PeriodForSendingWorkerStatusToController  10
#define PeriodForRestartingDeadCoreWorkers        60
/* ****************************************************************************
*
* run - 
*/
void EndpointMgr::run()
{
	int             fds;
	fd_set          rFds;
	struct timeval  timeVal;
	time_t          now   = 0;
	time_t          then  = time(NULL);
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
			for (ix = 0; ix < Endpoints; ix++)
			{
				if (endpoint[ix] == NULL)
					continue;

				if ((endpoint[ix]->state == Endpoint::Connected || endpoint[ix]->state == Endpoint::Listening) && (endpoint[ix]->rFd >= 0))
				{
					FD_SET(endpoint[ix]->rFd, &rFds);
					max = MAX(max, endpoint[ix]->rFd);
					LM_F(("++ %02d: %-12s %-22s %-15s %15s:%05d %18s  fd: %d",
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
					LM_F(("-- %02d: %-12s %-22s %-15s %15s:%05d %18s  fd: %d",
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
				int        fd;
				char       hostName[128];

				LM_T(LMT_SELECT, ("incoming message from my listener - I will accept ..."));
				--fds;
				fd = iomAccept(listener->rFd, hostName, sizeof(hostName));
				if (fd == -1)
					LM_P(("iomAccept(%d)", listener->rFd));
				else
				{
					std::string  s   = std::string("tmp:") + std::string(hostName);
					Endpoint*    ep  = endpointAdd(fd, fd, (char*) s.c_str(), NULL, 0, Endpoint::Temporal, (char*) "ip", 0);

					ep->state = Endpoint::Connected;
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
						LM_T(LMT_SELECT, ("incoming message from '%s' endpoint %s", endpoint[ix]->typeName(), endpoint[ix]->name.c_str()));
						msgTreat(endpoint[ix]->rFd, (char*) endpoint[ix]->name.c_str());
					}
				}
			}
		}
	}
}



/* ****************************************************************************
*
* readyCheck - 
*/
void EndpointMgr::readyCheck(void)
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


// Network methods using the variable 'me'
//   o getWorkerFromIdentifier
//   o send
//   o run

}

