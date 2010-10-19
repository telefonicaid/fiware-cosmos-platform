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
#include <vector>				// ss::vector


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
	receiver   = NULL;
	iAmReady   = false;
	controller = NULL;
	delilah    = NULL;
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
void Network::init(Endpoint* myEndpoint, bool server)
{
	me = new Endpoint(*myEndpoint);

	if (me == NULL)
		LM_XP(1, ("unable to allocate room for Endpoint 'me'"));

	me->type     = myEndpoint->type;
	me->workers  = 0;
	me->name     = progName;
	me->state    = Endpoint::Connected; /* not really true ... */
	me->port     = myEndpoint->port;

	if (server)
	{
		me->fd       = iomServerOpen(me->port);
		me->state    = Endpoint::Listening;

		if (me->fd == -1)
			LM_XP(1, ("unable to open port %d for listening", me->port));

		LM_T(LMT_FDS, ("opened fd %d to accept incoming connections", me->fd));
	}

	ipSet(NULL);
}

/* ****************************************************************************
 *
 * setPacketReceiverInterface - set the element to be notified when packages arrive
 */
	
void Network::setPacketReceiverInterface( PacketReceiverInterface* _receiver)
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
	{
		me->ip = ip;
		return;
	}

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

	

/* ****************************************************************************
*
* helloSend - 
*/
int Network::helloSend(int fd, char* name)
{
	Packet req;

	LM_T(LMT_WRITE, ("sending hello req (name: '%s')", me->name.c_str()));

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
	unsigned int  ix;

	for (size_t i = 0 ; i < peers.size() ; i++)
		endpointV.push_back( Endpoint(Endpoint::Worker, peers[i] ) ); 
	
	Endpoint myEndpoint(Endpoint::Listener, port);
	
	init(&myEndpoint, true);
	LM_T(LMT_SELECT, ("me->name: '%s'", me->name.c_str()));

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
void Network::initAsSamsonWorker(int port, std::string _controller)
{

	Endpoint myEndpoint(Endpoint::Listener, port);
	Packet  packet(ss::network::Message_Type_WorkerVector);

	Packet  ackPacket;

	myEndpoint.name   = "accepter";
	controller        = new Endpoint( Endpoint::Controller, _controller );
	controller->name  = "controller";

	init(&myEndpoint, true);

    controller->fd = iomConnect((const char*) controller->ip.c_str(), (unsigned short) controller->port);
    if (controller->fd == -1)
        LM_X(1, ("error connecting to controller at %s:%d", controller->ip.c_str(), controller->port));

    controller->state = ss::Endpoint::Connected;
}



/* ****************************************************************************
*
* initAsDelilah - 
*/
void Network::initAsDelilah(std::string _controller)
{
	Endpoint* myEndpoint = new Endpoint(Endpoint::Delilah, "delilah");

	controller        = new Endpoint( Endpoint::Controller, _controller );
    controller->name  = "controller";
	controller->type  = Endpoint::Controller;

	init(myEndpoint, false);
	me->type = Endpoint::Delilah;
	me->port = 1; /* not to be used */

	controller->fd = iomConnect((const char*) controller->ip.c_str(), (unsigned short) controller->port);
	if (controller->fd == -1)
		LM_X(1, ("error connecting to controller at %s:%d", controller->ip.c_str(), controller->port));

	controller->state = ss::Endpoint::Connected;
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

	LM_T(LMT_SELECT, ("treating incoming connection from '%s'", name));
	if ((s = iomMsgRead(fd, name, &req)) != 0)
	{
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

	ss::network::Message_Type msgType = /* (ss::network::Message_Type) */ req.message.type();
	ss::network::Message_Info msgInfo = /* (ss::network::Message_Info) */ req.message.info();

	switch (msgType)
	{
	case ss::network::Message_Type_Hello:
		char*                name;
		int                  workers;
		unsigned short       port;
		char*                ip;
		Endpoint::Type       epType;

		req.helloGet(&name, &workers, &epType, &ip, &port);
		endpointAdd(fd, name, workers, epType, ip, port);
		LM_M(("epType == %d", epType));
		free(name);
		free(ip);

		if (msgInfo == ss::network::Message_Info_Msg)
		{
			LM_T(LMT_WRITE, ("sending Hello ack (name: '%s') - msg type: 0x%x, msg type: 0x%x",  me->name.c_str(),
							 ss::network::Message_Type_Hello, ss::network::Message_Info_Ack));

			ack.message.set_type(ss::network::Message_Type_Hello);
			ack.message.set_info(ss::network::Message_Info_Ack);
			ack.helloAdd((char*) me->name.c_str(), me->workers, me->type, (char*) me->ip.c_str(), me->port);

			iomMsgSend(fd, name, &ack, progName, NULL, 0);
		}

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
		if (controller != NULL)
			LM_X(1, ("Got a WorkerVector request from '%s' but I'm not the controller ...", name));

		LM_T(LMT_MSG, ("Got a WorkerVector message from '%s' (I have %d workers in my vector)", name, endpointV.size()));

		LM_T(LMT_WRITE, ("sending ack with entire worker vector"));

		if (msgInfo == ss::network::Message_Info_Msg)
		{
			ack.message.set_type(ss::network::Message_Type_WorkerVector);
			ack.message.set_info(ss::network::Message_Info_Ack);
			ack.endpointVectorAdd(endpointV);

			iomMsgSend(fd, name, &ack, progName, NULL, 0);
		}

	default:
		assert(0);
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

			if (me && (me->fd != -1) && (me->state == Endpoint::Listening))
			{
				FD_SET(me->fd, &rFds);
				max = MAX(max, me->fd);
				LM_T(LMT_SELECT, ("Added my listen fd %d to fd-list", me->fd));
			}

			if ((controller != NULL) && (controller->state == Endpoint::Connected))
			{
				FD_SET(controller->fd, &rFds);
				max = MAX(max, controller->fd);
				LM_T(LMT_SELECT, ("Added controller fd %d to fd-list", controller->fd));
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
					LM_T(LMT_SELECT, ("added worker %d (%s - %s:%d) - state '%s'", ix, endpointV[ix].name.c_str(), endpointV[ix].ip.c_str(), endpointV[ix].port, endpointV[ix].stateName()));
				}
				else
					LM_T(LMT_SELECT, ("Not adding worker %d (%s - %s:%d) - state '%s'", ix, endpointV[ix].name.c_str(), endpointV[ix].ip.c_str(), endpointV[ix].port, endpointV[ix].stateName()));
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
			else if (me && (me->state == Endpoint::Listening) && FD_ISSET(me->fd, &rFds))
			{
				int   fd;
				char  hostName[128];

				LM_T(LMT_SELECT, ("incoming message from my listener - I accept ..."));
				fd = iomAccept(me, hostName, sizeof(hostName));
				if (fd == -1)
					LM_P(("iomAccept(%d)", me->fd));
				else
					helloSend(fd, hostName);
			}
			else if ((delilah != NULL) && (delilah->state == Endpoint::Connected) && FD_ISSET(delilah->fd, &rFds))
			{
				LM_T(LMT_SELECT, ("incoming message from delilah"));
				msgTreat(delilah->fd, (char*) delilah->name.c_str());
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

    while (ix < endpointV.size())
	{
        if (endpointV[ix].fd == fd)
			return &endpointV[ix];
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

	LM_T(LMT_ENDPOINT, ("adding endpoint '%s' of type %d (fd %d)   (I have %d endpoints)", name, type, fd, endpointV.size()));

	if (type == Endpoint::Controller)
	{
		if (controller == NULL)
		{
			LM_M(("Creating controller"));
			controller = new Endpoint();
		}

		controller->name  = std::string(name);
		controller->type  = type;
		controller->ip    = ip;
		controller->port  = port;
		controller->state = Endpoint::Connected;
		controller->fd    = fd;

		return;
	}

	while (ix < endpointV.size())
	{
		if (endpointV[ix].state > Endpoint::Taken)
		{
			++ix;
			continue;
		}

		LM_T(LMT_ENDPOINT, ("comparing '%s' to '%s' AND '%s'", name, endpointV[ix].ip.c_str(), endpointV[ix].hostname.c_str()));
		if ((endpointV[ix].port == port) &&
			((strncmp(endpointV[ix].ip.c_str(),       name, strlen(endpointV[ix].ip.c_str()))       == 0)
		 ||  (strncmp(endpointV[ix].hostname.c_str(), name, strlen(endpointV[ix].hostname.c_str())) == 0)))
		{
			endpointV[ix].name  = std::string(name);
			endpointV[ix].type  = type;
			endpointV[ix].ip    = ip;
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
