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
* messageTypeSet - 
*/
void Packet::messageTypeSet(ss::network::Message_Type type)
{
	message.set_type(type);
}
	


/* ****************************************************************************
*
* messageTypeGet - 
*/
int Packet::messageTypeGet()
{
	return message.type();
}

	

/* ****************************************************************************
*
* messageInfoSet - 
*/
void Packet::messageInfoSet(ss::network::Message_Info info)
{
	message.set_info(info);
}
	


/* ****************************************************************************
*
* messageInfoGet - 
*/
int Packet::messageInfoGet()
{
	return message.info();
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
	ss::network::Hello* hP = message.mutable_hello();

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
	ss::network::Hello hello = message.hello();

	*nameP             = strdup(hello.name().c_str());
	*connectedWorkersP = hello.workers();
	*typeP             = (Endpoint::Type) hello.type();
	*ipP               = strdup(hello.ip().c_str());
	*portP             = hello.port();
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
* msgTypeName - 
*/
char* Packet::msgTypeName(ss::network::Message_Type type)
{
	switch (type)
	{
	case ss::network::Message_Type_Hello:                       return (char*) "Hello";
	case ss::network::Message_Type_WorkerVector:                return (char*) "WorkerVector";
	case ss::network::Message_Type_WorkerTask:		            return (char*) "WorkerTask";
	case ss::network::Message_Type_WorkerTaskConfirmation:		return (char*) "WorkerTaskConfirmation";
	case ss::network::Message_Type_Command:				        return (char*) "Command";
	case ss::network::Message_Type_CommandResponse:		        return (char*) "CommandResponse";
	}

	return (char*) "UnknownMsgType";
}

}
