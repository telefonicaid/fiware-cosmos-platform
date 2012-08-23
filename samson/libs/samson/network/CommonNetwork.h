


#ifndef _H_SAMSON_COMMON_NETWORK
#define _H_SAMSON_COMMON_NETWORK

#include "engine/Object.h"

#include "au/network/NetworkListener.h"

#include "samson/common/ports.h"
#include "samson/common/status.h"

#include "samson/common/samson.pb.h"

#include "samson/network/NetworkInterface.h"
#include "samson/network/NetworkManager.h"
#include "samson/network/PacketQueue.h"
#include "samson/network/PacketReceivedNotification.h"

namespace samson {
class CommonNetwork : public NetworkManager, public engine::Object {
  // My identifier in the cluster
  NodeIdentifier node_identifier_;

  // Cluster information ( workers to be connected to )
  au::SharedPointer<gpb::ClusterInfo> cluster_information_;

  // Receiver to be notified about packets
  NetworkInterfaceReceiver *receiver_;

  // Mutex protection
  au::Token token_;

public:

  CommonNetwork(NodeIdentifier my_node_identifier
                , NetworkInterfaceReceiver *receiver
                , au::SharedPointer<gpb::ClusterInfo> cluster_information) : token_("CommonNetwork") {
    // Keep pointer to receiver
    receiver_ = receiver;

    // Identify myself
    node_identifier_ = my_node_identifier;

    LM_V(("CommonNetwork %s", node_identifier_.str().c_str()));

    // No cluster information at the moment
    cluster_information_ = cluster_information;

    // Listen and create notifications for network manager review
    listen("notification_network_manager_review");
    engine::Engine::shared()->notify(new engine::Notification("notification_network_manager_review"), 1);

    // Listen notification to send packets
    listen("send_to_all_delilahs");
  }

  ~CommonNetwork() {
  }

  // Get a cluster information packet
  PacketPointer getClusterInfoPacket();

  // Get my node identifier
  NodeIdentifier node_identifier();

  // Set a new custer information ( if version is really new compared with the other ones )
  void set_cluster_information(au::SharedPointer<gpb::ClusterInfo> cluster_information);

  // Remove previous cluster_info ( deslilah to disconnect )
  void remove_cluster_information();

  // Review connections
  void review_connections();

  // Engine notification interface
  // ----------------------------------------------------------------
  void notify(engine::Notification *notification);

  // NetworkManager interface
  // ----------------------------------------------------------------
  void receive(NetworkConnection *connection, const PacketPointer& packet);

  // Send packets
  void Send(const PacketPointer& packet);     // Bypass to avoid sending data to myself
  void SendToAllWorkers(const PacketPointer& packet, std::set<size_t>& workers);

  // Get a collection of current network to show a list of them on a console/ rest / etc..
  gpb::Collection *getConnectionsCollection(const Visualization& visualization);

  // Get my node identifier
  NodeIdentifier  getMynodeIdentifier();

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
  size_t getRandomWorkerId(size_t previous_worker = (size_t)-1) {
    au::TokenTaker tt(&token_);

    // If no information, no worker
    if (cluster_information_ == NULL) {
      return -1;
    }

    // Get list of connected workers
    std::vector<size_t> connected_worker_ids;
    for (int i = 0; i < cluster_information_->workers_size(); i++) {
      size_t worker_id = cluster_information_->workers(i).worker_id();
      if (isConnected(NodeIdentifier(WorkerNode, worker_id).getCodeName())) {
        connected_worker_ids.push_back(worker_id);
      }
    }

    // If no worker connected, no worker selected
    if (connected_worker_ids.size() == 0) {
      return -1;
    }





    if (previous_worker == (size_t)-1) {
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
  bool isValidWorkerId(size_t worker_id) {
    return isConnected(NodeIdentifier(WorkerNode, worker_id).getCodeName());
  }

private:

  // Add output worker connections
  Status addWorkerConnection(size_t worker_id, std::string host, int port);

  // Schedule a notification using engine
  void schedule_receive(PacketPointer packet);
};
}

#endif  // ifndef _H_SAMSON_COMMON_NETWORK
