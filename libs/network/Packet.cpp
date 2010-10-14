/* *******************************************************************************
*
* FILE                     Packet.cpp
*
* DESCRIPTION              Definition of the packet to be exchange in the samson-ecosystem
*
*/
#include <sstream>              // std::ostringstream

#include "logMsg.h"             // LM_*
#include "networkTraceLevels.h" // LMT_WRITE, ...

#include "Format.h"             // au::Format
#include "Endpoint.h"           // Endpoint
#include "Packet.h"             // Own interface


namespace ss
{

/* ****************************************************************************
*
* setMessageCode - 
*/
void Packet::setMessageCode(MessageCode code)
{
	message.set_code(code);
}
	


/* ****************************************************************************
*
* addEndpoint - 
*/
void Packet::addEndpoint(Endpoint* ep)
{
	network::EndPointVector  v = message.endpoints();
	::ss::network::EndPoint* e = v.add_item();

	e->set_ip(ep->ip);
	e->set_port(ep->port);
	e->set_name(ep->name);

	LM_T(LMT_WRITE, ("size of message: %d", message.ByteSize()));
}
	


/* ****************************************************************************
*
* addEndPoints - 
*/
void Packet::addEndPoints(std::vector<Endpoint>& eps)
{
	LM_T(LMT_WRITE, ("Adding %d endpoints", eps.size()));

	for (size_t i = 0; i < eps.size(); i++)
	{
		LM_T(LMT_WRITE, ("Adding endpoint %d: '%s'", i, eps[i].name.c_str()));
		addEndpoint(&eps[i]);
	}
}
	


/* ****************************************************************************
*
* getNumEndpoints - 
*/
int Packet::getNumEndpoints()
{
	return message.endpoints().item_size();
}
	


/* ****************************************************************************
*
* getEndpoint - 
*/
Endpoint Packet::getEndpoint( int i )
{
	network::EndPointVector v = message.endpoints();
	::ss::network::EndPoint e = v.item(i);
	
	std::ostringstream o;
	o << e.ip() << ":" << e.port();

	Endpoint ep(o.str());
	return ep;
}
	


/* ****************************************************************************
*
* str - 
*/
std::string Packet::str()
{
	std::ostringstream o;
	o << "Packet buffer with " << au::Format::string( buffer.getLength() ) << " bytes"; 
	return o.str();
}
	
	

/* ****************************************************************************
*
* msgCodeName - 
*/
char* Packet::msgCodeName(MessageCode code)
{
	switch (code)
	{
	case Hello:            return (char*) "Hello";
	case WorkerVector:     return (char*) "WorkerVector";
	}

	return (char*) "UnknownMsgCode";
}

}
