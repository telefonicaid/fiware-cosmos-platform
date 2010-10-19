#ifndef IOM_MSG_SEND
#define IOM_MSG_SEND

/* ****************************************************************************
*
* FILE                     iomMsgSend.h
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
* iomMsgSend - send a message to a peer and await the reply
*/
extern int iomMsgSend(ss::Endpoint* epP, ss::Packet* packetP, char* sender, void* data = NULL, int dataLen = 0);

#endif
