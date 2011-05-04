#ifndef LISTENER_ENDPOINT_H
#define LISTENER_ENDPOINT_H

/* ****************************************************************************
*
* FILE                     ListenerEndpoint.h
*
* DESCRIPTION              Class for listener endpoints
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 27 2011
*
*/
#include "Endpoint2.h"          // Endpoint2



namespace ss
{


class UnhelloedEndpoint;



/* ****************************************************************************
*
* ListenerEndpoint - 
*/
class ListenerEndpoint : public Endpoint2
{
public:
	ListenerEndpoint
	(
		EndpointManager* _epMgr,
		const char*      _name,
		const char*      _alias,
		Host*            _host,
		unsigned short   _port  = 0,
		int              _rFd   = -1,
		int              _wFd   = -1
	);
	~ListenerEndpoint();

	UnhelloedEndpoint*  accept(void);
	virtual void        msgTreat(void);

private:
	Endpoint2::Status   init(void);
};

}
#endif
