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
#include <unistd.h>             // close
#include <fcntl.h>              // F_SETFD
#include <pthread.h>            // pthread_t

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "samson/common/status.h"
#include "samson/common/ports.h"
#include "Packet.h"
#include "EndpointManager.h"
#include "WorkerEndpoint.h"



namespace samson
{



/* ****************************************************************************
*
* WorkerEndpoint - 
*/
WorkerEndpoint::WorkerEndpoint
(
	EndpointManager*  _epMgr,
	int               _id,
	Host*             _host,
	int               _rFd,
	int               _wFd
) : Endpoint2(_epMgr, Worker, _id, _host, WORKER_PORT, _rFd, _wFd)
{
}



/* ****************************************************************************
*
* ~WorkerEndpoint - 
*/
WorkerEndpoint::~WorkerEndpoint() // : ~Endpoint2()
{
}



/* ****************************************************************************
*
* msgTreat2 - 
*/
Status WorkerEndpoint::msgTreat2(Packet* packetP)
{
	switch (packetP->msgCode)
	{
        default:
            if (epMgr->packetReceiver == NULL)
                LM_W(("No packetReceiver - got a '%s' %s from %s.Ignoring...", messageCode(packetP->msgCode), messageType(packetP->msgType), name()));
            else
                epMgr->packetReceiver->_receive(packetP);
            break;
	}
    
	return OK;
}
    
}
