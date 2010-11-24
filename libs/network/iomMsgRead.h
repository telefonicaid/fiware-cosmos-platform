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
#include "Packet.h"             // Packet
#include "Message.h"            // MessageType, Code, etc.



/* ****************************************************************************
*
* iomMsgRead - read a message from a peer
*/
extern int iomMsgRead
(
	int                        fd,
	const char*                from,
	ss::Message::MessageCode*  msgCodeP,
	ss::Message::MessageType*  msgTypeP,
	void**                     dataPP,
	int*                       dataLenP,
	ss::Packet*                packetP,
	void*                      kvData,
	int*                       kvDataLenP
);



/* ****************************************************************************
*
* iomMsgRead - read a message from an endpoint
*/
extern int iomMsgRead
(
	ss::Endpoint*              ep,
	ss::Message::Header*       headerP,
	ss::Message::MessageCode*  msgCodeP,
	ss::Message::MessageType*  msgTypeP,
	void**                     dataPP,
	int*                       dataLenP,
	ss::Packet*                packetP,
	void*                      kvData,
	int*                       kvDataLenP
);

#endif
