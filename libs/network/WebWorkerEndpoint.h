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
#include "Endpoint2.h"          // Endpoint2



namespace ss
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
		const char*      _name,
		const char*      _alias,
		Host*            _host,
		unsigned short   _port  = 0,
		int              _rFd   = -1,
		int              _wFd   = -1
	);
	~WebWorkerEndpoint();

	void  msgTreat(void);
	void  run(void);
};

}
#endif
