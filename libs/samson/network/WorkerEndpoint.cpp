/* ****************************************************************************
*
* FILE                     WorkerEndpoint.h
*
* DESCRIPTION              Class for worker endpoints
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 02 2011
*
*/
#include <unistd.h>             // close
#include <fcntl.h>              // F_SETFD
#include <pthread.h>            // pthread_t

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "samson/common/ports.h"
#include "Packet.h"
#include "EndpointManager.h"
#include "WorkerEndpoint.h"



namespace samson
{



/* ****************************************************************************
*
* WorkerEndpoint - 
*/
WorkerEndpoint::WorkerEndpoint
(
	EndpointManager*  _epMgr,
	int               _id,
	const char*       _name,
	const char*       _alias,
	Host*             _host,
	int               _rFd,
	int               _wFd
) : Endpoint2(_epMgr, Worker, _id, _name, _alias, _host, WORKER_PORT, _rFd, _wFd)
{
}



/* ****************************************************************************
*
* ~WorkerEndpoint - 
*/
WorkerEndpoint::~WorkerEndpoint() // : ~Endpoint2()
{
}



/* ****************************************************************************
*
* msgTreat2 - 
*/
Endpoint2::Status WorkerEndpoint::msgTreat2(Message::Header* headerP, void* dataP, int dataLen, Packet* packetP)
{
	switch (headerP->code)
	{
	case Message::WorkerStatus:
	case Message::Command:
		if (epMgr->packetReceiver == NULL)
			LM_X(1, ("No packetReceiver - no real use to contiune, this is a SW bug!"));

		epMgr->packetReceiver->_receive(packetP);
		break;

	default:
		LM_X(1, ("Sorry, no message treat implemented for '%s' '%s'", messageCode(headerP->code), messageType(headerP->type)));
		return Error;
	}

	return OK;
}

}
