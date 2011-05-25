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
#include <unistd.h>               // close
#include <fcntl.h>                // F_SETFD
#include <pthread.h>              // pthread_t

#include "logMsg/logMsg.h"               // LM_*
#include "logMsg/traceLevels.h"          // Lmt*

#include "samson/common/ports.h"                // CONTROLLER_PORT
#include "samson/network/EndpointManager.h"      // EndpointManager
#include "ControllerEndpoint.h"   // Own interface



namespace samson
{



/* ****************************************************************************
*
* ControllerEndpoint - 
*/
ControllerEndpoint::ControllerEndpoint
(
	EndpointManager*  _epMgr,
	const char*       _name,
	const char*       _alias,
	Host*             _host,
	int               _rFd,
	int               _wFd
) : Endpoint2(_epMgr, Controller, 0, _name, _alias, _host, CONTROLLER_PORT, _rFd, _wFd)
{
}



/* ****************************************************************************
*
* ~ControllerEndpoint - 
*/
ControllerEndpoint::~ControllerEndpoint() // : ~Endpoint2()
{
}



/* ****************************************************************************
*
* msgTreat2 - 
*/
Endpoint2::Status ControllerEndpoint::msgTreat2(Message::Header* headerP, void* dataP, int dataLen, Packet* packetP)
{
	switch (headerP->code)
	{
	default:
		LM_X(1, ("No messages treated - got a '%s'", messageCode(headerP->code)));
		return Error;
	}

	return OK;
}

}
