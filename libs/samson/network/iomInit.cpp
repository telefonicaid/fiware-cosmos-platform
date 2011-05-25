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

#include "logMsg/logMsg.h"             // LM_*
#include "samson/network/Endpoint.h"			// Endpoint
#include "iomInit.h"            // Own interface
#include "samson/network/iomConnect.h"         // iomConnect



/* ****************************************************************************
*
* iomInit -  worker init
*/
void iomInit(samson::Endpoint* controller)
{
	controller->rFd = iomConnect((const char*) controller->ip, (unsigned short) controller->port);
	if (controller->rFd == -1)
		LM_X(1, ("error connecting to controller at %s:%d", controller->ip, controller->port));

	controller->state = samson::Endpoint::Connected;
	controller->wFd   = controller->rFd;
}
