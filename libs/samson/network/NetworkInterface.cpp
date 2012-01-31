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

#include "engine/DiskStatistics.h"      // engine::DiskStatistics

#include "engine/ProcessManager.h"      // engine::ProcessManager
#include "engine/MemoryManager.h"       // engine::MemoryManager    
#include "engine/DiskManager.h"         // engine::DiskManager    
#include "engine/Engine.h"


namespace samson
{



/* ****************************************************************************
*
* run_network_interface_background_thread - 
*/
void* run_network_interface_background_thread(void* p)
{
    // Free resources automatically when this thread finish
    pthread_detach(pthread_self());
    
	((NetworkInterface*) p)->run();
	return NULL;
}
	

    /* ****************************************************************************
     *
     * NetworkInterface - 
     */

    NetworkInterface::NetworkInterface()
    {
        node_name = "Unknown node";
        statistics = new engine::DiskStatistics(); 
    }

    /* ****************************************************************************
     *
     * ~NetworkInterface - 
     */
    
    
    NetworkInterface::~NetworkInterface()
    {
        delete statistics;
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
    /*
void NetworkInterface::sendToController( Packet *p)
{
    //LM_M(("Sending message to controller"));
	int endPoint = controllerGetIdentifier();
    //LM_M(("End sending message to controller"));
	send(endPoint, p);
}
*/


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
	// Using the engine to call the packet receiver asynchronously in a unique thread form
	LM_T(LmtNetworkInterface, ("NETWORK_INTERFACE Received packet type %s",messageCode(packet->msgCode)));
	engine::Engine::shared()->add( new PacketReceivedNotification( this , packet ) );
}
    
    
    // Auxiliar function to get generic engine - wide information
    
    void getInfoEngineSystem( std::ostringstream &output , NetworkInterface* network )
    {
        au::xml_open(output, "engine_system");
        engine::MemoryManager::shared()->getInfo( output );
        engine::DiskManager::shared()->getInfo( output  );
        engine::ProcessManager::shared()->getInfo( output );
        size_t uptime = engine::Engine::shared()->uptime.diffTimeInSeconds();
        au::xml_simple( output , "uptime" , uptime );
        
        // Network information
        au::xml_simple(output, "network_output",  network->getOutputBuffersSize() );
        
        au::xml_close(output, "engine_system");
        
    }
    
}
