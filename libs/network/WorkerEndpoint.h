#ifndef WORKER_ENDPOINT_H
#define WORKER_ENDPOINT_H

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
#include "Endpoint2.h"          // Endpoint2



namespace ss
{



/* ****************************************************************************
*
* WorkerEndpoint - 
*/
class WorkerEndpoint : public Endpoint2
{
public:
	WorkerEndpoint
	(
		EndpointManager* _epMgr,
		int              _id,
		const char*      _name,
		const char*      _alias,
		Host*            _host,
		int              _rFd   = -1,
		int              _wFd   = -1
	);
	~WorkerEndpoint();

	Status msgTreat2(Message::Header* headerP, void* dataP, int dataLen, Packet* packetP);
};

}
#endif
