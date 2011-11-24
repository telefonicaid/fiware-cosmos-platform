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
Status DelilahEndpoint::msgTreat2(Packet* packetP)
{
	switch (packetP->msgCode)
	{
	case Message::ProcessVector:
		ack(packetP->msgCode, epMgr->procVecGet(), epMgr->procVecGet()->processVecSize);
		break;
            
        default:
            if (epMgr->packetReceiver == NULL)
            {
                // Some delilahs may not implement this
                //LM_W(("No packetReceiver (SW bug) - got a '%s' %s from %s", messageCode(packetP->msgCode), messageType(packetP->msgType), name()));
            }
            else
                epMgr->packetReceiver->_receive(packetP);
            return OK;
	}
    
	return OK;
}

}
