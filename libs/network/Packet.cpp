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
* messageCodeSet - 
*/
void Packet::messageCodeSet(MessageCode code)
{
	message.set_code(code);
}
	


/* ****************************************************************************
*
* messageTypeSet - 
*/
void Packet::messageTypeSet(MessageType type)
{
	message.set_msgtype(type);
}
	


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
* helloAdd - 
*/
void Packet::helloAdd(char* name, int connectedWorkers, Endpoint::Type type, char* ip, unsigned short port)
{
	network::Hello*    hP = message.mutable_hello();

	hP->set_name(name);
	hP->set_workers(connectedWorkers);
	hP->set_type(type);

	if (port != 0)
		hP->set_port(port);

	if (ip != NULL)
		hP->set_ip(ip);
}



/* ****************************************************************************
*
* helloGet - 
*/
void Packet::helloGet(char** nameP, int* connectedWorkersP, Endpoint::Type* typeP, char** ipP, unsigned short* portP)
{
	network::Hello hello = message.hello();


	*nameP             = strdup(hello.name().c_str());
	*connectedWorkersP = hello.workers();
	*typeP             = (Endpoint::Type) hello.type();

	if (ipP != NULL)
		*ipP = strdup(hello.ip().c_str());

	if (portP != NULL)
		*portP = hello.port();
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
