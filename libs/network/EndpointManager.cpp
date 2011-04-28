/* ****************************************************************************
*
* FILE                     EndpointManager.cpp - 
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 06 2011
*
*/
#include <pthread.h>              // pthread_t

#include "logMsg.h"               // LM_*
#include "traceLevels.h"          // Lmt*

#include "NetworkInterface.h"     // ss:NetworkInterface 
#include "ports.h"                // All ports for the samson system
#include "Packet.h"               // Packet
#include "Process.h"              // Process, ProcessVector
#include "platformProcesses.h"    // platformProcessesGet
#include "Host.h"                 // Host
#include "Endpoint2.h"            // Endpoint2
#include "ListenerEndpoint.h"     // ListenerEndpoint
#include "WebListenerEndpoint.h"  // WebListenerEndpoint
#include "UnhelloedEndpoint.h"    // UnhelloedEndpoint
#include "EndpointManager.h"      // Own interface



namespace ss
{



/* ****************************************************************************
*
* platformProcessLookup - 
*
* To be moved to libs/common/platformProcesses.[ch]
*/
static Process* platformProcessLookup(HostMgr* hostMgr, ProcessVector* procVec, Endpoint2::Type type, Host* host, int* ixP = NULL)
{
	for (int ix = 0; ix < procVec->processes; ix++)
	{
		Process* p = &procVec->processV[ix];

		if (p->type != (ProcessType) type)
			continue;

		if (hostMgr->match(host, p->host) == false)
			continue;

		if (ixP != NULL)
			*ixP = ix;
		return p;
	}

	return NULL;
}



/* ****************************************************************************
*
* Constructor
*/
EndpointManager::EndpointManager(Endpoint2::Type type, unsigned int _endpoints, const char* controllerIp)
{
	Host*     host;
	Process*  self = NULL;

	endpoints   = (_endpoints == 0)? 100 : _endpoints;
	workers     = 0;
	controller  = NULL;
	listener    = NULL;



	//
	// 	Type Validity Check
	//
	switch (type)
	{
	case Endpoint2::Worker:
	case Endpoint2::Controller:
	case Endpoint2::Spawner:
	case Endpoint2::Delilah:
	case Endpoint2::Supervisor:
		LM_V(("Creating Endpoint Manager for %s", Endpoint2::typeName(type)));
		break;

	default:
		LM_X(1, ("The endpoint type '%s' cannot have an Endpoint Manager", Endpoint2::typeName(type)));
	}



	//
	// Endpoints
	//
	endpoint = (Endpoint2**) calloc(endpoints, sizeof(Endpoint2*));
	if (endpoint == NULL)
		LM_X(1, ("Error allocating endpoint vector of %d endpoint pointers", endpoint));



	//
	// Host Manager
	//
	if ((hostMgr = new HostMgr(endpoints)) == NULL)
		LM_X(1, ("error creating Host Manager for %d endpoints", endpoints));

	host = hostMgr->lookup("localhost");
	if (host == NULL)
		LM_X(1, ("host manager cannot find 'localhost' ... - should have been setup it the HostMgr constructor"));



	//
	// Create the 'self' or 'me' endpoint
	//
	// Do I put this endpoint into the list or not ... ?
	// For now, it's left out from the list, just a part of EndpointManager.
	//
	me = new Endpoint2(this, type, 0, "unknown name", "unknown alias", host); // port == 0 by default
	if (me == NULL)
		LM_X(1, ("error allocating 'me' endpoint: %s", strerror(errno)));



	//
	// Process Vector, part I
	//
	// The Worker and the Controller needs the Process Vector to continue.
	// Their port number and alias are in there, for example.
	//
	if ((type == Endpoint2::Worker) || (type == Endpoint2::Controller))
	{
		int ix;

		if ((procVec = platformProcessesGet()) == NULL)
			LM_X(1, ("Error retrieving vector of platform processes"));

		if ((self = platformProcessLookup(hostMgr, procVec, type, host, &ix)) == NULL)
			LM_X(1, ("Cannot find myself in platform processes vector."));

		me->portSet(self->port);
		me->aliasSet(self->alias);        // This method could check the alias for validity ('WorkerXX', 'Controller', ...)
		me->idSet(ix);
	}
	else if (type == Endpoint2::Spawner)
	{
		me->portSet(SPAWNER_PORT);
		me->aliasSet("Spawner");
	}
	else if (type ==  Endpoint2::Delilah)
		me->aliasSet("Delilah");
	else if (type ==  Endpoint2::Supervisor)
		me->aliasSet("Supervisor");



	//
	// If this endpoint needs to accept incoming connections, add the listener endpoint
	//
	if (me->portGet() != 0)
		listener = (ListenerEndpoint*) add(Endpoint2::Listener, 0, "ME", "Listener", me->hostGet(), me->portGet(), -1, -1);
	//  listener = new ListenerEndpoint(this, Endpoint2::Listener, 0, "ME", "Listener", me->hostGet(), me->portGet(), -1, -1);



	//
	// Controller also serves as Web Listener
	//
	//if (me->typeGet() == Endpoint2::Controller)
	//	webListener = (WebListenerEndpoint*) add(Endpoint2::WebListener, 0, "ME", "Web Listener", me->hostGet(), WEB_SERVICE_PORT, -1, -1);
	//  webListener = new ListenerEndpoint(this, Endpoint2::WebListener, 0, "ME", "Web Listener", me->hostGet(), WEB_SERVICE_PORT, -1, -1);



	//
	// Process Vector, part II (Spawner only)
	//
	if (type == Endpoint2::Spawner)
	{
		if ((procVec = platformProcessesGet()) == NULL)
		{
			setupAwait();
			platformProcessesSave(procVec);

			// After this, I can start my local processes and also ...
			// ... send the procVec to all other Spawners (if I continue to do that).
		}
	}


	//
	// Workers connect to controller
	//
	if (type == Endpoint2::Worker)
	{
		Process*   p;
		Host*      hostP;
		Endpoint2* ep;

		p = &procVec->processV[0];
		if (p->type != PtController)
			LM_X(1, ("First process in process vector has to be the controller!"));

		hostP = hostMgr->lookup(p->host);
		if (hostP == NULL)
			hostP = hostMgr->insert(p->host, NULL);

		ep = add(Endpoint2::Controller, 0, p->name, p->alias, hostP, p->port, -1, -1);
		ep->connect();
	}



	//
	// Delilahs connect to controller
	//
	// Delilahs need to ask the Controller for the platform process list
	//
	//
	if (type == Endpoint2::Delilah)
	{
		Host* hostP;

		hostP = hostMgr->lookup(controllerIp);
		if (hostP == NULL)
			hostP = hostMgr->insert(controllerIp, NULL);

		controller = add(Endpoint2::Controller, 0, "Controller", "Controller", hostP, CONTROLLER_PORT, -1, -1);
		controller->connect();
		return;
	}


	

	//
	// Workers to connect to all workers with an identifier (ep->id) less than own id
	//
	if (type == Endpoint2::Worker)
	{
		for (int ix = 1; ix < procVec->processes; ix++)
		{
			Process*    p;
			Host*       hostP;
			Endpoint2*  ep;

			p = &procVec->processV[ix];
			if (p->type != PtWorker)
				LM_X(1, ("Process %d in process vector has to be a worker - corrupt platform processes file!", ix));

			hostP = hostMgr->lookup(p->host);
			if (hostP == NULL)
				hostP = hostMgr->insert(p->host, NULL);

			ep = add(Endpoint2::Worker, ix, p->name, p->alias, hostP, p->port, -1, -1);
			if (ep->idGet() < me->idGet())
				ep->connect();  // Connect, but don't add to endpoint vector
		}
	}
}



/* ****************************************************************************
*
* Destructor
*/
EndpointManager::~EndpointManager()
{
    if (endpoint != NULL)
		delete endpoint;

    if (hostMgr != NULL)
		delete hostMgr;

	if (procVec != NULL)
		delete procVec;
}



/* ****************************************************************************
*
* endpointRunInThread - 
*/
static void* endpointRunInThread(void* vP)
{
	Endpoint2* ep = (Endpoint2*) vP;

	LM_M(("Calling run for endpoint %s@%s", ep->nameGet(), ep->hostGet()->name));
	ep->run();

	ep->stateSet(Endpoint2::ScheduledForRemoval);
	LM_W(("Endpoint '%s@%s' is back from Endpoint2::run", ep->nameGet(), ep->hostGet()->name));

	return NULL;
}



/* ****************************************************************************
*
* add - 
*/
Endpoint2* EndpointManager::add(Endpoint2* ep)
{
	for (unsigned int ix = 0; ix < endpoints; ix++)
	{
		if (endpoint[ix] != NULL)
			continue;

		endpoint[ix] = ep;

		if ((ep->type == Endpoint2::Controller) || (ep->type == Endpoint2::Worker) || (ep->type == Endpoint2::Delilah))
		{
			int        s;
			pthread_t  tid;

			LM_M(("Creating thread for endpoint %s@%s", ep->name, ep->host->name));
			if ((s = pthread_create(&tid, NULL, endpointRunInThread, ep)) != 0)
			{
				LM_E(("pthread_create returned %d for %s@%s", s, ep->name, ep->host->name));
				remove(ep);
				return NULL;
			}
		}
		return ep;
	}

	LM_X(1, ("No room in endpoint vector (%d occupied slots) - realloc?", endpoints));
	return NULL;
}



/* ****************************************************************************
*
* add - 
*/
Endpoint2* EndpointManager::add(Endpoint2::Type type, int id, const char* name, const char* alias, Host* host, unsigned short port, int rFd, int wFd)
{
	Endpoint2* ep;

	switch (type)
	{
	case Endpoint2::Listener:
		ep = new ListenerEndpoint(this, id, name, alias, host, port, rFd, wFd);
		break;

	case Endpoint2::Unhelloed:
		ep = new UnhelloedEndpoint(this, id, name, alias, host, port, rFd, wFd);
		break;

	default:
		LM_X(1, ("Please Implement!"));
	}

	if (ep == NULL)
		LM_X(1, ("Error allocating endpoint of %d bytes", sizeof(Endpoint2)));

	return add(ep);
}



/* ****************************************************************************
*
* remove - 
*/
void EndpointManager::remove(Endpoint2* ep)
{
	if (ep == NULL)
		LM_RVE(("cannot remove NULL endpoint"));

	for (unsigned int ix = 0; ix < endpoints; ix++)
    {
        if (endpoint[ix] != ep)
            continue;

		endpoint[ix] = NULL;
		delete ep;
	}
}



/* ****************************************************************************
*
* get - 
*/
Endpoint2* EndpointManager::get(unsigned int index)
{
	if (index >= endpoints)
		return NULL;

	return endpoint[index];
}



/* ****************************************************************************
*
* get - 
*/
Endpoint2* EndpointManager::get(unsigned int index, int* rFdP)
{
	unsigned int found = 0;

	for (unsigned int ix = 0; ix <= endpoints; ix++)
	{
		if (index == endpoints)
			return NULL;

		if (endpoint[ix] == NULL)
			continue;

		if (index == found++)
		{
			if (rFdP != NULL)
				*rFdP = endpoint[index]->rFdGet();
			return endpoint[index];
		}
	}

	return NULL;
}



/* ****************************************************************************
*
* lookup - 
*/
Endpoint2* EndpointManager::lookup(Endpoint2::Type type, const char* host)
{
	for (unsigned int ix = 0; ix < endpoints; ix++)
    {
        if (endpoint[ix] == NULL)
            continue;

		if (hostMgr->match(endpoint[ix]->hostGet(), host) == true)
			return endpoint[ix];
	}

	return NULL;
}



/* ****************************************************************************
*
* lookup
*/
Endpoint2* EndpointManager::lookup(const char* alias)
{
	if ((alias == NULL) || (alias[0] == 0))
		LM_RE(NULL, ("NULL or empty alias"));

	for (unsigned int ix = 0; ix < endpoints; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (strcmp(endpoint[ix]->aliasGet(), alias) == 0)
			return endpoint[ix];
	}
		
	return NULL;
}



/* ****************************************************************************
*
* list
*/
void EndpointManager::list(const char* why, bool forced)
{
	bool savedVerbose = lmVerbose;

	if (forced)
		lmVerbose = true;

	for (unsigned int ix = 0; ix < endpoints; ix++)
	{
		Endpoint2* ep;

		if (endpoint[ix] == NULL)
			continue;

		ep = endpoint[ix];
		LM_V(("%c %02d: %-20s %-20s %-20s", (ep->rFd == -1)? '-' : '+', ix, ep->aliasGet(), ep->nameGet(), ep->hostname()));
	}

	if (forced)
		lmVerbose = savedVerbose;
}



/* ****************************************************************************
*
* setupAwait - 
*/
void EndpointManager::setupAwait(void)
{
	if (listener == NULL)
		LM_X(1, ("Cannot await the setup to arrive if I have no Listener ..."));

	LM_V(("Awaiting samsonSetup/Spawner to connect and forward the Process Vector"));
	while (1)
	{
		UnhelloedEndpoint*    ep;
		Message::Header       header;
		int                   dataLen = 0;
		void*                 dataP   = NULL;
		Message::HelloData*   helloP;
		Packet                packet;


		// Await FOREVER for an incoming connection
		if (listener->msgAwait(-1, -1) != 0)
			LM_X(1, ("Endpoint2::msgAwait error"));


		if ((ep = listener->accept()) == NULL)
		{
			LM_E(("error accepting an incoming connection"));
			continue;
		}


		// Await an incoming message during 5 seconds
		if (ep->msgAwait(5, 0) != 0)
		{
			LM_E(("Endpoint2::msgAwait error, expecting a Hello"));
			delete ep;
			continue;
		}


		// Receiving ?Hello? data ...
		if (ep->receive(&header, &dataP, &dataLen, &packet) != 0)
		{
			LM_E(("Endpoint2::receive error"));
			delete ep;
			continue;
		}


		// Checking validity of message
		if ((header.code != Message::Hello) || (header.type != Message::Msg))
		{
			LM_E(("Message read not a Hello Msg (%s %s)", messageCode(header.code), messageType(header.type)));
			delete ep;
			continue;
		}


		// Is the newly connected peer a 'samsonSetup' ?
		helloP = (Message::HelloData*) dataP;
		if (helloP->type != Endpoint2::Setup) //   && (helloP->type != Endpoint2::Spawner) ???
		{
			LM_E(("The incoming connection was from a '%s'", Endpoint2::typeName((Endpoint2::Type) helloP->type)));
			delete ep;
			continue;
		}


		// Responding to the Hello
		if (ep->helloSend(Message::Ack) != 0)
		{
			LM_E(("error acking Hello to '%s@%s'", helloP->alias, ep->hostname()));
			delete ep;
			continue;
		}


		// Hello exchanged, now the endpoint will send a ProcessVector message
		// Awaiting the message to arrive 
		if (listener->msgAwait(5, 0) != 0)
		{
			LM_E(("5 second timeout expired, expecting a ProcessVector"));
			delete ep;
			continue;
		}


		// Reading the message
		dataP   = NULL;
		dataLen = 0;
		if (ep->receive(&header, &dataP, &dataLen, &packet) != 0)
		{
			LM_E(("Endpoint2::receive error"));
			delete ep;
			continue;
		}


		// All OK ?
		if ((header.code != Message::ProcessVector) || (header.type != Message::Msg))
		{
			LM_E(("Message read not a ProcessVector Msg (%s %s)", messageCode(header.code), messageType(header.type)));
			if (dataP != NULL)
				free(dataP);
			delete ep;
			continue;
		}


		// Copying 
		this->procVec = (ProcessVector*) malloc(dataLen);
		if (this->procVec == NULL)
			LM_X(1, ("Error allocating %d bytes for the Process Vector", dataLen));

		memcpy(this->procVec, dataP, dataLen);
		free(dataP);

		// Await samsonSetup to close connection ?
		// What if it's another Spawner ... ? 
		delete ep;

		return;
	}
}


/* ****************************************************************************
*
* run - 
*/
void EndpointManager::run(bool oneShot)
{
	Endpoint2*       ep;
	int              ix;
	fd_set           rFds;
	int              max;
	struct timeval   tv;
	int              fds;

	if ((listener == NULL) && (webListener == NULL))
		LM_X(1, ("No listeners ..."));
		
	while (1)
	{
		// Call cleanup function that removes all endpoints marked as 'ScheduledForRemoval' 

		// Populate rFds for select
		do
		{
			FD_ZERO(&rFds);
			ix   = 0;
			max  = 0;

			for (unsigned int ix = 0; ix < endpoints; ix++)
			{
				ep = endpoint[ix];

				if (ep == NULL)
					continue;
				if ((ep->type != Endpoint2::Listener) && (ep->type != Endpoint2::WebListener) && (ep->type != Endpoint2::Unhelloed))
					continue;

				max = MAX(max, ep->rFd);
				FD_SET(ep->rFd, &rFds);
			}

			tv.tv_sec  = tmoSecs;
			tv.tv_usec = tmoUSecs;
			
			fds = select(max + 1,  &rFds, NULL, NULL, &tv);
		} while ((fds == -1) && (errno == EINTR));

		if (fds == -1)
			LM_X(1, ("select: %s", strerror(errno)));
		else if (fds == 0)
		{
			LM_D(("timeout"));
		}
		else
		{
			ep = NULL;
			if ((listener != NULL) && (FD_ISSET(listener->rFd, &rFds)))
				listener->msgTreat();

			if ((webListener != NULL) && (FD_ISSET(webListener->rFd, &rFds)))
				webListener->msgTreat();
		}

		if (oneShot)
			return;
	}
}



/* ****************************************************************************
*
* setPacketReceiver - 
*/
void EndpointManager::setPacketReceiver(PacketReceiverInterface* receiver)
{
	packetReceiver = receiver;
}

}
