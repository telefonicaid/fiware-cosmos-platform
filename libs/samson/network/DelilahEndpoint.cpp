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
#include <unistd.h>             // close
#include <fcntl.h>              // F_SETFD
#include <pthread.h>            // pthread_t

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "samson/common/ports.h"
#include "Packet.h"
#include "EndpointManager.h"
#include "DelilahEndpoint.h"



namespace samson
{



/* ****************************************************************************
*
* DelilahEndpoint - 
*/
DelilahEndpoint::DelilahEndpoint
(
	EndpointManager*  _epMgr,
	int               _id,
	const char*       _name,
	const char*       _alias,
	Host*             _host,
	int               _rFd,
	int               _wFd
) : Endpoint2(_epMgr, Delilah, _id, _name, _alias, _host, 0, _rFd, _wFd)
{
}



/* ****************************************************************************
*
* ~DelilahEndpoint - 
*/
DelilahEndpoint::~DelilahEndpoint() // : ~Endpoint2()
{
}



/* ****************************************************************************
*
* msgTreat2 - 
*/
Endpoint2::Status DelilahEndpoint::msgTreat2(Message::Header* headerP, void* dataP, int dataLen, Packet* packetP)
{
	switch (headerP->code)
	{
	case Message::ProcessVector:
		ack(headerP->code, epMgr->procVecGet(), epMgr->procVecGet()->processVecSize);
		break;

	default:
		LM_X(1, ("Sorry, no message treat implemented for '%s' '%s'", messageCode(headerP->code), messageType(headerP->type)));
		return Error;
	}

	return OK;
}

}
