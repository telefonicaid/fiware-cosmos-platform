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
#include "samson/stream/WorkerTaskManager.h"   // Own interface
#include <fnmatch.h>

#include "au/ExecesiveTimeAlarm.h"
#include <set>
#include <sstream>                              // std::ostringstream

#include "engine/Notification.h"                // engine::Notification
#include "engine/ProcessManager.h"      // engine::ProcessManager
#include "samson/common/Logs.h"
#include "samson/common/NotificationMessages.h"  // notification_process_request
#include "samson/common/SamsonSetup.h"          // SamsonSetup
#include "samson/module/ModulesManager.h"
#include "samson/stream/BlockManager.h"
#include "samson/stream/StreamOperationGlobalInfo.h"
#include "samson/stream/StreamOperationRangeInfo.h"
#include "samson/stream/WorkerSystemTask.h"     // WorkerSystemTask
#include "samson/stream/WorkerTask.h"           // samson::stream::WorkerTask
#include "samson/worker/SamsonWorker.h"

#define notification_run_stream_tasks_if_necessary "notification_run_stream_tasks_if_necessary"

namespace samson {
namespace stream {
WorkerTaskManager::WorkerTaskManager(SamsonWorker *samson_worker) {
  id_ = 1;
  samson_worker_ = samson_worker;

  listen(notification_run_stream_tasks_if_necessary);
  listen("cancel");

  // Periodic notification to check if tasks are ready
  engine::Notification *notification = new engine::Notification(notification_run_stream_tasks_if_necessary);
  engine::Engine::shared()->notify(notification, 1);
}

WorkerTaskManager::~WorkerTaskManager() {
  LOG_D(logs.cleanup, ("Entering ~WorkerTaskManager"));
  stream_operations_statistics_.clearMap();
  stream_operations_global_info_.clearMap();
  LOG_D(logs.cleanup, ("Finished ~WorkerTaskManager"));
}

size_t WorkerTaskManager::getNewId() {
  return ++id_;
}

void WorkerTaskManager::Add(au::SharedPointer<WorkerTaskBase> task) {
  // Insert in the list of queues
  pending_tasks_.Push(task);

  // Check if it is necessary to run a task
  reviewPendingWorkerTasks();
}

size_t WorkerTaskManager::AddBlockRequestTask(size_t block_id, const std::vector<size_t>& worker_ids) {
  // Add a new block request task...
  size_t task_id = getNewId();

  au::SharedPointer<WorkerTaskBase> task(new BlockRequestTask(samson_worker_, task_id, block_id, worker_ids));
  Add(task);
  return task_id;
}

void WorkerTaskManager::notify(engine::Notification *notification) {
  if (notification->isName(notification_process_request_response)) {
    // Get the identifier of this tasks that has finished
    size_t id = notification->environment().Get("system.task_id", 0);

    // Extract this task
    au::SharedPointer<WorkerTaskBase> task_base = running_tasks_.Extract(id);
    if (task_base == NULL) {
      LOG_W(logs.task_manager, ("Notification of an unknown finished system queue tasks (task = %lu ).Ignoring", id));
      return;
    }

    LOG_V(logs.task_manager, ("Finished task %lu (%s)", id, task_base->str().c_str()));

    // Process outputs generated by this task ( if it is a worker task )
    au::SharedPointer<WorkerTask> worker_task = task_base.dynamic_pointer_cast<WorkerTask> ();

    // Commit to data model
    if (worker_task != NULL) {
      worker_task->commit();
    }

    // Log for this task ( for ls_last_tasks command )
    WorkerTaskLog worker_task_log;
    worker_task_log.description = task_base->str();
    worker_task_log.waiting_time_seconds = task_base->waiting_time_seconds();
    worker_task_log.running_time_seconds = task_base->running_time_seconds();
    worker_task_log.inputs = task_base->str_inputs();
    worker_task_log.outputs = task_base->str_outputs();
    worker_task_log.times = task_base->GetActivitySummary();
    worker_task_log.process_time = task_base->GetProcessSummary();

    if (notification->environment().IsSet("error")) {
      std::string error = notification->environment().Get("error", "???");
      worker_task_log.result = error;
    } else {
      worker_task_log.result = "OK";
    }
    last_tasks_.push_front(worker_task_log);
    while (last_tasks_.size() > 100) {    // remove execise logs ( just last 100 )
      last_tasks_.pop_back();
    }

    // Take statistics about stream operations
    if (worker_task != NULL) {
      std::string stream_operation_name = worker_task->stream_operation_name();
      if (stream_operation_name[0] != '.') {
        FullKVInfo input_info = worker_task->GetInputDataInfo();
        FullKVInfo state_info = worker_task->GetStateDataInfo();

        double time = worker_task->GetProcessTime();
        int num_ranges = samson_worker_->worker_controller()->GetKVRanges().size();

        StreamOperationStatistics *statistics =  stream_operations_statistics_.findOrCreate(stream_operation_name);
        statistics->UpdateTaskInformation(num_ranges, input_info, state_info, time);
      }
    }

    // Mark the task as finished
    if (notification->environment().IsSet("error")) {
      std::string error = notification->environment().Get("error", "???");
      task_base->SetWorkerTaskFinishedWithError(error);
      if (!task_base->error().HasErrors()) {
        LM_X(1, ("Internal error"));
      }
    } else {
      task_base->SetWorkerTaskFinished();
    }
  }

  if (notification->isName(notification_run_stream_tasks_if_necessary)) {
    reviewPendingWorkerTasks();
  }
}

void WorkerTaskManager::reviewPendingWorkerTasks() {
  // It is not recommended to execute more tasks than cores
  // since memory content of all the operations is locked in memory

  int num_processors = au::Singleton<SamsonSetup>::shared()->GetInt("general.num_processess");
  int num_running_tasks = running_tasks_.size();

  int max_running_operations = static_cast<int> (num_processors);

  while (num_running_tasks < max_running_operations) {
    // LOG_SM(("Scheduling since running rask %d < %d", (int) num_running_tasks , (int) max_running_operations));

    bool runReturn = runNextWorkerTasksIfNecessary();

    if (!runReturn) {
      return;  // Nothing more to schedule
    }
  }
}

size_t WorkerTaskManager::get_num_running_tasks() const {
  return running_tasks_.size();
}

size_t WorkerTaskManager::get_num_tasks() const {
  size_t total = 0;

  total += running_tasks_.size();
  total += pending_tasks_.size();
  return total;
}

bool WorkerTaskManager::runNextWorkerTasksIfNecessary() {
  if (pending_tasks_.size() == 0) {
    return false;  // No more pending task to be executed
  }
  std::vector<au::SharedPointer<WorkerTaskBase> > tasks = pending_tasks_.items();

  for (size_t i = 0; i < tasks.size(); ++i) {
    au::SharedPointer<WorkerTaskBase> base_task = tasks[i];

    if (base_task->is_ready()) {
      // Extract the task from the queue of pending tasks
      pending_tasks_.ExtractAll(base_task);

      // For simple tasks ( like BlockRequestTask ) we execute everything here in the main engine thread
      if (base_task->simple_task()) {
        base_task->StartActivity();  // Init cronometer for this process item
        base_task->run();            // Run the process
        base_task->StopActivity();   // Stop cronometer for this process item

        return true;
      }

      // Insert in the running vector & Add the task to the process manager
      size_t task_id = base_task->worker_task_id();

      LOG_V(logs.task_manager, ("Scheduled task %lu (%s)", task_id, base_task->str().c_str()));

      running_tasks_.Set(task_id, base_task);
      base_task->SetTaskState("Scheduled");
      engine::Engine::process_manager()->Add(base_task.static_pointer_cast<engine::ProcessItem> (), engine_id());
    }
  }

  return false;
}

void WorkerTaskManager::Reset() {
  // Reset current model for stream operations ( default ranges have changed for instance )
  stream_operations_global_info_.clearMap();

  // Remove running task...
  std::vector<size_t> pending_tasks_ids = running_tasks_.getKeysVector();
  for (size_t i = 0; i < pending_tasks_ids.size(); ++i) {
    size_t task_id = pending_tasks_ids[i];
    au::SharedPointer<WorkerTaskBase> task = running_tasks_.Get(task_id);
    if (task->concept() != "block_distribution") {
      running_tasks_.Extract(task_id);
    }
  }

  // Remove pending task (except block_request )
  std::vector<au::SharedPointer<WorkerTaskBase> > pending_task = pending_tasks_.items();
  pending_tasks_.Clear();
  for (size_t i = 0; i < pending_task.size(); ++i) {
    if (pending_task[i]->concept() == "block_distribution") {
      pending_tasks_.Push(pending_task[i]);
    }
  }
}

gpb::CollectionPointer WorkerTaskManager::GetSOStatisticsCollection(const ::samson::Visualization& visualization) {
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name("Stream Operation statistics");
  au::map<std::string, StreamOperationStatistics >::iterator it;;
  for (it = stream_operations_statistics_.begin(); it != stream_operations_statistics_.end(); ++it) {
    gpb::CollectionRecord *record = collection->add_record();
    add(record, "Stream Operation", it->first, "different,left");
    it->second->fill(record, visualization);
  }
  return collection;
}

au::SharedPointer<gpb::Collection> WorkerTaskManager::GetLastTasksCollection(
  const ::samson::Visualization& visualization) {
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name("last_tasks");

  std::list<WorkerTaskLog>::iterator it;
  for (it = last_tasks_.begin(); it != last_tasks_.end(); ++it) {
    gpb::CollectionRecord *record = collection->add_record();

    add(record, "description", it->description, "different,left");
    add(record, "result", it->result, "left,different");

    if (visualization.get_flag("times")) {
      add(record, "times", it->times, "left,different");
    } else {
      add(record, "inputs", it->inputs, "left,different");
      add(record, "outputs", it->outputs, "left,different");
      add(record, "times", it->process_time, "left,different");
    }
  }

  return collection;
}

au::SharedPointer<gpb::Collection> WorkerTaskManager::GetCollection(const ::samson::Visualization& visualization) {
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name("ps_tasks");

  std::vector<au::SharedPointer<WorkerTaskBase> > running_tasks = running_tasks_.items();
  std::vector<au::SharedPointer<WorkerTaskBase> > pending_tasks = pending_tasks_.items();

  for (size_t i = 0; i < running_tasks.size(); ++i) {
    au::SharedPointer<WorkerTaskBase> task = running_tasks[i];
    if (name_match(visualization.pattern().c_str(), task->worker_task_id())) {
      task->fill(collection->add_record(), visualization);
    }
  }

  for (size_t i = 0; i < pending_tasks.size(); ++i) {
    au::SharedPointer<WorkerTaskBase> task = pending_tasks[i];
    if (name_match(visualization.pattern().c_str(), task->worker_task_id())) {
      task->fill(collection->add_record(), visualization);
    }
  }

  return collection;
}

bool compare_StreamOperationRangeInfo(StreamOperationRangeInfo *a, StreamOperationRangeInfo *b) {
  return (a->priority_rank() > b->priority_rank());
}

std::vector<KVRange> WorkerTaskManager::CompressKVRanges(std::vector<KVRange> input_ranges, int reduction_factor,
                                                         au::ErrorManager& error) {
  std::vector<KVRange> result;

  // Check coherence of input tranges
  for (size_t i = 0; i < (input_ranges.size() - 1); i++) {
    if (input_ranges[i].hg_end_  != input_ranges[i + 1].hg_begin_) {
      error.AddError("Non coherent ranges");
      return result;
    }
  }

  int pos = 0;
  int num_input_ranges = input_ranges.size();
  while (pos < num_input_ranges) {
    int begin = pos;
    int end = pos;
    while ((end < num_input_ranges) && ((end - begin + 1) < reduction_factor)) {
      end++;
    }

    // Create a unified KVRange
    result.push_back(KVRange(input_ranges[begin].hg_begin_, input_ranges[end].hg_end_));

    // move to the next range
    pos = end + 1;
  }
  return result;
}

void WorkerTaskManager::review_stream_operations() {
  au::ExecesiveTimeAlarm alarm("WorkerTaskManager::reviewStreamOperations");
  int num_processors = au::Singleton<SamsonSetup>::shared()->GetInt("general.num_processess");

  // If I have no information about ranges, do nothing...
  if (samson_worker_->worker_controller() == NULL) {
    Reset();
    return;  // Nothing to do here
  }

  // Set of stream operations-ranges revies in this loop ( remove non-reviewed elements )
  std::set<size_t> keys_stream_operation_ids;

  // Get a copy of data mode
  au::SharedPointer<gpb::DataModel> data_model = samson_worker_->data_model()->getCurrentModel();
  gpb::Data *data = data_model->mutable_current_data();

  // Review all stream operations
  for (int s = 0; s < data->operations_size(); ++s) {
    // Get reference to the stream operation
    const gpb::StreamOperation& stream_operation = data->operations(s);
    // Get the identifier of this stream operation
    size_t stream_operation_id = stream_operation.stream_operation_id();
    std::string stream_operation_name = stream_operation.name();
    std::string operation_name =  stream_operation.operation();

    // Recover glocal information for this stream operation
    StreamOperationGlobalInfo *global_info = stream_operations_global_info_.findInMap(stream_operation_id);
    if (!global_info) {
      // Full set of ranges
      std::vector<KVRange> worker_ranges = samson_worker_->worker_controller()->GetMyKVRanges();

      // Apply a reduction if operation is known & it is a forward operation
      Operation *operation = au::Singleton<ModulesManager>::shared()->GetOperation(operation_name);
      if (operation) {
        if (operation->IsForwardOperation()) {
          // Apply a reduction
          int reduction_factor = samson_worker_->data_model()->getCurrentModel()->parallelization_reduction_factor();
          au::ErrorManager error;
          std::vector<KVRange> new_worker_ranges = CompressKVRanges(worker_ranges, reduction_factor, error);
          if (!error.HasErrors()) {
            worker_ranges = new_worker_ranges;
          } else {
            LOG_W(logs.task_manager, ("Error compacting ranges: %s", error.GetLastError().c_str()));
          }
        }
      } else {
        LOG_W(logs.task_manager, ("Unknown operation  %s: not possible to compact if forward", operation_name.c_str()));
      }

      global_info = new StreamOperationGlobalInfo(samson_worker_, stream_operation_id, stream_operation_name,
                                                  worker_ranges);
      stream_operations_global_info_.insertInMap(stream_operation_id, global_info);
    }

    // Complete review of this stream operation ( defrag operations, divisions, compute priority rank,.... )
    global_info->Review(data);

    // Insert this key into the global set to remove non-used previously defined stream operations
    keys_stream_operation_ids.insert(stream_operation_id);
  }

  // Remove elements in the map not considered so far ( old elements after a reasignation )
  stream_operations_global_info_.removeInMapIfNotIncludedIn(keys_stream_operation_ids);

  // If enough tasks have been scheduled, do not schedule more.
  size_t num_tasks = get_num_tasks();
  if (num_tasks > (1.5 * num_processors)) {
    return;
  }

  // Check memory status. New tasks are not scheduled if memory usage is too high
  double memory_usage = engine::Engine::memory_manager()->memory_usage();
  if (memory_usage >= 1.0) {
    LOG_SW(("Not scheduling new stream-tasks since memory usage is %s >= 100%"
            , au::str_percentage(memory_usage).c_str()));
    return;
  }

  // Schedule only the maximum priority tasks ( in order ) while get_num_tasks() < (1.5 * num_processors)
  std::vector<StreamOperationRangeInfo *> stream_operation_range_info;
  au::map<size_t, StreamOperationGlobalInfo>::iterator it;
  for (it = stream_operations_global_info_.begin(); it != stream_operations_global_info_.end(); ++it) {
    // Get info for each range
    const au::vector<StreamOperationRangeInfo>& ranges = it->second->stream_operations_range_info();
    for (size_t i = 0; i < ranges.size(); ++i) {
      if (ranges[i]->priority_rank() > 0) {
        stream_operation_range_info.push_back(ranges[i]);
      }
    }
  }

  // Sort tasks by priority
  std::sort(stream_operation_range_info.begin(), stream_operation_range_info.end(), compare_StreamOperationRangeInfo);

  size_t pos = 0;
  while (get_num_tasks() < (1.5 * num_processors)) {
    // no more tasks to be scheduled
    if (pos >= stream_operation_range_info.size()) {
      break;
    }

    // Schedule a tasks in stream_operation_range_info[pos]
    StreamOperationRangeInfo *stream_operation = stream_operation_range_info[pos];
    au::SharedPointer<WorkerTask> queue_task = stream_operation->schedule_new_task(getNewId(), data);
    if (queue_task == NULL) {
      LOG_SW(("Worker task finally not scheduled for stream operation"));
    } else {
      Add(queue_task.static_pointer_cast<WorkerTaskBase> ());
    }
    ++pos;
  }
}

gpb::CollectionPointer WorkerTaskManager::GetCollectionForStreamOperationsRanges(
  const ::samson::Visualization& visualization) {
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name("stream operations ranges");

  au::SimplePattern pattern(visualization.pattern());

  au::map<size_t, StreamOperationGlobalInfo>::iterator iter;
  for (iter = stream_operations_global_info_.begin(); iter != stream_operations_global_info_.end(); ++iter) {
    if (!pattern.match(iter->second->stream_operation_name())) {
      continue;
    }

    const au::vector<StreamOperationRangeInfo>& ranges = iter->second->stream_operations_range_info();
    for (size_t i = 0; i < ranges.size(); ++i) {
      // Create a new record for this instance
      gpb::CollectionRecord *record = collection->add_record();

      // Common type to joint queries ls_workers -group type
      ranges[i]->fill(record, visualization);
    }
  }

  return collection;
}

gpb::CollectionPointer WorkerTaskManager::GetCollectionForStreamOperations(const ::samson::Visualization& visualization)
{
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name("stream operations");

  au::SimplePattern pattern(visualization.pattern());

  au::map<size_t, StreamOperationGlobalInfo>::iterator iter;
  for (iter = stream_operations_global_info_.begin(); iter != stream_operations_global_info_.end(); ++iter) {
    if (!pattern.match(iter->second->stream_operation_name())) {
      continue;
    }

    // Create a new record for this entry
    iter->second->fill(collection->add_record(), visualization);
  }

  return collection;
}

void WorkerTaskManager::Update(GlobalBlockSortInfo *info) const {
  // Review pending tasks
  {
    std::vector< au::SharedPointer<WorkerTaskBase> > items  = pending_tasks_.items();
    for (size_t i = 0; i < items.size(); i++) {
      size_t task_id =  items[i]->id();
      const std::vector<size_t>& block_ids = items[i]->input_block_ids();
      for (size_t j = 0; j < block_ids.size(); j++) {
        info->NotifyInputForTask(block_ids[j], task_id);
      }
    }
  }

  // Review runnning tasks
  {
    std::vector< au::SharedPointer<WorkerTaskBase> > items = running_tasks_.items();
    for (size_t i = 0; i < items.size(); i++) {
      size_t task_id =  items[i]->id();
      const std::vector<size_t>& block_ids = items[i]->input_block_ids();
      for (size_t j = 0; j < block_ids.size(); j++) {
        info->NotifyInputForTask(block_ids[j], task_id);
      }
    }
  }
}
}
}
