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
	default:
		if (epMgr->packetReceiver == NULL)
			LM_X(1, ("No packetReceiver (SW bug) - got a '%s' %s from %s%d@%s", messageCode(headerP->code), messageType(headerP->type), typeName(), id, host->name));

		epMgr->packetReceiver->_receive(packetP);
		break;
	}

	return OK;
}

}
