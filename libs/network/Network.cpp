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
* Constructor 
*/
Network::Network()
{
	iAmReady   = false;

	receiver   = NULL;

	me         = NULL;
	listener   = NULL;
	controller = NULL;
	delilah    = NULL;
	temporal   = NULL;

	memset(endpoint, 0, sizeof(endpoint));
}



/* ****************************************************************************
*
* endpointType - 
*/
static const char* endpointType(Endpoint::Type type)
{
	switch (type)
	{
	case Endpoint::Unknown:       return "Unknown";
	case Endpoint::Listener:      return "Listener";
	case Endpoint::Controller:    return "Controller";
	case Endpoint::Worker:        return "Worker";
	case Endpoint::Delilah:       return "Delilah";
	}

	return "UnknownEndpointType";
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
	me->state    = Endpoint::Connected; /* not really true ... */

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
* setPacketReceiverInterface - set the element to be notified when packages arrive
*/
void Network::setPacketReceiverInterface(PacketReceiverInterface* _receiver)
{
	receiver = _receiver;
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

	LM_T(LMT_WRITE, ("sending hello req (name: '%s', type: %s (%d))", me->name.c_str(), endpointType(me->type), me->type));

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

	for (size_t i = 0; i < peers.size(); i++)
		endpointV.push_back(Endpoint(Endpoint::Worker, peers[i])); 

	init(Endpoint::Controller, port);
	LM_M(("I am a '%s', my name: '%s', ip: %s", endpointType(me->type), me->name.c_str(), me->ip.c_str()));

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
	LM_M(("I am a '%s', my name: '%s', ip: %s", endpointType(me->type), me->name.c_str(), me->ip.c_str()));

	endpoint[1 + 1 + Workers] = new Endpoint(Endpoint::Controller, controllerName);
	if (endpoint[1 + 1 + Workers] == NULL)
		LM_XP(1, ("new Endpoint"));
	controller = endpoint[1 + 1 + Workers];

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
	LM_M(("I am a '%s', my name: '%s', ip: %s", endpointType(me->type), me->name.c_str(), me->ip.c_str()));

	endpoint[1 + 1 + Workers] = new Endpoint(Endpoint::Controller, controllerName);
	if (endpoint[1 + 1 + Workers] == NULL)
		LM_XP(1, ("new Endpoint"));
	controller = endpoint[1 + 1 + Workers];

	controller->fd = iomConnect((const char*) controller->ip.c_str(), (unsigned short) controller->port);
	if (controller->fd == -1)
		LM_X(1, ("error connecting to controller at %s:%d", controller->ip.c_str(), controller->port));

	controller->state = ss::Endpoint::Connected;
}
	
	

/* ****************************************************************************
*
* listenerGet - 
*/
Endpoint* Network::listenerGet()
{
	return listener;
}
	



/* ****************************************************************************
*
* me - 
*/
Endpoint* Network::meGet()
{
	return me;
}
	



/* ****************************************************************************
*
* controller - 
*/
Endpoint* Network::controllerGet()
{
	return controller;
}



/* ****************************************************************************
*
* worker - 
*/
Endpoint* Network::workerGet(int i)
{
	return &endpointV[i];
}


	

/* ****************************************************************************
*
* worker - 
*/
int Network::worker( Endpoint* endPoint )
{
	return -1;
}
	
	

/* ****************************************************************************
*
* 
*/
std::vector<Endpoint*> Network::endPoints()
{
	std::vector<Endpoint*> tmp;
	for (size_t i = 0 ; i < endpointV.size() ; i++)
		tmp.push_back( &endpointV[i] );
	return tmp;
}
	


/* ****************************************************************************
*
* samsonWorkerEndpoints - 
*/
std::vector<Endpoint*> Network::samsonWorkerEndpoints()
{
	std::vector<Endpoint*> tmp;
	for (size_t i = 0 ; i < endpointV.size() ; i++)
		tmp.push_back( &endpointV[i] );
	return tmp;
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
* msgTreat - 
*/
void Network::msgTreat(int fd, char* name)
{
	Packet    req;
	Packet    ack;
	int       s;
	Endpoint* ep = endpointLookupByFd(fd);

	LM_T(LMT_SELECT, ("treating incoming message from '%s' (ep at %p)", name, ep));

	if ((s = iomMsgRead(fd, name, &req)) != 0)
	{
		LM_T(LMT_SELECT, ("iomMsgRead returned %d", s));

		if (s == -2) /* Connection closed */
		{
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
			else if ((ep != NULL) && (ep == delilah))
			{
				delilah = NULL;
			}
			else if (ep != NULL)
			{
				--me->workers;
			}

			if (ep != NULL)
				ep->reset();
		}

		LM_RVE(("iomMsgRead: error reading message from '%s'", name));
	}

	ss::network::Message_Type  msgType = req.message.type();
	ss::network::Message_Info  msgInfo = req.message.info();

	switch (msgType)
	{
	case ss::network::Message_Type_Hello:
		char*                helloname;
		int                  workers;
		unsigned short       port;
		char*                ip;
		Endpoint::Type       epType;

		req.helloGet(&helloname, &workers, &epType, &ip, &port);
		endpointAdd(fd, helloname, workers, epType, ip, port);

		if (temporal)
		{
		   free(temporal);
		   temporal = NULL;
		}

		LM_M(("epType == %d", epType));

		if (msgInfo == ss::network::Message_Info_Msg)
		{
			LM_T(LMT_WRITE, ("sending Hello ack (name: '%s') - msg type: 0x%x, msg type: 0x%x",  me->name.c_str(),
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
		else
			LM_M(("not controller: ep: %p, controller: %p", ep, controller));
		break;

	case ss::network::Message_Type_WorkerVector:
		LM_T(LMT_MSG, ("Got a WorkerVector message from '%s' (I have %d workers in my vector)", name, endpointV.size()));

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
			LM_M(("Got the worker vector from the Controller - now connect to them all ..."));

			int ix;
			for (ix = 0; ix < req.endpointVecSize(); ix++)
			{
				Endpoint  endpoint = Endpoint(req.endpointGet(ix));

				endpointV.push_back(endpoint);

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
				int wfd;
				if ((wfd = iomConnect(endpoint.ip.c_str(), endpoint.port)) == -1)
					LM_T(LMT_WORKERS, ("worker %d: %s (host %s, port %d) not there - no problem, he'll connect to me",
									   ix, endpoint.name.c_str(), endpoint.ip.c_str(), endpoint.port));
				else
				{
					temporal        = new Endpoint();
					temporal->state = Endpoint::Connected;
					temporal->fd    = wfd;
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

	LM_T(LMT_NWRUN, ("running"));

	while (1)
	{
		do
		{
			timeVal.tv_sec  = 5;
			timeVal.tv_usec = 0;

			FD_ZERO(&rFds);
			max = 0;

			if (listener && (listener->fd != -1) && (listener->state == Endpoint::Listening))
			{
				FD_SET(listener->fd, &rFds);
				max = MAX(max, listener->fd);
				LM_T(LMT_SELECT, ("Added my listen fd %d to fd-list", listener->fd));
			}

			if ((controller != NULL) && (controller->state == Endpoint::Connected))
			{
				FD_SET(controller->fd, &rFds);
				max = MAX(max, controller->fd);
				LM_T(LMT_SELECT, ("Added controller fd %d to fd-list", controller->fd));
			}

			if ((temporal != NULL) && (temporal->state == Endpoint::Connected))
			{
				FD_SET(temporal->fd, &rFds);
				max = MAX(max, temporal->fd);
				LM_T(LMT_SELECT, ("Added temporal fd %d to fd-list", temporal->fd));
			}

			if ((delilah != NULL) && (delilah->state == Endpoint::Connected))
			{
				FD_SET(delilah->fd, &rFds);
				max = MAX(max, delilah->fd);
				LM_T(LMT_SELECT, ("Added delilah fd %d to fd-list", delilah->fd));
			}

			unsigned int ix;
			for (ix = 0; ix < endpointV.size(); ix++)
			{
				if ((endpointV[ix].state == Endpoint::Connected) && (endpointV[ix].fd >= 0))
				{
					FD_SET(endpointV[ix].fd, &rFds);
					max = MAX(max, endpointV[ix].fd);
					LM_T(LMT_SELECT, ("added worker %d (%s - %s:%d) - state '%s'",
									  ix,
									  endpointV[ix].name.c_str(),
									  endpointV[ix].ip.c_str(),
									  endpointV[ix].port,
									  endpointV[ix].stateName()));
				}
				else
					LM_T(LMT_SELECT, ("Not adding worker %d (%s - %s:%d) - state '%s'",
									  ix,
									  endpointV[ix].name.c_str(),
									  endpointV[ix].ip.c_str(),
									  endpointV[ix].port,
									  endpointV[ix].stateName()));
			}

			LM_T(LMT_SELECT, ("-----------------------------------------------"));
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
			if (controller && (controller->state == Endpoint::Connected) && FD_ISSET(controller->fd, &rFds))
			{
				LM_T(LMT_SELECT, ("incoming message from controller"));
				msgTreat(controller->fd, (char*) controller->name.c_str());
			}
			else if (listener && (listener->state == Endpoint::Listening) && FD_ISSET(listener->fd, &rFds))
			{
				int   fd;
				char  hostName[128];

				LM_T(LMT_SELECT, ("incoming message from my listener - I accept ..."));
				fd = iomAccept(listener, hostName, sizeof(hostName));
				if (fd == -1)
					LM_P(("iomAccept(%d)", listener->fd));
				else
					helloSend(fd, hostName);
				temporal = new Endpoint();
				temporal->state = Endpoint::Connected;
				temporal->fd    = fd;
			}
			else if ((delilah != NULL) && (delilah->state == Endpoint::Connected) && FD_ISSET(delilah->fd, &rFds))
			{
				LM_T(LMT_SELECT, ("incoming message from delilah"));
				msgTreat(delilah->fd, (char*) delilah->name.c_str());
			}
			else if ((temporal != NULL) && (temporal->state == Endpoint::Connected) && FD_ISSET(temporal->fd, &rFds))
			{
				LM_T(LMT_SELECT, ("incoming message from temporal (fd %d)", temporal->fd));
				msgTreat(temporal->fd, (char*) temporal->name.c_str());
			}
			else
			{
				unsigned int ix;
				for (ix = 0; ix < endpointV.size(); ix++)
				{
					if ((endpointV[ix].state != Endpoint::Connected) || (endpointV[ix].fd < 0))
						continue;

					if (FD_ISSET(endpointV[ix].fd, &rFds))
					{
						LM_T(LMT_SELECT, ("incoming message from worker %d", ix));
						msgTreat(endpointV[ix].fd, (char*) endpointV[ix].name.c_str());
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

	while (ix < endpointV.size())
	{
		if (endpointV[ix].state <= Endpoint::Taken)
		{
			iAmReady = false;
			return;
		}

		++ix;
	}

	iAmReady = true;
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

	if ((listener != NULL) && (fd == listener->fd))
		return listener;

	if ((temporal != NULL) && (fd == temporal->fd))
		return temporal;

	if ((controller != NULL) && (fd == controller->fd))
		return controller;

	if ((delilah != NULL) && (fd == delilah->fd))
		return delilah;

	while (ix < endpointV.size())
	{
		if (endpointV[ix].fd == fd)
			return &endpointV[ix];
		++ix;
	}

	return NULL;
}



/* ****************************************************************************
*
* endpointLookupByIpAndPort - 
*/
Endpoint* Network::endpointLookupByIpAndPort(const char* ip, unsigned short port)
{
	unsigned int ix = 0;

	if ((listener != NULL) && (strcmp((char*) ip, listener->ip.c_str()) == 0) && (listener->port == port))
		return listener;
	if ((temporal != NULL) && (strcmp((char*) ip, temporal->ip.c_str()) == 0) && (temporal->port == port))
		return temporal;
	if ((controller != NULL) && (strcmp((char*) ip, controller->ip.c_str()) == 0) && (controller->port == port))
		return controller;
	if ((delilah != NULL) && (strcmp((char*) ip, delilah->ip.c_str()) == 0) && (delilah->port == port))
		return delilah;

	while (ix < endpointV.size())
	{
		if ((endpointV[ix].port == port) && (strcmp(endpointV[ix].ip.c_str(), ip) == 0))
			return &endpointV[ix];
		++ix;
	}

	return NULL;
}



/* ****************************************************************************
*
* endpointAdd - 
*/
void Network::endpointAdd(int fd, char* name, int workers, Endpoint::Type type, std::string ip, unsigned short port)
{
	unsigned int ix = 0;

	LM_T(LMT_ENDPOINT, ("adding endpoint '%s' of type %d (%s) (fd %d)   (I have %d endpoints)", name, type, endpointType(type), fd, endpointV.size()));

	if (type == Endpoint::Controller)
	{
		if (controller == NULL)
		{
			LM_M(("Creating controller"));
			endpoint[1 + 1 + Workers] = new Endpoint();
			if (endpoint[1 + 1 + Workers] == NULL)
				LM_XP(1, ("new Endpoint"));
			controller = endpoint[1 + 1 + Workers];
		}

		controller->name  = std::string(name);
		controller->type  = type;
		controller->ip    = ip;
		controller->port  = port;
		controller->state = Endpoint::Connected;
		controller->fd    = fd;

		return;
	}

	LM_T(LMT_ENDPOINT, ("endpointV.size: %d", endpointV.size()));

	while (ix < endpointV.size())
	{
		if (endpointV[ix].state > Endpoint::Taken)
		{
			++ix;
			LM_T(LMT_ENDPOINT, ("Endpoint %s:%d occupied", endpointV[ix].ip.c_str(), endpointV[ix].port));
			continue;
		}


		LM_T(LMT_ENDPOINT, ("comparing IPs: '%s' to '%s' AND '%s'", ip.c_str(), endpointV[ix].ip.c_str(), endpointV[ix].hostname.c_str()));
		LM_T(LMT_ENDPOINT, ("comparing ports: %d to %d", port, endpointV[ix].port));

		if ((endpointV[ix].port == port) &&	((strcmp(ip.c_str(), endpointV[ix].ip.c_str()) == 0) || (strcmp(ip.c_str(), endpointV[ix].hostname.c_str()) == 0)))
		{
			endpointV[ix].name  = std::string(name);
			endpointV[ix].type  = type;
			endpointV[ix].ip    = std::string(ip);
			endpointV[ix].port  = port;
			endpointV[ix].state = Endpoint::Connected;
			endpointV[ix].fd    = fd;

			LM_T(LMT_ENDPOINT, ("Set fd %d for endpoint '%s'", fd, endpointV[ix].ip.c_str()));
			return;
		}

		++ix;
	}

	LM_W(("No endpoint found ..."));
	close(fd);
}



/* ****************************************************************************
*
* quit - 
*/
void Network::quit()
{
}	

}
