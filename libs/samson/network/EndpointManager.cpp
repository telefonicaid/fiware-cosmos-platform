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

#include "au/TokenTaker.h"                          // au::TokenTaker

#include "engine/Notification.h"
#include "engine/Engine.h"

#include "samson/common/NotificationMessages.h"

#include "samson/common/status.h"
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
#include "StarterEndpoint.h"
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
EndpointManager::EndpointManager(Endpoint2::Type type, const char* controllerIp) : token("EndpointManager")
{
	tmoSecs          = 2;
	tmoUSecs         = 0;

	networkInterface = NULL;
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
	case Endpoint2::Spawner:       break;
	case Endpoint2::Delilah:       initDelilah(controllerIp);    break;
	case Endpoint2::Supervisor:    initSupervisor();             break;
	case Endpoint2::Starter:       initStarter();                  break;

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
* initStarter - 
*/
void EndpointManager::initStarter(void)
{
	// LM_W(("Nothing done here, not sure if anything is needed ..."));
}



/* ****************************************************************************
*
* Destructor
*/
EndpointManager::~EndpointManager()
{
    for (unsigned int ix = 0; ix < endpoints; ix++)
    {
        if (endpoint[ix] != NULL)
            delete endpoint[ix];
    }

    if (endpoint != NULL)
        free(endpoint);

    if (hostMgr != NULL)
        delete hostMgr;

	if (procVec != NULL)
        delete procVec;

	if (me != NULL)
        delete me;
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

	case Endpoint2::Starter:
		ep = new StarterEndpoint(this, id, host, rFd, wFd);
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
    au::TokenTaker tokenTaker( &token );   // Mutex to protect the vector
    
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

    // andreu: Converted to a trace ( not a warning anymore )
    // Not a warning since a lookup is perform averytime and unhelloed connection send a hellp to make sure we do not duplicate...

	if (ixP != NULL)
	{
		LM_W(("No '%s' endpoint with id %d found - returning an index of -94 ...", Endpoint2::typeName(typ), _id));
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

	LM_W(("Cannot find endpoint of type '%d' in host '%s' - might be that the host is a long name, including domain ...", typ, host));

	char* dot = (char*) strchr(host, '.');

	if (dot != NULL)
	{
		LM_M(("Yes!  A dot WAS found in the hostname - I remove the domain from the hostname and I make a recursive call !"));
		if (dot == host)
			LM_W(("Ehhhh !!!!   hostname starts with a dot!  Sorry, can't do much about that, can I ?"));
		else
		{
			*dot = 0;
			LM_M(("recursive lookup call, new hostname: '%s'", host));
			return lookup(typ, host);
		}
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



// ----------------------------------------------------------------------------
//
// Temporary test - send ping to all workers
//
void EndpointManager::pingTest(void)
{
	char*    pingstring = strdup("0123456789");
	Packet*  p          = new Packet(Message::Msg, Message::Ping, pingstring, strlen(pingstring) + 1);

	if ((me->type == Endpoint2::Worker) && (me->id == 0))
	{
		LM_F(("Pinging all workers"));
		multiSend(Endpoint2::Worker, p);
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

        // Wake up writing thread if still pending...
        {
            //LM_M(("Endpoint %d will be removed %c %c" , ix , endpoint[ix]->thread_writer_running?'T':'F' ,  endpoint[ix]->thread_reader_running?'T':'F'   ));
            au::TokenTaker tt( &endpoint[ix]->jobQueueSem );
            tt.wakeUpAll();
        }
        
        // Andreu: Not possible to remove endpoint until all threads have finished
		if (endpoint[ix]->thread_writer_running)
			continue;
        
 		if (endpoint[ix]->thread_reader_running)
			continue;

        
        //LM_M(("Endpoint %d removed!!" , ix ));
        
		LM_T(LmtThreads, ("readerId: 0x%x", endpoint[ix]->readerId));
		LM_T(LmtThreads, ("writerId: 0x%x", endpoint[ix]->writerId));
		LM_T(LmtThreads, ("threaded: %s",   (endpoint[ix]->threaded == true)? "TRUE" : "FALSE"));

		// endpoint[ix]->threaded = false;

		if (endpoint[ix]->type == Endpoint2::Worker)
        {
			endpoint[ix]->state = Endpoint2::Disconnected;
        }
		else if (endpoint[ix]->type == Endpoint2::Controller)
        {
			endpoint[ix]->state = Endpoint2::Disconnected;
        }
		else
		{
			delete endpoint[ix];
			endpoint[ix] = NULL;
		}

        // Notification about this disconnection
        engine::Notification *notification = new engine::Notification( notification_network_diconnected );
        notification->environment.setInt("id", ix);
        engine::Engine::shared()->notify( notification );
        
        
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

				if (ep->rFd == -1)
					continue;

                // Andreu: If it is threaded, this has not to be handled here (independently of the state )
				//if ((ep->isThreaded() == true) && (ep->state == Endpoint2::Ready))
                if (ep->isThreaded() == true)
					continue;

				if (((ep->state == Endpoint2::Ready) || (ep->state == Endpoint2::Connected)) && (ep->rFd == -1))
					LM_X(1, ("Endpoint %s is in state %s but its rFd is -1 ...", ep->name(), ep->stateName()));
                
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
                /*
				if (firstTimeForNoFdsToListenTo == true)
					LM_W(("No fds to listen to ..."));
                 */

				firstTimeForNoFdsToListenTo = false;
                usleep(100000);
				//sleep(2);
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

				if ((endpoint[ix]->isThreaded() == true) && (endpoint[ix]->state == Endpoint2::Ready))
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
void EndpointManager::send(int endpointIx, Packet* packetP)
{
    au::TokenTaker tokenTaker( &token );   // Mutex to protect the vector
    
	if ((endpointIx < 0) || ((unsigned int) endpointIx >= endpoints))
		LM_X(1, ("Bad endpointIx: %d", endpointIx));

	if (endpoint[endpointIx] == NULL)
		LM_RVE(("Cannot send to endpoint %d - NULL", endpointIx));

	packetP->fromId = endpointIx;
	endpoint[endpointIx]->send(packetP);
}


/* ****************************************************************************
 *
 * multiSend - 
 */
int EndpointManager::multiSend(Endpoint2::Type typ, Packet* packetP)
{
   int sends = 0;

   for (unsigned int ix = 0; ix < endpoints; ix++)
   {
	  if (endpoint[ix] == NULL)
		 continue;

	  if (endpoint[ix]->type != typ)
		 continue;

		send(ix, new Packet(packetP));
		++sends;
	}

   delete packetP;
    
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
		endpoint[ix]->send(packetP);
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
	int tLevel = LmtEndpointList2;

	if (forced)
		tLevel = LmtEndpointList1;

	LM_T(tLevel, (""));
	LM_T(tLevel, ("-------------------- Endpoint list (%s) ------------------------------", why));
	LM_T(tLevel, (""));
	LM_T(tLevel, ("  ix  %-12s id  %-20s %-20s Port  rFd", "Type", "Host", "State"));
	LM_T(tLevel, ("----------------------------------------------------------------------"));

	for (unsigned int ix = 0; ix < endpoints; ix++)
	{
		Endpoint2* ep;
		char       buf[256];

		ep = endpoint[ix];
		if (ep == NULL)
			continue;

		LM_T(tLevel, ("%s", ep->statusString(buf, sizeof(buf), ix, false)));
	}
	LM_T(tLevel, ("----------------------------------------------------------------------"));
}



/* ****************************************************************************
*
* procVecSet - 
*/
int EndpointManager::procVecSet(ProcessVector* _procVec, bool save)
{
	int size;

	if (_procVec == NULL)
		return 0;

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
