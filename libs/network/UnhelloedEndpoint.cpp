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

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*

#include "Packet.h"             // Packet
#include "EndpointManager.h"    // EndpointManager
#include "UnhelloedEndpoint.h"  // Own interface



namespace ss
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
	Message::HelloData* helloP;
	Endpoint2::Status   s;

	switch (headerP->code)
	{
	case Message::Hello:
		helloP = (Message::HelloData*) dataP;
		s = helloDataSet((Type) helloP->type, helloP->name, helloP->alias);
		if (s != OK)
		{
			stateSet(ScheduledForRemoval);
			LM_RE(s, ("Bad hello data"));
		}

		if (headerP->type == Message::Msg)
		{
			if ((s = helloSend(Message::Ack)) != OK)
			{
				stateSet(ScheduledForRemoval);
				LM_RE(s, ("helloSend error"));
			}
		}
		epMgr->add(this);
		break;

	default:
		LM_E(("I only treat Hello messages - got a '%s', removing endpoint", messageCode(headerP->code)));
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
* helloSend - 
*/
Endpoint2::Status UnhelloedEndpoint::helloSend(Message::MessageType type)
{
	Message::HelloData hello;

	memset(&hello, 0, sizeof(hello));

	strncpy(hello.name,   epMgr->me->nameGet(),    sizeof(hello.name));
	strncpy(hello.ip,     epMgr->me->hostname(),   sizeof(hello.ip));
	strncpy(hello.alias,  epMgr->me->aliasGet(),   sizeof(hello.alias));

	hello.type     = epMgr->me->typeGet();
	hello.coreNo   = 0;
	hello.workerId = 0;

	LM_T(LmtWrite, ("sending hello %s to '%s' (my name: '%s', my type: '%s')", messageType(type), name, hello.name, epMgr->me->typeName()));

	return send(type, Message::Hello, &hello, sizeof(hello));
}



/* ****************************************************************************
*
* helloExchange - 
*/
Endpoint2::Status UnhelloedEndpoint::helloExchange(int secs, int usecs)
{
	Message::Header       header;
	int                   dataLen = 0;
	void*                 dataP   = NULL;
	Message::HelloData*   helloP;
	Packet                packet(Message::Unknown);
	Endpoint2::Status     s;

	LM_M(("Sending Hello Msg to %s@%s", name, hostname()));
	if ((s = helloSend(Message::Msg)) != 0)
	{
		free(dataP);
		LM_RE(s, ("helloSend(%s@%s): %s", helloP->alias, hostname(), status(s)));
	}
	LM_M(("Hello sent successfully"));

	LM_M(("Awaiting reply"));
	if ((s = msgAwait(secs, usecs)) != 0)
		LM_RE(s, ("Endpoint2::msgAwait(expecting Hello): %s", status(s)));
	LM_M(("Reply seems on its way in"));

	LM_M(("Reading reply"));
	if ((s = receive(&header, &dataP, &dataLen, &packet)) != OK)
		LM_RE(s, ("Endpoint2::receive(expecting Hello): %s", status(s)));

	LM_M(("Checking validity of reply (code: 0x%x)", header.code));
	if ((header.code != Message::Hello) || (header.type != Message::Ack))
	{
		free(dataP);
		LM_RE(Error, ("Message read not a Hello Msg (%s %s)", messageCode(header.code), messageType(header.type)));
	}

    LM_M(("Adapting the KNOWN endpoints characteristics"));
	helloP = (Message::HelloData*) dataP;
	if ((s = helloDataSet((Endpoint2::Type) helloP->type, helloP->name, helloP->alias)) != OK)
	{
		free(dataP);
		LM_RE(s, ("helloDataSet(): %s", status(s)));
	}

	free(dataP);
	LM_M(("Successful Hello interchange"));
	
	return OK;
}

}
