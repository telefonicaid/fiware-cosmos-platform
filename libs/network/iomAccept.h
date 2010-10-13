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

#include "Endpoint.h"			// Endpoint



namespace ss
{


/* ****************************************************************************
*
* iomAccept -  connect to the controller
*/
extern int iomAccept(Endpoint* listener, char* hostName, int hostNameLen);

}

#endif
