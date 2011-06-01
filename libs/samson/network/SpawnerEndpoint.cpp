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

#include "samson/common/ports.h"                // SPAWNER_PORT
#include "samson/common/samsonDirectories.h"    // SAMSON_PLATFORM_PROCESSES
#include "samson/network/EndpointManager.h"      // EndpointManager
#include "UnhelloedEndpoint.h"    // UnhelloedEndpoint
#include "SpawnerEndpoint.h"      // Own interface



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
Endpoint2::Status SpawnerEndpoint::msgTreat2(Message::Header* headerP, void* dataP, int dataLen, Packet* packetP)
{
	if (epMgr->dataReceiver == NULL)
		LM_X(1, ("No data receiver"));

	return (Endpoint2::Status) epMgr->dataReceiver->receive(epMgr->ixGet(this), dataLen, headerP, dataP);
}

}
