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

#include "logMsg.h"               // LM_*
#include "traceLevels.h"          // Lmt*

#include "ports.h"                // SPAWNER_PORT
#include "EndpointManager.h"      // EndpointManager
#include "UnhelloedEndpoint.h"    // UnhelloedEndpoint
#include "SpawnerEndpoint.h"      // Own interface



namespace ss
{



/* ****************************************************************************
*
* SpawnerEndpoint - 
*/
SpawnerEndpoint::SpawnerEndpoint
(
	EndpointManager*  _epMgr,
	int               _id,
	const char*       _name,
	const char*       _alias,
	Host*             _host,
	unsigned short    _port,
	int               _rFd,
	int               _wFd
) : Endpoint2(_epMgr, Spawner, _id, _name, _alias, _host, SPAWNER_PORT, _rFd, _wFd)
{
}



/* ****************************************************************************
*
* Constructor - 
*/
	SpawnerEndpoint::SpawnerEndpoint(UnhelloedEndpoint* ep) : Endpoint2(ep->epMgrGet(), Spawner, ep->idGet(), ep->nameGet(), ep->aliasGet(), ep->hostGet(), SPAWNER_PORT, ep->rFdGet(), ep->wFdGet())
{
	LM_M(("Created a Spawner Endpoint from an Unhelloed Endpoint"));
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
	switch (headerP->code)
	{
	default:
		LM_X(1, ("No messages treated - got a '%s'", messageCode(headerP->code)));
		return Error;
	}

	return OK;
}

}
