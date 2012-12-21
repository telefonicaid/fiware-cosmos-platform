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
#include "samson/network/CommonNetwork.h"  // Own interface

#include <set>
#include <vector>

#include "au/network/NetworkListener.h"
#include "au/network/SocketConnection.h"

#include "samson/common/Logs.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/gpb_operations.h"
#include "samson/network/NetworkConnection.h"

namespace samson {
CommonNetwork::CommonNetwork(const NodeIdentifier& my_node_identifier) :
  token_("CommonNetwork") {
  // Identify myself
  node_identifier_ = my_node_identifier;

  LOG_V(logs.network_connection, ("CommonNetwork created for node %s", node_identifier_.str().c_str()));

  // No cluster information at the moment
  cluster_information_.Reset();

  // Listen and create notifications for network manager review
  listen("notification_network_manager_review");
  engine::Engine::shared()->notify(new engine::Notification("notification_network_manager_review"), 1);

  listen("send_to_all_delilahs");   // Listen notification to send packets
  listen(notification_send_packet);   // Listen to send packet
}

void CommonNetwork::remove_cluster_information() {
  cluster_information_.Reset(NULL);
  ReviewConnections();
}

void CommonNetwork::set_cluster_information(au::SharedPointer<gpb::ClusterInfo> cluster_information) {
  au::TokenTaker tt(&token_);

  if (cluster_information == NULL) {
    return;
  }

  // Not update if we have a newer version of this cluster info
  if (cluster_information_ != NULL) {
    if (cluster_information_->version() >= cluster_information->version()) {
      return;
    }
  }

  // Replace intern cluster information
  engine::Engine::shared()->notify(new engine::Notification("notification_cluster_info_changed"));
  cluster_information_ = cluster_information;

  // If I am a worker, update all my delilah connections
  if (node_identifier_.node_type() == WorkerNode) {
    PacketPointer ci_packet(new Packet(Message::ClusterInfoUpdate));
    ci_packet->message->mutable_cluster_info()->CopyFrom(*cluster_information_);
    ci_packet->from = node_identifier_;

    LOG_V(logs.network_connection, ("Sending cluster info version %lu update packages to all delilahs",
                                    cluster_information_->version()));

    SendToAllDelilahs(ci_packet);
  }

  // Review connections ( based on this new cluster setup )
  ReviewConnections();
}

void CommonNetwork::ReviewConnections() {
  au::TokenTaker tt(&token_);

  NetworkManager::Review();   // Check pending packets to be removed after 1 minute disconnected

  // Check workers to be connected to
  if (cluster_information_ == NULL) {
    LOG_V(logs.network_connection, ("No cluster information available. Not possible to review connections"));
    return;
  }

  // Stablish new connections with all workers if not previously connected
  int num_workers = cluster_information_->workers_size();
  LOG_V(logs.network_connection, ("Review connections to all workers (cluster with %d workers)", num_workers));
  for (int i = 0; i < num_workers; i++) {
    size_t worker_id = cluster_information_->workers(i).worker_id();
    NodeIdentifier node_identifier(WorkerNode, worker_id);
    std::string host = cluster_information_->workers(i).worker_info().host();
    int port = cluster_information_->workers(i).worker_info().port();

    LOG_V(logs.network_connection, ("Checking connection %s at %s:%d",
                                    node_identifier.str().c_str(), host.c_str(), port));

    // If I am a worker, do not connect with workers with a lower id since they will try to connect with me
    if (node_identifier_.node_type() == WorkerNode) {
      if (node_identifier_.id() < worker_id) {
        LOG_V(logs.network_connection, ("Skipping worker %lu (%s:%d): My id is lower", worker_id, host.c_str(), port));
        continue;
      }
      if (node_identifier_.id() == worker_id) {
        LOG_V(logs.network_connection, ("Skipping worker %lu (%s:%d): It is me", worker_id, host.c_str(), port));
        continue;
      }
    }

    if (NetworkManager::IsConnected(node_identifier)) {
      LOG_V(logs.network_connection, ("Skipping worker %lu (%s:%d): Already connected", worker_id, host.c_str(), port));
    } else {
      LOG_V(logs.network_connection, ("Connecting with worker %lu (%s:%d)",
                                      worker_id, host.c_str(), port));

      addWorkerConnection(worker_id, host, port);
    }
  }

  // Close old connections ( workers not included in the cluster or unconnected delilahs )
  {
    std::vector<NodeIdentifier> node_identifiers = GetAllNodeIdentifiers();
    for (size_t i = 0; i < node_identifiers.size(); ++i) {
      if (!IsNecessaryToProcess(node_identifiers[i])) {    // This packets are not necessary any more
        LOG_W(logs.network_connection, ("Removing connection to %s (not necessary any more)",
                                        node_identifiers[i].str().c_str()));
        if (!Remove(node_identifiers[i])) {
          LOG_E(logs.network_connection, ("Not possible to remove connection %s",
                                          node_identifiers[i].str().c_str()));
        }
      }
    }
  }

  // Remove packets for unnecessary nodes
  {
    std::vector<NodeIdentifier> node_identifiers = multi_packet_queue_.GetAllNodeIdentifiers();
    for (size_t i = 0; i < node_identifiers.size(); ++i) {
      if (!IsNecessaryToProcess(node_identifiers[i])) {    // This packets are not necessary any more
        LOG_W(logs.network_connection,
              ("Removing packets for %s (not necessary any more)", node_identifiers[i].str().c_str()));
        multi_packet_queue_.Remove(node_identifiers[i]);
      }
    }
  }
}

void CommonNetwork::notify(engine::Notification *notification) {
  if (notification->isName(notification_send_packet)) {
    au::SharedPointer<Packet> packet = notification->dictionary().Get<Packet> ("packet");

    if (packet == NULL) {
      LOG_SW(("Notification notification_send_packet without packet"));
      return;
    }

    // Send real packet
    Send(packet);
  }

  if (notification->isName("send_to_all_delilahs")) {
    au::SharedPointer<Packet> packet = notification->dictionary().Get<Packet> ("packet");

    if (packet != NULL) {
      SendToAllDelilahs(packet);
    } else {
      LOG_SW(("Notification send_to_all_delilahs without packet"));
    }
    return;
  }

  ReviewConnections();
}

std::string CommonNetwork::str() {
  std::ostringstream output;

  output << "-----------------------------------------------\n";
  output << "NetworkManager (" << node_identifier_.str() << ") \n";
  output << "-----------------------------------------------\n";
  output << NetworkManager::str();
  output << "-----------------------------------------------\n";

  return output.str();
}

// Add a new connection to a worker

Status CommonNetwork::addWorkerConnection(size_t worker_id, std::string host, int port) {
  NodeIdentifier node_identifier = NodeIdentifier(WorkerNode, worker_id);

  LOG_V(logs.network_connection, ("**** (%s) Adding connection for worker %lu at %s:%d"
                                  , node_identifier_.str().c_str()
                                  , worker_id
                                  , host.c_str()
                                  , port));


  // Check if we already have this connection
  if (NetworkManager::IsConnected(node_identifier)) {
    return Error;
  }

  // Init connection
  au::SocketConnection *socket_connection;
  au::Status s = au::SocketConnection::Create(host, port, &socket_connection);

  // If there is an error, just return the error
  if (s != au::OK) {
    LOG_SW(("Unable to connect to %s:%d (%s)", host.c_str(), port, au::status(s)));
    return Error;   // Generic error
  }

  // Sync send hello message
  // Sent hello packages to make sure the other end identify ourselves
  au::SharedPointer<Packet> hello_packet(new Packet(Message::Hello));
  gpb::Hello *pb_hello = hello_packet->message->mutable_hello();
  node_identifier_.fill(pb_hello->mutable_node_identifier());

  if (hello_packet->write(socket_connection, NULL) != au::OK) {
    LOG_SW(("Not possible to send hello message to %s:%d (%s)", host.c_str(), port, au::status(s)));
    return Error;
  }

  // Create network connection with this socket
  NetworkManager::AddConnection(node_identifier, socket_connection);

  return OK;
}

void CommonNetwork::Send(const PacketPointer& packet) {
  if (packet->msgCode == Message::Hello) {
    LOG_W(logs.worker, ("Not allowed to send hello message outside initial handshake"));
    return;
  }

  // Set me as "from" identifier
  packet->from = node_identifier_;

  if (packet->to == node_identifier_) {
    schedule_receive(packet);    // Local loop
    return;
  }

  if (!IsNecessaryToProcess(packet->to)) {
    LOG_SW(("Packet '%s' to node '%s' not processed", packet->str().c_str(), packet->to.str().c_str()));
    return;
  }

  // Push a packet to a connection or eventually keep in queue to see if it connects back soon ;)
  LOG_V(logs.out_messages, ("Sending packet %s to %s"
                            , packet->str().c_str()
                            , packet->to.str().c_str()));

  NetworkManager::Send(packet);
}

void CommonNetwork::SendToAllWorkers(const PacketPointer& packet, std::set<size_t>& workers) {
  au::TokenTaker tt(&token_);

  for (int i = 0; i < cluster_information_->workers_size(); ++i) {
    size_t worker_id = cluster_information_->workers(i).worker_id();
    PacketPointer new_paket(new Packet(packet.shared_object()));
    new_paket->to = NodeIdentifier(WorkerNode, worker_id);
    Send(new_paket);

    workers.insert(worker_id);
  }
}

// Receive a packet from a connection
void CommonNetwork::receive(NetworkConnection *connection, const PacketPointer& packet) {
  LOG_V(logs.network_connection, ("RECEIVED from %s: PACKET %s\n",
                                  connection->node_identifier().str().c_str(), packet->str().c_str()));

  if (packet->msgCode == Message::Hello) {
    LOG_SW(("Received a hello packet once connection is identified. Ignoring..."));
    return;   // Ignore hello message here
  }

  if (packet->msgCode == Message::ClusterInfoUpdate) {
    if (node_identifier_.node_type() == WorkerNode) {
      LOG_SW(("ClusterInfoUpdate packet received at a worker node from connection %s. Closing connection"
              , connection->node_identifier().str().c_str()));
      connection->Close();
      return;
    }   // This is managed as a normal message in delilah
  }
  // Check we do now receive messages from unidenfitied node elements
  if (connection->node_identifier().node_type() == UnknownNode) {
    LOG_SW(("Packet %s received from a non-identified node %s. Closing connection"
            , packet->str().c_str()
            , connection->node_identifier().str().c_str()));
    connection->Close();
    return;
  }

  schedule_receive(packet);
}

au::SharedPointer<au::tables::Table> CommonNetwork::GetClusterConnectionsTable() const {
  au::TokenTaker tt(&token_);

  au::SharedPointer<au::tables::Table> table(new au::tables::Table(au::StringVector("Worker", "Host", "Status")));

  if (cluster_information_ != NULL) {
    table->setTitle("Cluster");

    for (int i = 0; i < cluster_information_->workers_size(); ++i) {
      size_t worker_id = cluster_information_->workers(i).worker_id();
      std::string host = cluster_information_->workers(i).worker_info().host();
      int port = cluster_information_->workers(i).worker_info().port();

      au::StringVector values;
      values.push_back(au::str("%lu", worker_id));
      values.push_back(au::str("%s:%d", host.c_str(), port));

      NodeIdentifier node_identifier = NodeIdentifier(WorkerNode, worker_id);

      if (node_identifier == node_identifier_) {
        values.push_back("me");
      } else {
        values.push_back(NetworkManager::GetStatusForConnection(node_identifier));
      }
      table->addRow(values);
    }
  } else {
    table->setTitle("Not cluster defined");
  }

  return table;
}

size_t CommonNetwork::get_rate_in() {
  return NetworkManager::GetRateIn();
}

size_t CommonNetwork::get_rate_out() {
  return NetworkManager::GetRateOut();
}

void CommonNetwork::schedule_receive(PacketPointer packet) {
  // Create a notification containing the packet we have just received
  engine::Notification *notification = new engine::Notification(notification_packet_received);

  notification->dictionary().Set<Packet> ("packet", packet);
  engine::Engine::shared()->notify(notification);
}

size_t CommonNetwork::cluster_information_version() const {
  if (cluster_information_ == NULL) {
    return SIZE_T_UNDEFINED;
  }
  return cluster_information_->version();
}

std::string CommonNetwork::getClusterConnectionStr() const {
  if (cluster_information_ == NULL) {
    return "Disconnected";
  } else {
    return au::str("Cluster v %d / %d nodes"
                   , static_cast<int>(cluster_information_->version())
                   , static_cast<int>(cluster_information_->workers_size()));
  }
}

std::string CommonNetwork::getClusterSetupStr() const {
  if (cluster_information_ == NULL) {
    return "Delilah is not connected to any SAMSON cluster\n";
  }

  au::tables::Table table("Worker|Host|Connected");
  table.setTitle(au::str("Cluster setup ( version %lu )", cluster_information_->version()));
  for (int i = 0; i < cluster_information_->workers_size(); ++i) {
    size_t worker_id = cluster_information_->workers(i).worker_id();

    au::StringVector values;
    values.Push(worker_id);

    std::string host = au::str("%s:%d (rest :%d)", cluster_information_->workers(i).worker_info().host().c_str(),
                               cluster_information_->workers(i).worker_info().port(),
                               cluster_information_->workers(i).worker_info().port_web());
    values.push_back(host);

    if (IsConnected(NodeIdentifier(WorkerNode, worker_id))) {
      values.push_back("yes");
    } else {
      values.push_back("no");
    }
    table.addRow(values);
  }
  return table.str();
}

std::string CommonNetwork::getClusterAssignationStr() const {
  au::tables::Table table("ProcessUnit|Worker|Replicas");

  table.setTitle("Assignation");
  for (int i = 0; i < cluster_information_->process_units_size(); i++) {
    int hg_begin = cluster_information_->process_units(i).hg_begin();
    int hg_end = cluster_information_->process_units(i).hg_end();

    size_t worker_id = cluster_information_->process_units(i).worker_id();

    au::StringVector values;
    values.push_back(au::str("%d-%d", hg_begin, hg_end));
    values.Push(worker_id);

    std::ostringstream replicas;

    for (int r = 0; r < cluster_information_->process_units(i).replica_worker_id_size(); r++) {
      replicas << cluster_information_->process_units(i).replica_worker_id(r) << " ";
    }

    values.Push(replicas.str());

    table.addRow(values);
  }
  return table.str();
}

// Get a cluster information packet
PacketPointer CommonNetwork::getClusterInfoPacket() {
  au::TokenTaker tt(&token_);

  // Update cluster information to delilah....
  PacketPointer ci_packet(new Packet(Message::ClusterInfoUpdate));

  ci_packet->message->mutable_cluster_info()->CopyFrom(*cluster_information_);
  return ci_packet;
}

// Get my node identifier
NodeIdentifier CommonNetwork::node_identifier() {
  return node_identifier_;
}

size_t CommonNetwork::getRandomWorkerId(size_t previous_worker) {
  au::TokenTaker tt(&token_);

  // If no information, no worker
  if (cluster_information_ == NULL) {
    return SIZE_T_UNDEFINED;
  }

  // Get list of connected workers
  std::vector<size_t> connected_worker_ids;
  for (int i = 0; i < cluster_information_->workers_size(); ++i) {
    size_t worker_id = cluster_information_->workers(i).worker_id();
    if (IsConnected(NodeIdentifier(WorkerNode, worker_id))) {
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

bool CommonNetwork::IsNecessaryToProcess(const NodeIdentifier& node) const {
  switch (node.node_type()) {
    case UnknownNode:
      return false;   // Never process packets to unknown nodes

      break;
    case WorkerNode:
      if (cluster_information_ == NULL) {
        return false;   // If no cluster information , do not sent message to workers
      }
      return gpb::isWorkerIncluded(cluster_information_.shared_object(), node.id());

      break;
    case DelilahNode:
      return IsConnected(node);

      break;
  }

  return false;
}

bool CommonNetwork::IsWorkerConnected(size_t worker_id) const {
  return IsConnected(NodeIdentifier(WorkerNode, worker_id));
}

// Check if this worker id is valid
bool CommonNetwork::IsWorkerInCluster(size_t worker_id) const {
  if (cluster_information_ == NULL) {
    return false;
  }
  // Check if this worker is part of the cluster
  for (int i = 0; i < cluster_information_->workers_size(); ++i) {
    if (cluster_information_->workers(i).worker_id() == worker_id) {
      return true;
    }
  }

  return false;
}
}
