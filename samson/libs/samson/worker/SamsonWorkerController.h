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

#ifndef _H_SAMSON_ZOO_SAMSON_WORKER_CONTROLLER
#define _H_SAMSON_ZOO_SAMSON_WORKER_CONTROLLER

#include <map>
#include <set>
#include <string>
#include <vector>

#include "au/containers/SharedPointer.h"
#include "au/containers/Uint64Vector.h"

#include "engine/Engine.h"
#include "engine/Notification.h"
#include "engine/NotificationListener.h"

#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/gpb_operations.h"
#include "samson/common/samson.pb.h"
#include "samson/worker/CommitCommand.h"
#include "zoo/Connection.h"

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
class SamsonWorkerController : public engine::NotificationListener {
public:

  SamsonWorkerController(au::zoo::Connection *zoo_connection, int port, int port_web);
  ~SamsonWorkerController() {
  }

  // Function to init everything with this connection( error code returned if not possible )
  int init();

  // virtual method in engine::NotificationListener
  virtual void notify(engine::Notification *notification);

  // Get my worker id
  size_t worker_id() const {
    return worker_id_;
  };

  // Get a copy of the current cluster setup
  au::SharedPointer<samson::gpb::ClusterInfo> GetCurrentClusterInfo() const {
    return cluster_info_;
  }

  // Get workers that should have a copy of a block in this range ( I am excluded from the list )
  au::Uint64Set GetWorkerIdsForRange(KVRange range) const;

  // Get ALL workers that should have a copy of a block
  au::Uint64Set GetAllWorkerIdsForRange(KVRange range) const;

  // Get all the workers identifiers
  std::set<size_t> GetWorkerIds() const;

  // Get ranges this worker should process
  std::vector<KVRange> GetMyKVRanges() const;     // Where I am responsible
  std::vector<KVRange> GetAllMyKVRanges() const;     // Where I am responsible or replica

  // Update worker-node with information about me
  int UpdateWorkerNode(size_t last_commit_id);

  // Get a new identifier for a block
  size_t get_new_block_id();

  // Get ranges to process data
  std::vector<KVRange> GetKVRanges() const {
    std::vector<KVRange> ranges;
    int num_divisions = cluster_info_->process_units_size();
    for (int i = 0; i < num_divisions; ++i) {
      ranges.push_back(GetKVRangeForDivision(i, num_divisions));
    }
    return ranges;
  }

  bool cluster_leader() const {
    return cluster_leader_;
  }

  // Ask for information based on local data
  std::set<size_t> GetWorkers();

  size_t GetMyLastCommitId();
  bool CheckDataModelCommitId(size_t last_commit_id);

private:

  // Get all worker identifiers
  int GetAllWorkersFromZk();

  // Name for node in zk for a particular worker
  static std::string node_for_worker(size_t worker_id) {
    return au::str("/samson/workers/w%010lu", worker_id);
  }

  static size_t worker_from_node(const std::string&node_name) {
    return atoll(node_name.substr(strlen(NODE_WORKER_BASE)).c_str());
  }

  // Main function to check when a watcher is received
  int Review();
  int ReviewClusterLeather();
  int ReviewNonClusterLeather();

  // Check it if is necessary to redefine the cluster
  bool IsValidClusterInfo();

  // Create a new cluster info for the current cluster situation
  int CreateClusterInfo(size_t version);

  // Function to get how to access me from the other workers
  std::string get_local_ip() const;

  // Recover cluster information setting up a watcher to observe updates
  int RecoverClusterInfo();

  // Main connection with the zk
  au::zoo::Connection *zoo_connection_;

  // Internal data of this worker
  int port_;
  int port_web_;
  bool cluster_leader_;                                      // Flag to indicate I am cluster leader
  size_t worker_id_;                                         // Identifier of this woker in the cluster ( unique in cluster history )
  std::string node_worker_;                                  // zk node name /samson/workers/wXXXXXXXXXX
  samson::gpb::WorkerInfo worker_info_;     // Information about this worker ( cpu, memory, ...)
  unsigned int block_id_counter_;                            // Counter to create new blocks

  // Data of the cluster
  std::vector<size_t> worker_ids_;                           // List of workers ids in this cluster
  au::SharedPointer<samson::gpb::ClusterInfo> cluster_info_; // Information about current cluster setup
  au::Dictionary<size_t, samson::gpb::WorkerInfo > workers_info_;       // Information observed from workers

  // Mutex protection
  mutable au::Token token_;
};


size_t worker_from_block_id(size_t block_id);
}

#endif  // ifndef _H_SAMSON_ZOO_SAMSON_WORKER_CONTROLLER
