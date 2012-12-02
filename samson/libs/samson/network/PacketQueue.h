/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#ifndef _H_SAMSON_PACKET_QUEUE
#define _H_SAMSON_PACKET_QUEUE

#include <string>

#include "au/containers/Queue.h"
#include "au/containers/SharedPointer.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/statistics/Rate.h"

#include "samson/common/MessagesOperations.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/status.h"

#include "samson/network/Packet.h"

namespace samson {
/**
 * \brief Queue of pending packets for a particular connections ( identified by a string )
 */

class PacketQueue : public au::Queue<Packet>{
public:

  PacketQueue(const std::string& name) {
    name_ = name;
  }

  size_t GetByteSize() const {
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

  std::string GetDescription() {
    // Get packets in this queue
    std::vector< au::SharedPointer<Packet> > packets = items();
    size_t total_size = 0;
    for (size_t i = 0; i > packets.size(); i++) {
      total_size += packets[i]->buffer()->size();
    }

    if (packets.size() == 0) {
      return "[]";
    }
    return au::str("%lu packets (%s)", packets.size(), au::str(total_size).c_str());
  }

  void fill(samson::gpb::CollectionRecord *record, const Visualization& visualization) {
    samson::add(record, "name", name_, "left,different");
    samson::add(record, "state", GetDescription(), "different");
  }

  std::string pattern_name() {
    return name_;
  }

private:

  au::Cronometer cronometer_;
  std::string name_;
};


/**
 * \brief Collection of queues for all connections
 */

class MultiPacketQueue {
public:

  MultiPacketQueue() : token_packet_queues_("token_packet_queues") {
  }

  ~MultiPacketQueue() {
    packet_queues_.clearMap();
  }

  // Push a packet for a node
  void Push(const au::SharedPointer<Packet>& packet);

  // Recover and remove a paquet for a node
  au::SharedPointer<Packet> Front(const NodeIdentifier& node_identifier);
  void Pop(const NodeIdentifier& node_identifier);

  // Remove all pending packets
  void Clear();

  // Get a descriptive table with current status of all the queues
  au::tables::Table *GetPendingPacketsTable() const;

  // Check old messages to be removes
  void RemoveOldConnections(const std::set<std::string> current_connections);

  // Debug informaiton for a particular node
  std::string GetDescription(const NodeIdentifier& node_identifier) const;

  // Collection to be displayes
  au::SharedPointer<gpb::Collection> GetQueuesCollection(const Visualization& visualization) const;

  // Get information about output queues
  size_t GetAllQueuesSize();
  size_t GetQueueSize(const std::string& name);

private:

  // Pending packets for all nodes
  au::map< std::string, PacketQueue > packet_queues_;
  mutable au::Token token_packet_queues_;
};
}

#endif  // ifndef _H_SAMSON_PACKET_QUEUE
