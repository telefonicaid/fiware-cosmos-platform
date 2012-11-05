#include "samson/network/WorkerNetwork.h"  // Own interface

#include "au/utils.h"
#include "au/log/LogMain.h"
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
      LM_X(1, ("Not possible to open main samson port %d (%s). Probably another worker is running...", port, status(s)));
    }
  }
}

WorkerNetwork::~WorkerNetwork() {
  if (worker_listener != NULL) {
    LM_T(LmtCleanup, ("In ~WorkerNetwork() calling StopNetworkListeners()"));
    worker_listener->StopNetworkListener();
    delete worker_listener;
    worker_listener = NULL;
  }
}

void WorkerNetwork::stop() {
  // Stop listeners
  LM_T(LmtCleanup, ("NetworkListener called to stop"));
  worker_listener->StopNetworkListener();

  // Close all connections
  LM_T(LmtCleanup, ("Close all connections"));
  NetworkManager::reset();
}

void WorkerNetwork::newSocketConnection(au::NetworkListener *listener, au::SocketConnection *socket_connection) {
  if (listener != worker_listener) {
    LM_X(1, ("New connections from an unknown listener"));
    socket_connection->Close();
    delete socket_connection;
  }

  if (cluster_information_version() == (size_t) -1) {   // Still not part of any cluster..
    AU_SW(("Connection rejected since I am still not part of any cluster..."));
    socket_connection->Close();
    delete socket_connection;
    return;
  }

  // Read hello message synchronously
  Packet packet;
  au::Status s = packet.read(socket_connection, NULL);

  if (s != au::OK) {
    LM_W(("Error receiving hello message form income connection (%s)", au::status(s)));
    socket_connection->Close();
    delete socket_connection;
    return;
  }

  if (packet.msgCode != Message::Hello) {
    LM_W(("Received message %s instead of the required hello message. Closing connection"
            , messageCode(packet.msgCode)));
    socket_connection->Close();
    delete socket_connection;
    return;
  }

  // Check valid hello message
  if (!packet.message->has_hello()) {
    LM_W(("Missing Hello information in a hello packet"));
    socket_connection->Close();
    delete socket_connection;
    return;
  }

  // Identifier from the incomming hello packet
  NodeIdentifier new_node_identifier(packet.message->hello().node_identifier());

  if (new_node_identifier.node_type == UnknownNode) {
    LM_W(("Hello message received with nodetype unknown. Closing connection"));
    socket_connection->Close();
    delete socket_connection;
    return;
  }

  // Check valid delilah id ( if this is a delilah connection )
  if (new_node_identifier.node_type == DelilahNode) {
    size_t new_delilah_id = new_node_identifier.id;
    if (!au::code64_is_valid(new_delilah_id)) {
      LM_W(("No valid delilah_id (%lu). Closing conneciton...", new_delilah_id));
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
      LM_W(("Not possible to write cluster information a new delilah connection (%s) ", au::status(s)));
      socket_connection->Close();
      delete socket_connection;
      return;
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
  p->message->set_delilah_component_id((size_t) -1);

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
  p->message->set_delilah_component_id((size_t) -1);

  // Direction of this packet
  p->to = NodeIdentifier(DelilahNode, delilah_id);
  Send(p);
}
}
