/* ****************************************************************************
*
* FILE                     EndpointManager.cpp - 
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 06 2011
*
*/
#include <signal.h>
#include <pthread.h>

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "samson/common/ports.h"
#include "samson/common/Process.h"
#include "samson/common/platformProcesses.h"

#include "NetworkInterface.h"
#include "Packet.h"
#include "Endpoint2.h"
#include "Host.h"
#include "ListenerEndpoint.h"
#include "WebListenerEndpoint.h"
#include "UnhelloedEndpoint.h"
#include "WorkerEndpoint.h"
#include "ControllerEndpoint.h"
#include "SpawnerEndpoint.h"
#include "DelilahEndpoint.h"
#include "EndpointManager.h"



namespace samson
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
	tmoSecs          = 2;
	tmoUSecs         = 0;

	procVec          = NULL;
	hostMgr          = NULL;

	workers          = 0;
	endpoints        = 0;
	delilahId        = 0;

	endpoint         = NULL;
	me               = NULL;
	controller       = NULL;
	listener         = NULL;
	webListener      = NULL;

	packetReceiver   = NULL;
	dataReceiver     = NULL;

	LM_T(LmtDelilah, ("Set delilahId to %d", delilahId));



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
	//    Controller:  when ProcessVector is read from file
	//    Worker:      when ProcessVector is read from file
	//    Spawner:     NOWHERE !!!
	//    Delilah:     NOWHERE !!!
	//
	me = new Endpoint2(this, type, 0, host); // id == 0, port == 0 ...
	if (me == NULL)
		LM_X(1, ("error allocating 'me' endpoint: %s", strerror(errno)));
	me->idInEndpointVector = 0;



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
* controllerConnect - 
*/
void EndpointManager::controllerConnect(void)
{
	Process*   p;
	Host*      hostP;

	p = &procVec->processV[0];
	if (p->type != PtController)
		LM_X(1, ("First process in process vector has to be the controller!"));

	hostP = hostMgr->lookup(p->host);
	if (hostP == NULL)
		hostP = hostMgr->insert(p->host, NULL);

	LM_TODO(("Here I use '0' as id instead of p->id ..."));
	controller = add(Endpoint2::Controller, 0, hostP, p->port, -1, -1);
}



/* ****************************************************************************
*
* workersAdd - 
*/
void EndpointManager::workersAdd(void)
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

		ep = add(Endpoint2::Worker, p->id, hostP, p->port, -1, -1);

		if ((me->type == Endpoint2::Worker) && (hostP == me->host))
		{
			ep->state = Endpoint2::Loopback;
			delete me;
			me = ep;
			LM_T(LmtMe, ("Set ME to point to Worker %d", ep->idGet()));
		}
	}
}



/* ****************************************************************************
*
* workersConnect - 
*/
void EndpointManager::workersConnect(void)
{
	Endpoint2* ep;

	for (unsigned int ix = 1; ix < endpoints; ix++)
	{
		ep = endpoint[ix];

		if (ep == NULL)
			continue;

		if (ep->type != Endpoint2::Worker)
			continue;

		if (ep->state == Endpoint2::Ready)
			continue;

		if (ep->state == Endpoint2::Connected)
			continue;

		if (ep->id == -1)
			continue;

		LM_TODO(("Should probably check for 'connection in progress' also - not only 'Endpoint2::Ready'"));

		if (me->type == Endpoint2::Worker)
		{
			if (me->idGet() > ep->idGet())
			{
				LM_T(LmtConnect, ("Connecting to %s %d in '%s' (my id: %d)", ep->typeName(), ep->idGet(), ep->host->name, me->idGet()));
				ep->connect();  // Connect, but don't add to endpoint vector
				show("Connected", true);
			}
			else
				LM_T(LmtConnect, ("NOT connecting to %s %d in '%s' (my id: %d)", ep->typeName(), ep->idGet(), ep->host->name, me->idGet()));
		}
		else
		{
			LM_T(LmtConnect, ("Connecting to %s %d in %s", ep->typeName(), ep->idGet(), ep->host->name));
			ep->connect();
		}
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
	int       ix   = 0;
	Process*  self = NULL;

	if ((procVec = platformProcessesGet()) == NULL)
		LM_X(1, ("Error retrieving vector of platform processes"));

	LM_T(LmtProcessVector, ("Got %d processes in procVec", procVec->processes));
	for (int ix = 0; ix < procVec->processes; ix++)
		LM_T(LmtProcessVector, ("Process %d: type %d, host %s", ix, procVec->processV[ix].type, procVec->processV[ix].host));

	if ((self = platformProcessLookup(hostMgr, procVec, Endpoint2::Worker, me->host, &ix)) == NULL)
		LM_X(1, ("Cannot find myself in platform processes vector."));

	me->portSet(WORKER_PORT);
	me->idSet(self->id);
	LM_T(LmtMe, ("ME: %s (ix: %d) id:%d, port: %d", me->name(), ixGet(me), me->idGet(), me->portGet()));

	controllerConnect();
	workersAdd();
	workersConnect();

	//
	// Opening listener to accept incoming connections
	//
	listener = (ListenerEndpoint*) add(Endpoint2::Listener, 0, me->hostGet(), me->portGet(), -1, -1);
	if (listener == NULL)
		LM_X(1, ("error creating listener endpoint - no use to continue ..."));
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
	
	me->portSet(CONTROLLER_PORT);


	//
	// Opening listeners to accept incoming connections
	//
	listener    = (ListenerEndpoint*)    add(Endpoint2::Listener,    0, me->hostGet(), me->portGet(),    -1, -1);
	webListener = (WebListenerEndpoint*) add(Endpoint2::WebListener, 0, me->hostGet(), WEB_SERVICE_PORT, -1, -1);

	if (listener == NULL)
		LM_X(1, ("error creating listener endpoint - no use to continue ..."));
	if (webListener == NULL)
		LM_W(("error creating web listener endpoint!"));

	workersAdd();
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

	listener = (ListenerEndpoint*) add(Endpoint2::Listener, 0, me->hostGet(), me->portGet(), -1, -1);

	if ((procVec = platformProcessesGet()) == NULL)
	{
		setupAwait();
		
		LM_T(LmtProcessVector, ("************* SAVING Process Vector"));
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
	// Connect to controller
	//
	Host* hostP;

	hostP = hostMgr->lookup(controllerIp);
	if (hostP == NULL)
		hostP = hostMgr->insert(controllerIp, NULL);

	controller = add(Endpoint2::Controller, 0, hostP, CONTROLLER_PORT, -1, -1);
	controller->connect();
}



/* ****************************************************************************
*
* initSupervisor - 
*/
void EndpointManager::initSupervisor(void)
{
	LM_X(1, ("Supervisor init needs to be implemented"));
}



/* ****************************************************************************
*
* initSetup - 
*/
void EndpointManager::initSetup(void)
{
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
* add - 
*/
Endpoint2* EndpointManager::add(Endpoint2* ep)
{
	for (unsigned int ix = 0; ix < endpoints; ix++)
	{
		if (endpoint[ix] != NULL)
			continue;

		endpoint[ix] = ep;
		ep->idInEndpointVector = ix;
		LM_T(LmtEndpointAdd, ("Added endpoint %d. type:%s, id:%d, host:%s", ix, ep->typeName(), ep->idGet(), ep->hostname()));

		show("Added an Endpoint", true);
		return ep;
	}

	LM_X(1, ("No room in endpoint vector (%d occupied slots) - realloc?", endpoints));
	return NULL;
}



/* ****************************************************************************
*
* add - 
*/
Endpoint2* EndpointManager::add(Endpoint2::Type type, int id, Host* host, unsigned short port, int rFd, int wFd)
{
	Endpoint2* ep = NULL;

	switch (type)
	{
	case Endpoint2::Listener:
		ep = new ListenerEndpoint(this, host, port, rFd, wFd);
		break;

	case Endpoint2::Unhelloed:
		ep = new UnhelloedEndpoint(this, host, port, rFd, wFd);
		break;

	case Endpoint2::Worker:
		ep = new WorkerEndpoint(this, id, host, rFd, wFd);
		break;

	case Endpoint2::Controller:
		ep = new ControllerEndpoint(this, host, rFd, wFd);
		break;

	case Endpoint2::Spawner:
		ep = new SpawnerEndpoint(this, id, host, rFd, wFd);
		break;

	case Endpoint2::Delilah:
		ep = new DelilahEndpoint(this, id, host, rFd, wFd);
		break;

	case Endpoint2::WebListener:
		ep = new WebListenerEndpoint(this, id, host, port, rFd, wFd);
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
	{
		LM_W(("index %d >= endpoints %d", index, endpoints));
		return NULL;
	}

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



static void badLookup(void)
{
}
/* ****************************************************************************
*
* lookup - 
*/
Endpoint2* EndpointManager::lookup(Endpoint2::Type typ, int _id, int* ixP)
{
	LM_T(LmtEndpointLookup, ("Looking for a %s with id %d", Endpoint2::typeName(typ), _id));

	show("lookup");

#if 0
	if ((typ == me->type) && (_id == me->id))
	{
		LM_T(LmtEndpointLookup, ("Found myself!"));
		if (me->type != Endpoint2::Worker)
			LM_X(1, ("Only Worker type should find itself ... (id == %d)", _id));

		if (ixP != NULL)
		{
			*ixP = -93; // Was: me->idGet(), but I don't see how that makes sense ...
			badLookup();
		}
		return me;
	}
#endif

	for (unsigned int ix = 0; ix < endpoints; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix]->type == typ)
		{
			if (endpoint[ix]->id == _id)
			{
				if (ixP != NULL)
					*ixP = ix;
				return endpoint[ix];
			}
		}
	}

	LM_W(("No %s endpoint with id %d found", Endpoint2::typeName(typ), _id));

	if (ixP != NULL)
	{
		*ixP = -94;
		badLookup();
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
* lookup - 
*/
Endpoint2* EndpointManager::lookup(Endpoint2::Type typ, Host* host)
{
	for (unsigned int ix = 0; ix < endpoints; ix++)
	{
        if (endpoint[ix] == NULL)
            continue;

		if (endpoint[ix]->type != typ)
			continue;

		if (host == endpoint[ix]->hostGet())
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

	LM_T(LmtSetup, ("Awaiting samsonSetup to connect and pass the Process Vector"));
	while (1)
	{
		UnhelloedEndpoint* ep;

		LM_T(LmtSetup, ("Await FOREVER for an incoming connection"));
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
			   LM_X(1, ("msgAwait(ProcessVector): %s", ep->status(s)));

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
				LM_W(("Read an unexpected '%s' Ack from '%s' - throwing it away!",  messageCode(header.code), ep->name()));
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
				LM_T(LmtSetup, ("Got a ProcessList, and that's OK, but I have nothing ... Let's Ack with NO DATA and continue to wait for a Process Vector ..."));
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
		ep->ack(header.code);

		if ((s = ep->msgAwait(2, 0, "Connection Closed")) != Endpoint2::OK)
			LM_W(("All OK, except that samsonSetup didn't close connection in time. msgAwait(): %s", ep->status(s)));
		else if ((s = ep->receive(&header, &dataP, &dataLen, &packet)) != Endpoint2::ConnectionClosed)
			LM_W(("All OK, except that samsonSetup didn't close connection when it was supposed to. receive(): %s", ep->status(s)));

		show("Before removing samsonSetup");
		remove(ep);
		show("After removing samsonSetup", true);
		return Endpoint2::OK;
	}
}



/* ****************************************************************************
*
* periodic - 
*/
void EndpointManager::periodic(void)
{
	static int periodicNo = 0;

	//
	// Worker and Delilah connecting to Controller and Workers 
	//
	if (me->type == Endpoint2::Worker || me->type == Endpoint2::Delilah)
	{
		LM_T(LmtReconnect, ("periodic - reconnecting to worker/controller?"));

		show("periodic");
		LM_T(LmtTimeout, ("In periodic %d - any controller/workers to connect to?", periodicNo));
		if (controller == NULL)
			LM_X(1, ("controller == NULL"));

		if ((controller->state != Endpoint2::Ready) && (controller->state != Endpoint2::Connected))
		{
			LM_T(LmtReconnect, ("Connecting to controller"));
			controller->connect();
		}

		workersConnect();
		show("periodic");
	}



	//
	// Removing endpoints that are Scheduled For Removal
	//
	for (unsigned int ix = 0; ix < endpoints; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix]->state != Endpoint2::ScheduledForRemoval)
			continue;

		if (endpoint[ix]->threaded == true)
		{
			LM_T(LmtThreads, ("Killing writer thread 0x%x for %s", endpoint[ix]->writerId, endpoint[ix]->name()));
			pthread_cancel(endpoint[ix]->writerId);
		}

		endpoint[ix]->threaded = false;

		if (endpoint[ix]->type == Endpoint2::Worker)
			endpoint[ix]->state = Endpoint2::Disconnected;
		else if (endpoint[ix]->type == Endpoint2::Controller)
			endpoint[ix]->state = Endpoint2::Disconnected;
		else
		{
			delete endpoint[ix];
			endpoint[ix] = NULL;
		}

		show("Removed a removal-scheduled endpoint", true);
	}

	if (callback[Periodic].func != NULL)
		callback[Periodic].func(NULL, callback[Periodic].userParam);

	++periodicNo;
}



/* ****************************************************************************
*
* timeout - 
*/
void EndpointManager::timeout(void)
{
	if (callback[Timeout].func != NULL)
		callback[Timeout].func(NULL, callback[Timeout].userParam);
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
	int              firstTimeForNoFdsToListenTo = true;

	while (1)
	{
		periodic();
		
		// Call cleanup function that removes all endpoints marked as 'ScheduledForRemoval' 
		// Don't forget to use a semaphore for endpoint vector and jobs vector

		do
		{
			char fdsString[512];

			FD_ZERO(&rFds);
			ix   = 0;
			max  = 0;
			eps  = 0;
			memset(fdsString, 0, sizeof(fdsString));
			for (unsigned int ix = 0; ix < endpoints; ix++)
			{
				char fdsSubString[16];

				ep = endpoint[ix];

				if (ep == NULL)
					continue;

				if (ep->isThreaded() == true)
					continue;

				if (((ep->state == Endpoint2::Ready) || (ep->state == Endpoint2::Connected)) && (ep->rFd == -1))
					LM_X(1, ("Endpoint %s is in state %s but its rFd is -1 ...", ep->name(), ep->stateName()));

				if (ep->rFd == -1)
					continue;

				eps += 1;
				max = MAX(max, ep->rFd);
				FD_SET(ep->rFd, &rFds);

				
				snprintf(fdsSubString, sizeof(fdsSubString), "%d", ep->rFd);
				if (fdsString[0] == 0)
					strcpy(fdsString, fdsSubString);
				else
				{
					strcat(fdsString, ", ");
					strcat(fdsString, fdsSubString);
				}
			}

			if (max == 0)
			{
				if (firstTimeForNoFdsToListenTo == true)
					LM_W(("No fds to listen to ..."));
				firstTimeForNoFdsToListenTo = false;
				sleep(2);
				fds = 0;
				break;
			}

			firstTimeForNoFdsToListenTo = true;

			tv.tv_sec  = tmoSecs;
			tv.tv_usec = tmoUSecs;

			LM_T(LmtSelect, ("Hanging on a select over fd list { %s }", fdsString));
			fds = select(max + 1,  &rFds, NULL, NULL, &tv);
		} while ((fds == -1) && (errno == EINTR));

		if (fds == -1)
			LM_X(1, ("select: %s", strerror(errno)));
		else if (fds == 0)
			timeout();
		else
		{
			for (unsigned int ix = 0; ix < endpoints; ix++)
			{
				if (endpoint[ix] == NULL)
					continue;

				if (endpoint[ix]->rFd == -1)
					continue;

				if (endpoint[ix]->isThreaded() == true)
					continue;
				
				if (FD_ISSET(endpoint[ix]->rFd, &rFds))
				{
					LM_T(LmtMsgTreat, ("Calling %s->msgTreat()", endpoint[ix]->typeName()));
					endpoint[ix]->msgTreat();
					LM_T(LmtMsgTreat, ("Back from %s->msgTreat()", endpoint[ix]->typeName()));
					break;
					// FD_CLR instead of break?
				}
			}
		}

		if (oneShot)
		{
			LM_W(("one-shot - I return"));
			return;
		}
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
void EndpointManager::send(PacketSenderInterface* psi, int endpointIx, Packet* packetP)
{
	if ((endpointIx < 0) || ((unsigned int) endpointIx >= endpoints))
		LM_X(1, ("Bad endpointIx: %d", endpointIx));

	if (endpoint[endpointIx] == NULL)
		LM_RVE(("Cannot send to endpoint %d - NULL", endpointIx));

	packetP->fromId = endpointIx;
	endpoint[endpointIx]->send(psi, packetP);
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
		if (endpoint[ix] == NULL)
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
	int     sends = 0;
	Packet* packetP;

	for (unsigned int ix = 0; ix < endpoints; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix]->type != typ)
			continue;

		packetP = new Packet(Message::Msg, code, dataP, dataLen);
		packetP->fromId = ix;
		endpoint[ix]->send(NULL, packetP);
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
	LM_V(("ix  %-12s id  %-20s %-20s Port  rFd", "Type", "Host", "State"));
	LM_V(("----------------------------------------------------------------------"));

	for (unsigned int ix = 0; ix < endpoints; ix++)
	{
		Endpoint2* ep;

		ep = endpoint[ix];
		if (ep == NULL)
			continue;

		LM_V(("%02d: %-12s %02d  %-20s %-20s %04d  %02d %s", ix, ep->typeName(), ep->idGet(), ep->hostname(), ep->stateName(), ep->port, ep->rFd,
			  (ep->isThreaded() == true)? "(threaded)" : (ep->state == Endpoint2::Loopback)? "(myself)" : ""));
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
		LM_T(LmtProcessVector, ("************* SAVING Process Vector"));
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
