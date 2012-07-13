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

#include "engine/ProcessManager.h"      // engine::ProcessManager
#include "engine/MemoryManager.h"       // engine::MemoryManager    
#include "engine/DiskManager.h"         // engine::DiskManager    
#include "engine/Engine.h"


namespace samson
{

    
    /* ****************************************************************************
     *
     * send - send a packet
     */
    
    void NetworkInterfaceBase::schedule_receive( Packet* packet )
    {
        if( !network_interface_receiver )
        {
            LM_W(("Not possible to send packet %s since receiver is still not defined" , packet->str().c_str() ));
            return;
        }
        
        // Using the engine to call the packet receiver asynchronously in a unique thread form
        LM_T(LmtNetworkInterface, ("NETWORK_INTERFACE Received packet(%p) type %s ", packet, messageCode(packet->msgCode)));
        engine::Engine::shared()->add( new PacketReceivedNotification( network_interface_receiver , packet ) );
    }
    
    
    // Auxiliar function to get generic engine - wide information
    
    void getInfoEngineSystem( std::ostringstream &output , NetworkInterface* network )
    {
        if (network == NULL)
            LM_D(("network == NULL"));
        
        au::xml_open(output, "engine_system");
        engine::DiskManager::shared()->getInfo( output  );
        engine::ProcessManager::shared()->getInfo( output );
        size_t uptime = engine::Engine::shared()->uptime.diffTimeInSeconds();
        au::xml_simple( output , "uptime" , uptime );
        
        au::xml_close(output, "engine_system");
        
    }
    
}
