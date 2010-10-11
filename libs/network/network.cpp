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
#include "Endpoint.h"			// Endpoint
#include "Packet.h"				// Packet
#include "iomInit.h"            // iomInit
#include "iomServerOpen.h"      // iomServerOpen
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

	me->fd   = iomServerOpen(me->port);
	if (me->fd == -1)
		LM_XP(1, ("unable to open port %d for listening", me->port));

	me->state = Endpoint::Listening;
}



/* ****************************************************************************
*
* initAsSamsonController - 
*/
void NetworkInterface::initAsSamsonController(Endpoint myEndpoint, std::vector<Endpoint> endpoints)
{
	unsigned int ix;

	init(myEndpoint);

	ix = 0;
	for (ix = 0; ix < endpoints.size(); ix++)
		endpoints[ix].state = Endpoint::Taken;
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

	while (1)
	{
		do
		{
			timeVal.tv_sec  = 2;
			timeVal.tv_usec = 0;

			FD_ZERO(&rFds);
			max = 0;

			if (me->state == Endpoint::Listening)
			{
				FD_SET(me->fd, &rFds);
				max = MAX(max, me->fd);
			}

			if (controller->state == Endpoint::Connected)
			{
				FD_SET(controller->fd, &rFds);
				max = MAX(max, controller->fd);
			}

			if (delilah->state == Endpoint::Connected)
			{
				FD_SET(delilah->fd, &rFds);
				max = MAX(max, delilah->fd);
			}

			unsigned int ix;
			for (ix = 0; ix < endpointV.size(); ix++)
			{
				if (endpointV[ix].state == Endpoint::Connected)
				{
					FD_SET(endpointV[ix].fd, &rFds);
					max = MAX(max, endpointV[ix].fd);
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
			if ((controller->state == Endpoint::Connected) && FD_ISSET(controller->fd, &rFds))
				;
			else if ((me->state == Endpoint::Listening) && FD_ISSET(me->fd, &rFds))
				;
			else if ((delilah->state == Endpoint::Connected) && FD_ISSET(delilah->fd, &rFds))
				;
			else
			{
				unsigned int ix;
				for (ix = 0; ix < endpointV.size(); ix++)
				{
					if (FD_ISSET(endpointV[ix].fd, &rFds))
						;
				}
			}
		}
	}
}



/* ****************************************************************************
*
* quit - 
*/
void NetworkInterface::quit()
{
}	

}
