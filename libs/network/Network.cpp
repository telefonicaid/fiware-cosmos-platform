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
#include "network.h"			// Own interface



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
void Network::init(Endpoint* myEndpoint)
{
	me = new Endpoint(*myEndpoint);

	if (me == NULL)
		LM_XP(1, ("unable to allocate room for Endpoint 'me'"));

	me->type     = Endpoint::Listener;
	me->workers  = 0;
	me->name     = progName;
	me->state    = Endpoint::Listening;
	me->fd       = iomServerOpen(me->port);

	if (me->fd == -1)
		LM_XP(1, ("unable to open port %d for listening", me->port));

	LM_T(LMT_FDS, ("opened fd %d to accept incoming connections", me->fd));

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
* initAsSamsonController - 
*/
void Network::initAsSamsonController(Endpoint myEndpoint, std::vector<Endpoint> endpoints)
{
	unsigned int  ix;

	endpointV = endpoints;

	init(&myEndpoint);
	LM_T(LMT_SELECT, ("me->name: '%s'", me->name.c_str()));

	ix = 0;
	for (ix = 0; ix < endpoints.size(); ix++)
	{
		char name[32];

		LM_T(LMT_ENDPOINTS, ("endpointV.ip: '%s'", endpointV[ix].ip.c_str()));
		endpoints[ix].state = Endpoint::Taken;

		snprintf(name, sizeof(name), "worker %d", ix);
		endpoints[ix].name = name;
	}
}



/* ****************************************************************************
*
* helloSend - 
*/
void Network::helloSend(Endpoint* epP)
{
	Packet  req;
	
	LM_T(LMT_WRITE, ("sending hello req (name: '%s')", me->name.c_str()));

	req.messageCodeSet(Packet::Hello);
	req.messageTypeSet(Packet::Req);

	req.helloAdd((char*) me->name.c_str(), me->workers, me->type, (char*) me->ip.c_str(), me->port);

	iomMsgSend(epP, &req, NULL, 0);
}



/* ****************************************************************************
*
* initAsSamsonWorker - 
*
* NOTE
*/
void Network::initAsSamsonWorker(Endpoint myEndpoint, Endpoint controllerEndpoint)
{
	Packet  packet(Packet::WorkerVector);
	Packet  ackPacket;

	init(&myEndpoint);

	myEndpoint.name   = "accepter";
	controller        = new Endpoint(controllerEndpoint);
	controller->name  = "controller";

	iomInit(controller);

	/* ask controller for list of workers */
	packet.messageTypeSet(Packet::Req);
	iomMsgSend(controller, &packet, NULL, 0);

	/* Should probably call msgTreat here ... */
	iomMsgRead(controller, &ackPacket);

	if (!ackPacket.message.has_endpoints())
		LM_X(1, ("controller didn't give me the Worker Vector ..."));

	int ix;

	for (ix = 0; ix < ackPacket.endpointVecSize(); ix++)
	{
		Endpoint  endpoint = Endpoint(ackPacket.endpointGet(ix));

		LM_T(LMT_ENDPOINT, ("comparing '%s' to '%s' or '%s'    AND    %d to %d",
							endpoint.ip.c_str(),
							me->ip.c_str(),
							me->hostname.c_str(),
							endpoint.port,
							me->port));

		if (((strcmp(endpoint.ip.c_str(), me->ip.c_str()) == 0) || (strcmp(endpoint.ip.c_str(), me->hostname.c_str()) == 0)) && (endpoint.port == me->port))
		{
			LM_T(LMT_WORKERS, ("NOT connecting to myself ..."));
			continue;
		}

		LM_T(LMT_WORKERS, ("Connect to worker %d: %s (host %s, port %d)", ix, endpoint.name.c_str(), endpoint.ip.c_str(), endpoint.port));
		endpoint.fd = iomConnect(endpoint.ip.c_str(), endpoint.port);
		if (endpoint.fd != -1)
		{
			endpoint.state = Endpoint::Connected;
			helloSend(&endpoint);
		}

		endpointV.push_back(endpoint);
	}

	LM_T(LMT_WORKERS, ("I now have %d workers in my vector", endpointV.size()));

	helloSend(controller);
}



/* ****************************************************************************
*
* initAsDelilah - 
*/
void Network::initAsDelilah(Endpoint controllerEndpoint)
{
	Endpoint* myEndpoint = new Endpoint(Endpoint::Delilah, "delilah");

	controller        = new Endpoint(controllerEndpoint);
    controller->name  = "controller";
	controller->type  = Endpoint::Controller;

	init(myEndpoint);
	iomInit(controller);
	helloSend(controller);
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
int Network::worker( Endpoint endPoint )
{
	return -1;
}
	
	

/* ****************************************************************************
*
* 
*/
std::vector<Endpoint> Network::endPoints()
{
	return endpointV;
}
	


/* ****************************************************************************
*
* samsonWorkerEndpoints - 
*/
std::vector<Endpoint> Network::samsonWorkerEndpoints()
{
    return endpointV;
}
	


/* ****************************************************************************
*
* send - 
*/
size_t Network::send(Packet* p, Endpoint* endpoint, PacketSenderInterface* sender)
{
	// Right now, local loop
	receiver->receive(p, endpoint);

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
void Network::msgTreat(Endpoint* epP)
{
	Packet  req;
	Packet  ack;
	int     s;

	LM_T(LMT_SELECT, ("treating incoming connection from '%s'", epP->name.c_str()));
	if ((s = iomMsgRead(epP, &req)) != 0)
	{
		if (s == -2) /* Connection closed */
		{
			if (epP == controller)
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
				helloSend(controller);
				return;
			}
			else if (epP == delilah)
			{
				delilah = NULL;
			}
			else /* Worker closed connection */
			{
				--me->workers;
			}

			epP->reset();
		}

		LM_RVE(("iomMsgRead: error reading message from '%s'", epP->name.c_str()));
	}

	Packet::MessageCode msgCode = (Packet::MessageCode) req.message.code();
	Packet::MessageType msgType = (Packet::MessageType) req.message.msgtype();

	switch (msgCode)
	{
	case Packet::Hello:
		char*                name;
		int                  workers;
		unsigned short       port;
		char*                ip;
		Endpoint::Type       epType;

		req.helloGet(&name, &workers, &epType, &ip, &port);
		epP->name    = std::string(name);
		epP->workers = workers;
		epP->type    = epType;

		if (port != 0)
			epP->port    = port;
		if (ip[0] != 0)
			epP->ip = std::string(ip);

		free(name);
		free(ip);

		if (msgType == Packet::Req)
		{
			LM_T(LMT_WRITE, ("sending Hello ack (name: '%s')",  me->name.c_str()));

			ack.messageCodeSet(Packet::Hello);
			ack.messageTypeSet(Packet::Ack);
			ack.helloAdd((char*) me->name.c_str(), me->workers, me->type, (char*) me->ip.c_str(), me->port);

			iomMsgSend(epP, &ack, NULL, 0);
		}
		break;

	case Packet::WorkerVector:
		if (controller != NULL)
			LM_X(1, ("Got a WorkerVector request from '%s' but I'm not the controller ...", epP->name.c_str()));

		LM_T(LMT_MSG, ("Got a WorkerVector message from '%s' (I have %d workers in my vector)", epP->name.c_str(), endpointV.size()));

		LM_T(LMT_WRITE, ("sending ack with entire worker vector"));

		if (msgType == Packet::Req)
		{
			ack.messageCodeSet(Packet::WorkerVector);
			ack.messageTypeSet(Packet::Ack);
			ack.endpointVectorAdd(endpointV);

			iomMsgSend(epP, &ack, NULL, 0);
		}
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

			if (me && (me->state == Endpoint::Listening))
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
				msgTreat(controller);
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
				{
					endpointAdd(fd, hostName);
					++me->workers;
				}

				checkInitDone();
			}
			else if ((delilah != NULL) && (delilah->state == Endpoint::Connected) && FD_ISSET(delilah->fd, &rFds))
			{
				LM_T(LMT_SELECT, ("incoming message from delilah"));
				msgTreat(delilah);
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
						msgTreat(&endpointV[ix]);
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
* endpointAdd - 
*/
void Network::endpointAdd(int fd, char* hostName)
{
	unsigned int ix = 0;

	LM_T(LMT_ENDPOINT, ("adding endpoint '%s' with fd %d", hostName, fd));
	LM_T(LMT_ENDPOINT, ("I have %d endpoints", endpointV.size()));

	while (ix < endpointV.size())
	{
		if (endpointV[ix].state > Endpoint::Taken)
		{
			++ix;
			continue;
		}

		LM_T(LMT_ENDPOINT, ("comparing '%s' to '%s' AND '%s'", hostName, endpointV[ix].ip.c_str(), endpointV[ix].hostname.c_str()));
		if ((strncmp(endpointV[ix].ip.c_str(),       hostName, strlen(endpointV[ix].ip.c_str()))       == 0)
		||  (strncmp(endpointV[ix].hostname.c_str(), hostName, strlen(endpointV[ix].hostname.c_str())) == 0))
		{
			endpointV[ix].fd    = fd;
			endpointV[ix].state = Endpoint::Connected;

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
