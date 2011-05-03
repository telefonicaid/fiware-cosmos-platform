#ifndef CONTROLLER_ENDPOINT_H
#define CONTROLLER_ENDPOINT_H

/* ****************************************************************************
*
* FILE                     ControllerEndpoint.h
*
* DESCRIPTION              Class for controller endpoints
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
* ControllerEndpoint - 
*/
class ControllerEndpoint : public Endpoint2
{
public:
	ControllerEndpoint
	(
		EndpointManager* _epMgr,
		int              _id,
		const char*      _name,
		const char*      _alias,
		Host*            _host,
		unsigned short   _port  = 0,
		int              _rFd   = -1,
		int              _wFd   = -1
	);
	~ControllerEndpoint();

	Status msgTreat2(Message::Header* headerP, void* dataP, int dataLen, Packet* packetP);
};

}
#endif
