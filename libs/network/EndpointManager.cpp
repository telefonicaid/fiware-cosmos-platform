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
#include "WorkerEndpoint.h"       // WorkerEndpoint
#include "ControllerEndpoint.h"   // ControllerEndpoint
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
EndpointManager::EndpointManager(Endpoint2::Type type, const char* controllerIp)
{
	workers     = 0;
	controller  = NULL;
	listener    = NULL;



	//
	// Create the endpoint vector
	//
	endpoints = 100;
	endpoint  = (Endpoint2**) calloc(endpoints, sizeof(Endpoint2*));
	if (endpoint == NULL)
		LM_X(1, ("Error allocating endpoint vector of %d endpoint pointers", endpoint));



	//
	// Create Host Manager
	//
	Host* host;

	if ((hostMgr = new HostMgr(endpoints)) == NULL)
		LM_X(1, ("error creating Host Manager for %d endpoints", endpoints));

	host = hostMgr->lookup("localhost");
	if (host == NULL)
		LM_X(1, ("host manager cannot find 'localhost' ... - should have been setup it the HostMgr constructor"));



	//
	// Create the endpoint 'me'
	//
	me = new Endpoint2(this, type, 0, "unknown name", "unknown alias", host); // port == 0 by default
	if (me == NULL)
		LM_X(1, ("error allocating 'me' endpoint: %s", strerror(errno)));



	//
	// Endpoint-type specialized initializations
	//
	// These methods to be moved to their respective executables
	//
	switch (type)
	{
	case Endpoint2::Worker:        initWorker();                 break;
	case Endpoint2::Controller:    initController();             break;
	case Endpoint2::Spawner:       initSpawner();                break;
	case Endpoint2::Delilah:       initDelilah(controllerIp);    break;
	case Endpoint2::Supervisor:    initSupervisor();             break;

    default:
		LM_X(1, ("The endpoint type '%s' cannot have an Endpoint Manager", Endpoint2::typeName(type)));
	}
}



/* ****************************************************************************
*
* initWorker - 
*
* This method should probably be moved to apps/worker
*/
void EndpointManager::initWorker(void)
{
	//
	// Process Vector
	//
	int       ix;
	Process*  self = NULL;

	if ((procVec = platformProcessesGet()) == NULL)
		LM_X(1, ("Error retrieving vector of platform processes"));

	if ((self = platformProcessLookup(hostMgr, procVec, Endpoint2::Worker, me->host, &ix)) == NULL)
		LM_X(1, ("Cannot find myself in platform processes vector."));
	
	me->aliasSet(self->alias);        // This method could check the alias for validity ('WorkerXX', 'Controller', ...)
	me->idSet(ix);
	me->portSet(WORKER_PORT);



	//
	// Opening listener to accept incoming  connections
	//
	listener = (ListenerEndpoint*) add(Endpoint2::Listener, 0, "ME", "Listener", me->hostGet(), me->portGet(), -1, -1);
	if (listener == NULL)
		LM_X(1, ("error creating listener endpoint - no use to continue ..."));



	//
	// Connect to Controller
	//
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



/* ****************************************************************************
*
* initController - 
*
* This method should probably be moved to apps/controller
*/
void EndpointManager::initController(void)
{
	//
	// Process Vector
	//
	Process*  self;

	if ((procVec = platformProcessesGet()) == NULL)
		LM_X(1, ("Error retrieving vector of platform processes"));

	if ((self = platformProcessLookup(hostMgr, procVec, Endpoint2::Controller, me->host)) == NULL)
		LM_X(1, ("Cannot find myself in platform processes vector."));
	
	me->aliasSet(self->alias);        // This method could check the alias for validity ('WorkerXX', 'Controller', ...)
	me->portSet(CONTROLLER_PORT);



	//
	// Opening listeners to accept incoming connections
	//
	listener    = (ListenerEndpoint*)    add(Endpoint2::Listener,    0, "ME", "Listener",     me->hostGet(), me->portGet(),    -1, -1);
	webListener = (WebListenerEndpoint*) add(Endpoint2::WebListener, 0, "ME", "Web Listener", me->hostGet(), WEB_SERVICE_PORT, -1, -1);

	if (listener == NULL)
		LM_X(1, ("error creating listener endpoint - no use to continue ..."));
	if (webListener == NULL)
		LM_W(("error creating web listener endpoint!"));
}



/* ****************************************************************************
*
* initSpawner - 
*
* This method should probably be moved to apps/spawner
*/
void EndpointManager::initSpawner(void)
{
	me->portSet(SPAWNER_PORT);
	me->aliasSet("Spawner");

	listener = (ListenerEndpoint*) add(Endpoint2::Listener, 0, "ME", "Listener", me->hostGet(), me->portGet(), -1, -1);


	if ((procVec = platformProcessesGet()) == NULL)
	{
		setupAwait();
		platformProcessesSave(procVec);
	}
}



/* ****************************************************************************
*
* initDelilah - 
*
* This method should probably be moved to apps/delilah
*/
void EndpointManager::initDelilah(const char* controllerIp)
{
	//
	// alias
	//
	me->aliasSet("Delilah");



	//
	// Connect to controller
	//
	Host* hostP;

	hostP = hostMgr->lookup(controllerIp);
	if (hostP == NULL)
		hostP = hostMgr->insert(controllerIp, NULL);

	controller = add(Endpoint2::Controller, 0, "Controller", "Controller", hostP, CONTROLLER_PORT, -1, -1);
	controller->connect();



	//
	// Delilahs need to ask the Controller for the platform process list
	//
	LM_X(1, ("Delilah init needs to be implemented"));
}



/* ****************************************************************************
*
* initSupervisor - 
*/
void EndpointManager::initSupervisor(void)
{
	me->aliasSet("Supervisor");

	LM_X(1, ("Supervisor init needs to be implemented"));
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
	Endpoint2* ep = NULL;

	switch (type)
	{
	case Endpoint2::Listener:
		ep = new ListenerEndpoint(this, id, name, alias, host, port, rFd, wFd);
		break;

	case Endpoint2::Unhelloed:
		ep = new UnhelloedEndpoint(this, id, name, alias, host, port, rFd, wFd);
		break;

	case Endpoint2::Worker:
		ep = new WorkerEndpoint(this, id, name, alias, host, port, rFd, wFd);
		break;

	case Endpoint2::Controller:
		ep = new ControllerEndpoint(this, id, name, alias, host, port, rFd, wFd);
		break;

	default:
		LM_X(1, ("Please Implement addition of '%s' endpoint!", Endpoint2::typeName(type)));
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
Endpoint2* EndpointManager::lookup(Endpoint2::Type type, int id, int* ixP)
{
	for (unsigned int ix = 0; ix < endpoints; ix++)
    {
        if (endpoint[ix] == NULL)
            continue;

		if ((endpoint[ix]->type == true) && (endpoint[ix]->id == id))
		{
			if (ixP != NULL)
				*ixP = ix;
			return endpoint[ix];
		}
	}
	return NULL;
}



/* ****************************************************************************
*
* lookup - 
*/
Endpoint2* EndpointManager::lookup(Endpoint2::Type typ, const char* host)
{
	for (unsigned int ix = 0; ix < endpoints; ix++)
    {
        if (endpoint[ix] == NULL)
            continue;

		if (endpoint[ix]->type != typ)
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
		Packet                packet(Message::Unknown);


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
			show("timeout");
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



/* ****************************************************************************
*
* setDataReceiver - 
*/
void EndpointManager::setDataReceiver(DataReceiverInterface* receiver)
{
	dataReceiver = receiver;
}



/* ****************************************************************************
*
* endpointCount - 
*/
int EndpointManager::endpointCount(Endpoint2::Type epType)
{
	int noOf = 0;

	for (unsigned int ix = 0; ix < endpoints; ix++)
    {
        if (endpoint[ix] == NULL)
            continue;

		if (endpoint[ix]->type == epType)
			++noOf;
	}

	return noOf;
}



/* ****************************************************************************
*
* send - 
*/
size_t EndpointManager::send(PacketSenderInterface* psi, int endpointIx, Packet* packetP)
{
	if (endpoint[endpointIx] == NULL)
		LM_RE(1, ("Cannot send to endpoint %d - NULL", endpointIx));

	return endpoint[endpointIx]->send(psi, packetP->msgCode, packetP);
}



/* ****************************************************************************
*
* multiSend - 
*/
int EndpointManager::multiSend(PacketSenderInterface* psi, Endpoint2::Type typ, Packet* packetP)
{
	int sends = 0;

	for (unsigned int ix = 0; ix < endpoints; ix++)
	{
		if (endpoint[ix] != NULL)
			continue;

		if (endpoint[ix]->type != typ)
			continue;

		send(psi, ix, packetP); // Probably need to 'new' packetP so it wont be deleted before used by all ...
		++sends;
	}

	return sends;
}



/* ****************************************************************************
*
* multiSend - 
*/
int EndpointManager::multiSend(Endpoint2::Type typ, Message::MessageCode code, void* dataP, int dataLen)
{
	int sends = 0;

	for (unsigned int ix = 0; ix < endpoints; ix++)
	{
		if (endpoint[ix] != NULL)
			continue;

		if (endpoint[ix]->type != typ)
			continue;

		endpoint[ix]->send(Message::Msg, code, dataP, dataLen);
		++sends;
	}

	return sends;
}



/* ****************************************************************************
*
* show - 
*/
void EndpointManager::show(const char* why, bool forced)
{
	bool verbose = lmVerbose;

	if (forced)
		lmVerbose = true;

	LM_V((""));
	LM_V(("-------------------- Endpoint list (%s) ---------------", why));
	LM_V((""));
	LM_V(("ix  %-12s id  %-20s Port", "Type", "Name"));
	LM_V(("-------------------------------------------------------"));

	for (unsigned int ix = 0; ix < endpoints; ix++)
	{
		Endpoint2* ep;

		ep = endpoint[ix];
		if (ep == NULL)
			continue;

		LM_V(("%02d: %-12s %02d  %-20s %d", ix, ep->typeName(), ep->idGet(), ep->nameGet(), ep->port));
	}
	LM_V(("-------------------------------------------------------"));

	if (forced)
		lmVerbose = verbose;
}



/* ****************************************************************************
*
* procVecSet - 
*/
int EndpointManager::procVecSet(ProcessVector* _procVec)
{
	int size;

	if ((_procVec->processes <= 0) || (_procVec->processes > 100))
		LM_RE(1, ("Bad number of processes in process vector: %d", _procVec->processes));

	if (procVec)
		free(procVec);

	size    = sizeof(ProcessVector) + _procVec->processes * sizeof(Process);
	procVec = (ProcessVector*) malloc(size);
	memcpy(procVec, _procVec, size);

	platformProcessesSave(procVec);
	LM_W(("sleeping for a second after  saving Process Vector to file system"));
	sleep(1);

	return 0;
}



/* ****************************************************************************
*
* procVecGet - 
*/
ProcessVector* EndpointManager::procVecGet(void)
{
	return procVec;
}



/* ****************************************************************************
*
* tmoSet - 
*/
void EndpointManager::tmoSet(int secs, int usecs)
{
	tmoSecs   = secs;
	tmoUSecs  = usecs;
}

}
