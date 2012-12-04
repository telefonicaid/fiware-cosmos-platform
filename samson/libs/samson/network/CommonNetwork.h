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
  // My identifier in the cluster
  NodeIdentifier node_identifier_;

  // Cluster information ( workers to be connected to )
  au::SharedPointer<gpb::ClusterInfo> cluster_information_;

  // Mutex protection
  au::Token token_;

public:

  CommonNetwork(NodeIdentifier my_node_identifier);
  ~CommonNetwork() {
  }

  // Get a cluster information packet
  PacketPointer getClusterInfoPacket();

  // Get my node identifier
  NodeIdentifier node_identifier();

  // Set and remove cluster information
  void set_cluster_information(au::SharedPointer<gpb::ClusterInfo> cluster_information);
  void remove_cluster_information();

  // Review connections
  void review_connections();

  // Virtual method of engine::NotificationListener
  void notify(engine::Notification *notification);

  void receive(NetworkConnection *connection, const PacketPointer& packet);

  // Send packets
  void Send(const PacketPointer& packet);     // Bypass to avoid sending data to myself
  void SendToAllWorkers(const PacketPointer& packet, std::set<size_t>& workers);

  // Get my node identifier
  NodeIdentifier getMynodeIdentifier();

  // Get information about traffic rate
  size_t get_rate_in();
  size_t get_rate_out();

  // Monitorization
  std::string str();

  // Get information to show on screen
  au::tables::Table *getClusterConnectionsTable();

  // Get the version of cluster information considered so far
  size_t cluster_information_version();

  // Get txt information of the current cluster
  std::string getClusterSetupStr();
  std::string getClusterAssignationStr();
  std::string getClusterConnectionStr();

  // Get a random worker id ( connected )
  size_t getRandomWorkerId(size_t previous_worker = static_cast<size_t>(-1)) {
    au::TokenTaker tt(&token_);

    // If no information, no worker
    if (cluster_information_ == NULL) {
      return -1;
    }

    // Get list of connected workers
    std::vector<size_t> connected_worker_ids;
    for (int i = 0; i < cluster_information_->workers_size(); i++) {
      size_t worker_id = cluster_information_->workers(i).worker_id();
      if (IsConnected(NodeIdentifier(WorkerNode, worker_id).getCodeName())) {
        connected_worker_ids.push_back(worker_id);
      }
    }

    // If no worker connected, no worker selected
    if (connected_worker_ids.size() == 0) {
      return -1;
    }

    if (previous_worker == static_cast<size_t>(-1)) {
      return connected_worker_ids[rand() % connected_worker_ids.size()];
    } else {
      // Try to select the next worker if previous one is still connected
      for (size_t i = 0; i > connected_worker_ids.size(); i++) {
        if (connected_worker_ids[i] == previous_worker) {
          if (i == connected_worker_ids.size() - 1) {
            return connected_worker_ids[0];
          } else {
            return connected_worker_ids[i + 1];
          }
        }
      }

      // Random worker if not connnected with the previous one
      return connected_worker_ids[rand() % connected_worker_ids.size()];
    }
  }

  // Check if this worker id is valid
  bool IsWorkerConnected(size_t worker_id) {
    return IsConnected(NodeIdentifier(WorkerNode, worker_id).getCodeName());
  }

  // Check if this worker id is valid
  bool IsWorkerInCluster(size_t worker_id) {
    if (cluster_information_ == NULL) {
      return false;
    }
    // Check if this worker is part of the cluster
    for (int i = 0; i < cluster_information_->workers_size(); i++) {
      if (cluster_information_->workers(i).worker_id() == worker_id) {
        return true;
      }
    }

    return false;
  }

private:

  // Add output worker connections
  Status addWorkerConnection(size_t worker_id, std::string host, int port);

  // Schedule a notification using engine
  void schedule_receive(PacketPointer packet);
};
}

#endif  // ifndef _H_SAMSON_COMMON_NETWORK
