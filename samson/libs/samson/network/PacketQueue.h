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
 * \brief Queue of pending packets for a particular connection (identified by a string)
 */

class PacketQueue : public au::Queue<Packet>{
public:

  explicit PacketQueue(const NodeIdentifier& node_identifier) {
    node_identifier_ = node_identifier;
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
    samson::add(record, "name", node_identifier_.str(), "left,different");
    samson::add(record, "state", GetDescription(), "different");
  }

  std::string pattern_name() {
    return node_identifier_.str();
  }

private:

  au::Cronometer cronometer_;
  NodeIdentifier node_identifier_;
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
  void Push(au::SharedPointer<Packet> packet);

  /**
   * \brief Get the next packet to be sent to a particular SAMSON node
   */
  au::SharedPointer<Packet> Front(const NodeIdentifier& node_identifier);

  /**
   * \brief Remove the next packet to be sent to a particular SAMSON node
   *
   * It is supposed that this packet has been sent correctly, so we remove from pending paquests queue
   */
  void Pop(const NodeIdentifier& node_identifier);

  /**
   * \brief Remove all pending packets
   */
  void Clear();

  /**
   * \brief Get a table with current status of all the queues
   */
  au::tables::Table *GetPendingPacketsTable() const;

  /**
   * \brief Get a vector with all connection names
   */
  std::vector<NodeIdentifier> GetAllNodeIdentifiers() {
    return packet_queues_.getKeysVector();
  }

  /**
   * \brief Remove queue for a particular connection
   */
  void Remove(const NodeIdentifier& node_identifier) {
    packet_queues_.extractAndDeleteFromMap(node_identifier);
  }

  /**
   * \brief Debug information for a particular node
   */
  std::string GetDescription(const NodeIdentifier& node_identifier) const;

  /**
   * \brief Get a collection to inform about current queues ( displaying table on delilah )
   */
  au::SharedPointer<gpb::Collection> GetQueuesCollection(const Visualization& visualization) const;

  /**
   * \brief Get total size accumulated in all queues
   */
  size_t GetAllQueuesSize();

  /**
   * \brief Get total size accumulated in a particular queue
   */
  size_t GetQueueSize(const NodeIdentifier& node_identifier);


private:

  // Pending packets for all nodes
  au::map< NodeIdentifier, PacketQueue > packet_queues_;
  mutable au::Token token_packet_queues_;
};
}

#endif  // ifndef _H_SAMSON_PACKET_QUEUE
