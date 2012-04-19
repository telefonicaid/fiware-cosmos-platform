#ifndef _H_PACKET_RECEIVER_NOTIFICATION
#define _H_PACKET_RECEIVER_NOTIFICATION

#include "engine/EngineElement.h"// samson::EngineElement
#include "engine/MemoryManager.h"

#include "samson/network/NetworkInterface.h"
#include "samson/network/Packet.h"			// samson::Packet

namespace samson
{

/**
 
 PacketReceived
 Process a packet received from the network library
 
 */
    
    class PacketReceivedNotification : public engine::EngineElement
    {
        
        NetworkInterfaceReceiver* receiver;
        Packet *packet;
        
    public:
        
        PacketReceivedNotification( NetworkInterfaceReceiver* _receiver , Packet *_packet )
        {
            receiver = _receiver;
            packet = _packet; 
            
            std::ostringstream txt;
            txt << "PacketReceivedNotification: ";
            txt << " Packet: " << packet->str() << "";
            description = txt.str();
            shortDescription = "Packet";
            
        }
        
        void run()
        {
            if( ! receiver )
            {
                LM_W(("Packet %s lost since ntework interface is still not activated." , packet->str().c_str() ));
                delete packet;// Remove the packet recieved from the network
                return;
            }
            else
            {
                receiver->receive( packet );
                delete packet; // Remove the packet recieved from the network ( releasing internal buffer )
            }
        }
        
    };
    
}

#endif
