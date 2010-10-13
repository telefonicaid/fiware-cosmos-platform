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
#include "Packet.h"				// Packet
#include "iomInit.h"            // iomInit
#include "iomServerOpen.h"      // iomServerOpen
#include "iomAccept.h"          // iomAccept
#include "network.h"			// Own interface



namespace ss
{



/* ****************************************************************************
*
* Constructor 
*/
NetworkInterface::NetworkInterface()
{
	receiver = NULL;
}



/* ****************************************************************************
*
* Constructor 
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

	me->fd = iomServerOpen(me->port);
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
		LM_T(LMT_ENDPOINTS, ("endpointV.ip: '%s'", endpointV[ix].ip.c_str()));
		endpoints[ix].state = Endpoint::Taken;
	}
}



/* ****************************************************************************
*
* initAsSamsonWorker - 
*/
void NetworkInterface::initAsSamsonWorker(Endpoint myEndpoint, Endpoint controllerEndpoint)
{
	init(myEndpoint);
	controller = new Endpoint(controllerEndpoint);

	iomInit(controller);
}



/* ****************************************************************************
*
* initAsDelailah - 
*/
void NetworkInterface::initAsDelailah(Endpoint controllerEndpoint)
{
	controller = new Endpoint(controllerEndpoint);

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
	return false;
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
			timeVal.tv_sec  = 2;
			timeVal.tv_usec = 0;

			FD_ZERO(&rFds);
			max = 0;

			if (me && (me->state == Endpoint::Listening))
			{
				FD_SET(me->fd, &rFds);
				max = MAX(max, me->fd);
				LM_T(LMT_SELECT, ("Added my listen fd %d to fd-list", me->fd));
			}

			if (controller && (controller->state == Endpoint::Connected))
			{
				FD_SET(controller->fd, &rFds);
				max = MAX(max, controller->fd);
				LM_T(LMT_SELECT, ("Added controller fd %d to fd-list", controller->fd));
			}

			if (delilah && (delilah->state == Endpoint::Connected))
			{
				FD_SET(delilah->fd, &rFds);
				max = MAX(max, delilah->fd);
				LM_T(LMT_SELECT, ("Added delilah fd %d to fd-list", delilah->fd));
			}

			unsigned int ix;
			LM_T(LMT_SELECT, ("endpointV.size: %d", endpointV.size()));
			for (ix = 0; ix < endpointV.size(); ix++)
			{
				LM_T(LMT_SELECT, ("checking endpoint %d (state %d)", ix, endpointV[ix].state));
				if (endpointV[ix].state == Endpoint::Connected)
				{
					FD_SET(endpointV[ix].fd, &rFds);
					max = MAX(max, endpointV[ix].fd);
					LM_T(LMT_SELECT, ("Added worker fd %d to fd-list", endpointV[ix].fd));
				}
			}

			LM_T(LMT_SELECT, ("Awaiting on select (%d.%06d seconds timeout)", timeVal.tv_sec, timeVal.tv_usec));
			fds = select(max + 1, &rFds, NULL, NULL, &timeVal);
			LM_T(LMT_SELECT, ("select returned %d", fds));
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
				LM_T(LMT_SELECT, ("incoming message from controller"));
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
			}
			else if ((delilah->state == Endpoint::Connected) && FD_ISSET(delilah->fd, &rFds))
				LM_T(LMT_SELECT, ("incoming message from delilah"));
			else
			{
				unsigned int ix;
				for (ix = 0; ix < endpointV.size(); ix++)
				{
					if (FD_ISSET(endpointV[ix].fd, &rFds))
						LM_T(LMT_SELECT, ("incoming message from worker %d", ix));
				}
			}
		}
	}
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
		if (endpointV[ix].state == Endpoint::Free)
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
		}
		++ix;
	}

	exit(1);
}



/* ****************************************************************************
*
* quit - 
*/
void NetworkInterface::quit()
{
}	

}
