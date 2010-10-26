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

#include "logMsg.h"             // LM_*
#include "networkTraceLevels.h" // LMT_*

#include "Endpoint.h"			// Endpoint
#include "MsgHeader.h"          // MsgHeader
#include "Packet.h"				// Packet
#include "iomInit.h"            // iomInit
#include "iomServerOpen.h"      // iomServerOpen
#include "iomConnect.h"         // iomConnect
#include "iomAccept.h"          // iomAccept
#include "iomMsgSend.h"         // iomMsgSend
#include "iomMsgRead.h"         // iomMsgRead
#include "iomMsgAwait.h"        // iomMsgAwait
#include "workerStatus.h"       // workerStatus, WorkerStatus
#include "Network.h"			// Own interface



namespace ss
{



/* ****************************************************************************
*
* global vars ...
*/



/* ****************************************************************************
*
* msgTypeName - 
*/
static char* msgTypeName(ss::network::Message_Type type)
{
	switch (type)
	{
	case ss::network::Message_Type_Hello:                       return (char*) "Hello";
	case ss::network::Message_Type_WorkerVector:                return (char*) "WorkerVector";
	case ss::network::Message_Type_WorkerTask:		            return (char*) "WorkerTask";
	case ss::network::Message_Type_WorkerTaskConfirmation:		return (char*) "WorkerTaskConfirmation";
	case ss::network::Message_Type_Command:				        return (char*) "Command";
	case ss::network::Message_Type_CommandResponse:		        return (char*) "CommandResponse";
	case ss::network::Message_Type_Data:						return (char*) "Data";
	case ss::network::Message_Type_WorkerStatus:				return (char*) "WorkerStatus";
	}

	return (char*) "UnknownMsgType";
}



/* ****************************************************************************
*
* msgInfoName - 
*/
static char* msgInfoName(ss::network::Message_Info info)
{
	switch (info)
	{
	case ss::network::Message_Info_Msg:     return (char*) "Msg";
	case ss::network::Message_Info_Evt:     return (char*) "Evt";
	case ss::network::Message_Info_Ack:     return (char*) "Ack";
	case ss::network::Message_Info_Nak:     return (char*) "Nak";
	}

	return (char*) "UnknownMsgInfo";
}



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
int Network::helloSend(int fd, char* name)
{
	Packet req;

	LM_T(LMT_WRITE, ("sending hello req (name: '%s', type: %s (%d))", me->name.c_str(), endpointTypeName(me->type), me->type));

	req.messageTypeSet(ss::network::Message_Type_Hello);
	req.messageInfoSet(ss::network::Message_Info_Msg);

	req.helloAdd((char*) me->name.c_str(), me->workers, me->type, (char*) me->ip.c_str(), me->port);

	return iomMsgSend(fd, name, &req, progName, NULL, 0);
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
* initAsSamsonWorker -
*
* NOTE
*/
void Network::initAsSamsonWorker(int port, std::string controllerName)
{
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
	LM_X(1, ("IN"));

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
size_t Network::send(Packet* packetP, int endpointId, PacketSenderInterface* sender)
{
	Endpoint*                  ep        = endpoint[endpointId];
	ss::network::Message_Type  type      = packetP->message.type();

	if (ep == NULL)
		LM_RE(-1, ("No endpoint at index %d", endpointId));
	if (ep->state != Endpoint::Connected)
		LM_RE(-1, ("Endpoint %d in state '%s'", endpointId, ep->stateName()));

	LM_T(LMT_DELILAH, ("sending a '%s' message to endpoint %d", msgTypeName(type), endpointId));

	packetP->message.set_info(ss::network::Message_Info_Msg);
	int nb = iomMsgSend(ep->fd, (char*) ep->name.c_str(), packetP, (char*) me->name.c_str(), packetP->buffer.getDataPointer(), packetP->buffer.getLength());

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
Endpoint* Network::endpointAdd(int fd, char* name, int workers, Endpoint::Type type, std::string ip, unsigned short port)
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
		return NULL;
		break;

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

				return endpoint[ix];
			}
		}

		if (endpoint[ix] == NULL)
			LM_X(1, ("No temporal endpoint slots available - redefine and recompile!"));
		break;


	case Endpoint::Delilah:
		for (ix = 3 + Workers; ix < (int) (sizeof(endpoint) / sizeof(endpoint[0]) - 1); ix++)
		{
			if (endpoint[ix] == NULL)
			{
				endpoint[ix] = new Endpoint();
				if (endpoint[ix] == NULL)
					LM_XP(1, ("allocating temporal Endpoint"));

				endpoint[ix]->name  = std::string(name);
				endpoint[ix]->fd    = fd;
				endpoint[ix]->state = Endpoint::Connected;
				endpoint[ix]->type  = Endpoint::Delilah;

                return endpoint[ix];
			}
		}

		if (endpoint[ix] == NULL)
			LM_X(1, ("No endpoint slots available for Delilah - redefine and recompile!"));
		break;

	case Endpoint::Worker:
		for (ix = 3; ix < 3 + Workers; ix++)
		{
			if (endpoint[ix] == NULL)
				LM_X(1, ("NULL worker endpoint at slot %d", ix));

			if ((strcmp(endpoint[ix]->ip.c_str(), ip.c_str()) == 0) && (endpoint[ix]->port == port))
			{
				endpoint[ix]->fd    = fd;
				endpoint[ix]->name  = std::string(name);
				endpoint[ix]->state = Endpoint::Connected;
				endpoint[ix]->type  = Endpoint::Worker;

                return endpoint[ix];
			}
		}

		LM_X(1, ("No endpoint slots available for Worker - important internal bug!"));
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
	Packet    req;
	Packet    ack;
	int       s;
	int       endpointId;
	Endpoint* ep = endpointLookup(fd, &endpointId);

	LM_T(LMT_SELECT, ("treating incoming message from '%s' (ep at %p)", name, ep));
	s = iomMsgRead(fd, name, &req);
	if (s != 0)
	{
		LM_T(LMT_SELECT, ("iomMsgRead returned %d", s));

		if (s == -2) /* Connection closed */
		{
			LM_T(LMT_SELECT, ("Connection closed - ep at %p", ep));
			if ((ep != NULL) && (ep == controller))
			{
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
				else
					endpointRemove(ep);
			}
		}

		LM_RVE(("iomMsgRead: error reading message from '%s'", name));
	}


	ss::network::Message_Type  msgType = req.message.type();
	ss::network::Message_Info  msgInfo = req.message.info();

	LM_T(LMT_TREAT, ("Treating %s %s from %s", msgTypeName(msgType), msgInfoName(msgInfo), name));
	switch (msgType)
	{
	case ss::network::Message_Type_Hello:
		char*                helloname;
		int                  workers;
		unsigned short       port;
		char*                ip;
		Endpoint::Type       epType;

		req.helloGet(&helloname, &workers, &epType, &ip, &port);
		LM_T(LMT_HELLO, ("Got Hello %s from %s, type %s, %s:%d, workers: %d",
						 msgInfoName(msgInfo), helloname, endpointTypeName(epType), ip, port, workers));

		endpointAdd(fd, helloname, workers, epType, ip, port);
		if (ep && ep->type == Endpoint::Temporal)
			endpointRemove(ep);

		if (msgInfo == ss::network::Message_Info_Msg)
		{
			LM_T(LMT_HELLO, ("sending Hello ack (name: '%s') - msg type: 0x%x, msg type: 0x%x",  me->name.c_str(),
							 ss::network::Message_Type_Hello, ss::network::Message_Info_Ack));

			ack.message.set_type(ss::network::Message_Type_Hello);
			ack.message.set_info(ss::network::Message_Info_Ack);
			ack.helloAdd((char*) me->name.c_str(), me->workers, me->type, (char*) me->ip.c_str(), me->port);

			iomMsgSend(fd, helloname, &ack, progName, NULL, 0);
		}

		if (helloname) 
			free(helloname);
		if (ip)
			free(ip);

		if ((ep != NULL) && (ep == controller))
		{
			Packet packet;

			// Ask controller for list of workers
			packet.message.set_type(ss::network::Message_Type_WorkerVector);
			packet.message.set_info(ss::network::Message_Info_Msg);

			iomMsgSend(controller->fd, (char*) controller->name.c_str(), &packet, progName, NULL, 0);
		}
		break;

	case ss::network::Message_Type_WorkerVector:
		LM_T(LMT_MSG, ("Got a WorkerVector message from '%s'", name));

		if ((msgInfo == ss::network::Message_Info_Msg) && (me->type != Endpoint::Controller))
			LM_X(1, ("Got a WorkerVector request from '%s' but I'm not the controller ...", name));

		if ((me->type == Endpoint::Controller) && (msgInfo == ss::network::Message_Info_Msg))
		{
			ack.message.set_type(ss::network::Message_Type_WorkerVector);
			ack.message.set_info(ss::network::Message_Info_Ack);
			ack.endpointVectorAdd(endpointV);

			LM_T(LMT_WRITE, ("sending ack with entire worker vector"));
			iomMsgSend(fd, name, &ack, progName, NULL, 0);
		}
		else if (msgInfo == ss::network::Message_Info_Ack)
		{
			LM_T(LMT_ENDPOINT, ("Got the worker vector from the Controller - now connect to them all ..."));

			if (req.endpointVecSize() != Workers)
				LM_X(1, ("bad size of worker vector (%d). Should be %d!", req.endpointVecSize(), Workers));

			int ix;
			for (ix = 0; ix < req.endpointVecSize(); ix++)
			{
				Endpoint  ep = Endpoint(req.endpointGet(ix));
				Endpoint* epP;

				if (endpoint[3 + ix] == NULL)
					endpoint[3 + ix] = new Endpoint(ep);

				epP = endpoint[3 + ix];

				LM_TODO(("compare endpoint[3 + ix] and endpoint - should be equal in everything (name, ip, port ...)!"));

				if (((strcmp(epP->ip.c_str(), me->ip.c_str()) == 0) || (strcmp(epP->ip.c_str(), me->hostname.c_str()) == 0)) && (epP->port == me->port))
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

	default:
		if (receiver == NULL)
			LM_X(1, ("no packet receiver and unknown message type: %d", msgType));
		receiver->receive(&req, endpointId);
		break;
	}
}



/* ****************************************************************************
*
* run - 
*/
void Network::run()
{
	int             fds;
	fd_set          rFds;
	struct timeval  timeVal;
	time_t          then = 0;
	time_t          now  = 0;
	int             max;

	while (1)
	{
		int ix;

		do
		{
			if (me->type == Endpoint::Worker)
			{
				now = time(NULL);
				if (now - then > 1)
				{
					then = now;
#if 0
					packetP->message.set_info(ss::network::Message_Info_Msg);
					packetP->message.set_type(ss::network::Message_Type_WorkerStatus);
					
					ss::network::WorkerStatus* sP = message.mutable_workerstatus();
					sP->cores = 8;
#else
					WorkerStatus ws;
					int          ix;

					workerStatus(&ws);

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
							LM_T(LMT_STAT, ("Net I/F %02d %-10s: sending at %.2f Mbytes/s", ix, ws.netInfo.iface[ix].name, ws.netInfo.iface[ix].sndSpeed / 1024.0 / 1024.0));
						else if (ws.netInfo.iface[ix].sndSpeed > 1024)
							LM_T(LMT_STAT, ("Net I/F %02d %-10s: sending at %.2f kbytes/s", ix, ws.netInfo.iface[ix].name, ws.netInfo.iface[ix].sndSpeed / 1024.0));
						else
							LM_T(LMT_STAT, ("Net I/F %02d %-10s: sending at %.2f bytes/s", ix, ws.netInfo.iface[ix].name, ws.netInfo.iface[ix].sndSpeed));

						if (ws.netInfo.iface[ix].rcvSpeed > 1024 * 1024)
							LM_T(LMT_STAT, ("Net I/F %02d %-10s: receiving at %.2f Mbytes/s", ix, ws.netInfo.iface[ix].name, ws.netInfo.iface[ix].rcvSpeed / 1024.0 / 1024.0));
						else if (ws.netInfo.iface[ix].rcvSpeed > 1024)
							LM_T(LMT_STAT, ("Net I/F %02d %-10s: receiving at %.2f kbytes/s", ix, ws.netInfo.iface[ix].name, ws.netInfo.iface[ix].rcvSpeed / 1024.0));
						else
							LM_T(LMT_STAT, ("Net I/F %02d %-10s: receiving at %.2f bytes/s", ix, ws.netInfo.iface[ix].name, ws.netInfo.iface[ix].rcvSpeed));
					}
#endif
				}
			}


			timeVal.tv_sec  = 2;
			timeVal.tv_usec = 0;

			FD_ZERO(&rFds);
			max = 0;

			if (listener == NULL)  /* I am a delilah - reconnect to dead workers */
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

			LM_T(LMT_SELECT, ("------------------------------------------------------------------------"));
			for (ix = 0; ix < (int) (sizeof(endpoint) / sizeof(endpoint[0])); ix++)
			{
				if (endpoint[ix] == NULL)
					continue;

				if ((endpoint[ix]->state == Endpoint::Connected || endpoint[ix]->state == Endpoint::Listening) && (endpoint[ix]->fd >= 0))
				{
					FD_SET(endpoint[ix]->fd, &rFds);
					max = MAX(max, endpoint[ix]->fd);
					
					LM_T(LMT_SELECT, ("+ endpoint %02d %-15s %-20s %20s:%05d %20s (fd: %d)",
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
					LM_T(LMT_SELECT, ("- endpoint %02d %-15s %-20s %20s:%05d %20s (fd: %d)",
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
					std::string s = std::string("tmp:") + std::string(hostName);

					endpointAdd(fd, (char*) s.c_str(), 0, Endpoint::Temporal, (char*) "ip", 0);
					helloSend(fd, hostName);
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
						LM_T(LMT_SELECT, ("incoming message from endpoint %s", endpoint[ix]->name.c_str()));
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
