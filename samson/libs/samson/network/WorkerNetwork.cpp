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
#include "samson/network/WorkerNetwork.h"  // Own interface

#include "au/log/LogMain.h"
#include "au/log/LogMain.h"
#include "au/utils.h"
#include "samson/common/Logs.h"
#include "samson/network/NetworkConnection.h"

namespace samson {
WorkerNetwork::WorkerNetwork(size_t worker_id, int port) :
  CommonNetwork(NodeIdentifier(WorkerNode, worker_id)) {
  // Add listener for incoming connections
  // ----------------------------------------------------------------------------
  {
    worker_listener = new au::NetworkListener(this);
    au::Status s = worker_listener->InitNetworkListener(port);

    if (s != au::OK) {
      // Not allow to continue without incoming connections...
      LOG_X(1, ("Not possible to open main samson port %d (%s). Probably another worker is running...", port, status(s)));
    }
  }
}

WorkerNetwork::~WorkerNetwork() {
  LOG_D(logs.cleanup, ("Entering ~WorkerNetwork"));
  if (worker_listener != NULL) {
    worker_listener->StopNetworkListener();
    delete worker_listener;
    worker_listener = NULL;
  }
  LOG_D(logs.cleanup, ("Finished ~WorkerNetwork"));
}

void WorkerNetwork::stop() {
  // Stop listeners
  worker_listener->StopNetworkListener();

  // Close all connections
  NetworkManager::Reset();
}

void WorkerNetwork::newSocketConnection(au::NetworkListener *listener, au::SocketConnection *socket_connection) {
  if (listener != worker_listener) {
    LM_X(1, ("New connections from an unknown listener"));
    socket_connection->Close();
    delete socket_connection;
  }

  if (cluster_information_version() == static_cast<size_t>(-1)) {   // Still not part of any cluster..
    LOG_SW(("Connection rejected since I am still not part of any cluster..."));
    socket_connection->Close();
    delete socket_connection;
    return;
  }

  // Read hello message synchronously
  Packet packet;
  au::Status s = packet.read(socket_connection, NULL);

  if (s != au::OK) {
    LOG_SW(("Error receiving hello message form income connection (%s)", au::status(s)));
    socket_connection->Close();
    delete socket_connection;
    return;
  }

  if (packet.msgCode != Message::Hello) {
    LOG_SW(("Received message %s instead of the required hello message. Closing connection"
            , messageCode(packet.msgCode)));
    socket_connection->Close();
    delete socket_connection;
    return;
  }

  // Check valid hello message
  if (!packet.message->has_hello()) {
    LOG_SW(("Missing Hello information in a hello packet"));
    socket_connection->Close();
    delete socket_connection;
    return;
  }

  // Identifier from the incoming hello packet
  NodeIdentifier new_node_identifier(packet.message->hello().node_identifier());

  LOG_V(logs.network_connection, ("New connection from %s proving node identifier %s",
                                  socket_connection->host_and_port().c_str(), new_node_identifier.str().c_str()));


  if (new_node_identifier.node_type == UnknownNode) {
    LOG_SW(("Hello message received with nodetype unknown. Closing connection"));
    socket_connection->Close();
    delete socket_connection;
    return;
  }

  // Check valid delilah id ( if this is a delilah connection )
  if (new_node_identifier.node_type == DelilahNode) {
    size_t new_delilah_id = new_node_identifier.id;
    if (!au::code64_is_valid(new_delilah_id)) {
      LOG_SW(("No valid delilah_id (%lu). Closing conneciton...", new_delilah_id));
      socket_connection->Close();
      delete socket_connection;
      return;
    }
  }

  // If a delilah connection is received, send back a Cluster information message rigth now
  if (new_node_identifier.node_type == DelilahNode) {
    PacketPointer ci_packet = getClusterInfoPacket();
    ci_packet->from = node_identifier();
    ci_packet->to = new_node_identifier;

    au::Status s = ci_packet->write(socket_connection, NULL);

    if (s != au::OK) {
      LOG_SW(("Not possible to write cluster information a new delilah connection (%s) ", au::status(s)));
      socket_connection->Close();
      delete socket_connection;
      return;
    }
  }

  // In case of worker connection, check it is defined in the cluster
  if (new_node_identifier.node_type == WorkerNode) {
    if (IsWorkerInCluster(new_node_identifier.id)) {
      LOG_W(logs.network_connection, ("Rejecting connection from %s since it worker is not in cluster %s",
                                      socket_connection->host_and_port().c_str(), new_node_identifier.str().c_str()));
    } else {
      LOG_V(logs.network_connection, ("Worker %s is part of the cluster. Adding new connection...",
                                      new_node_identifier.str().c_str()));
    }
  }

  // Add connnection for this node identifier
  AddConnection(new_node_identifier, socket_connection);
}

void WorkerNetwork::SendAlertToAllDelilahs(std::string type, std::string context, std::string message) {
  PacketPointer p(new Packet(Message::Alert));
  gpb::Alert *alert = p->message->mutable_alert();

  alert->set_type(type);
  alert->set_context(context);
  alert->set_text(message);

  // This message do not belong to any delilah operation
  p->message->set_delilah_component_id(static_cast<size_t>(-1));

  // Send packet
  SendToAllDelilahs(p);
}

void WorkerNetwork::SendAlertToDelilah(size_t delilah_id, std::string type, std::string context, std::string message) {
  PacketPointer p(new Packet(Message::Alert));
  gpb::Alert *alert = p->message->mutable_alert();

  alert->set_type(type);
  alert->set_context(context);
  alert->set_text(message);

  // This message do not belong to any delilah operation
  p->message->set_delilah_component_id(static_cast<size_t>(-1));

  // Direction of this packet
  p->to = NodeIdentifier(DelilahNode, delilah_id);
  Send(p);
}
}
