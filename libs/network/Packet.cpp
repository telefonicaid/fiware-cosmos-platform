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

#include "Buffer.h"				// ss::Buffer

namespace ss
{



/* ****************************************************************************
*
* endpointAdd - 
*/
void Packet::endpointAdd(Endpoint* ep)
{
	network::EndPointVector  *v = message.mutable_endpoints();
	::ss::network::EndPoint* e = v->add_item();

	e->set_ip(ep->ip);
	e->set_port(ep->port);
	e->set_type(ep->type);
	e->set_name(ep->name);

	LM_T(LMT_WRITE, ("size of message: %d", message.ByteSize()));
}
	


/* ****************************************************************************
*
* endpointVectorAdd - 
*/
void Packet::endpointVectorAdd(std::vector<Endpoint>& eps)
{
	LM_T(LMT_WRITE, ("Adding %d endpoints", eps.size()));

	for (size_t i = 0; i < eps.size(); i++)
	{
		LM_T(LMT_WRITE, ("Adding endpoint %d: '%s'", i, eps[i].name.c_str()));
		endpointAdd(&eps[i]);
	}
}
	


/* ****************************************************************************
*
* endpointVecSize - 
*/
int Packet::endpointVecSize(void)
{
	return message.endpoints().item_size();
}
	


/* ****************************************************************************
*
* endpointGet - 
*/
Endpoint Packet::endpointGet(int i)
{
	network::EndPointVector v = message.endpoints();
	::ss::network::EndPoint e = v.item(i);

	LM_T(LMT_INIT_EP, ("Creating new endpoint with the '%s:%d'", e.ip().c_str(), e.port()));
	Endpoint ep((ss::Endpoint::Type) e.type(), std::string("noname"), e.ip(), e.port(), -1);

	return ep;
}



/* ****************************************************************************
*
* str - 
*/
std::string Packet::str()
{
	std::ostringstream o;
	o << "Packet buffer with " << au::Format::string( buffer->getSize() ) << " bytes"; 
	return o.str();
}

}
