#ifndef SPAWNER_ENDPOINT_H
#define SPAWNER_ENDPOINT_H

/* ****************************************************************************
*
* FILE                     SpawnerEndpoint.h
*
* DESCRIPTION              Class for spawner endpoints
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 03 2011
*
*/
#include "samson/network/Endpoint2.h"             // Endpoint2
#include "UnhelloedEndpoint.h"     // UnhelloedEndpoint



namespace samson
{



/* ****************************************************************************
*
* SpawnerEndpoint - 
*/
class SpawnerEndpoint : public Endpoint2
{
public:
	SpawnerEndpoint(UnhelloedEndpoint*);
	SpawnerEndpoint
	(
		EndpointManager* _epMgr,
		int              _id,
		const char*      _name,
		const char*      _alias,
		Host*            _host,
		int              _rFd   = -1,
		int              _wFd   = -1
	);
	~SpawnerEndpoint();

	Status    msgTreat2(Message::Header* headerP, void* dataP, int dataLen, Packet* packetP);
};

}
#endif
