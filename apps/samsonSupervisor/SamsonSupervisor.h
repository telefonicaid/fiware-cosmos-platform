#ifndef SAMSON_SUPERVISOR_H
#define SAMSON_SUPERVISOR_H

/* ****************************************************************************
*
* FILE                     SamsonSupervisor.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 15 2010
*
*/
#include "NetworkInterface.h"   // DataReceiverInterface, EndpointUpdateInterface
#include "Endpoint.h"           // Endpoint
#include "Network.h"            // Network
#include "Message.h"            // ss::Message::Header



/* ****************************************************************************
*
* SamsonSupervisor - 
*/
class SamsonSupervisor : public ss::DataReceiverInterface, public ss::EndpointUpdateInterface
{
public:
	SamsonSupervisor(ss::Network* nwP) { networkP = nwP; }

	virtual int receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP);
	virtual int endpointUpdate(ss::Endpoint* ep, const char* reason, void* info = NULL);

private:
	ss::Network*    networkP;
};

#endif
