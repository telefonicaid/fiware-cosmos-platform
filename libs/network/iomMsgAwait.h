#ifndef IOM_MSG_AWAIT
#define IOM_MSG_AWAIT

/* ****************************************************************************
*
* FILE                     iomMsgAwait.h
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
* iomMsgAwait - await a message from a peer
*/
extern int iomMsgAwait(ss::Endpoint* epP, int secs, int usecs);

#endif
