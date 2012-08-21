


#ifndef _H_SAMSON_NETWORK_MANAGER
#define _H_SAMSON_NETWORK_MANAGER

#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#include "au/containers/map.h"
#include "au/tables/Table.h"

#include "engine/Engine.h"
#include "engine/Notification.h"
#include "engine/Object.h"

#include "au/network/NetworkListener.h"
#include "samson/network/Packet.h"
#include "samson/network/PacketQueue.h"


namespace au {
class SocketConnection;
}

namespace samson {
class NetworkConnection;
class NetworkListener;

class NetworkManager {
  // Multi queue for all unconnected connections
  MultiPacketQueue multi_packet_queue;

  // All managed connection ( name = code name of node identifier )
  au::map<std::string, NetworkConnection> connections;

  // Token to block add and move operations on connections
  au::Token token_connections_;

  friend class NetworkConnection;

public:

  NetworkManager() : token_connections_("token_connections_") {
  }

  ~NetworkManager() {
    // Remove all pending packets to be sent
    multi_packet_queue.Clear();
  }

  // Unique method to add connections
  void AddConnection(NodeIdentifier new_node_identifier, au::SocketConnection *socket_connection);

  // Interface to inform about a received packet from a network connection
  virtual void receive(NetworkConnection *connection, const PacketPointer& packet) {
    LM_W(("NetworkManager::receive not implemented"));
  }

  // Push a packet to a connection
  void Send(const PacketPointer& packet);
  void SendToAllDelilahs(const PacketPointer& packet);

  // Review
  //  --> Remove packets sent to long unconnected nodes
  //  --> Remove disconnected connections
  void Review();


  // Remove this connection ( whatever name it has )
  void Remove(NetworkConnection *network_connection);
  void Remove(const std::string& connection_name);

  // Remove all unconnected connections
  void RemoveDisconnectedConnections();

  // Check connection
  bool isConnected(std::string connection_name);

  // Get table with connection information
  au::tables::Table *getConnectionsTable();

  // Get pending packets table
  au::tables::Table *getPendingPacketsTable();

  // Get delilah ids
  std::vector<size_t> getDelilahIds();

  // Get all connections
  std::vector<std::string> getAllConnectionNames();

  // Debug str
  std::string str();

  // Get a collection for all connections
  gpb::Collection *getConnectionsCollection(const Visualization& visualization);

  // Reset all connections
  void reset();

  // Get number of connections
  size_t getNumConnections();

  size_t get_rate_in();
  size_t get_rate_out();
  std::string getStatusForConnection(std::string connection_name);
};
}

#endif // ifndef _H_SAMSON_NETWORK_MANAGER
