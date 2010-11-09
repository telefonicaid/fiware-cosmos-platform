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

#endif
