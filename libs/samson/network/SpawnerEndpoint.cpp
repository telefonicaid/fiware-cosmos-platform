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
#include <unistd.h>               // close
#include <fcntl.h>                // F_SETFD
#include <pthread.h>              // pthread_t

#include "logMsg/logMsg.h"               // LM_*
#include "logMsg/traceLevels.h"          // Lmt*

#include "samson/common/status.h"
#include "samson/common/ports.h"                 // SPAWNER_PORT
#include "samson/common/samsonDirectories.h"     // SAMSON_PLATFORM_PROCESSES
#include "samson/network/EndpointManager.h"      // EndpointManager
#include "Packet.h"
#include "UnhelloedEndpoint.h"                   // UnhelloedEndpoint
#include "SpawnerEndpoint.h"                     // Own interface



namespace samson
{



/* ****************************************************************************
*
* SpawnerEndpoint Constructor
*/
SpawnerEndpoint::SpawnerEndpoint
(
	EndpointManager*  _epMgr,
	int               _id,
	Host*             _host,
	int               _rFd,
	int               _wFd
) : Endpoint2(_epMgr, Spawner, _id, _host, SPAWNER_PORT, _rFd, _wFd)
{
}



/* ****************************************************************************
*
* SpawnerEndpoint Constructor - 
*/
SpawnerEndpoint::SpawnerEndpoint(UnhelloedEndpoint* ep) : Endpoint2(ep->epMgrGet(), Spawner, ep->idGet(), ep->hostGet(), SPAWNER_PORT, ep->rFdGet(), ep->wFdGet())
{
	LM_T(LmtUnhelloed, ("Created a Spawner Endpoint from an Unhelloed Endpoint"));
	// Fix id ?
}



/* ****************************************************************************
*
* ~SpawnerEndpoint - 
*/
SpawnerEndpoint::~SpawnerEndpoint() // : ~Endpoint2()
{
}



/* ****************************************************************************
*
* msgTreat2 - 
*/
Status SpawnerEndpoint::msgTreat2(Packet* packetP)
{
	switch (packetP->msgCode)
	{
	default:
		if (epMgr->packetReceiver == NULL)
			LM_X(1, ("No packetReceiver (SW bug) - got a '%s' %s from %s", messageCode(packetP->msgCode), messageType(packetP->msgType), name()));

		epMgr->packetReceiver->_receive(packetP);
		break;
	}

	return OK;
}

}
