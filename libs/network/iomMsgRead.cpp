/* ****************************************************************************
*
* FILE                     iomMsgRead.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Oct 13 2010
*
*/
#include <unistd.h>             // write
#include <memory.h>             // memset

#include "logMsg.h"             // LM_*
#include "networkTraceLevels.h" // LMT_NWRUN, ...

#include "Endpoint.h"           // Endpoint
#include "Packet.h"             // Packet
#include "MsgHeader.h"          // MsgHeader
#include "iomMsgRead.h"         // Own interface



/* ****************************************************************************
*
* iomMsgRead - read a message from an endpoint
*/
int iomMsgRead(ss::Endpoint* epP, ss::Packet* packetP, void* data, int dataLen)
{
	return 0;
}	
