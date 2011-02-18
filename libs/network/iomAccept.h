#ifndef IOM_ACCEPT
#define IOM_ACCEPT

/* ****************************************************************************
*
* FILE                     iomAccept.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Oct 11 2010
*
*/
#include <netinet/in.h>         // sockaddr_in



/* ****************************************************************************
*
* iomAccept -  accept an incoming connection
*/
extern int iomAccept(int fd, struct sockaddr_in* sinP, char* hostName = NULL, int hostNameLen = 0, char* ip = NULL, int ipLen = 0);

#endif
