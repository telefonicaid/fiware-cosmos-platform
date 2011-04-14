/* ****************************************************************************
*
* FILE                     Endpoint2.h
*
* DESCRIPTION              Class for endpoints
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 12 2011
*
*/
#include <unistd.h>             // close

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "ports.h"              // WORKER_PORT, ...
#include "HostMgr.h"            // HostMgr
#include "Message.h"            // Message::Code, Message::Type
#include "EndpointManager.h"    // EndpointManager
#include "Endpoint2.h"          // Own interface


namespace ss
{



/* ****************************************************************************
*
* Endpoint2::Endpoint2 - Constructor
*/
Endpoint2::Endpoint2(Type _type, const char* _name, const char* _alias, Host* _host, unsigned short _port, int _rFd, int _wFd)
{
	type             = _type;
	host             = _host;
	rFd              = _rFd;
	wFd              = _wFd;
	port             = _port;
	state            = NeverConnected;
	useSenderThread  = false;

	name             = NULL;
	alias            = NULL;
	
	memset(&sockin, 0, sizeof(sockin));

	if (_name != NULL)
		name  = strdup(_name);
	if (_alias != NULL)
		alias = strdup(_alias);

	if ((type == Listener) || (type == WebListener))
		listenerPrepare();
}



/* ****************************************************************************
*
* Endpoint2::
*/
Endpoint2::~Endpoint2()
{
	if (rFd != -1)
		close(rFd);
	if ((wFd != rFd) && (wFd != -1))
		close(wFd);

	if (name != NULL)
		free(name);

	if (alias != NULL)
		free(alias);
}



/* ****************************************************************************
*
* hostGet - 
*/
Host* Endpoint2::hostGet(void)
{
	// What if NULL ?

	return host;
}



/* ****************************************************************************
*
* hostname - 
*/
const char* Endpoint2::hostname(void)
{
	if (host == NULL)
		return "NullHost";

	return host->name;
}



/* ****************************************************************************
*
* typeGet - 
*/
Endpoint2::Type Endpoint2::typeGet(void)
{
	return type;
}



/* ****************************************************************************
*
* typeSet - 
*/
void Endpoint2::typeSet(Type _type)
{
	type = _type;
}



/* ****************************************************************************
*
* portSet - 
*/
void Endpoint2::portSet(unsigned short _port)
{
	if ((type == Endpoint2::Worker) && (_port != WORKER_PORT))
		LM_W(("Setting port for Worker to %d", _port));

	if ((type == Endpoint2::Controller) && (_port != CONTROLLER_PORT))
		LM_W(("Setting port for Controller to %d", _port));

	if ((type == Endpoint2::Spawner) && (_port != SPAWNER_PORT))
		LM_W(("Setting port for Spawner to %d", _port));

	port = _port;
}



/* ****************************************************************************
*
* portGet - 
*/
unsigned short Endpoint2::portGet(void)
{
	return port;
}



/* ****************************************************************************
*
* nameSet - 
*/
void Endpoint2::nameSet(const char* _name)
{
	if (name != NULL)
		free(name);

	name = strdup(_name);
}



/* ****************************************************************************
*
* nameGet - 
*/
const char* Endpoint2::nameGet(void)
{
	if (name == NULL)
		return "NULL name";

	return name;
}



/* ****************************************************************************
*
* aliasSet - 
*/
void Endpoint2::aliasSet(const char* _alias)
{
	if (alias != NULL)
		free(alias);

	alias = strdup(_alias);
}



/* ****************************************************************************
*
* aliasGet - 
*/
const char* Endpoint2::aliasGet(void)
{
	if (alias == NULL)
		return "NULL alias";

	return alias;
}



/* ****************************************************************************
*
* processLookup - 
*/
static Process* processLookup(ProcessVector* procVec, const char* alias)
{
	for (int ix = 0; ix < procVec->processes; ix++)
	{
		Process* p = &procVec->processV[ix];

		if (strcmp(p->alias, alias) == 0)
			return p;
	}

	return NULL;
}



/* ****************************************************************************
*
* helloDataAdd - 
*
* Controller and Worker endpoints should all be added with the type 'Unknown'.
* When the Hello data arrives, a check will be performed to see whether the 
* connection is valid (not occupied).
*/
void Endpoint2::helloDataAdd(EndpointManager* epMgr, Endpoint2::Type _type, const char* _name, const char* _alias)
{
	if ((_type == Endpoint2::Controller) || (_type == Endpoint2::Worker))
	{
		Process*  proc;

		if (_alias == NULL)
		{
			state = ScheduledForRemoval;
			LM_RVE(("NULL alias"));
		}

		if ((proc = processLookup(epMgr->procVec, _alias)) == NULL)
		{
			state = ScheduledForRemoval;
			LM_RVE(("alias '%s' not found in process vector", _alias));
		}

		if (epMgr->hostMgr->match(host, proc->host) == false)  // Host could become a class and to its own matching
		{
			state = ScheduledForRemoval;
			LM_RVE(("The host for alias '%s' must be '%s'. This endpoints host ('%s') is incorrect",
					_alias, hostname(), proc->host));
		}

		if (epMgr->lookup(_type, _alias) != NULL)
		{
			state = ScheduledForRemoval;
			LM_RVE(("Duplicated process"));
		}

		if (_type == Endpoint2::Controller)
			epMgr->controller = this;
	}

	type = _type;
	nameSet(_name);
	aliasSet(_alias);

	// Create sender thread right here ? Guess so ...
}



/* ****************************************************************************
*
* send - 
*/
int Endpoint2::send(Endpoint2* me, Message::MessageType type, Message::MessageCode code, void* data, int dataLen, Packet* packetP)
{
	// Implement iomMsgSend here

	LM_RE(-1, ("Not implemented"));
}



/* ****************************************************************************
*
* receive - 
*/
int Endpoint2::receive(Endpoint2* me, Message::Header* headerP, void** dataPP, int* dataLenP, ss::Packet* packetP)
{
	// Implement iomMsgRead here

	LM_RE(-1, ("Not implemented"));
}



/* ****************************************************************************
*
* connect - 
*/
Endpoint2* Endpoint2::connect(bool addToEpVec)
{
	if (port == 0)
		LM_RE(NULL, ("Cannot connect to '%s@%s' - port is ZERO", name, host->name));

	// Implement iomConnect here
	// if (addToEpVec) - add the accepted endpoint to endpoint vector with type Anonymous

	return NULL;
}



/* ****************************************************************************
*
* accept - 
*/
Endpoint2* Endpoint2::accept(bool addToEpVec)
{
	if (type != Listener)
		LM_RE(NULL, ("A non Listener endpoint cannot accept connections ..."));

	// Implement iomAccept here
	// Add the host to the Host Manager
	// if (addToEpVec) - add the accepted endpoint to endpoint vector with type Anonymous

	return NULL;
}



/* ****************************************************************************
*
* helloSend - 
*/
int Endpoint2::helloSend(Endpoint2* self, Message::MessageType type)
{
	Message::HelloData hello;

	memset(&hello, 0, sizeof(hello));

	strncpy(hello.name,   self->nameGet(),    sizeof(hello.name));
	strncpy(hello.ip,     self->hostname(),   sizeof(hello.ip));
	strncpy(hello.alias,  self->aliasGet(),   sizeof(hello.alias));

	hello.type     = self->typeGet();
	hello.coreNo   = 0;
	hello.workerId = 0;

	LM_T(LmtWrite, ("sending hello %s to '%s' (my name: '%s', my type: '%s')", messageType(type), name, hello.name, self->typeName()));

	return send(self, type, Message::Hello, &hello, sizeof(hello));
}



/* ****************************************************************************
*
* listenerPrepare - 
*/
int Endpoint2::listenerPrepare(void)
{
	// Implement iomServerOpen here

	return 0;
}



/* ****************************************************************************
*
* msgAwait - 
*/
int Endpoint2::msgAwait(int secs, int usecs)
{
	// Implement iomMsgAwait here

	return 0;
}



/* ****************************************************************************
*
* msgTreat - 
*/
int Endpoint2::msgTreat(void)
{
	// Implement Network::msgTreat/msgPreTreat here

	// This method will use Endpoint2::receive to read the message
	// [ or Endpoint2::accept in the case of a Listener ]
	// and Endpoint2::send to respond with an ack

	return 0;
}



/* *******************************************************************************
*
* typeName - 
*/
const char* Endpoint2::typeName(Type type)
{
	switch (type)
	{
	case Anonymous:      return "Anonymous";
	case Worker:         return "Worker";
	case Controller:     return "Controller";
	case Spawner:        return "Spawner";
	case Supervisor:     return "Supervisor";
	case Delilah:        return "Delilah";
	case Killer:         return "Killer";
	case Setup:          return "Setup";
	case Listener:       return "Listener";
	case Sender:         return "Sender";
	case CoreWorker:     return "CoreWorker";
	case ThreadReader:   return "ThreadReader";
	case ThreadSender:   return "ThreadSender";
	case WebListener:    return "WebListener";
	case WebWorker:      return "WebWorker";
	}

	return "Unknown Endpoint Type";
}



/* *******************************************************************************
*
* typeName - 
*/
const char* Endpoint2::typeName(void)
{
	return typeName(type);
}

}
