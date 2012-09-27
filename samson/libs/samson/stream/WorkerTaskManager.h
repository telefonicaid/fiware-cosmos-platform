
#ifndef _H_SAMSON_QUEUE_TASK_MANAGER
#define _H_SAMSON_QUEUE_TASK_MANAGER

#include <string>           // std::string

#include "au/containers/Dictionary.h"
#include "au/containers/Queue.h"

#include "engine/NotificationListener.h"  // engine::NotificationListener

#include "samson/module/ModulesManager.h"
#include "samson/module/Operation.h"

#include "samson/common/Visualitzation.h"
#include "samson/common/samson.pb.h"
#include "samson/common/status.h"

#include "samson/stream/WorkerSystemTask.h"
#include "samson/stream/WorkerTask.h"

namespace samson {
class SamsonWorker;

namespace stream {
class StreamOperationInfo;

// Logs for debugging

  struct WorkerTaskLog {
  std::string task;         // General description
  std::string result;       // Result of the operation
  std::string operation;    // Name of the operation
  std::string inputs;       // Information at the input of the operation
  std::string outputs;      // Information at output of the operation
  int waiting_time_seconds; // Waiting time until execution starts
  int running_time_seconds; // Running time
    
};


class WorkerTaskManager : public ::engine::NotificationListener {
public:

  WorkerTaskManager(SamsonWorker *samson_worker);

  void Add(au::SharedPointer<WorkerTaskBase> task);  // Add new task to the manager

  size_t getNewId();  // Get identifier for a new task

  // Add a system task to distribute a particular block to a set of workers
  void AddBlockDistributionTask(size_t block_id, const std::vector<size_t>& worker_ids);

  // Review schedules tasks
  void reviewPendingWorkerTasks();
  bool runNextWorkerTasksIfNecessary();

  // Review stream operations to schedule new stuff
  void review_stream_operations();

  // Notifications
  void notify(engine::Notification *notification);

  // Reset all the content of this manager
  void Reset();

  // Get a collection for monitoring
  samson::gpb::Collection *getCollection(const ::samson::Visualization& visualization);
  samson::gpb::Collection *getLastTasksCollection(const ::samson::Visualization& visualization);


  size_t get_num_running_tasks();
  size_t get_num_tasks();

  // Get collection to list in delilah
  gpb::Collection *getCollectionForStreamOperationsInfo(const ::samson::Visualization& visualization);

private:

  size_t id_;                                                      // Id of the current task
  au::Queue< WorkerTaskBase > pending_tasks_;                      // List of pending task to be executed
  au::Dictionary< size_t, WorkerTaskBase > running_tasks_;         // Map of running tasks

  // Information about execution of current stream operations
  au::map<std::string, StreamOperationInfo > stream_operations_info_;

  // Pointer to samson worker
  SamsonWorker *samson_worker_;

  // Log of last tasks...
  std::list<WorkerTaskLog> last_tasks_;
};
}
}

#endif  // ifndef _H_SAMSON_QUEUE_TASK_MANAGER
