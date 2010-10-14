#ifndef IOM_MSG_READ
#define IOM_MSG_READ

/* ****************************************************************************
*
* FILE                     iomMsgRead.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Oct 13 2010
*
*/
#include "Endpoint.h"           // Endpoint
#include "Packet.h"             // Packet



/* ****************************************************************************
*
* iomMsgRead - read a message to a peer and await the reply
*/
extern int iomMsgRead(ss::Endpoint* epP, ss::Packet* packetP, ss::Endpoint* controller, void* data = NULL, int dataLen = 0);

#endif
