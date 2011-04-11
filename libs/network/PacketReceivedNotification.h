#ifndef _H_PACKET_RECEIVER_NOTIFICATION
#define _H_PACKET_RECEIVER_NOTIFICATION

#include "EngineElement.h"// ss::EngineElement
#include "Network.h"		// ss::DataReceiverInterface
#include "Packet.h"			// ss::Packet

namespace ss
{

/**
    PacketReceived
     
     Process a packet received from the network library
*/

class PacketReceivedNotification : public EngineElement
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
