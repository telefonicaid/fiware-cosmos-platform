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
#ifndef _H_SAMSON_COMMON_NETWORK
#define _H_SAMSON_COMMON_NETWORK

#include "au/containers/SharedPointer.h"
#include "au/network/NetworkListener.h"

#include "engine/NotificationListener.h"

#include "samson/common/NotificationMessages.h"
#include "samson/common/ports.h"

#include "samson/common/status.h"
#include "samson/network/NetworkInterface.h"
#include "samson/network/NetworkManager.h"
#include "samson/network/PacketQueue.h"
#include "samson/network/PacketReceivedNotification.h"

namespace samson {
class CommonNetwork : public NetworkManager, public engine::NotificationListener {
public:

  CommonNetwork(const NodeIdentifier& my_node_identifier);
  ~CommonNetwork() {
  }

  /**
   * \brief Get a packet with all cluster information ( to be sent to a new delilah client )
   */
  PacketPointer getClusterInfoPacket();


  // Set and remove cluster information
  void set_cluster_information(au::SharedPointer<gpb::ClusterInfo> cluster_information);

  // remove previous information about SAMSON cluster
  void remove_cluster_information();

  /**
   * \brief Review connections to remove unnecessary connections
   */
  void ReviewConnections();

  /**
   * \brief Virtual method of engine::NotificationListener
   */
  void notify(engine::Notification *notification);

  /**
   * \brief Process an incoming packet from a connection
   */
  void receive(NetworkConnection *connection, const PacketPointer& packet);

  /**
   * \brief Schedule a packet to be send to a node in the SAMSON network
   */
  void Send(const PacketPointer& packet);     // Bypass to avoid sending data to myself

  /**
   * \brief Schedule a packet to be send to all workers ( usuaslly from delilah client )
   */
  void SendToAllWorkers(const PacketPointer& packet, std::set<size_t>& workers);

  /**
   * \brief Get my node identifier
   */
  NodeIdentifier node_identifier();

  // Get information about traffic rate
  size_t get_rate_in();
  size_t get_rate_out();

  /**
   * \brief Debug string
   */
  std::string str();

  // Get information to show on screen
  au::SharedPointer<au::tables::Table> GetClusterConnectionsTable() const;

  // Get the version of cluster information considered so far
  size_t cluster_information_version() const;

  // Get txt information of the current cluster
  std::string getClusterSetupStr() const;
  std::string getClusterAssignationStr() const;
  std::string getClusterConnectionStr() const;

  // Check if this worker id is valid
  bool IsWorkerConnected(size_t worker_id) const;

  // Check if this worker id is valid
  bool IsWorkerInCluster(size_t worker_id) const;

  // Get a random worker id ( connected )
  size_t getRandomWorkerId(size_t previous_worker = SIZE_T_UNDEFINED);

private:

  /**
   * \brief Check if a particular node has to be processed
   *
   * Packets to workers are only processed if they are included in the cluster
   * Packets to delilahs are only processed if they are connected to
   */

  bool IsNecessaryToProcess(const NodeIdentifier& node) const;

  // Add output worker connections
  Status addWorkerConnection(size_t worker_id, std::string host, int port);

  // Schedule a notification using engine
  void schedule_receive(PacketPointer packet);

  // My identifier in the cluster
  NodeIdentifier node_identifier_;

  // Cluster information ( workers to be connected to )
  au::SharedPointer<gpb::ClusterInfo> cluster_information_;

  // Mutex protection
  mutable au::Token token_;
};
}

#endif  // ifndef _H_SAMSON_COMMON_NETWORK
