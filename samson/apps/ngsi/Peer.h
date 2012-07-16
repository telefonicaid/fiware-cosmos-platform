#ifndef PEER_H
#define PEER_H

/* ****************************************************************************
*
* FILE                  Peer.h -  
*
*
*
*/



/* ****************************************************************************
*
* PeerType - 
*/
typedef enum PeerType
{
	ContextConsumer    = 1,
	ContextProducer,
	ContextBroker,
	ContextApplication
} PeerType;



/* ****************************************************************************
*
* Peer - 
*/
typedef struct Peer
{
	char          hostname[64];
	int           ip;
	PeerType      type;
	struct Peer*  next;
} Peer;



/* ****************************************************************************
*
* peerAdd - 
*/
extern void peerAdd(const char* hostname, int ip);

#endif
