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
	receiver = _receiver;
}

	

/* ****************************************************************************
*
* setPacketReceiverInterface - set the element to be notified when packages arrive
*/
void Network::setPacketReceiverInterface(PacketReceiverInterface* _receiver)
{
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
	me->state    = Endpoint::Taken;

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
	for (ix = 0; ix < endpointV.size(); ix++)
	{
		char name[32];

		LM_T(LMT_ENDPOINTS, ("endpointV.ip: '%s'", endpointV[ix].ip.c_str()));
		endpointV[ix].state = Endpoint::Taken;

		snprintf(name, sizeof(name), "worker %d", ix);
		endpointV[ix].name = name;
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
	return 2;
}



/* ****************************************************************************
*
* workerGetIdentifier - 
*/
int Network::workerGetIdentifier(int nthWorker)
{
	unsigned int  ix;
	int           workerIx = 0;

	for (ix = 0; ix < sizeof(endpoint) / sizeof(endpoint[0]); ix++)
	{
		if (endpoint[ix]->type == Endpoint::Worker)
		{
			if (workerIx == nthWorker)
				return ix;

			++workerIx;
		}
	}

	return -1;
}
	
	

/* ****************************************************************************
*
* getWorkerFromIdentifier - 
*/
int Network::getWorkerFromIdentifier(int identifier)
{
    unsigned int  ix;
    int           workers = 0;

    for (ix = 0; ix < sizeof(endpoint) / sizeof(endpoint[0]); ix++)
    {
        if (endpoint[ix]->type == Endpoint::Worker)
        {
            if (workers == identifier)
                return ix;

            ++workers;
        }
    }

    return -1;
}



/* ****************************************************************************
*
* getNumWorkers - 
*/
int Network::getNumWorkers(void)
{
	unsigned int  ix;
	int           workerIx = 0;

	for (ix = 0; ix < sizeof(endpoint) / sizeof(endpoint[0]); ix++)
	{
		if (endpoint[ix]->type == Endpoint::Worker)
			++workerIx;
	}

	return workerIx;
}
	
	

/* ****************************************************************************
*
* samsonWorkerEndpoints - 
*/
std::vector<Endpoint*> Network::samsonWorkerEndpoints()
{
    unsigned int            ix;
	std::vector<Endpoint*>  v;

	for (ix = 0; ix < sizeof(endpoint) / sizeof(endpoint[0]); ix++)
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
* send - 
*/
size_t Network::send(Packet* p, Endpoint* endpoint, PacketSenderInterface* sender)
{
	if (sender)
		sender->notificationSent(0, true);

	return 0;
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
* endpointAdd - 
*/
void Network::endpointAdd(int fd, char* name, int workers, Endpoint::Type type, std::string ip, unsigned short port)
{
	unsigned int  ix    = 0;

	LM_T(LMT_ENDPOINT, ("adding endpoint '%s' of type %d (%s) (fd %d)", name, type, endpointTypeName(type), fd));

	if (type == Endpoint::Controller)
	{
		if (controller == NULL)
		{
			endpoint[2] = new Endpoint();
			if (endpoint[2] == NULL)
				LM_XP(1, ("new Endpoint"));
			controller = endpoint[2];
		}

		controller->name  = std::string(name);
		controller->type  = type;
		controller->ip    = ip;
		controller->port  = port;
		controller->state = Endpoint::Connected;
		controller->fd    = fd;

		return;
	}


	// if Worker, we just might be reconnecting ...
	if (type == Endpoint::Worker)
	{
		LM_T(LMT_ENDPOINT, ("looping over endpoint vector (%d slots) ... (the fd is %d)", sizeof(endpoint) / sizeof(endpoint[0]), fd));
		// endpointList("might be reconnecting");
		for (ix = 0; ix < sizeof(endpoint) / sizeof(endpoint[0]); ix++)
		{
			if (endpoint[ix] == NULL)
				continue;
			if (endpoint[ix]->type != Endpoint::Worker)
				continue;
			if (endpoint[ix]->state != Endpoint::Reconnecting)
				continue;
			if (strcmp(endpoint[ix]->ip.c_str(), ip.c_str()) != 0)
				continue;
			if (endpoint[ix]->port != port)
				continue;

			// Found it !
			endpoint[ix]->name  = std::string(name);
			endpoint[ix]->type  = type;
			endpoint[ix]->ip    = std::string(ip);
			endpoint[ix]->port  = port;
			endpoint[ix]->state = Endpoint::Connected;
			endpoint[ix]->fd    = fd;

			LM_T(LMT_ENDPOINT, ("Set fd %d for endpoint %d: '%s'", fd, ix, endpoint[ix]->ip.c_str()));
			LM_TODO(("Any Temporal Endpoint pending ... ?"));
			return;
		}
	}

	LM_T(LMT_ENDPOINT, ("looping over endpoint vector (%d slots) ...", sizeof(endpoint) / sizeof(endpoint[0])));
	for (ix = 0; ix < sizeof(endpoint) / sizeof(endpoint[0]); ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix]->type != Endpoint::Temporal)
			continue;

		LM_T(LMT_ENDPOINT, ("comparing fds: %d - %d (name: '%s', fd: %d)", endpoint[ix]->fd, fd,  endpoint[ix]->name.c_str(),  endpoint[ix]->fd));
		if (endpoint[ix]->fd == fd)
		{
			endpoint[ix]->name  = std::string(name);
			endpoint[ix]->type  = type;
			endpoint[ix]->ip    = std::string(ip);
			endpoint[ix]->port  = port;
			endpoint[ix]->state = Endpoint::Connected;
			endpoint[ix]->fd    = fd;

			LM_T(LMT_ENDPOINT, ("Set fd %d for endpoint '%s'", fd, endpoint[ix]->ip.c_str()));
			return;
		}
	}

	LM_W(("No endpoint found for '%s' %s:%d ...", name, ip.c_str(), port));
	close(fd);
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
* endpointFreeGet - 
*/
Endpoint* Network::endpointFreeGet(Endpoint::Type type)
{
	unsigned int ix = 0;

	for (ix = 0; ix < sizeof(endpoint) / sizeof(endpoint[0]); ix++)
	{
		if (endpoint[ix] == NULL)
		{
			endpoint[ix] = new Endpoint();
			if (endpoint[ix] == NULL)
				LM_XP(1, ("new Endpoint"));
			
			endpoint[ix]->type = type;

			return endpoint[ix];
		}
	}

	LM_X(1, ("out of endpoints in the endpoint vector ... fix it and recompile !"));

	return NULL;
}



/* ****************************************************************************
*
* endpointLookupByFd - 
*/
Endpoint* Network::endpointLookupByFd(int fd)
{
	unsigned int ix = 0;

	if (fd < 0)
		return NULL;

	for (ix = 0; ix < sizeof(endpoint) / sizeof(endpoint[0]); ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix]->fd == fd)
			return endpoint[ix];
	}

	LM_E(("endpoint (fd:%d) not found", fd));
	return NULL;
}



/* ****************************************************************************
*
* endpointLookupByIpAndPort - 
*/
Endpoint* Network::endpointLookupByIpAndPort(const char* ip, unsigned short port)
{
	unsigned int ix = 0;

	for (ix = 0; ix < sizeof(endpoint) / sizeof(endpoint[0]); ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if ((endpoint[ix]->port == port) && (strcmp(endpoint[ix]->ip.c_str(), ip) == 0))
			return endpoint[ix];
	}

	LM_E(("endpoint %s:%d not found", ip, port));
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
	Endpoint* ep = endpointLookupByFd(fd);

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
				controller->state = ss::Endpoint::Taken;

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
					--me->workers;
				close(ep->fd);
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
			LM_F(("Got the worker vector from the Controller - now connect to them all ..."));

			int ix;
			for (ix = 0; ix < req.endpointVecSize(); ix++)
			{
				Endpoint  endpoint = Endpoint(req.endpointGet(ix));

				if (((strcmp(endpoint.ip.c_str(), me->ip.c_str()) == 0) || (strcmp(endpoint.ip.c_str(), me->hostname.c_str()) == 0)) && (endpoint.port == me->port))
				{
					LM_T(LMT_WORKERS, ("NOT connecting to myself ..."));
					continue;
				}

				Endpoint* peer;
				if ((peer = endpointLookupByIpAndPort(endpoint.ip.c_str(), endpoint.port)) != NULL)
				{
					if (peer->state > Endpoint::Taken)
					{
						LM_T(LMT_WORKERS, ("NOT connecting to endpoint %s:%d - we're already connected", endpoint.ip.c_str(), endpoint.port));
						continue;
					}
				}

				LM_T(LMT_WORKERS, ("Connect to worker %d: %s (host %s, port %d)", ix, endpoint.name.c_str(), endpoint.ip.c_str(), endpoint.port));
				int workerFd;
				if ((workerFd = iomConnect(endpoint.ip.c_str(), endpoint.port)) == -1)
				{
					LM_T(LMT_WORKERS, ("worker %d: %s (host %s, port %d) not there - no problem, he'll connect to me",
									   ix, endpoint.name.c_str(), endpoint.ip.c_str(), endpoint.port));
				}
				else
				{
					Endpoint* ep = endpointFreeGet(Endpoint::Temporal);

					if (ep == NULL)
						LM_X(1, ("cannot obtain a temporal endpoint - please change definitions for vector size and recompile !"));

					ep->state = Endpoint::Connected;
					ep->fd    = workerFd;
					ep->ip    = endpoint.ip;
					ep->port  = endpoint.port;
				}
			}
		}
		break;

	default:
		LM_X(1, ("unknown message type: %d", msgType));
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
	int             max;

	while (1)
	{
		unsigned int ix;

		do
		{
			timeVal.tv_sec  = 2;
			timeVal.tv_usec = 0;

			FD_ZERO(&rFds);
			max = 0;

			if (listener == NULL)  /* I am a delilah - reconnect to dead workers */
			{
				for (ix = 0; ix < sizeof(endpoint) / sizeof(endpoint[0]); ix++)
				{
					int workerFd;

					if (endpoint[ix] == NULL)
						continue;
						
					if (endpoint[ix]->type != Endpoint::Worker)
						continue;

					if (endpoint[ix]->state != Endpoint::Disconnected)
						continue;

					LM_T(LMT_RECONNECT, ("delilah reconnecting to %s:%d", endpoint[ix]->ip.c_str(), endpoint[ix]->port));
					workerFd = iomConnect(endpoint[ix]->ip.c_str(), endpoint[ix]->port);
					if (workerFd != -1)
					{
						Endpoint* ep = endpointFreeGet(Endpoint::Temporal);

						if (ep == NULL)
							LM_X(1, ("cannot obtain a temporal endpoint - please change definitions for vector size and recompile !"));

						ep->state = Endpoint::Connected;
						ep->fd    = workerFd;
						ep->ip    = endpoint[ix]->ip;
						ep->port  = endpoint[ix]->port;

						endpoint[ix]->state = Endpoint::Reconnecting;
					}
				}
			}

			LM_T(LMT_SELECT, ("------------------------------------------------------------------------"));
			for (ix = 0; ix < sizeof(endpoint) / sizeof(endpoint[0]); ix++)
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
				Endpoint*  tmp;

				LM_T(LMT_SELECT, ("incoming message from my listener - I will accept ..."));
				fd = iomAccept(listener, hostName, sizeof(hostName));
				if (fd == -1)
					LM_P(("iomAccept(%d)", listener->fd));
				else
					helloSend(fd, hostName);

				tmp        = endpointFreeGet(Endpoint::Temporal);
				tmp->state = Endpoint::Connected;
				tmp->fd    = fd;
				tmp->name  = std::string("tmp:") + std::string(hostName);
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
	unsigned int ix = 0;

	for (ix = 0; ix < sizeof(endpoint) / sizeof(endpoint[0]); ix++)	
	{
		if (endpoint[ix]->type == Endpoint::Worker)
		{
			if (endpoint[ix]->state != Endpoint::Connected)
			{
				iAmReady = false;
				return;
			}
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
