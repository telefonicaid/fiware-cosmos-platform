/* *******************************************************************************
*
* FILE                     Packet.cpp
*
* DESCRIPTION				Definition of the pakcet to be exchange in the samson-ecosystem
*
*/
#include <sstream>		// std::ostringstream

#include "Format.h"		// au::Format
#include "Packet.h"		// Own interface
#include "Endpoint.h"	// Endpoint


namespace ss
{
	
	void Packet::setMessageCode( int c )
	{
		message.set_code( c );
	}
	
	void Packet::addEndPoints( std::vector<Endpoint>& _es )
	{
		for (size_t i = 0 ; i < _es.size() ; i++)
			addEndpoint( &_es[i] );
	}
	
	void Packet::addEndpoint( Endpoint *_e )
	{
		network::EndPointVector v = message.endpoints();
		::ss::network::EndPoint *e = v.add_item();
		e->set_ip( _e->ip );
		e->set_port( _e->port );
		e->set_name(_e->name );
	}
	
	int Packet::getNumEndpoints()
	{
		return message.endpoints().item_size();
	}
	
	Endpoint Packet::getEndpoint( int i )
	{
		network::EndPointVector v = message.endpoints();
		::ss::network::EndPoint e = v.item(i);
		
		std::ostringstream o;
		o << e.ip() << ":" << e.port();
		Endpoint _e( o.str() );
		return _e;
	}
	
	std::string Packet::str()
	{
		std::ostringstream o;
		o << "Packet buffer with " << au::Format::string( buffer.getLength() ) << " bytes"; 
		return o.str();
	}
	
	
}
