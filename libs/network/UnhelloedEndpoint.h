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
#include "Endpoint2.h"          // Endpoint2



namespace ss
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
		int              _id,
		const char*      _name,
		const char*      _alias,
		Host*            _host,
		unsigned short   _port  = 0,
		int              _rFd   = -1,
		int              _wFd   = -1
	);
	~UnhelloedEndpoint();

	virtual Status       msgTreat2(Message::Header* headerP, void* dataP, int dataLen, Packet* packetP);
	Status               helloDataAdd(Type _type, const char* _name, const char* _alias);
	Status               helloSend(Message::MessageType type);   // send Hello Msg/Ack/Nak to endpoint
	Status               helloRead(void);

private:
	Endpoint2::Status  init(void);
};

}
#endif
