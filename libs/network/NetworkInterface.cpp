
#include "NetworkInterface.h"	// Own interface

namespace ss
{



/* ****************************************************************************
*
* run_network_interface_background_thread - 
*/
void* run_network_interface_background_thread(void *p)
{
	((NetworkInterface*)p)->run();
	return NULL;
}
	


/* ****************************************************************************
*
* runInBackground - 
*/
void NetworkInterface::runInBackground(void)
{
	// Run a differente thread with the "run" method
	pthread_t t;
	pthread_create(&t, NULL, run_network_interface_background_thread, this);
}
	


/* ****************************************************************************
*
* send - send a packet (return a unique id to inform the notifier later)
*/
size_t NetworkInterface::send(PacketSenderInterface* sender, int endpointId, ss::Message::MessageCode code, Packet* packetP )
{
	lock_send.lock();

	LM_T(LmtNetworkInterface, ("NETWORK INTERFACE Send packet type %s",messageCode(code)));

	size_t id = _send( sender , endpointId , code , packetP );
	lock_send.unlock();
	
	return id;
}
}
