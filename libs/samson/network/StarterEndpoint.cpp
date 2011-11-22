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
#include <unistd.h>             // close
#include <fcntl.h>              // F_SETFD
#include <pthread.h>            // pthread_t

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "samson/common/status.h"
#include "samson/common/ports.h"
#include "Packet.h"
#include "EndpointManager.h"
#include "StarterEndpoint.h"



namespace samson
{



/* ****************************************************************************
*
* StarterEndpoint - 
*/
StarterEndpoint::StarterEndpoint
(
	EndpointManager*  _epMgr,
	int               _id,
	Host*             _host,
	int               _rFd,
	int               _wFd
) : Endpoint2(_epMgr, Starter, _id, _host, 0, _rFd, _wFd)
{
}



/* ****************************************************************************
*
* ~StarterEndpoint - 
*/
StarterEndpoint::~StarterEndpoint() // : ~Endpoint2()
{
}



/* ****************************************************************************
*
* msgTreat2 - 
*/
Status StarterEndpoint::msgTreat2(Packet* packetP)
{
	if (epMgr->packetReceiver == NULL)
		LM_W(("No packetReceiver (SW bug) - got a '%s' %s from %s", messageCode(packetP->msgCode), messageType(packetP->msgType), name()));
	else
	{
		LM_T(LmtMsg, ("Got a message I cannot treat (%s) - forwarding to packetReceiver", messageCode(packetP->msgCode)));
		epMgr->packetReceiver->_receive(packetP);
	}
	return OK;
}

}
