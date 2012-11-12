/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
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
