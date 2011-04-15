/* ****************************************************************************
*
* FILE                     EndpointManager.cpp - 
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 06 2011
*
*/
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*

#include "ports.h"              // All ports for the samson system
#include "Packet.h"             // Packet
#include "Process.h"            // Process, ProcessVector
#include "platformProcesses.h"  // platformProcessesGet
#include "Host.h"               // Host
#include "Endpoint2.h"          // Endpoint2
#include "EndpointManager.h"    // Own interface



namespace ss
{



/* ****************************************************************************
*
* typeValidityCheck - 
*/
static void typeValidityCheck(Endpoint2::Type type)
{
	switch (type)
	{
	case Endpoint2::Worker:
	case Endpoint2::Controller:
	case Endpoint2::Spawner:
	case Endpoint2::Delilah:
	case Endpoint2::Supervisor:
		LM_V(("Creating the endpoint manager for %s", Endpoint2::typeName(type)));
		break;

	default:
		LM_X(1, ("The endpoint type '%s' cannot have an Endpoint Manager", Endpoint2::typeName(type)));
	}
}



/* ****************************************************************************
*
* platformProcessLookup - 
*/
static Process* platformProcessLookup(HostMgr* hostMgr, ProcessVector* procVec, Endpoint2::Type type, Host* host)
{
	for (int ix = 0; ix < procVec->processes; ix++)
	{
		Process* p = &procVec->processV[ix];

		if (p->type != (ProcessType) type)
			continue;

		if (hostMgr->match(host, p->host) == false)
			continue;

		return p;
	}

	return NULL;
}



/* ****************************************************************************
*
* Constructor
*/
EndpointManager::EndpointManager(Endpoint2::Type type, unsigned int _endpoints)
{
	Host* host;

	endpoints   = (_endpoints == 0)? 100 : _endpoints;
	workers     = 0;
	controller  = NULL;
	listener    = NULL;

	typeValidityCheck(type);



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
	// For now, it's left out from the list
	//
	me = new Endpoint2(this, type, "unknown name", "unknown alias", host); // port == 0 by default
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
		Process* self;

		if ((procVec = platformProcessesGet()) == NULL)
			LM_X(1, ("No process vector"));

		if ((self = platformProcessLookup(hostMgr, procVec, type, host)) == NULL)
			LM_X(1, ("I'm not in Process Vector ..."));

		me->portSet(self->port);
		me->aliasSet(self->alias);        // This method could check the alias for validity ('WorkerXX', 'Controller', ...)
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
		listener = add(Endpoint2::Listener, "ME", "Listener", me->hostGet(), me->portGet(), -1, -1);



	//
	// Controller also serves as Web Listener
	//
	if (me->typeGet() == Endpoint2::Controller)
		add(Endpoint2::WebListener, "ME", "Web Listener", me->hostGet(), WEB_SERVICE_PORT, -1, -1);



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
Endpoint2* EndpointManager::add(Endpoint2::Type type, const char* name, const char* alias, Host* host, unsigned short port, int rFd, int wFd)
{
	Endpoint2* ep;

	if ((type == Endpoint2::Controller) || (type == Endpoint2::Worker))
		LM_X(1, ("Please don't add Controller and Worker with correct type, await the Hello and use addHelloData for this purpose (use the type 'Anonymous')"));

	ep = new Endpoint2(this, type, name, alias, host, port, rFd, wFd);
	if (ep == NULL)
		LM_X(1, ("Error allocating endpoint of %d bytes", sizeof(Endpoint2)));

	for (unsigned int ix = 0; ix < endpoints; ix++)
	{
		if (endpoint[ix] != NULL)
			continue;

		endpoint[ix] = ep;

		return ep;
	}

	LM_X(1, ("No room in endpoint vector (%d occupied slots) - realloc?", endpoints));
	return NULL;
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
		LM_V(("%c %02d: %-20s %-20s %-20s", (ep->typeGet() != Endpoint2::Anonymous)? '+' : '-', ix, ep->aliasGet(), ep->nameGet(), ep->hostname()));
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
		Endpoint2*            ep;
		Message::Header       header;
		int                   dataLen = 0;
		void*                 dataP   = NULL;
		Message::HelloData*   helloP;
		ProcessVector*        procVecP;
		Packet                packet;


		// Await FOREVER for an incoming connection
		if (listener->msgAwait(-1, -1) != 0)
			LM_X(1, ("Endpoint2::msgAwait error"));


		// Accept the connection without adding the endpoint to the endpoint vector (false)
		if ((ep = listener->accept(false)) == NULL)
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
		if (ep->receive(me, &header, &dataP, &dataLen, &packet) != 0)
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
			LM_E(("The incoming connection was from a '%s' - closing the connection", Endpoint2::typeName((Endpoint2::Type) helloP->type)));
			delete ep;
			continue;
		}


		// Responding to the Hello
		if (ep->helloSend(me, Message::Ack) != 0)
		{
			LM_E(("error acking Hello to '%s@%s'", helloP->alias, ep->hostname()));
			delete ep;
			continue;
		}


		// Hello exchanged, now the endpoint will send a ProcessVector message
		// Awaiting the message to arrive 
		if (listener->msgAwait(5, 0) != 0)
		{
			LM_E(("Endpoint2::msgAwait error, expecting a ProcessVector"));
			delete ep;
			continue;
		}


		// Reading the message
		dataP   = NULL;
		dataLen = 0;
		if (ep->receive(me, &header, &dataP, &dataLen, &packet) != 0)
		{
			LM_E(("Endpoint2::receive error"));
			delete ep;
			continue;
		}


		// All OK ?
		if ((header.code != Message::ProcessVector) || (header.type != Message::Msg))
		{
			LM_E(("Message read not a ProcessVector Msg (%s %s)", messageCode(header.code), messageType(header.type)));
			delete ep;
			continue;
		}
		procVecP = (ProcessVector*) dataP;


		// Copying 
		this->procVec = (ProcessVector*) malloc(procVecP->processVecSize);
		if (this->procVec == NULL)
			LM_X(1, ("Error allocating %d bytes for the Process Vector", procVecP->processVecSize));

		memcpy(this->procVec, procVecP, procVecP->processVecSize);


		// Await samsonSetup to close connection ?
		// What if it's another Spawner ... ? 
		delete ep;

		return;
	}
}

}
