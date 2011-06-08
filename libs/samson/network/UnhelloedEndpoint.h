#ifndef UNHELLOED_ENDPOINT_H
#define UNHELLOED_ENDPOINT_H

/* ****************************************************************************
*
* FILE                     UnhelloedEndpoint.h
*
* DESCRIPTION              Class for unhelloed endpoints
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 27 2011
*
*/
#include "samson/common/status.h"
#include "samson/network/Endpoint2.h"          // Endpoint2



namespace samson
{



/* ****************************************************************************
*
* UnhelloedEndpoint - 
*/
class UnhelloedEndpoint : public Endpoint2
{
public:
	UnhelloedEndpoint
	(
		EndpointManager* _epMgr,
		Host*            _host,
		unsigned short   _port  = 0,
		int              _rFd   = -1,
		int              _wFd   = -1
	);
	~UnhelloedEndpoint();

	virtual Status       msgTreat2(Packet* packetP);
	Status               helloDataSet(Type _type, int _id);
	Status               helloExchange(int secs, int usecs);     // send Hello Msg and await Ack - if OK, the endpoint has adopted the 

private:
	Status               init(void);
};

}
#endif
