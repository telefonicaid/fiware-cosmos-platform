/* ****************************************************************************
*
* FILE                     Network.cpp - Definition for the network interface
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 08 2011
*
*/
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "iomServerOpen.h"      // iomServerOpen
#include "iomMsgSend.h"         // iomMsgSend
#include "Host.h"               // Host
#include "HostMgr.h"            // HostMgr
#include "Endpoint2.h"          // Endpoint2
#include "EndpointManager.h"    // EndpointManager
#include "Network2.h"           // Own interface



namespace ss
{



/* ****************************************************************************
*
* Constructor
*/
Network2::Network2(EndpointManager* _epMgr)
{
	epMgr = _epMgr;
}



/* ****************************************************************************
*
* run
*/
void Network2::run(void)
{
}



/* ****************************************************************************
*
* poll
*/
int Network2::poll(void)
{
	return 0;
}

}
