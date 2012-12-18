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
#ifndef _H_SAMSON_QUEUE_TASK_MANAGER
#define _H_SAMSON_QUEUE_TASK_MANAGER

#include <list>
#include <map>
#include <string>           // std::string
#include <vector>

#include "au/containers/Dictionary.h"
#include "au/containers/Queue.h"
#include "au/containers/SharedPointer.h"
#include "au/statistics/Averager.h"

#include "engine/NotificationListener.h"  // engine::NotificationListener

#include "samson/common/Visualitzation.h"
#include "samson/common/gpb_operations.h"
#include "samson/common/status.h"
#include "samson/module/ModulesManager.h"
#include "samson/module/Operation.h"
#include "samson/stream/WorkerSystemTask.h"
#include "samson/stream/WorkerTask.h"

namespace samson {
class SamsonWorker;
class GlobalBlockSortInfo;

namespace stream {
class StreamOperationRangeInfo;
class StreamOperationGlobalInfo;

// Logs for debugging

struct WorkerTaskLog {
  std::string description;     // Description of the task
  std::string result;      // Result of the operation
  std::string inputs;      // Information at the input of the operation
  std::string outputs;     // Information at output of the operation
  std::string times;
  std::string process_time;
  int waiting_time_seconds;     // Waiting time until execution starts
  int running_time_seconds;     // Running time
};

class StreamOperationStatistics {
public:

  /**
   * \brief Inform about a finished task to update statistics
   */

  void UpdateTaskInformation(int num_hgs, BlockInfo input, BlockInfo output, BlockInfo state, double process_time) {
    // Update information about input
    input_blocks_.Push(input.num_blocks);
    input_size_.Push(input.info.size);
    input_kvs_.Push(input.info.kvs);

    // Update information about input
    output_blocks_.Push(output.num_blocks);
    output_size_.Push(output.info.size);
    output_kvs_.Push(output.info.kvs);

    // Update information about state
    state_size_.Push(state.info.size);
    state_kvs_.Push(state.info.kvs);

    // Update information about process
    real_process_rate_.Push((double)(input.size) / process_time);
    process_rate_.Push((double)(input.size + state.size) / process_time);

    // Just keep the number of divisions to compute erlangs
    num_hgs_ = num_hgs;
  }

  void fill(samson::gpb::CollectionRecord *record, const Visualization& visualization) const {
    if (visualization.get_flag("input")) {
      add(record, "Input", GetInputRateStr(), "left,different");
      add(record, "Total input", GetInputTotalStr(), "left,different");
    }

    if (visualization.get_flag("output")) {
      add(record, "Output", GetOutputRateStr(), "left,different");
      add(record, "Total output", GetOutputTotalStr(), "left,different");
    }

    if (visualization.get_flag("state")) {
      add(record, "#hgs", num_hgs_, "left,different");
      add(record, "State", GetStateStr(), "left,different");
    }

    add(record, "#Ops/s", GetOperationsRateStr(), "left,different");
    add(record, "Total #Ops", GetOperationsTotalStr(), "left,different");
    add(record, "Process", GetProcessRateStr(), "left,different");
  }

  int num_hgs() const {
    return num_hgs_;
  }

  std::string GetStateStr() const {
    return state_kvs_.str_mean("kvs") + " " + state_size_.str_mean("B");
  }

  std::string GetInputRateStr() const {
    return au::str(input_blocks_.rate(), "blocks/s") + au::str(input_kvs_.rate(), "kvs/s") + " " + au::str(
             input_size_.rate(), "B/s");
  }

  std::string GetOutputRateStr() const {
    return au::str(output_blocks_.rate(), "blocks/s") + au::str(output_kvs_.rate(), "kvs/s") + " " + au::str(
             output_size_.rate(), "B/s");
  }

  std::string GetOperationsRateStr() const {
    return au::str(input_kvs_.hit_rate(), "ops/s");
  }

  std::string GetInputTotalStr() const {
    return au::str(input_blocks_.size(), "blocks") + au::str(input_kvs_.size(), "kvs") + " " + au::str(input_size_.size(
                                                                                                         ), "B");
  }

  std::string GetOutputTotalStr() const {
    return au::str(output_blocks_.size(), "blocks") + au::str(output_kvs_.size(), "kvs") + " " + au::str(
             output_size_.size(), "B");
  }

  std::string GetOperationsTotalStr() const {
    return au::str(input_kvs_.hits(), "ops");
  }

  std::string GetProcessRateStr() const {
    return au::str(real_process_rate_.GetAverage(), "B/s") + " " + au::str(process_rate_.GetAverage(), "B/s");
  }

private:

  int num_hgs_;

  au::Rate input_blocks_;
  au::Rate input_size_;
  au::Rate input_kvs_;

  au::Rate output_blocks_;
  au::Rate output_size_;
  au::Rate output_kvs_;

  au::Averager state_size_;
  au::Averager state_kvs_;

  au::Averager real_process_rate_;
  au::Averager process_rate_;
};


class WorkerTaskManager : public ::engine::NotificationListener {
public:

  explicit WorkerTaskManager(SamsonWorker *samson_worker);
  ~WorkerTaskManager();

  void Add(au::SharedPointer<WorkerTaskBase> task);     // Add new task to the manager

  size_t getNewId();     // Get identifier for a new task

  // Add a system task to distribute a particular block to a set of workers
  size_t AddBlockRequestTask(size_t block_id, const std::vector<size_t>& worker_ids);

  // Review schedules tasks
  void reviewPendingWorkerTasks();

  // Review stream operations to schedule new stuff
  void review_stream_operations();

  // Notifications
  void notify(engine::Notification *notification);

  // Reset all the content of this manager
  void Reset();

  // Get a collection for monitoring
  gpb::CollectionPointer GetCollection(const ::samson::Visualization& visualization);
  gpb::CollectionPointer GetLastTasksCollection(const ::samson::Visualization& visualization);

  size_t get_num_running_tasks() const;
  size_t get_num_tasks() const;

  // Get collection to list in delilah
  gpb::CollectionPointer GetCollectionForStreamOperationsRanges(const ::samson::Visualization& visualization);
  gpb::CollectionPointer GetCollectionForStreamOperations(const ::samson::Visualization& visualization);
  gpb::CollectionPointer GetSOStatisticsCollection(const ::samson::Visualization& visualization);

  // Update information about blocks
  void Update(GlobalBlockSortInfo *info) const;

  /**
   * \brief Receive a new worker task log to keep activity of tasks
   */
  void Push(WorkerTaskLog log) {
    last_tasks_.push_back(log);
    while (last_tasks_.size() > 100) {
      last_tasks_.pop_back();
    }
  }

  /**
   * \brief Update statistics about a particular stream operation
   */
  void UpdateStreamOperationStatistics(const std::string& stream_operation_name,
                                       int num_hgs,
                                       BlockInfo input,
                                       BlockInfo output,
                                       BlockInfo state,
                                       double process_time) {
    StreamOperationStatistics *statistics = stream_operations_statistics_.findOrCreate(stream_operation_name);

    statistics->UpdateTaskInformation(num_hgs, input, output, state, process_time);
  }

private:

  bool runNextWorkerTasksIfNecessary();

  /**
   * \brief Compress ranges in groups ( applyied to forward operations )
   */
  static std::vector<KVRange> CompressKVRanges(std::vector<KVRange> input_ranges, int reduction_factor,
                                               au::ErrorManager& error);

  size_t id_;     // Id of the current task
  au::Queue<WorkerTaskBase> pending_tasks_;                  // List of pending task to be executed
  au::Dictionary<size_t, WorkerTaskBase> running_tasks_;     // Map of running tasks

  // Information about execution of current stream operations
  au::map<size_t, StreamOperationGlobalInfo> stream_operations_global_info_;

  // Pointer to samson worker
  SamsonWorker *samson_worker_;

  // Log of last tasks...
  std::list<WorkerTaskLog> last_tasks_;

  // Statistics about tasks
  au::map<std::string, StreamOperationStatistics > stream_operations_statistics_;

  // Get the number of current running tasks for a particular stream operation
  int GetRunningTasks(size_t stream_operation_id);
};
}
}

#endif  // ifndef _H_SAMSON_QUEUE_TASK_MANAGER
