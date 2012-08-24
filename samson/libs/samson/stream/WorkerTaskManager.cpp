

#include "WorkerTask.h"                         // samson::stream::WorkerTask
#include "WorkerTaskManager.h"   // Own interface
#include <fnmatch.h>
#include <sstream>                              // std::ostringstream
#include <vector>

#include "au/ExecesiveTimeAlarm.h"

#include "engine/Notification.h"                // engine::Notification
#include "engine/ProcessManager.h"      // engine::ProcessManager

#include "samson/common/NotificationMessages.h"  // notification_process_request
#include "samson/common/SamsonSetup.h"          // SamsonSetup
#include "samson/module/ModulesManager.h"

#include "samson/stream/StreamOperationInfo.h"

#include "samson/worker/SamsonWorker.h"

#include "BlockManager.h"
#include "WorkerSystemTask.h"        // WorkerSystemTask

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

size_t WorkerTaskManager::getNewId() {
  return id_++;
}

void WorkerTaskManager::Add(au::SharedPointer<WorkerTaskBase> task) {
  // Insert in the list of queues
  pending_tasks_.Push(task);

  // Check if it is necessary to run a task
  reviewPendingWorkerTasks();
}

void WorkerTaskManager::add_block_request_task(size_t block_id, size_t worker_id) {
  // Add a new block request task...
  au::SharedPointer<WorkerSystemTask> task(new BlockRequestTask(samson_worker_, getNewId(), block_id, worker_id));
  Add(task.static_pointer_cast<WorkerTaskBase>());
}

void WorkerTaskManager::notify(engine::Notification *notification) {
  if (notification->isName(notification_process_request_response)) {
    // Get the identifier of this tasks that has finished
    size_t id = notification->environment().Get("system.task_id", 0);

    // Extract this task
    au::SharedPointer<WorkerTaskBase> task_base = running_tasks_.Extract(id);


    if (task_base == NULL) {
      LM_W(("Notification of an unknown finished system queue tasks (task = %lu ).Ignoring", id));
      return;
    }

    // Common log for tasks
    if (notification->environment().IsSet("error")) {
      std::string error = notification->environment().Get("error", "???");
      last_tasks_.push_back(WorkerTaskLog(task_base->str(), error));
    } else {
      last_tasks_.push_back(WorkerTaskLog(task_base->str(), "OK"));
    }

    // remove execise logs ( just last 100 )
    while (last_tasks_.size() > 100) {
      last_tasks_.pop_front();
    }


    // One of this is correct
    au::SharedPointer<WorkerSystemTask> system_task = task_base.dynamic_pointer_cast<WorkerSystemTask>();
    au::SharedPointer<WorkerTask> task = task_base.dynamic_pointer_cast<WorkerTask>();

    LM_T(LmtBlockManager, ("Notification of a finish at WorkerTaskManager, task %lu ", id));

    if (system_task != NULL) {
      // Nothing spetial to do with system tasks
    } else {
      // Mark the task as finished
      if (notification->environment().IsSet("error")) {
        std::string error = notification->environment().Get("error", "???");
        task->set_finished_with_error(error);
      } else {
        task->set_finished();
      }
    }
  }

  if (notification->isName(notification_run_stream_tasks_if_necessary)) {
    reviewPendingWorkerTasks();
  }
}

void WorkerTaskManager::reviewPendingWorkerTasks() {
  // It is not recomended to execute more tasks than cores
  // since memory content of all the operations is locked in memory

  int num_processors      = au::Singleton<SamsonSetup>::shared()->getInt("general.num_processess");
  int num_running_tasks   = running_tasks_.size();

  int max_running_operations =  (int)( num_processors );

  while (num_running_tasks < max_running_operations) {
    // LM_M(("Scheduling since running rask %d < %d", (int) num_running_tasks , (int) max_running_operations));

    bool runReturn = runNextWorkerTasksIfNecessary();

    if (!runReturn) {
      return;     // Nothing more to schedule
    }
  }
}

size_t WorkerTaskManager::get_num_running_tasks() {
  return running_tasks_.size();
}

size_t WorkerTaskManager::get_num_tasks() {
  size_t total = 0;

  total += running_tasks_.size();
  total += pending_tasks_.size();
  return total;
}

bool WorkerTaskManager::runNextWorkerTasksIfNecessary() {
  if (pending_tasks_.size() == 0) {
    return false;     // No more pending task to be executed
  }
  std::vector< au::SharedPointer< WorkerTaskBase > > tasks = pending_tasks_.items();

  for (size_t i = 0; i < tasks.size(); i++) {
    au::SharedPointer<WorkerTaskBase> base_task = tasks[i];

    if (base_task->is_ready()) {
      // Extract the task from the queue of pending tasks
      pending_tasks_.ExtractAll(base_task);

      // Insert in the running vector
      size_t task_id = base_task->get_id();

      running_tasks_.Set(task_id, base_task);

      base_task->SetTaskState("Scheduled");

      LM_T(LmtBlockManager, ("Scheduled task %lu ", task_id));

      // Add this process item ( note that a notification will be used to notify when finished )
      au::SharedPointer<WorkerTask> task = base_task.dynamic_pointer_cast<WorkerTask>();
      au::SharedPointer<WorkerSystemTask> system_task = base_task.dynamic_pointer_cast<WorkerSystemTask>();

      if (task != NULL) {
        engine::Engine::process_manager()->Add(task.static_pointer_cast<engine::ProcessItem>(), engine_id());
      } else if (system_task != NULL) {
        engine::Engine::process_manager()->Add(system_task.static_pointer_cast<engine::ProcessItem>(), engine_id());
      } else {
        LM_X(1, ("WorkerTaskBase cannot be converted to WorkerTask or WorkerSystemTask"));
      } return true;
    }
  }

  return false;
}

void WorkerTaskManager::reset() {
  stream_operations_info_.clearMap();
}

samson::gpb::Collection *WorkerTaskManager::getLastTasksCollection(const ::samson::Visualization& visualization) {
  samson::gpb::Collection *collection = new samson::gpb::Collection();

  collection->set_name("last_tasks");

  std::list<WorkerTaskLog>::iterator it;
  for (it = last_tasks_.begin(); it != last_tasks_.end(); it++) {
    gpb::CollectionRecord *record = collection->add_record();

    add(record, "tasks", it->task, "different");
    add(record, "result", it->result, "left,different");
  }

  return collection;
}

samson::gpb::Collection *WorkerTaskManager::getCollection(const ::samson::Visualization& visualization) {
  samson::gpb::Collection *collection = new samson::gpb::Collection();

  collection->set_name("ps_tasks");

  std::vector< au::SharedPointer<WorkerTaskBase> > running_tasks = running_tasks_.items();
  std::vector< au::SharedPointer<WorkerTaskBase> > pending_tasks = pending_tasks_.items();

  for (size_t i = 0; i < running_tasks.size(); i++) {
    au::SharedPointer<WorkerTaskBase> task = running_tasks[i];
    if (name_match(visualization.pattern().c_str(), task->get_id())) {
      task->fill(collection->add_record(), visualization);
    }
  }

  for (size_t i = 0; i < pending_tasks.size(); i++) {
    au::SharedPointer<WorkerTaskBase> task = pending_tasks[i];
    if (name_match(visualization.pattern().c_str(), task->get_id())) {
      task->fill(collection->add_record(), visualization);
    }
  }

  return collection;
}

void WorkerTaskManager::review_stream_operations() {
  au::ExecesiveTimeAlarm alarm("WorkerTaskManager::reviewStreamOperations");
  int num_processors = au::Singleton<SamsonSetup>::shared()->getInt("general.num_processess");

  // Get a copy of data mode
  au::SharedPointer<gpb::Data> data = samson_worker_->data_model()->getCurrentModel();

  while (true) {
    // Check if enougth tasks have been scheduled
    size_t num_tasks = get_num_tasks();
    if (num_tasks > ( 1.5 * num_processors)) {
      break;  // Check memory status. New tasks are not scheduled if memory usage is too high
    }
    double memory_usage = engine::Engine::memory_manager()->memory_usage();
    if (memory_usage >= 1.0) {
      LM_W(("Not schedulling new stream-tasks since memory usage is %s >= 100%"
            , au::str_percentage(memory_usage).c_str()));
      break;
    }

    // Find the most urgent stream operation to be processed
    size_t max_priority_rank = 0;
    StreamOperationInfo *max_priority_stream_operation_info = NULL;

    // Review all stream operations
    for (int s = 0; s < data->operations_size(); s++) {
      // Get reference to the stream operation
      const gpb::StreamOperation& stream_operation = data->operations(s);
      // Get the identifier of this stream operation
      size_t stream_operation_id = stream_operation.stream_operation_id();

      for (size_t r = 0; r < ranges_.size(); r++) {
        // Get the range we are considering
        KVRange range = ranges_[r];

        // Index in the map of StreamOperationInfo elements
        IdRange id_range = IdRange(stream_operation_id, range);

        // Recover information for this stream operation
        StreamOperationInfo *stream_operation_info = stream_operations_info_.findInMap(id_range);
        if (!stream_operation_info) {
          stream_operation_info = new StreamOperationInfo(samson_worker_
                                                          , stream_operation_id
                                                          , range
                                                          , stream_operation);

          stream_operations_info_.insertInMap(id_range, stream_operation_info);
        }

        // Review tasks to be executed
        stream_operation_info->review(data.shared_object());

        // Compute the priority parameter based on data accumulated here
        size_t priority_rank = stream_operation_info->priority_rank();

        // Keep reference to the maximum priority
        if (( priority_rank > 0 ) && (priority_rank > max_priority_rank )) {
          max_priority_rank = priority_rank;
          max_priority_stream_operation_info = stream_operation_info;
        }
      }
    }

    if (max_priority_rank == 0) {
      LM_T(LmtEngineTime, ("No more operations"));
      break;     // No more operations to be schedule
    }

    // Schedule next task
    if (max_priority_stream_operation_info) {
      au::SharedPointer<WorkerTask>  queue_task =  max_priority_stream_operation_info->schedule_new_task(id_++);
      if (queue_task == NULL) {
        LM_X(1, ("Internal error"));
      }
      Add(queue_task.static_pointer_cast<WorkerTaskBase>());
    }
  }
}

gpb::Collection *WorkerTaskManager::getCollectionForStreamOperationsInfo(const ::samson::Visualization& visualization)
{
  // Template-based creation of collection based on map
  return getCollectionForMap("stream operations"
                             , stream_operations_info_
                             , visualization);
}

// Update ranges ( this removes all current running operations )
void WorkerTaskManager::update_ranges(const KVRanges& ranges) {
  // Replace currect ranges
  ranges_ = ranges;

  // Reset all the operations
  reset();
}
}
}
