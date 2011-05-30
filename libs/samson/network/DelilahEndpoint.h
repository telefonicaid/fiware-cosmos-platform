#ifndef DELILAH_ENDPOINT_H
#define DELILAH_ENDPOINT_H

/* ****************************************************************************
*
* FILE                     DelilahEndpoint.h
*
* DESCRIPTION              Class for delilah endpoints
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
* DelilahEndpoint - 
*/
class DelilahEndpoint : public Endpoint2
{
public:
	DelilahEndpoint
	(
		EndpointManager* _epMgr,
		int              _id,
		const char*      _name,
		const char*      _alias,
		Host*            _host,
		int              _rFd   = -1,
		int              _wFd   = -1
	);
	~DelilahEndpoint();

	Status msgTreat2(Message::Header* headerP, void* dataP, int dataLen, Packet* packetP);
};

}
#endif
