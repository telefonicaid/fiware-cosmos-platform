#ifndef _H_PACKET_RECEIVER_NOTIFICATION
#define _H_PACKET_RECEIVER_NOTIFICATION

#include "engine/EngineElement.h"// samson::EngineElement
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

            if( ! _receiver )
                LM_X(1, ("No receiver in PacketReceivedNotification"));
                             
            
            std::ostringstream txt;
            txt << "PacketReceivedNotification ";
            txt << " Packet: " << messageCode( packet->msgCode ) << ")";
            description = txt.str();
            
            shortDescription = "Packet";
            
        }
        
        void run()
        {
            receiver->receive( packet );
            delete packet;// Remove the packet recieved from the network
        }
        
    };
    
}

#endif
