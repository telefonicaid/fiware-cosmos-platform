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
#ifndef SAMSON_WORKER_H
#define SAMSON_WORKER_H

/* ****************************************************************************
 *
 * FILE                     SamsonWorker.h
 *
 * DESCRIPTION			   Main class for the worker element
 *
 */

#include <iomanip>                              // setiosflags()
#include <iostream>                             // std::cout
#include <string>

#include "au/console/Console.h"
#include "au/containers/SharedPointer.h"
#include "au/network/RESTService.h"
#include "au/network/RESTServiceCommand.h"
#include "au/string/StringUtilities.h"
#include "zoo/Connection.h"

#include "engine/EngineElement.h"               // samson::EngineElement
#include "logMsg/logMsg.h"

#include "samson/common/KVHeader.h"
#include "samson/common/Macros.h"               // exit(.)
#include "samson/common/NotificationMessages.h"
#include "samson/common/samson.pb.h"            // samson::network::
#include "samson/common/samsonDirectories.h"    // SAMSON_WORKER_DEFAULT_PORT
#include "samson/delilah/Delilah.h"
#include "samson/module/Operation.h"
#include "samson/network/WorkerNetwork.h"
#include "samson/stream/BlockManager.h"
#include "samson/stream/WorkerTaskManager.h"
#include "samson/stream/WorkerTaskManager.h"     // samson::stream::WorkerTaskManager
#include "samson/worker/DataModel.h"
#include "samson/worker/SamsonWorkerController.h"
#include "samson/worker/SamsonWorkerRest.h"
#include "samson/worker/SamsonWorkerSamples.h"
#include "samson/worker/WorkerBlockManager.h"
#include "samson/worker/WorkerCommandManager.h"

/*
 *
 * SamsonWorker
 *
 * Status
 *
 * unconnected    --> Non connected to any cluster
 * connected      --> Connected to ZK
 * included       --> Part of the cluster
 * ready          --> All blocks required for processing are here
 * cluster_ready  --> All elements in the cluster are ready
 */

namespace samson {
// Forward declarations
class NetworkInterface;
class Info;
class DistributionOperation;
class PushOperation;
class SamsonWorkerRest;
namespace stream {
class WorkerTaskManager;
}


template<typename C>
class Minimum {
public:

  Minimum() {
    defined_ = false;
  }

  void Push(C c) {
    if (!defined_) {
      defined_ = true;
      c_ = c;
    } else if (c < c_) {
      c_ = c;
    }
  }

  bool IsDefined() const {
    return defined_;
  }

  C value() const {
    return c_;
  }

private:
  C c_;
  bool defined_;
};

class BlockSortInfo {
public:

  BlockSortInfo() {
    so_is_state_ = false;
    so_pos_ = SIZE_T_UNDEFINED;
    previous_data_model_ = false;
  }

  void NotifyQueue(const std::string& queue) {
    queues_.insert(queue);   // Accumulate list of queues
  }

  void NotifyInputForStreamOperation(const std::string&  stream_operation_name, const std::string& queue_name,
                                     bool state,
                                     size_t pos) {
    if (so_is_state_ && !state) {
      return;
    }
    if (( so_pos_ != SIZE_T_UNDEFINED ) && ( so_pos_ < pos )) {
      return;
    }

    // Update information
    so_is_state_ = state;
    so_pos_ =  pos;
    so_name_ = stream_operation_name;
    so_queue_ = queue_name;
  }

  void NotifyInputForTask(size_t task) {
    min_task_id_.Push(task);
    tasks_.insert(task);
  }

  void NotifyPreviousDataModel() {
    previous_data_model_ = true;
  }

  size_t min_task_id() {
    if (!min_task_id_.IsDefined()) {
      return SIZE_T_UNDEFINED;
    }
    return min_task_id_.value();
  }

  std::string State() const {
    std::ostringstream output;

    // Inform about main task
    if (tasks_.size() > 0) {
      output << "WT" << au::str(tasks_) << " ";
    }
    if (queues_.size() > 0) {
      output << "Q" << au::str(queues_) << " ";
    } else if (previous_data_model_) {
      output << "[Previous Data Model]";
    }

    if (so_pos_ != SIZE_T_UNDEFINED) {
      output << "SO[ " << so_name_ << " ";
      output << (so_is_state_ ? "[STATE]" : "") << so_queue_ << " " << au::str(so_pos_) << "]";
    }

    return output.str();
  }

  bool Compare(BlockSortInfo *other) {
    size_t min_task_id_1 = min_task_id_.value();
    size_t min_task_id_2 = other->min_task_id_.value();

    if (min_task_id_1 != min_task_id_2) {
      return (min_task_id_1 < min_task_id_2);
    }

    // Stream operation order
    if (( so_pos_ != SIZE_T_UNDEFINED ) && ( other->so_pos_ == SIZE_T_UNDEFINED )) {
      return true;
    }
    if (( so_pos_ == SIZE_T_UNDEFINED ) && ( other->so_pos_ != SIZE_T_UNDEFINED )) {
      return false;
    }

    if (( so_pos_ != SIZE_T_UNDEFINED ) && ( other->so_pos_ != SIZE_T_UNDEFINED )) {
      // State first
      if (so_is_state_ && !other->so_is_state_) {
        return true;
      }
      if (!so_is_state_ && other->so_is_state_) {
        return false;
      }

      // Order in bytes
      return ( so_pos_ < other->so_pos_ );
    }

    // Same order
    return true;
  }

private:

  Minimum<size_t> min_task_id_;     // Minimum task
  std::set<size_t> tasks_;          // List of task
  std::set<std::string> queues_;    // List of queues

  // Previous data model
  bool previous_data_model_;

  // Higher priority information for a stream operation
  bool so_is_state_;
  size_t so_pos_;
  std::string so_name_;
  std::string so_queue_;
};


class BlocksSortInfo {
public:

  BlocksSortInfo() {
  }

  ~BlocksSortInfo() {
    info_.clearMap();
  }

  BlockSortInfo *Get(size_t block_id) {
    return info_.findInMap(block_id);
  }

  void NotifyPreviousDataModel(size_t block_id) {
    info_.findOrCreate(block_id)->NotifyPreviousDataModel();
  }

  void NotifyQueue(size_t block_id, const std::string& queue) {
    info_.findOrCreate(block_id)->NotifyQueue(queue);
  }

  void NotifyInputForTask(size_t block_id, size_t task_id) {
    info_.findOrCreate(block_id)->NotifyInputForTask(task_id);
  }

  void NotifyInputForStreamOperation(size_t block_id, const std::string&  stream_operation_name,
                                     const std::string& queue_name, bool state,
                                     size_t pos) {
    info_.findOrCreate(block_id)->NotifyInputForStreamOperation(stream_operation_name, queue_name, state, pos);
  }

  // String state for each block
  std::string GetStateForBlock(size_t block_id) {
    BlockSortInfo *info = info_.findInMap(block_id);

    if (!info) {
      return "To be removed";
    }
    return info->State();
  }

  // Sort two blocks
  bool CompareBlocks(size_t block_id1, size_t block_id2) const {
    BlockSortInfo *info_1 = info_.findInMap(block_id1);
    BlockSortInfo *info_2 = info_.findInMap(block_id2);

    if (!info_2) {
      return true;
    }

    if (!info_1) {
      return false;
    }

    // Other criterias...
    return info_1->Compare(info_2);
  }

private:

  au::map<size_t, BlockSortInfo> info_;   // map of information for all blocks
};


class SamsonWorker : public engine::NotificationListener, public au::Console {
public:

  SamsonWorker(std::string zoo_host, int port, int web_port);
  ~SamsonWorker() {
  }

  // All internal components are shared pointers

  // Interface to receive Packets
  void receive(const PacketPointer& packet);

  // Notification from the engine about finished tasks
  void notify(engine::Notification *notification);

  // au::Console ( debug mode with fg )
  void autoComplete(au::ConsoleAutoComplete *info);
  void evalCommand(std::string command);
  std::string getPrompt();

  // Function to get information about current status
  au::SharedPointer<gpb::Collection> GetWorkerCollection(const Visualization& visualization);
  au::SharedPointer<gpb::Collection> GetWorkerLogStatus(const Visualization& visualization);
  au::SharedPointer<gpb::Collection> GetWorkerAllLogChannels(const Visualization& visualization);
  au::SharedPointer<gpb::Collection> GetCollectionForDataModelStatus(const Visualization& visualization);
  au::SharedPointer<gpb::Collection> GetCollectionForDataModelCommits(const Visualization& visualization);
  au::SharedPointer<gpb::Collection> GetKVRangesCollection(const Visualization& visualization);


  bool IsReady();     // Method to access if worker is ready
  bool IsConnected();     // Method to access if worker is ready

  // Accessors to individual components of this worker
  au::SharedPointer<au::zoo::Connection> zoo_connection();
  au::SharedPointer<SamsonWorkerController> worker_controller();
  au::SharedPointer<DataModel> data_model();
  au::SharedPointer<WorkerNetwork> network();

  // Accessors to individual components of this worker
  au::SharedPointer<SamsonWorkerRest> samson_worker_rest();
  au::SharedPointer<WorkerBlockManager> worker_block_manager();
  au::SharedPointer<stream::WorkerTaskManager> task_manager();
  au::SharedPointer<WorkerCommandManager> workerCommandManager();

  // Reload modules
  void ReloadModulesIfNecessary();

  int port() {
    return port_;
  }

  // Get complete information about blocks in the system for this worker
  au::SharedPointer<BlocksSortInfo> GetBlocksSortInfo();

private:

  enum State {
    unconnected, connected, ready
  };

  // Main function to review samson worker and all its elements
  // This function is preiodically called from engine
  void Review();

  void ResetToUnconnected();     // Reset when come back to unconnected
  void ResetToConnected();     // Reset when come back to connected ( change cluster setup )

  // Visualitzation of current data model
  void fill(gpb::CollectionRecord *record, const std::string& name, gpb::Data *data, const Visualization& visualization);

  // State of this worker
  std::string str_state();

  // Connection values ( to reconnect if connections fails down )
  std::string zoo_host_;
  int port_;
  int web_port_;

  // Initial time stamp for this worker
  au::Cronometer cronometer_;

  // Main elements of the worker
  au::SharedPointer<au::zoo::Connection> zoo_connection_;     // Main connection with the zk
  au::SharedPointer<SamsonWorkerController> worker_controller_;     // Cluster setup controller
  au::SharedPointer<DataModel> data_model_;     // Data model
  au::SharedPointer<WorkerNetwork> network_;     // Network manager to manage connections
  au::SharedPointer<SamsonWorkerRest> samson_worker_rest_;     // REST Service
  au::SharedPointer<WorkerBlockManager> worker_block_manager_;     // Map of blocks recently created
  au::SharedPointer<stream::WorkerTaskManager> task_manager_;     // Manager for tasks
  au::SharedPointer<WorkerCommandManager> workerCommandManager_;     // Manager of the "Worker commands"

  State state_;     // Current state of this worker
  std::string state_message_;     // Message of the last review of the state

  bool modules_available_;          // Flag to determine if blocks for modules are available
  size_t last_modules_version_;     // Last version of the queue .modules observed so far

  // Cronometer for last candidate data model
  au::Cronometer cronometer_candidate_data_model_;
};
}

#endif  // ifndef SAMSON_WORKER_H
