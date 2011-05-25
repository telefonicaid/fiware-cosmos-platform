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
#include "samson/network/Endpoint.h"           // Endpoint
#include "samson/network/Packet.h"             // Packet
#include "samson/network/Message.h"            // Message::MessageCode, Message::MessageType


#if 0
/* ****************************************************************************
*
* iomMsgSend - send a message to a peer and await the reply
*/
extern int iomMsgSend
(
	int                       fd,
	const char*               to,
	const char*               from,
	samson::Message::MessageCode  code,
	samson::Message::MessageType  type       = samson::Message::Msg,
	void*                     data       = NULL,
	int                       dataLen    = 0,
	samson::Packet*               packetP    = NULL
);
#endif



/* ****************************************************************************
*
* iomMsgSend - send a message to an endpoint
*/
extern int iomMsgSend
(
	samson::Endpoint*               to,
	samson::Endpoint*               from,
	samson::Message::MessageCode    code,
	samson::Message::MessageType    type    = samson::Message::Msg,
	void*                       data    = NULL,
	int                         dataLen = 0,
	samson::Packet*                 packetP = NULL
);

#endif
