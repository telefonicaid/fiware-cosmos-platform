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

namespace samson {
void MultiPacketQueue::Clear() {
  au::TokenTaker tt(&token_packet_queues_);

  packet_queues_.clearMap();
}

// Push a packet to be sent
void MultiPacketQueue::Push(const au::SharedPointer<Packet>& packet) {
  // Robust agains NULL Packets...
  if (packet == NULL) {
    return;
  }

  // Get target node
  const NodeIdentifier& node_identifier = packet->to;

  // Select the correct queue and push the new packet
  std::string name = node_identifier.getCodeName();
  packet_queues_.findOrCreate(name, name)->Push(packet);
}

au::SharedPointer<Packet> MultiPacketQueue::Front(const NodeIdentifier& node_identifier) {
  au::TokenTaker tt(&token_packet_queues_);
  std::string name = node_identifier.getCodeName();
  PacketQueue *paquet_queue = packet_queues_.findOrCreate(name, name);

  // Get next packet
  return paquet_queue->Front();
}

std::string MultiPacketQueue::GetDescription(const NodeIdentifier& node_identifier) const {
  au::TokenTaker tt(&token_packet_queues_);
  std::string name = node_identifier.getCodeName();
  PacketQueue *paquet_queue = packet_queues_.findInMap(name);

  if (!paquet_queue) {
    return "[No queue]";
  }
  return paquet_queue->GetDescription();
}

void MultiPacketQueue::Pop(const NodeIdentifier& node_identifier) {
  au::TokenTaker tt(&token_packet_queues_);
  std::string name = node_identifier.getCodeName();
  PacketQueue *paquet_queue = packet_queues_.findInMap(name);

  // Pop packet
  paquet_queue->Pop();
}

au::tables::Table *MultiPacketQueue::getPendingPacketsTable() {
  au::tables::Table *table = new au::tables::Table(au::StringVector("Connection", "#Packets", "Size"));


  au::map<std::string, PacketQueue>::iterator it;

  for (it = packet_queues_.begin(); it != packet_queues_.end(); it++) {
    au::StringVector values;

    values.push_back(it->first);     // Name of the connection
    PacketQueue *packet_queue = it->second;
    values.push_back(au::str(packet_queue->size()));
    values.push_back(au::str(packet_queue->byte_size()));
    table->addRow(values);
  }

  table->setTitle("Pending packets");

  return table;
}

void MultiPacketQueue::RemoveOldConnections(const std::set<std::string> current_connections) {
  au::TokenTaker tt(&token_packet_queues_);

  au::map<std::string, PacketQueue>::iterator it_packet_queues;
  for (it_packet_queues = packet_queues_.begin(); it_packet_queues != packet_queues_.end(); ) {
    std::string name = it_packet_queues->first;

    if (current_connections.find(name) != current_connections.end()) {
      // It is still connected
      it_packet_queues->second->ResetInactivityCronometer();
      ++it_packet_queues;
      continue;
    }

    if (it_packet_queues->second->inactivity_time() > 60) {
      LOG_SW(("Removing  pending packets for %s since it has been disconnected mote than 60 secs", name.c_str()));
      packet_queues_.erase(it_packet_queues++);
    } else {
      ++it_packet_queues;
    }
  }
}

au::SharedPointer<gpb::Collection> MultiPacketQueue::GetQueuesCollection(const Visualization& visualization) {
  au::TokenTaker tt(&token_packet_queues_);

  return GetCollectionForMap("network_queues", packet_queues_, visualization);
}
}