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
#include "SpawnerEndpoint.h"      // SpawnerEndpoint
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
		Process* p           = &procVec->processV[ix];
		Host*    otherHost;

		if (p->type != (ProcessType) type)
			continue;

		otherHost = hostMgr->lookup(p->host);
		if (otherHost != host)
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
	procVec     = NULL;


	//
	// Nulling the callback vector
	//
	for (unsigned int ix = 0; ix < sizeof(callback) / sizeof(callback[0]); ix++)
		callback[ix].func = NULL;



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
	//  Who sets name and alias?
	//    Controller:  when ProcessVector is read from file
	//    Worker:      when ProcessVector is read from file
	//    Spawner:     NOWHERE !!!
	//    Delilah:     NOWHERE !!!
	//
	me = new Endpoint2(this, type, -1, NULL, NULL, host); // id == -1, port == 0 ...
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
	case Endpoint2::Setup:         initSetup();                  break;

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
	int       ix=0;
	Process*  self = NULL;

	if ((procVec = platformProcessesGet()) == NULL)
		LM_X(1, ("Error retrieving vector of platform processes"));

	LM_M(("Got %d processes in procVec", procVec->processes));
	for (int ix = 0; ix < procVec->processes; ix++)
		LM_M(("Process %d: type %d, host %s", ix, procVec->processV[ix].type, procVec->processV[ix].host));

	LM_M(("me->host: %s", me->host->name));

	if ((self = platformProcessLookup(hostMgr, procVec, Endpoint2::Worker, me->host, &ix)) == NULL)
		LM_X(1, ("Cannot find myself in platform processes vector."));
	
	me->aliasSet(self->alias);        // This method could check the alias for validity ('WorkerXX', 'Controller', ...)
	me->idSet(self->id);
	me->portSet(WORKER_PORT);



	//
	// Opening listener to accept incoming connections
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
		
		LM_M(("************* SAVING Process Vector"));
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
* initSetup - 
*/
void EndpointManager::initSetup(void)
{
	me->aliasSet("Setup");

	LM_W(("Nothing done here, not sure if anything is needed ..."));
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
* readerThread - 
*/
static void* readerThread(void* vP)
{
	Endpoint2* ep = (Endpoint2*) vP;

	LM_M(("reader thread for endpoint %s@%s is running", ep->nameGet(), ep->hostGet()->name));
	ep->run();

	ep->stateSet(Endpoint2::ScheduledForRemoval);
	LM_W(("Endpoint '%s@%s' is back from Endpoint2::run", ep->nameGet(), ep->hostGet()->name));

	return NULL;
}



/* ****************************************************************************
*
* writerThread - 
*/
static void* writerThread(void* vP)
{
	Endpoint2* ep  = (Endpoint2*) vP;
	int        wFd = ep->wFdGet();

	LM_M(("writer thread for endpoint %s@%s is running (wFd: %d))", ep->nameGet(), ep->hostGet()->name, wFd));
	while (1)
	{
		// Await message from father,
		// read the message, and ...
		// forward the message to 'wFd'
	}

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
		LM_M(("Added endpoint %d. type:%s, id:%d, name:%s, host:%s", ix, ep->typeName(), ep->idGet(), ep->nameGet(), ep->hostname()));

		if ((ep->host != hostMgr->localhostP) && (ep->type != me->type))
		{
			if ((ep->type == Endpoint2::Controller) || (ep->type == Endpoint2::Worker) || (ep->type == Endpoint2::Delilah))
			{
				int        s;
				pthread_t  tid;

				if ((s = pthread_create(&tid, NULL, readerThread, ep)) != 0)
				{
					LM_E(("pthread_create returned %d for %s@%s", s, ep->name, ep->host->name));
					remove(ep);
					return NULL;
				}

				LM_M(("Creating writer thread for endpoint %s@%s", ep->name, ep->host->name));
				if ((s = pthread_create(&tid, NULL, writerThread, ep)) != 0)
				{
					LM_E(("pthread_create returned %d for %s@%s", s, ep->name, ep->host->name));
					remove(ep);
					return NULL;
				}
			}
		}
		else
			LM_M(("Not creating threads for %s in host %s", ep->typeName(), hostMgr->localhostP->name));

		show("Added an Endpoint");
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
		ep = new ListenerEndpoint(this, name, alias, host, port, rFd, wFd);
		break;

	case Endpoint2::Unhelloed:
		ep = new UnhelloedEndpoint(this, host, port, rFd, wFd);
		break;

	case Endpoint2::Worker:
		ep = new WorkerEndpoint(this, id, name, alias, host, rFd, wFd);
		break;

	case Endpoint2::Controller:
		ep = new ControllerEndpoint(this, name, alias, host, rFd, wFd);
		break;

	case Endpoint2::Spawner:
		ep = new SpawnerEndpoint(this, id, name, alias, host, rFd, wFd);
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
Endpoint2* EndpointManager::lookup(Endpoint2::Type typ, int id, int* ixP)
{
	for (unsigned int ix = 0; ix < endpoints; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix]->type == typ)
		{
			if ((id == -1) || (endpoint[ix]->id == id))
			{
				if (ixP != NULL)
					*ixP = ix;
				return endpoint[ix];
			}
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
* ixGet - 
*/
int EndpointManager::ixGet(Endpoint2* ep)
{
	for (unsigned int ix = 0; ix < endpoints; ix++)
	{
		if (endpoint[ix] == ep)
			return ix;
	}
	return -1;
}



/* ****************************************************************************
*
* indexedGet - 
*/
Endpoint2* EndpointManager::indexedGet(unsigned int ix)
{
	if (ix >= endpoints)
		return NULL;

	return endpoint[ix];
}



/* ****************************************************************************
*
* setupAwait - 
*/
Endpoint2::Status EndpointManager::setupAwait(void)
{
	Endpoint2::Status  s;
	Message::Header    header;
	void*              dataP   = NULL;
	long               dataLen = 0;
	Packet             packet(Message::Unknown);

	LM_TODO(("Instead of all this, perhaps I can just start the spawner and treat the messages in SamsonSpawner::receive ... ?"));

	if (listener == NULL)
		LM_RE(Endpoint2::Error, ("Cannot await the setup to arrive if I have no Listener ..."));

	LM_M(("Awaiting samsonSetup to connect and pass the Process Vector"));
	while (1)
	{
		UnhelloedEndpoint* ep;

		LM_M(("Await FOREVER for an incoming connection"));
		if (listener->msgAwait(-1, -1, "Incoming Connection") != 0)
			LM_X(1, ("Endpoint2::msgAwait error"));

		if ((ep = listener->accept()) == NULL)
		{
			LM_E(("error accepting an incoming connection"));
			continue;
		}

		if ((s = ep->helloExchange(5, 0)) != Endpoint2::OK)
		{
			LM_E(("Hello Exchange error: %s", ep->status(s)));
			delete ep;
			continue;
		}

		
		// Is the newly connected peer a 'samsonSetup' ?
		if (ep->type != Endpoint2::Setup)
		{
			LM_E(("The incoming connection was from a '%s' (only Setup allowed)", ep->typeName()));
			remove(ep);
			continue;
		}


		// Reading the message (supposedly a ProcessVector)
		while (1)
		{
			// Hello exchanged, now the endpoint will send a ProcessVector message
			// Awaiting the message to arrive 
			if ((s = ep->msgAwait(5, 0, "ProcessVector Message")) != 0)
			{
				LM_E(("msgAwait(ProcessVector): %s", ep->status(s)));
				remove(ep);
				continue;
			}

			if ((s = ep->receive(&header, &dataP, &dataLen, &packet)) != 0)
			{
				LM_E(("Endpoint2::receive error"));
				remove(ep);
				ep = NULL;
				break;
			}

			// All OK ?
			if (header.type != Message::Msg)
			{
				LM_W(("Read an unexpected '%s' Ack from '%s@%s' - throwing it away!",  messageCode(header.code), ep->nameGet(), ep->hostname()));
				if (dataP != NULL)
                    free(dataP);
				remove(ep);
				ep = NULL;
				break;
			}
			else if (header.code == Message::ProcessVector)
				break;
			else if (header.code == Message::Reset)
				LM_W(("Got a reset, and that's OK but I don't need to do anything, I have nothing started ... Let's just keep reading ..."));
			else if (header.code == Message::ProcessList)
			{
				LM_M(("Got a ProcessList, and that's OK, but I have nothing ... Let's Ack with NO DATA and continue to wait for a Process Vector ..."));
				ep->ack(Message::ProcessList);
			}
			else
			{
				LM_E(("Unexpected Message (%s %s)", messageCode(header.code), messageType(header.type)));
				if (dataP != NULL)
					free(dataP);
				remove(ep);
				continue;
			}
		}

		if (ep == NULL)
			continue;

		// Copying the process vector
		this->procVec = (ProcessVector*) malloc(dataLen);
		if (this->procVec == NULL)
			LM_X(1, ("Error allocating %d bytes for the Process Vector", dataLen));

		memcpy(this->procVec, dataP, dataLen);
		free(dataP);

		if ((s = ep->ack(header.code)) != Endpoint2::OK)
			LM_E(("Error acking Process Vector: %s", ep->status(s)));

		if ((s = ep->msgAwait(2, 0, "Connection Closed")) != Endpoint2::OK)
			LM_W(("All OK, except that samsonSetup didn't close connection in time. msgAwait(): %s", ep->status(s)));
		else if ((s = ep->receive(&header, &dataP, &dataLen, &packet)) != Endpoint2::ConnectionClosed)
			LM_W(("All OK, except that samsonSetup didn't close connection when it was supposed to. receive(): %s", ep->status(s)));

		show("Before removing samsonSetup");
		remove(ep);
		show("After removing samsonSetup");
		return Endpoint2::OK;
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
	int              eps;

	while (1)
	{
		if (callback[Periodic].func != NULL)
			callback[Periodic].func(NULL, callback[Periodic].userParam);

		// Call cleanup function that removes all endpoints marked as 'ScheduledForRemoval' 
		// Don't forget to use a semaphore for endpoint vector and jobs vector

		// Populate rFds for select
		do
		{
			FD_ZERO(&rFds);
			ix   = 0;
			max  = 0;
			eps  = 0;
			for (unsigned int ix = 0; ix < endpoints; ix++)
			{
				ep = endpoint[ix];

				if (ep == NULL)
					continue;

				if (ep->threaded() == true)
					continue;

				if (ep->rFd == -1)
					continue;

				eps += 1;
				max = MAX(max, ep->rFd);
				FD_SET(ep->rFd, &rFds);
			}

			tv.tv_sec  = tmoSecs;
			tv.tv_usec = tmoUSecs;
			
			if (max == 0)
				LM_X(1, ("No fds to listen to ..."));

			fds = select(max + 1,  &rFds, NULL, NULL, &tv);
		} while ((fds == -1) && (errno == EINTR));

		if (fds == -1)
			LM_X(1, ("select: %s", strerror(errno)));
		else if (fds == 0)
		{
			if (callback[Timeout].func != NULL)
				callback[Timeout].func(NULL, callback[Timeout].userParam);
			show("timeout");
		}
		else
		{
			for (unsigned int ix = 0; ix < endpoints; ix++)
			{
				if (endpoint[ix] == NULL)
					continue;

				if (endpoint[ix]->rFd == -1)
					continue;

                if (endpoint[ix]->threaded() == true)
                    continue;
				
				if (FD_ISSET(endpoint[ix]->rFd, &rFds))
					endpoint[ix]->msgTreat();
			}
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
* endpointCapacity - 
*/
int EndpointManager::endpointCapacity(void)
{
	return endpoints;
}



/* ****************************************************************************
*
* endpointCount - 
*/
int EndpointManager::endpointCount(void)
{
	int noOf = 0;

	for (unsigned int ix = 0; ix < endpoints; ix++)
    {
        if (endpoint[ix] == NULL)
            continue;

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
	LM_V(("-------------------- Endpoint list (%s) ------------------------------", why));
	LM_V((""));
	LM_V(("ix  %-12s id  %-20s %-20s %-20s Port  rFd", "Type", "Name", "Host", "State"));
	LM_V(("----------------------------------------------------------------------"));

	for (unsigned int ix = 0; ix < endpoints; ix++)
	{
		Endpoint2* ep;

		ep = endpoint[ix];
		if (ep == NULL)
			continue;

		LM_V(("%02d: %-12s %02d  %-20s %-20s %-20s %d  %d", ix, ep->typeName(), ep->idGet(), ep->nameGet(), ep->hostname(), ep->stateName(), ep->port, ep->rFd));
	}
	LM_V(("----------------------------------------------------------------------"));

	if (forced)
		lmVerbose = verbose;
}



/* ****************************************************************************
*
* procVecSet - 
*/
int EndpointManager::procVecSet(ProcessVector* _procVec, bool save)
{
	int size;

	if ((_procVec->processes <= 0) || (_procVec->processes > 100))
		LM_RE(1, ("Bad number of processes in process vector: %d", _procVec->processes));

	if (procVec)
		free(procVec);

	size    = sizeof(ProcessVector) + _procVec->processes * sizeof(Process);
	procVec = (ProcessVector*) malloc(size);
	memcpy(procVec, _procVec, size);

	if (save == true)
	{
		LM_M(("************* SAVING Process Vector"));
		platformProcessesSave(procVec);
		LM_W(("sleeping for a second after  saving Process Vector to file system"));
		sleep(1);
	}

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



/* ****************************************************************************
*
* callbackSet - 
*/
void EndpointManager::callbackSet(CallbackId id, EpMgrCallback func, void* userParam)
{
	if ((unsigned int) id >= (sizeof(callback) / sizeof(callback[0])))
		LM_X(1, ("Callback ID too high (%d), max is %d - this bug must be fixed NOW!", id, sizeof(callback) / sizeof(callback[0])));

	if (callback[id].func != NULL)
		LM_W(("Overwriting previous callback %d - is this really what you want?", id));

	callback[id].func      = func;
	callback[id].userParam = userParam;
}

}
