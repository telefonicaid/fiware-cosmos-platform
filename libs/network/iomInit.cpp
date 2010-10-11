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
#include "iomConnect.h"         // iomConnect
#include "iomInit.h"            // Own interface



#if 1
/* ****************************************************************************
*
* Temporal stuff just so that it compiles ...
*/
typedef enum SsioMsgCode
{
	SsioWorkerList
} SsioMsgCode;

typedef struct SsioHeader
{
	SsioMsgCode msgCode;
} SsioHeader;

typedef union SsioData
{
	char data[1];
	char workerList[1024];
} SsioData;

void iomMsgSend(ss::Endpoint* epP, SsioMsgCode msgCode, SsioHeader* headerP, SsioData* dataP)
{
}

void connectToAll(char* workerList)
{
}
#endif

namespace ss
{



/* ****************************************************************************
*
* iomInit -  worker init
*/
void iomInit(Endpoint* controller)
{
	SsioHeader header;
	SsioData   data;

	controller->fd = iomConnect((const char*) controller->ip.c_str(), (unsigned short) controller->port);
	if (controller->fd == -1)
		LM_X(1, ("error connecting to controller at %s:%d", controller->ip.c_str(), controller->port));

	controller->state = Endpoint::Connected;

	/* ask controller for list of workers */
	iomMsgSend(controller, SsioWorkerList, &header, &data);

	connectToAll(data.workerList);
}

}
