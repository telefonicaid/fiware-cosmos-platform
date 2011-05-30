/* ****************************************************************************
*
* FILE                     UnhelloedEndpoint.h
*
* DESCRIPTION              Class for unhelloed endpoints
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 27 2011
*
*/
#include <unistd.h>             // close
#include <fcntl.h>              // F_SETFD
#include <pthread.h>            // pthread_t

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Lmt*

#include "samson/network/Packet.h"             // Packet
#include "samson/network/EndpointManager.h"    // EndpointManager
#include "UnhelloedEndpoint.h"  // Own interface



namespace samson
{



/* ****************************************************************************
*
* processLookup - 
*/
static Process* processLookup(ProcessVector* procVec, const char* alias)
{
	if (procVec == NULL)
		LM_RE(NULL, ("NULL process vector"));

	if (alias == NULL)
		LM_RE(NULL, ("NULL alias"));

	if (alias[0] == 0)
		LM_RE(NULL, ("EMPTY alias"));

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
* UnhelloedEndpoint - 
*/
UnhelloedEndpoint::UnhelloedEndpoint
(
	EndpointManager*  _epMgr,
	Host*             _host,
	unsigned short    _port,
	int               _rFd,
	int               _wFd
) : Endpoint2(_epMgr, Unhelloed, -1, "unhelloed", "unhelloed", _host, _port, _rFd, _wFd)
{
}



/* ****************************************************************************
*
* ~UnhelloedEndpoint - 
*/
UnhelloedEndpoint::~UnhelloedEndpoint() // : ~Endpoint2()
{
}



/* ****************************************************************************
*
* msgTreat2 - 
*/
Endpoint2::Status UnhelloedEndpoint::msgTreat2(Message::Header* headerP, void* dataP, int dataLen, Packet* packetP)
{
	Message::HelloData*  helloP;
	Endpoint2::Status    s;
	Endpoint2*           ep;

	switch (headerP->code)
	{
	case Message::Hello:
		LM_T(LmtUnhelloed, ("Read a Hello message"));
		helloP = (Message::HelloData*) dataP;
		s = helloDataSet((Type) helloP->type, helloP->name, helloP->alias);
		if (s != OK)
		{
			LM_E(("helloDataSet error - setting endpoint in ScheduledForRemoval state"));
			stateSet(ScheduledForRemoval);
			LM_RE(s, ("Bad hello data"));
		}

		if (headerP->type == Message::Msg)
			helloSend(Message::Ack);

		state = Ready;
		if ((samson::Endpoint2::Type) helloP->type == Delilah)
		{
			LM_M(("Changing to a Delilah Endpoint - setting endpoint in ScheduledForRemoval state"));
			state = ScheduledForRemoval;
			ep = epMgr->add((samson::Endpoint2::Type) helloP->type, id, name, alias, host, port, rFd, wFd);
			ep->state = Ready;
			LM_M(("Anything more, turning into a Delilah Endpoint ... ?"));
		}
		else if ((samson::Endpoint2::Type) helloP->type == Worker)
		{
			LM_T(LmtUnhelloed, ("Time to update corresponding Worker endpoint and remove myself ..."));
			ep = epMgr->lookup((samson::Endpoint2::Type) helloP->type, host);

			if (ep == NULL)
				LM_X(1, ("No endpoint found for type '%s' and host '%s' - should be a worker", typeName(), host->name));

			LM_M(("Converting to %s endpoint '%s%d@%s'", ep->typeName(), ep->typeName(), ep->id, ep->host->name));
			ep->rFd   = rFd;
			ep->wFd   = wFd;
			ep->state = Ready;
			
			// Unhelloed to be removed but without closing any file descriptors
			state = ScheduledForRemoval;
			rFd   = -1;
			wFd   = -1;
		}
		else if ((samson::Endpoint2::Type) helloP->type == Controller)
			LM_W(("Anything to be done here (Hello from Controller)?"));
		else
			LM_X(1, ("What?"));

		break;

	default:
		LM_E(("I only treat Hello messages - got a '%s', removing endpoint", messageCode(headerP->code)));
		LM_E(("Setting endpoint in state ScheduledForRemoval"));
		stateSet(ScheduledForRemoval);
		return Error;
	}

	return OK;
}



/* ****************************************************************************
*
* helloDataSet - 
*/
Endpoint2::Status UnhelloedEndpoint::helloDataSet(Type _type, const char* _name, const char* _alias)
{
	Process*  proc;

	if (_alias == NULL)
		LM_RE(NullAlias, ("NULL alias"));

	// If we have the process vector, check that alias is consistent
	if (epMgr->procVecGet() != NULL)
	{
		if ((proc = processLookup(epMgr->procVecGet(), _alias)) != NULL)
		{
			LM_TODO(("Host could become a class and do its own matching"));
			if (epMgr->hostMgr->match(host, proc->host) == false)  
				LM_RE(BadHost, ("The host for alias '%s' must be '%s'. This endpoints host ('%s') is incorrect",
								_alias, hostname(), proc->host));
		}
	}

	if (epMgr->lookup(_type, _alias) != NULL)
		LM_RE(Duplicated, ("Duplicated process"));

	type = _type;
	nameSet(_name);
	aliasSet(_alias);

	return OK;
}



/* ****************************************************************************
*
* helloExchange - 
*/
Endpoint2::Status UnhelloedEndpoint::helloExchange(int secs, int usecs)
{
	Message::Header       header;
	long                  dataLen = 0;
	void*                 dataP   = NULL;
	Message::HelloData*   helloP;
	Packet                packet(Message::Unknown);
	Endpoint2::Status     s;

	LM_T(LmtUnhelloed, ("Sending Hello Msg to %s@%s", name, hostname()));
	helloSend(Message::Msg);

	LM_T(LmtUnhelloed, ("Awaiting reply"));
	if ((s = msgAwait(secs, usecs, "Hello Ack")) != 0)
		LM_RE(s, ("Endpoint2::msgAwait(expecting Hello): %s", status(s)));
	LM_T(LmtUnhelloed, ("Reply seems on its way in"));

	LM_T(LmtUnhelloed, ("Reading reply"));
	if ((s = receive(&header, &dataP, &dataLen, &packet)) != OK)
		LM_RE(s, ("Endpoint2::receive(expecting Hello): %s", status(s)));

	LM_T(LmtUnhelloed, ("Checking validity of reply (code: 0x%x)", header.code));
	if ((header.code != Message::Hello) || (header.type != Message::Ack))
	{
		free(dataP);
		LM_RE(Error, ("Message read not a Hello Msg (%s %s)", messageCode(header.code), messageType(header.type)));
	}

    LM_T(LmtUnhelloed, ("Adapting the KNOWN endpoints characteristics"));
	helloP = (Message::HelloData*) dataP;
	if ((s = helloDataSet((Endpoint2::Type) helloP->type, helloP->name, helloP->alias)) != OK)
	{
		free(dataP);
		LM_RE(s, ("helloDataSet(): %s", status(s)));
	}

	free(dataP);
	LM_T(LmtUnhelloed, ("Successful Hello interchange"));
	
	return OK;
}

}
