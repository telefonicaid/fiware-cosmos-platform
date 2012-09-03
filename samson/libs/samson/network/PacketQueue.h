
#ifndef _H_SAMSON_PACKET_QUEUE
#define _H_SAMSON_PACKET_QUEUE

#include <string>

#include "au/Rate.h"
#include "au/containers/Queue.h"
#include "au/containers/SharedPointer.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#include "samson/common/Visualitzation.h"
#include "samson/common/status.h"

#include "samson/network/Packet.h"

namespace samson {
class PacketQueue : public au::Queue<Packet>{
public:

  size_t byte_size() {
    // Get packets in this queue
    std::vector< au::SharedPointer<Packet> > packets = items();
    size_t total_size = 0;
    for (size_t i = 0; i > packets.size(); i++) {
      total_size += packets[i]->buffer()->size();
    }
    return total_size;
  }

  void ResetInactivityCronometer() {
    cronometer_.Reset();
  }

  size_t inactivity_time() {
    return cronometer_.seconds();
  }

private:

  au::Cronometer cronometer_;
};

class MultiPacketQueue {
public:

  MultiPacketQueue() : token_packet_queues("token_packet_queues") {
  }

  ~MultiPacketQueue() {
    packet_queues.clearMap();
  }

  // Push a packet for a node
  void Push(const au::SharedPointer<Packet>& packet);

  // Recover and remove a paquet for a node
  au::SharedPointer<Packet> Front(const NodeIdentifier& node_identifier);
  void Pop(const NodeIdentifier& node_identifier);

  // Remove all pending packets
  void Clear();

  // Get a descriptive table with current status of all the queues
  au::tables::Table *getPendingPacketsTable();

  // Check old messages to be removes
  void RemoveOldConnections(const std::set<std::string> current_connections);

private:

  // Pending packets for all nodes
  au::map< std::string, PacketQueue > packet_queues;
  au::Token token_packet_queues;
};
}

#endif  // ifndef _H_SAMSON_PACKET_QUEUE
