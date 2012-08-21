

#ifndef _H_SAMSON_ZOO_SAMSON_WORKER_CONTROLLER
#define _H_SAMSON_ZOO_SAMSON_WORKER_CONTROLLER

#include "au/containers/Uint64Vector.h"

#include "engine/Engine.h"
#include "engine/Notification.h"
#include "engine/Object.h"

#include "samson/common/SamsonSetup.h"
#include "samson/common/gpb_operations.h"
#include "samson/common/samson.pb.h"
#include "samson/zoo/CommitCommand.h"
#include "samson/zoo/Connection.h"
#include "samson/zoo/ConnectionWatcherInterface.h"


#include "samson/common/MessagesOperations.h"
#include "samson/common/Visualitzation.h"

#define NODE_WORKER_BASE "/samson/workers/w"

// ------------------------------------------------------------------
//
// class SamsonWorkerController
//
// Main interface between a SAMSON worker and the zookeeper connection
// associated with current SAMSON cluster
//
// ------------------------------------------------------------------

namespace samson {
class SamsonWorkerController : public engine::Object, public samson::zoo::ConnectionWatcherInterface {
public:

  SamsonWorkerController(zoo::Connection *zoo_connection, int port, int port_web);
  ~SamsonWorkerController() {
  }

  // Function to init everything with this connection( error code returned if not possible )
  int init();

  // virtual method in samson::zoo::ConnectionWatcherInterface
  virtual void watcher(zoo::Connection *connection, int type, int state, const char *path);

  // Get my worker id
  size_t worker_id();

  // Get a copy of the current cluster setup
  void GetClusterInfo(samson::gpb::ClusterInfo **, size_t *version);

  // Get workers that should have a copy of a block in this range ( I am excluded from the list )
  au::Uint64Set GetWorkerIdsForRange(KVRange range);

  // Get ranges this worker should process
  KVRanges GetMyKVRanges();

  // Update worker-node with information about me
  int UpdateWorkerNode();

  // Get complete information on how blocks are distributed in the workers
  int GetBlockMap(std::multimap<size_t, size_t>& blocks_map);

private:

  // Get all worker identifiers
  int get_all_workers_from_zk(std::vector<size_t>&  worker_ids);

  // Name for node in zk for a particular worker
  static std::string node_for_worker(size_t worker_id) {
    return au::str("/samson/workers/w%010lu", worker_id);
  }

  std::vector<size_t> get_all_workers_from_cluster_info(samson::gpb::ClusterInfo *cluster_info);

  // Main function to check when a watcher is received
  int check();

  // Speficic function to check cluster_info when i am the cluster leader
  int check_cluster_info();

  // Check it if is necessary to redefine the cluster
  bool is_valid_cluster_info(samson::gpb::ClusterInfo *cluster_info);

  // Create a new cluster info for the current cluster situation
  int create_cluster_info(samson::gpb::ClusterInfo *cluster_info);

  // Alert that a new cluster_info has been received
  void alert_new_cluster_info();

  // Function to get how to access me from the other workers
  std::string get_local_ip();

  int recover_cluster_info();

  zoo::Connection *zoo_connection_;        // Main connection with the zk

  // Internal data of this worker
  int port_;
  int port_web_;

  std::string node_worker_;                  // zk node name /samson/workers/wXXXXXXXXXX
  samson::gpb::WorkerInfo worker_info_;      // Information about this worker ( cpu, memory, ...)

  samson::gpb::ClusterInfo cluster_info_;    // Information about cluster setup
  int64_t verion_node_cluster_info_;         // Last version we got about cluster setup

  // Identifier of this woker in the cluster ( unique in cluster history )
  size_t worker_id_;

  // Flag to indicate I am cluster leader
  bool cluster_leader_;

  // Mutex protection
  au::Token token_;
};
}

#endif // ifndef _H_SAMSON_ZOO_SAMSON_WORKER_CONTROLLER
