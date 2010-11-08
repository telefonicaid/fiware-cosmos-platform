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
* iomAccept -  connect to the controller
*/
extern int iomAccept(int fd, char* hostName = NULL, int hostNameLen = 0);

#endif
