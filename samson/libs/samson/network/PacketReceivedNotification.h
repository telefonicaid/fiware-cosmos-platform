#ifndef _H_PACKET_RECEIVER_NOTIFICATION
#define _H_PACKET_RECEIVER_NOTIFICATION


#include "au/containers/SharedPointer.h"

#include "engine/EngineElement.h"  // samson::EngineElement
#include "engine/MemoryManager.h"
#include "samson/network/NetworkInterface.h"
#include "samson/network/Packet.h"                      // samson::Packet

namespace samson {
/**
 *
 * PacketReceived
 * Process a packet received from the network library
 *
 */

class PacketReceivedNotification : public engine::EngineElement {
  NetworkInterfaceReceiver *receiver_;
  PacketPointer packet_;

public:

  PacketReceivedNotification(NetworkInterfaceReceiver *receiver, const PacketPointer& packet)
    : engine::EngineElement("packet_received")
      , packet_(packet) {
    if (packet_ == NULL) {
      LM_X(1, ("Internal error"));
    }

    receiver_ = receiver;

    std::ostringstream txt;
    txt << "PacketReceivedNotification: ";
    txt << " Packet: " << packet_->str() << "";
    set_description(txt.str());
    set_short_description("Packet");
  }

  void run() {
    if (!receiver_) {
      LM_W(("Packet %s lost since network interface is still not activated.", packet_->str().c_str()));
    } else {
      receiver_->receive(packet_);
    }
  }
};
}

#endif  // ifndef _H_PACKET_RECEIVER_NOTIFICATION
