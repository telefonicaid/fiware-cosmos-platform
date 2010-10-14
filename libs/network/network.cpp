/* ****************************************************************************
*
* FILE                     network.cpp - Definition for the network interface
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Oct 11 2010
*
*/
#include <sys/select.h>         // select

#include "logMsg.h"             // LM_*
#include "networkTraceLevels.h" // LMT_NWRUN, ...

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
* Constructor 
*/
NetworkInterface::NetworkInterface()
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
void NetworkInterface::setPacketReceiver(PacketReceiverInterface* _receiver)
{
	receiver = _receiver;
}

	

/* ****************************************************************************
*
* init - open listen socket on port specifid by 'me' endpoint
*/
void NetworkInterface::init(Endpoint myEndpoint)
{
	me = new Endpoint(myEndpoint);

	if (me == NULL)
		LM_XP(1, ("unable to allocate room for Endpoint 'me'"));

	me->name = "accepter";
	me->fd   = iomServerOpen(me->port);
	if (me->fd == -1)
		LM_XP(1, ("unable to open port %d for listening", me->port));

	LM_T(LMT_FDS, ("opened fd %d to accept incoming connections", me->fd));

	me->state = Endpoint::Listening;
}



/* ****************************************************************************
*
* initAsSamsonController - 
*/
void NetworkInterface::initAsSamsonController(Endpoint myEndpoint, std::vector<Endpoint> endpoints)
{
	unsigned int  ix;

	LM_T(LMT_SELECT, ("endpointV.size: %d", endpointV.size()));
	endpointV = endpoints;
	LM_T(LMT_SELECT, ("endpointV.size: %d", endpointV.size()));

	init(myEndpoint);

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
* initAsSamsonWorker - 
*
* NOTE
*/
void NetworkInterface::initAsSamsonWorker(Endpoint myEndpoint, Endpoint controllerEndpoint)
{
	Packet  packet(Packet::WorkerVector);
	Packet  ackPacket;

	init(myEndpoint);

	myEndpoint.name   = "accepter";
	controller        = new Endpoint(controllerEndpoint);
	controller->name  = "controller";

	iomInit(controller);

	/* ask controller for list of workers */
	iomMsgSend(controller, &packet, NULL, 0);
	iomMsgRead(controller, &ackPacket, controller);

	if (!ackPacket.message.has_endpoints())
		LM_X(1, ("controller didn't give me the Worker Vector ..."));

	int ix;

	for (ix = 0; ix < ackPacket.getNumEndpoints(); ix++)
	{
		int       fd;
		Endpoint  endpoint = Endpoint(ackPacket.getEndpoint(ix));

		LM_T(LMT_WORKERS, ("Connect to worker %d: %s (host %s, port %d)", ix, endpoint.name.c_str(), endpoint.ip.c_str(), endpoint.port));
		fd = iomConnect(endpoint.ip.c_str(), endpoint.port);
		if (fd != -1)
		{
			endpoint.fd    = fd;
			endpoint.state = Endpoint::Connected;
		}

		endpointV.push_back(endpoint);
	}

	LM_T(LMT_WORKERS, ("I now have %d workers in my vector", endpointV.size()));

	// delete ackPacket;
}



/* ****************************************************************************
*
* initAsDelailah - 
*/
void NetworkInterface::initAsDelailah(Endpoint controllerEndpoint)
{
	controller        = new Endpoint(controllerEndpoint);
    controller->name  = "controller";
	
	iomInit(controller);
}
	
	

/* ****************************************************************************
*
* me - 
*/
Endpoint* NetworkInterface::meGet()
{
	return me;
}
	



/* ****************************************************************************
*
* controller - 
*/
Endpoint* NetworkInterface::controllerGet()
{
	return controller;
}



/* ****************************************************************************
*
* worker - 
*/
Endpoint* NetworkInterface::workerGet(int i)
{
	return &endpointV[i];
}


	

/* ****************************************************************************
*
* worker - 
*/
int NetworkInterface::worker( Endpoint endPoint )
{
	return -1;
}
	
	

/* ****************************************************************************
*
* 
*/
std::vector<Endpoint> NetworkInterface::endPoints()
{
	return endpointV;
}
	


/* ****************************************************************************
*
* samsonWorkerEndpoints - 
*/
std::vector<Endpoint> NetworkInterface::samsonWorkerEndpoints()
{
    return endpointV;
}
	


/* ****************************************************************************
*
* send - 
*/
size_t NetworkInterface::send(Packet* p, Endpoint* endpoint, PacketSenderInterface* sender)
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
bool NetworkInterface::ready()
{
	return iAmReady;
}



/* ****************************************************************************
*
* msgTreat - 
*/
void NetworkInterface::msgTreat(Endpoint* epP)
{
	Packet outPacket;

	LM_T(LMT_SELECT, ("treating incoming connection from '%s'", epP->name.c_str()));
	if (iomMsgRead(epP, &outPacket, controller) != 0)
		LM_RVE(("iomMsgRead: error reading message from '%s'", epP->name.c_str()));

	Packet::MessageCode msgCode = (Packet::MessageCode) outPacket.message.code();
	switch (msgCode)
	{
	case Packet::Hello:
		LM_X(1, ("Got a Hello message from '%s'", epP->name.c_str()));
		break;

	case Packet::WorkerVector:
		if (controller != NULL)
			LM_X(1, ("Got a WorkerVector request from '%s' but I'm not the controller ...", epP->name.c_str()));

		LM_T(LMT_MSG, ("Got a WorkerVector message from '%s' (I have %d workers in my vector)", epP->name.c_str(), endpointV.size()));
		Packet ack;

		ack.setMessageCode(Packet::WorkerVector);
		ack.addEndPoints(endpointV);
		LM_T(LMT_WRITE, ("sending ack with entire worker vector"));
		iomMsgSend(epP, &ack);
	}
}



/* ****************************************************************************
*
* run - 
*/
void NetworkInterface::run()
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
					LM_T(LMT_SELECT, ("added worker %d (%s:%d) - state '%s'", ix, endpointV[ix].ip.c_str(), endpointV[ix].port, endpointV[ix].stateName()));
				}
				else
					LM_T(LMT_SELECT, ("Not adding worker %d (%s:%d) - state '%s'", ix, endpointV[ix].ip.c_str(), endpointV[ix].port, endpointV[ix].stateName()));
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
					endpointAdd(fd, hostName);

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
void NetworkInterface::checkInitDone(void)
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
void NetworkInterface::endpointAdd(int fd, char* hostName)
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

		LM_T(LMT_ENDPOINT, ("comparing '%s' to '%s'", endpointV[ix].ip.c_str(), hostName));
		if (strcmp(endpointV[ix].ip.c_str(), hostName) == 0)
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
void NetworkInterface::quit()
{
}	

}
