#ifndef STARTER_ENDPOINT_H
#define STARTER_ENDPOINT_H

/* ****************************************************************************
*
* FILE                     StarterEndpoint.h
*
* DESCRIPTION              Class for starter endpoints
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 02 2011
*
*/
#include "samson/network/Endpoint2.h"          // Endpoint2



namespace samson
{



/* ****************************************************************************
*
* StarterEndpoint - 
*/
class StarterEndpoint : public Endpoint2
{
public:
	StarterEndpoint
	(
		EndpointManager* _epMgr,
		int              _id,
		Host*            _host,
		int              _rFd   = -1,
		int              _wFd   = -1
	);
	~StarterEndpoint();

	Status msgTreat2(Packet* packetP);
};

}
#endif
