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
/* ****************************************************************************
*
* FILE                  Peer.h -  
*
*
*
*/
#include "logMsg/logMsg.h"              // LM_*

#include "traceLevels.h"                // Trace levels for log msg library
#include "Peer.h"                       /// Own interface



/* ****************************************************************************
*
* Globals
*/
static Peer*         peerV            = NULL;
static int           peers            = 0;



/* ****************************************************************************
*
* peerInit - 
*/
void peerInit(void)
{
	peerV = NULL;
	peers = 0;
}



/* ****************************************************************************
*
* peerAdd - 
*/
void peerAdd(const char* hostname, int ip)
{
	Peer* peerP = peerV;

	while (peerP != NULL)
	{
		if (peerP->ip == ip)
		{
			LM_T(LmtPeer, ("Peer '%s' already in per list", hostname));
			return;
		}

		if (peerP->next == NULL)
			break;

		peerP = peerP->next;
	}

	Peer* newPeer = (Peer*) malloc(sizeof(Peer));

	if (newPeer == NULL)
		LM_X(1, ("malloc: %s", strerror(errno)));

	strncpy(newPeer->hostname, hostname, sizeof(newPeer->hostname));
	newPeer->ip   = ip;
	newPeer->next = NULL;

	if (peerV == NULL)
		peerV = newPeer;
	else
		peerP->next = newPeer;

	LM_T(LmtPeer, ("Added peer '%s'", newPeer->hostname));
}
