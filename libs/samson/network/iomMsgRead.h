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
#include "samson/network/Packet.h"             // Packet
#include "samson/network/Message.h"            // MessageType, Code, etc.
#include "samson/network/Endpoint.h"           // Endpoint



/* ****************************************************************************
*
* iomMsgPartRead - read from a socket until completed or error
*/
ssize_t iomMsgPartRead(samson::Endpoint* ep, const char* what, char* buf, ssize_t bufLen);



/* ****************************************************************************
*
* iomMsgRead - read a message from an endpoint
*/
extern int iomMsgRead
(
	samson::Endpoint*              ep,
	samson::Message::Header*       headerP,
	samson::Message::MessageCode*  msgCodeP,
	samson::Message::MessageType*  msgTypeP,
	void**                     dataPP,
	int*                       dataLenP,
	samson::Packet*                packetP    = NULL,
	void*                      kvData     = NULL,
	int*                       kvDataLenP = NULL
);

#endif
