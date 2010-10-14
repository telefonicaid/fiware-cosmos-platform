/* ****************************************************************************
*
* FILE                     iomInit.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Oct 11 2010
*
*/
#include <vector>               // vector

#include "logMsg.h"             // LM_*
#include "Endpoint.h"			// Endpoint
#include "iomInit.h"            // Own interface
#include "iomConnect.h"         // iomConnect



/* ****************************************************************************
*
* iomInit -  worker init
*/
void iomInit(ss::Endpoint* controller)
{
	controller->fd = iomConnect((const char*) controller->ip.c_str(), (unsigned short) controller->port);
	if (controller->fd == -1)
		LM_X(1, ("error connecting to controller at %s:%d", controller->ip.c_str(), controller->port));

	controller->state = ss::Endpoint::Connected;
}
