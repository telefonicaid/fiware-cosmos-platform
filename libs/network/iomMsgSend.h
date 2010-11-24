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
#include "Message.h"            // Message::MessageCode, Message::MessageType



/* ****************************************************************************
*
* iomMsgSend - send a message to a peer and await the reply
*/
extern int iomMsgSend
(
	int                       fd,
	const char*               to,
	const char*               from,
	ss::Message::MessageCode  code,
	ss::Message::MessageType  type       = ss::Message::Msg,
	void*                     data       = NULL,
	int                       dataLen    = 0,
	ss::Packet*               packetP    = NULL
);



/* ****************************************************************************
*
* iomMsgSend - send a message to an endpoint
*/
extern int iomMsgSend
(
	ss::Endpoint*               to,
	ss::Endpoint*               from,
	ss::Message::MessageCode    code,
	ss::Message::MessageType    type    = ss::Message::Msg,
	void*                       data    = NULL,
	int                         dataLen = 0,
	ss::Packet*                 packetP = NULL
);

#endif
