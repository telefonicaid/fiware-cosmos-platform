
#include "samson/network/NetworkInterface.h"	// Own interface
#include "samson/network/Packet.h"				// samson::Packet
#include "engine/Engine.h"				// samson::Engine
#include "engine/EngineElement.h"		// samson::EngineElement
#include "PacketReceivedNotification.h"	// samson::PacketReceivedNotification
namespace samson
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
	

    void NetworkInterface::send( int endpoint,  Packet *p )
    {
        LM_T(LmtNodeMessages, ("%s sends %s" , node_name , p->str().c_str()));
        send(NULL, endpoint, p);
    }
    
    void NetworkInterface::sendToWorker( int workerId , Packet *p)
    {
        int endPoint = workerGetIdentifier(workerId);
        send( endPoint , p );
    }
    
    void NetworkInterface::sendToController( Packet *p)
    {
        int endPoint = controllerGetIdentifier();
        send( endPoint , p );
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
	
	void PacketReceiverInterface::_receive( Packet* packet )
	{
		LM_T(LmtNetworkInterface, ("NETWORK_INTERFACE Received packet type %s",messageCode(packet->msgCode)));
		
		// Using the engine to call the packet receiver asynchronously in a unique thread form
		engine::Engine::add( new PacketReceivedNotification( this , packet ) );
		//receive( packet );
	}
	

    
}
