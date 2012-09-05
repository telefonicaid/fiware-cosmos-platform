#include "au/network/SocketConnection.h"

#include "NetworkManager.h"  // Own interface
#include "au/network/NetworkListener.h"
#include "au/network/SocketConnection.h"
#include "samson/network/NetworkConnection.h"
#include "samson/network/NetworkConnection.h"

namespace samson {
// Get all connections
std::vector<std::string> NetworkManager::getAllConnectionNames() {
  au::TokenTaker tt(&token_connections_);

  return connections.getKeysVector();
}

void NetworkManager::Remove(NetworkConnection *network_connection) {
  au::map<std::string, NetworkConnection>::iterator it;
  for (it = connections.begin(); it != connections.end(); it++) {
    if (it->second == network_connection) {
      connections.extractFromMap(it->first);
      delete network_connection;
      break;
    }
  }

  LM_W(("Removing a network connection that is not part of this manager"));
}

void NetworkManager::Remove(const std::string& connection_name) {
  NetworkConnection *connection = connections.extractFromMap(connection_name);

  if (connection) {
    delete connection;
    return;
  } else {
    LM_W(("trying to remove connection %s that is not present in this connection manager"
          , connection_name.c_str()));
  }
}

void NetworkManager::AddConnection(NodeIdentifier new_node_identifier, au::SocketConnection *socket_connection) {
  // Mutex protection
  au::TokenTaker tt(&token_connections_, "token_connections_.add");

  // Name of this connection
  std::string name = new_node_identifier.getCodeName();

  LM_T(LmtNetworkConnection, ("Adding network_connection:%s", name.c_str()));

  if (connections.findInMap(name) != NULL) {
    LM_W(("Rejecting an incomming connection (%s) since it already exists", name.c_str()));
    delete socket_connection;
    return;
  }

  // Add to the map of connections
  LM_T(LmtNetworkConnection, ("Inserted new connection %s", name.c_str()));
  NetworkConnection *network_connection = new NetworkConnection(new_node_identifier, socket_connection, this);
  connections.insertInMap(name, network_connection);
}

size_t NetworkManager::getNumConnections() {
  au::TokenTaker tt(&token_connections_, "token_connections_.getNumConnections");

  return connections.size();
}

bool NetworkManager::isConnected(std::string connection_name) {
  au::TokenTaker tt(&token_connections_, "token_connections_.isConnected");
  bool connected = (connections.findInMap(connection_name) != NULL);

  return connected;
}

au::tables::Table *NetworkManager::getConnectionsTable() {
  au::TokenTaker tt(&token_connections_, "token_connections_.getConnectionsTable");

  au::tables::Table *table = new au::tables::Table(au::StringVector("Name", "Host", "In", "Out"));

  au::map<std::string, NetworkConnection>::iterator it_connections;

  for (it_connections = connections.begin(); it_connections != connections.end(); it_connections++) {
    au::StringVector values;

    values.push_back(it_connections->first);     // Name of the connection

    NetworkConnection *connection = it_connections->second;
    au::SocketConnection *socket_connection = connection->socket_connection_;
    values.push_back(socket_connection->host_and_port());
    values.push_back(au::str(connection->get_rate_in(), "B/s"));
    values.push_back(au::str(connection->get_rate_out(), "B/s"));

    table->addRow(values);
  }

  table->setTitle("Connections");

  return table;
}

void NetworkManager::RemoveDisconnectedConnections() {
  au::map<std::string, NetworkConnection>::iterator it;
  for (it = connections.begin(); it != connections.end(); ) {
    NetworkConnection *connection = it->second;

    if (connection->isDisconnectd()) {
      // Extract connection
      LM_M(("Removing connection %s since it is disconnected", it->first.c_str()));
      connections.erase(it++);
      delete connection;
    } else {
      ++it;
    }
  }
}

std::vector<size_t> NetworkManager::getDelilahIds() {
  // Return all connections with pattern delilah_X
  std::vector<size_t> ids;

  au::TokenTaker tt(&token_connections_);

  au::map<std::string, NetworkConnection>::iterator it_connections;
  for (it_connections = connections.begin(); it_connections != connections.end(); it_connections++) {
    NodeIdentifier _node_identifier = it_connections->second->node_identifier();

    if (_node_identifier.node_type  == DelilahNode) {
      size_t id = _node_identifier.id;

      if (it_connections->first == _node_identifier.getCodeName()) {
        // Add this id to the list
        ids.push_back(id);
      } else {
        LM_W(("Delilah %lu (%s) connected using wrong connection name %s",
              _node_identifier.id,
              _node_identifier.getCodeName().c_str(),
              it_connections->first.c_str()
              ));
      }
    }
  }

  return ids;
}

std::string NetworkManager::str() {
  au::TokenTaker tt(&token_connections_);

  std::ostringstream output;

  au::map<std::string, NetworkConnection>::iterator it_connections;
  for (it_connections = connections.begin(); it_connections != connections.end(); it_connections++) {
    output << it_connections->first << " : " << it_connections->second->str() << "\n";
  }

  return output.str();
}

void NetworkManager::Review() {
  std::set<std::string> current_connections = connections.getKeys();
  // Revie general queue to remove packets to long-unconnected nodes
  multi_packet_queue.RemoveOldConnections(current_connections);

  // Remove all unconnected elements
  RemoveDisconnectedConnections();
}

void NetworkManager::Send(const PacketPointer& packet) {
  au::TokenTaker tt(&token_connections_);

  // Accumulated packet in the global queue
  multi_packet_queue.Push(packet);

  // Wakeup writer in the connection if necessary
  std::string name = packet->to.getCodeName();
  NetworkConnection *connection = connections.findInMap(name);
  if (connection) {
    connection->WakeUpWriter();
  }
}

void NetworkManager::SendToAllDelilahs(const PacketPointer& packet) {
  au::TokenTaker tt(&token_connections_);

  // Send to all involved workers
  au::map<std::string, NetworkConnection>::iterator it_connections;
  for (it_connections = connections.begin(); it_connections != connections.end(); it_connections++) {
    std::string name = it_connections->first;
    NetworkConnection *connection = it_connections->second;
    NodeIdentifier connection_node_identifier = connection->node_identifier();

    if (connection_node_identifier.node_type == DelilahNode) {
      // Send to this one
      PacketPointer new_packet(new Packet(packet.shared_object()));
      new_packet->to = connection_node_identifier;
      Send(new_packet);
    }
  }
}

gpb::Collection *NetworkManager::getConnectionsCollection(const Visualization& visualization) {
  gpb::Collection *collection = new gpb::Collection();

  collection->set_name("connections");

  au::TokenTaker tt(&token_connections_);

  au::map<std::string, NetworkConnection>::iterator it_connections;

  for (it_connections = connections.begin(); it_connections != connections.end(); it_connections++) {
    gpb::CollectionRecord *record = collection->add_record();
    it_connections->second->fill(record, visualization);
  }

  return collection;
}

void NetworkManager::reset() {
  au::TokenTaker tt(&token_connections_);

  // Detele all connections
  connections.clearMap();

  au::map<std::string, NetworkConnection>::iterator it_connections;
  for (it_connections = connections.begin(); it_connections != connections.end(); it_connections++) {
    NetworkConnection *connection = it_connections->second;
    connection->Close();
  }

  // Node: We cannot wait for all connections to be disconnected because reset command is originated in a delilah connection
}

size_t NetworkManager::get_rate_in() {
  au::TokenTaker tt(&token_connections_);

  size_t total = 0;

  au::map<std::string, NetworkConnection>::iterator it_connections;
  for (it_connections = connections.begin(); it_connections != connections.end(); it_connections++) {
    total += it_connections->second->get_rate_in();
  }

  return total;
}

size_t NetworkManager::get_rate_out() {
  au::TokenTaker tt(&token_connections_);

  size_t total = 0;

  au::map<std::string, NetworkConnection>::iterator it_connections;
  for (it_connections = connections.begin(); it_connections != connections.end(); it_connections++) {
    total += it_connections->second->get_rate_out();
  }

  return total;
}

std::string NetworkManager::getStatusForConnection(std::string connection_name) {
  au::TokenTaker tt(&token_connections_);

  // Find this connection...
  NetworkConnection *connection = connections.findInMap(connection_name);

  if (!connection) {
    return "Unknown connection";
  } else if (connection->isDisconnectd()) {
    return "Disconnected";
  } else {
    return au::str("Connected In: %s Out: %s "
                   , au::str(connection->get_rate_in(), "B/s").c_str()
                   , au::str(connection->get_rate_out(), "B/s").c_str());
  }
}

au::tables::Table *NetworkManager::getPendingPacketsTable() {
  return multi_packet_queue.getPendingPacketsTable();
}
}
