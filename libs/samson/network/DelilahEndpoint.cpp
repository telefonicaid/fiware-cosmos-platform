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

#include "samson/common/status.h"
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
	Host*             _host,
	int               _rFd,
	int               _wFd
) : Endpoint2(_epMgr, Delilah, _id, _host, 0, _rFd, _wFd)
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
Status DelilahEndpoint::msgTreat2(Message::Header* headerP, void* dataP, int dataLen, Packet* packetP)
{
	switch (headerP->code)
	{
	case Message::ProcessVector:
		ack(headerP->code, epMgr->procVecGet(), epMgr->procVecGet()->processVecSize);
		break;

	default:
		if (epMgr->packetReceiver == NULL)
			LM_X(1, ("No packetReceiver (SW bug) - got a '%s' %s from %s", messageCode(headerP->code), messageType(headerP->type), name()));

		epMgr->packetReceiver->_receive(packetP);
		return OK;
	}

	return OK;
}

}
