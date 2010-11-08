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



/* ****************************************************************************
*
* iomMsgAwait - await a message from a peer
*/
extern int iomMsgAwait(int fd, int secs, int usecs);

#endif
