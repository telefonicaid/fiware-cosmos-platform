#ifndef WEB_WORKER_ENDPOINT_H
#define WEB_WORKER_ENDPOINT_H

/* ****************************************************************************
*
* FILE                     WebWorkerEndpoint.h
*
* DESCRIPTION              Class for web worker endpoints
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 28 2011
*
*/
#include "samson/network/Endpoint2.h"          // Endpoint2



namespace samson
{



/* ****************************************************************************
*
* WebWorkerEndpoint - 
*/
class WebWorkerEndpoint : public Endpoint2
{
public:
	WebWorkerEndpoint
	(
		EndpointManager* _epMgr,
		int              _id,
		Host*            _host,
		unsigned short   _port  = 0,
		int              _rFd   = -1,
		int              _wFd   = -1
	);
	~WebWorkerEndpoint();

	Status  msgTreat2(void);
	void    run(void);
};

}
#endif
