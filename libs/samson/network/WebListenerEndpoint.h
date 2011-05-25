#ifndef WEB_LISTENER_ENDPOINT_H
#define WEB_LISTENER_ENDPOINT_H

/* ****************************************************************************
*
* FILE                     WebListenerEndpoint.h
*
* DESCRIPTION              Class for listener endpoints
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 27 2011
*
*/
#include "samson/network/Endpoint2.h"            // Endpoint2



namespace samson
{


class UnhelloedEndpoint;



/* ****************************************************************************
*
* WebListenerEndpoint - 
*/
class WebListenerEndpoint : public Endpoint2
{
public:
	WebListenerEndpoint
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
	~WebListenerEndpoint();

	UnhelloedEndpoint*  accept(void);
	virtual void        msgTreat(void);

private:
	Endpoint2::Status   init(void);
};

}
#endif
