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

#include "PacketQueue.h"  // Own interface

#include "au/log/LogMain.h"
#include "samson/common/Logs.h"

namespace samson {
void MultiPacketQueue::Clear() {
  au::TokenTaker tt(&token_packet_queues_);

  packet_queues_.clearMap();
}

// Push a packet to be sent
void MultiPacketQueue::Push(au::SharedPointer<Packet> packet) {
  // Robust agains NULL Packets...
  if (packet == NULL) {
    return;
  }

  // Get target node
  const NodeIdentifier& node_identifier = packet->to;

  LOG_V(logs.out_messages, ("Scheduling packet %s in output queue %s",
                            packet->str().c_str(), node_identifier.str().c_str()));

  au::TokenTaker tt(&token_packet_queues_);
  packet_queues_.findOrCreate(node_identifier, node_identifier)->Push(packet);
}

au::SharedPointer<Packet> MultiPacketQueue::Front(const NodeIdentifier& node_identifier) {
  au::TokenTaker tt(&token_packet_queues_);
  PacketQueue *paquet_queue = packet_queues_.findOrCreate(node_identifier, node_identifier);

  // Get next packet
  return paquet_queue->Front();
}

std::string MultiPacketQueue::GetDescription(const NodeIdentifier& node_identifier) const {
  au::TokenTaker tt(&token_packet_queues_);
  PacketQueue *paquet_queue = packet_queues_.findInMap(node_identifier);

  if (!paquet_queue) {
    return "[No queue]";
  }
  return paquet_queue->GetDescription();
}

void MultiPacketQueue::Pop(const NodeIdentifier& node_identifier) {
  au::TokenTaker tt(&token_packet_queues_);

  PacketQueue *paquet_queue = packet_queues_.findInMap(node_identifier);

  // Pop packet
  if (paquet_queue) {
    PacketPointer packet = paquet_queue->Pop();
    LOG_V(logs.out_messages, ("Removed packet %s from otuput queue %s",
                              packet->str().c_str(), node_identifier.str().c_str()));
  }
}

au::tables::Table *MultiPacketQueue::GetPendingPacketsTable() const {
  au::TokenTaker tt(&token_packet_queues_);

  au::tables::Table *table = new au::tables::Table(au::StringVector("Connection", "#Packets", "Size"));

  au::map<NodeIdentifier, PacketQueue>::const_iterator it;
  for (it = packet_queues_.begin(); it != packet_queues_.end(); ++it) {
    au::StringVector values;
    values.push_back(it->first.str());     // Name of the connection
    PacketQueue *packet_queue = it->second;
    values.push_back(au::str(packet_queue->size()));
    values.push_back(au::str(packet_queue->GetByteSize()));
    table->addRow(values);
  }

  table->setTitle("Pending packets");

  return table;
}

au::SharedPointer<gpb::Collection> MultiPacketQueue::GetQueuesCollection(const Visualization& visualization) const {
  au::TokenTaker tt(&token_packet_queues_);

  return GetCollectionForMap("network_queues", packet_queues_, visualization);
}

size_t MultiPacketQueue::GetAllQueuesSize() {
  au::TokenTaker tt(&token_packet_queues_);
  size_t total = 0;

  au::map< NodeIdentifier, PacketQueue >::iterator it;
  for (it = packet_queues_.begin(); it != packet_queues_.end(); ++it) {
    total += it->second->GetByteSize();
  }
  return total;
}

size_t MultiPacketQueue::GetQueueSize(const NodeIdentifier& node_identifier) {
  au::TokenTaker tt(&token_packet_queues_);
  PacketQueue *queue = packet_queues_.findInMap(node_identifier);

  if (!queue) {
    return 0;
  }
  return queue->GetByteSize();
}
}