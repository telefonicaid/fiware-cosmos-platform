/* *******************************************************************************
*
* FILE                     Packet.cpp
*
* DESCRIPTION              Definition of the packet to be exchange in the samson-ecosystem
*
*/
#include <sstream>         // std::ostringstream

#include "Format.h"        // au::Format
#include "Endpoint.h"      // Endpoint
#include "Packet.h"        // Own interface


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
* addEndPoints - 
*/
void Packet::addEndPoints( std::vector<Endpoint>& _es )
{
	for (size_t i = 0 ; i < _es.size() ; i++)
		addEndpoint( &_es[i] );
}
	


/* ****************************************************************************
*
* addEndpoint - 
*/
void Packet::addEndpoint( Endpoint *_e )
{
	network::EndPointVector  v = message.endpoints();
	::ss::network::EndPoint* e = v.add_item();

	e->set_ip(_e->ip);
	e->set_port(_e->port);
	e->set_name(_e->name);
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
	Endpoint _e( o.str() );
	return _e;
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
