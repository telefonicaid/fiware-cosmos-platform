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
class SamsonSupervisor : public ss::DataReceiverInterface, public ss::EndpointUpdateReceiverInterface, public ss::ReadyReceiverInterface
{
public:
	SamsonSupervisor(ss::Network* nwP) { networkP = nwP; networkReady = false; }

	virtual int receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP);
	virtual int endpointUpdate(ss::Endpoint* ep, ss::Endpoint::UpdateReason reason, const char* reasonText, void* info = NULL);
	virtual int ready(const char* info);

	volatile bool networkReady;

private:
	ss::Network*    networkP;
};

#endif
