/* ****************************************************************************
*
* FILE                     NetworkInterface.cpp - 
*
* AUTHOR                   Andreu Urruela
*
* CREATION DATE            2010
*
*/
#include "engine/Engine.h"              // samson::Engine
#include "engine/EngineElement.h"       // samson::EngineElement
#include "Packet.h"                     // samson::Packet
#include "PacketReceivedNotification.h" // samson::PacketReceivedNotification
#include "NetworkInterface.h"           // Own interface



namespace samson
{



/* ****************************************************************************
*
* run_network_interface_background_thread - 
*/
void* run_network_interface_background_thread(void* p)
{
	((NetworkInterface*) p)->run();
	return NULL;
}
	


/* ****************************************************************************
*
* send - 
*/
void NetworkInterface::send(int endpoint,  Packet* p)
{
	LM_T(LmtNodeMessages, ("%s sends %s" , node_name , p->str().c_str()));
	send(NULL, endpoint, p);
}
    


/* ****************************************************************************
*
* sendToWorker - 
*/
void NetworkInterface::sendToWorker(int workerId, Packet* p)
{
	int ep = workerGetIdentifier(workerId);
	send(ep, p);
}
    


/* ****************************************************************************
*
* sendToController - 
*/
void NetworkInterface::sendToController( Packet *p)
{
	int endPoint = controllerGetIdentifier();
	send(endPoint, p);
}



/* ****************************************************************************
*
* runInBackground - 
*/
void NetworkInterface::runInBackground(void)
{
	// Run a different thread with the "run" method
	pthread_t t;
	pthread_create(&t, NULL, run_network_interface_background_thread, this);
}
	


/* ****************************************************************************
*
* send - send a packet
*/
void PacketReceiverInterface::_receive(Packet* packet)
{
	LM_T(LmtNetworkInterface, ("NETWORK_INTERFACE Received packet type %s",messageCode(packet->msgCode)));
		
	// Using the engine to call the packet receiver asynchronously in a unique thread form
	engine::Engine::add( new PacketReceivedNotification( this , packet ) );
	// receive( packet );
}
    
}
