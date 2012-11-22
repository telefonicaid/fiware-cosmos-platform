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
  void Update(int num_hgs, size_t input_size, size_t state_size, double time) {
    input_.Push(input_size);
    state_.Push(state_size);
    process_.Push((double)( input_size + state_size ) / time);

    num_hgs_ = num_hgs;   // Just keep the number of divisions to compute erlangs
  }

  std::string GetInputRate() const {
    return input_.str() + "B/s";
  }

  std::string  GetProcessRate() const {
    return process_.str("B/s");
  }

  std::string GetStateSize() const {
    return state_.str("B");
  }

  int num_hgs() {
    return num_hgs_;
  }

  void fill(samson::gpb::CollectionRecord *record, const Visualization& visualization) {
    add(record, "#hgs", num_hgs_, "left,different");
    add(record, "#Ops", input_.hits(), "left,different");
    add(record, "#Ops/s", au::str(input_.hit_rate()), "left,different");
    add(record, "Input", au::str(input_.rate(), "B/s"), "left,different");
    add(record, "State", au::str(state_.GetAverage(), "B"), "left,different");
    add(record, "Process", au::str(process_.GetAverage(), "B/s"), "left,different");
  }

private:

  int num_hgs_;
  au::Rate input_;
  au::Averager state_;
  au::Averager process_;
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

private:

  bool runNextWorkerTasksIfNecessary();


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
