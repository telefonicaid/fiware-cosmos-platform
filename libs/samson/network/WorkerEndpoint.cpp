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

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Lmt*

#include "samson/common/ports.h"              // WORKER_PORT
#include "samson/network/EndpointManager.h"    // EndpointManager
#include "WorkerEndpoint.h"     // Own interface



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
	default:
		LM_X(1, ("Sorry, no message treat implemented yet - got a '%s' '%s' (code %d)", messageCode(headerP->code), messageType(headerP->type), headerP->code));
		return Error;
	}

	return OK;
}

}
