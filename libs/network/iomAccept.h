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
#include <vector>               // vector



/* ****************************************************************************
*
* iomAccept -  accept an incoming connection
*/
extern int iomAccept(int fd, char* hostName = NULL, int hostNameLen = 0, char* ip = NULL, int ipLen = 0);

#endif
