#include "samson/worker/WorkerCommandManager.h"  // Own interface

#include "engine/Engine.h"
#include "engine/Notification.h"

#include "samson/common/NotificationMessages.h"
#include "samson/worker/WorkerCommand.h"

namespace samson {
WorkerCommandManager::WorkerCommandManager(SamsonWorker *samson_worker) {
  // Keep pointer to the worker
  samson_worker_ = samson_worker;

  // Init identifier for the command
  worker_task_id_ = 1;

  // Schedule a periodic notification ( every 5 seconds )
  listen(notification_review_worker_command_manager);
  {
    engine::Notification *notification = new engine::Notification(notification_review_worker_command_manager);
    engine::Engine::shared()->notify(notification, 5);
  }
}

void WorkerCommandManager::Add(WorkerCommand *worker_command) {
  size_t id = worker_task_id_++;
  worker_commands_.insertInMap(id, worker_command);

  AU_M(logs.worker_command_manager, ("Adding command %s" , worker_command->str().c_str() ));
  
  // First run of this worker command
  worker_command->Run();
}

// Cancel worker command
bool WorkerCommandManager::Cancel(std::string worker_command_id) {

  AU_M(logs.worker_command_manager, ("Canceling %s", worker_command_id.c_str() ));
  
  bool found = false;
  au::map<size_t, WorkerCommand>::iterator it_workerCommands;
  for (it_workerCommands = worker_commands_.begin(); it_workerCommands != worker_commands_.end(); it_workerCommands++) {
    std::string my_worker_command_id = it_workerCommands->second->worker_command_id_;
    if (my_worker_command_id == worker_command_id) {
      found = true;
      it_workerCommands->second->FinishWorkerTaskWithError("Canceled");
    }
  }
  
  if(!found)
    AU_M(logs.worker_command_manager, ("Canceling %s... not found", worker_command_id.c_str() ));
  
  return found;
}

void WorkerCommandManager::notify(engine::Notification *notification) {
  if (notification->isName(notification_review_worker_command_manager)) {
    
    AU_M(logs.worker_command_manager, ("Review worker command manager finished task...."));
    
    // Remove finished worker tasks elements
    worker_commands_.removeInMapIfFinished();

    // Review all WorkerCommand is necessary
    au::map<size_t, WorkerCommand>::iterator it_workerCommands;
    for (it_workerCommands = worker_commands_.begin(); it_workerCommands != worker_commands_.end(); it_workerCommands++) {
      it_workerCommands->second->Run();   // Execute if necessary
    }
    return;
  }
}

au::SharedPointer<gpb::Collection> WorkerCommandManager::GetCollection(const Visualization& visualization) {
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name("worker_commands");

  au::map<size_t, WorkerCommand>::iterator it;
  for (it = worker_commands_.begin(); it != worker_commands_.end(); it++) {
    std::string command = it->second->command_;
    if ( visualization.match(command)) {
      it->second->fill(collection->add_record(), visualization);
    }
  }
  return collection;
}
}
