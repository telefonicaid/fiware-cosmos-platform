#ifndef _H_PACKET_RECEIVER_NOTIFICATION
#define _H_PACKET_RECEIVER_NOTIFICATION

#include "engine/EngineElement.h"// samson::EngineElement
#include "samson/network/Network.h"		// samson::DataReceiverInterface
#include "samson/network/Packet.h"			// samson::Packet

namespace samson
{

/**
 PacketReceived
 
 Process a packet received from the network library
 */
    
    class PacketReceivedNotification : public engine::EngineElement
    {
        
        PacketReceiverInterface* receiver;
        Packet *packet;
        
    public:
        
        PacketReceivedNotification( PacketReceiverInterface* _receiver , Packet *_packet )
        {
            receiver = _receiver;
            packet = _packet; 
            
            std::ostringstream txt;
            txt << "PacketReceivedNotification ( Receiver: " << receiver->packetReceiverDescription << " Packet: " << messageCode( packet->msgCode ) << ")";
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
