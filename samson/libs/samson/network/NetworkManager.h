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
#ifndef _H_SAMSON_NETWORK_MANAGER
#define _H_SAMSON_NETWORK_MANAGER

#include <map>
#include <string>
#include <vector>

#include "au/containers/SharedPointer.h"
#include "au/containers/map.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/network/NetworkListener.h"
#include "au/statistics/ConceptTimeCounter.h"
#include "au/tables/Table.h"

#include "engine/Engine.h"
#include "engine/Notification.h"
#include "engine/NotificationListener.h"

#include "samson/network/Packet.h"
#include "samson/network/PacketQueue.h"

namespace au {
class SocketConnection;
}

namespace samson {
class NetworkConnection;
class NetworkListener;

class NetworkManager {
public:

  NetworkManager() : token_connections_("token_connections_"), connections_names_(60) {
  }

  virtual ~NetworkManager();

  // Unique method to add connections
  void AddConnection(NodeIdentifier new_node_identifier, au::SocketConnection *socket_connection);

  // Interface to inform about a received packet from a network connection
  virtual void receive(NetworkConnection *connection, const PacketPointer& packet) {
    LOG_SW(("NetworkManager::receive not implemented"));
  }

  // Push a packet to a connection
  void Send(const PacketPointer& packet);
  void SendToAllDelilahs(const PacketPointer& packet);

  /**
   * \brief Review current connections and queue of pending packets
   * Remove packets sent to long unconnected nodes
   * Remove disconnected connections
   * Track connection names to be able to say if I have been connected to a particular delilah
   */
  void Review();

  /**
   * \brief Remove a particular connection
   */
  // void Remove(NetworkConnection *network_connection);

  /**
   * \brief Remove a particular connection identifier by name
   *
   * \return true if element really removed
   */
  bool Remove(const NodeIdentifier& node_identifier);

  /**
   * \brief Remove connections with a closed socket
   */
  void RemoveDisconnectedConnections();

  /**
   * \brief Remove all connections
   */
  void ClearConnections();

  /**
   * \brief Check if a connection is stablished ( identified by name )
   */
  bool IsConnected(const NodeIdentifier& node_identifier) const;

  /**
   * \brief Get table with connection information
   */
  au::tables::Table *GetConnectionsTable() const;

  /**
   * \brief Get pending packets table
   */
  au::tables::Table *GetPendingPacketsTable() const;

  /**
   * \brief Get delilah ids in all current connections
   */
  std::vector<size_t> GetDelilahIds() const;

  /**
   * \brief Get all connections
   */
  std::vector<NodeIdentifier> GetAllNodeIdentifiers() const;

  /**
   * \brief Debug string
   */
  std::string str() const;

  /**
   * \brief Get a collection for all connections
   */
  au::SharedPointer<gpb::Collection> GetConnectionsCollection(const Visualization& visualization) const;

  /**
   * \brief Get a collection for all queues of pending packets
   */
  au::SharedPointer<gpb::Collection> GetQueuesCollection(const Visualization& visualization)  const;

  /**
   * \brief Reset all connections
   */
  void Reset();

  /**
   * \brief Get number of connections
   */
  size_t GetNumConnections() const;

  /**
   * \brief rate of bytes per second for input traffic
   */
  size_t GetRateIn() const;

  /**
   * \brief rate of bytes per second for output traffic
   */
  size_t GetRateOut() const;

  /**
   * \brief Get some debug information about a connection
   */
  std::string GetStatusForConnection(const NodeIdentifier& node_identifier) const;

  /**
   * \brief Get total size in all pending packets queues
   */
  size_t GetAllQueuesSize() {
    return multi_packet_queue_.GetAllQueuesSize();
  }

  /**
   * \brief Get size in pending packets for a particular qoeker
   */
  size_t GetQueueSizeForWorker(size_t worker_id) {
    return multi_packet_queue_.GetQueueSize(NodeIdentifier(WorkerNode, worker_id));
  }

  /**
   * \brief Check if I have "seen" a particular node during last minute
   */
  bool CheckValidNode(const NodeIdentifier& node) {
    if (connections_names_.time() < 60) {
      return true;  // We are still not ready to validate a node, so all of them are valid
    }
    return connections_names_.IsActive(node.str());
  }

protected:

  // Multi queue for all unconnected connections
  MultiPacketQueue multi_packet_queue_;

private:

  // Token to block add and move operations on connections
  mutable au::Token token_connections_;

  // All managed connection ( name = code name of node identifier )
  au::map<NodeIdentifier, NetworkConnection> connections_;

  // Register of connections established until last minute
  au::ConceptTimeCounter connections_names_;

  friend class NetworkConnection;
};
}

#endif  // ifndef _H_SAMSON_NETWORK_MANAGER
