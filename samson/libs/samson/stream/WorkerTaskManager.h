
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

struct IdRange {
  size_t id;
  KVRange range;

  IdRange(size_t _id, const KVRange& _range) {
    id = _id;
    range = _range;
  }

  inline bool operator <(const IdRange &id_range) const {
    if (id < id_range.id) {
      return true;
    }





    if (id > id_range.id) {
      return false;
    }





    return ( range.hg_begin < id_range.range.hg_begin );
  }
};


// Logs for debugging

struct WorkerTaskLog {
  std::string task;
  std::string result;

  WorkerTaskLog(const std::string& _task, const std::string& _result) {
    task = _task;
    result = _result;
  }
};


class WorkerTaskManager : public ::engine::NotificationListener {
  size_t id_;                                                // Id of the current task

  au::Queue< WorkerTaskBase > pending_tasks_;                // List of pending task to be executed
  au::Dictionary< size_t, WorkerTaskBase > running_tasks_;       // Map of running tasks

  // Information about execution of current stream operations
  au::map<IdRange, StreamOperationInfo > stream_operations_info_;

  // Pointer to samson worker
  SamsonWorker *samson_worker_;

  // Ranges I should process
  std::vector<KVRange> ranges_;

  // Log of last tasks...
  std::list<WorkerTaskLog> last_tasks_;

public:

  WorkerTaskManager(SamsonWorker *samson_worker);

  size_t getNewId();

  void Add(au::SharedPointer<WorkerTaskBase> task);

  void add_block_request_task(size_t block_id, size_t worker_id);

  // Review schedules tasks
  void reviewPendingWorkerTasks();
  bool runNextWorkerTasksIfNecessary();

  // Review stream operations to schedule new stuff
  void review_stream_operations();

  // Notifications
  void notify(engine::Notification *notification);

  // Reset all the content of this manager
  void reset();

  // Update ranges ( this removes all current running operations )
  void update_ranges(const KVRanges& ranges);

  // Get a collection for monitoring
  samson::gpb::Collection *getCollection(const ::samson::Visualization& visualization);
  samson::gpb::Collection *getLastTasksCollection(const ::samson::Visualization& visualization);


  size_t get_num_running_tasks();
  size_t get_num_tasks();

  // Get collection to list in delilah
  gpb::Collection *getCollectionForStreamOperationsInfo(const ::samson::Visualization& visualization);
};
}
}

#endif  // ifndef _H_SAMSON_QUEUE_TASK_MANAGER
