#include "samson/worker/WorkerCommandManager.h"  // Own interface

#include "engine/Engine.h"
#include "engine/Notification.h"

#include "samson/common/NotificationMessages.h"
#include "samson/worker/WorkerCommand.h"

namespace samson {
WorkerCommandManager::WorkerCommandManager(SamsonWorker *_samsonWorker) {
  // Keep pointer to the worker
  samsonWorker = _samsonWorker;

  // Init identifier for the command
  worker_task_id = 1;

  // Schedule a periodic notification ( every 5 seconds )
  listen(notification_review_worker_command_manager);
  {
    engine::Notification *notification = new engine::Notification(notification_review_worker_command_manager);
    engine::Engine::shared()->notify(notification, 5);
  }
}

void WorkerCommandManager::Add(WorkerCommand *workerCommand) {
  size_t id = worker_task_id++;
  workerCommands.insertInMap(id, workerCommand);

  // First run of this worker command
  workerCommand->Run();
}

// Cancel worker command
bool WorkerCommandManager::Cancel(std::string worker_command_id) {
  bool found = false;

  au::map<size_t, WorkerCommand>::iterator it_workerCommands;
  for (it_workerCommands = workerCommands.begin(); it_workerCommands != workerCommands.end(); it_workerCommands++) {
    std::string my_worker_command_id = it_workerCommands->second->worker_command_id_;

    if (my_worker_command_id == worker_command_id) {
      found = true;
      it_workerCommands->second->FinishWorkerTaskWithError("Canceled");
    }
  }
  return found;
}

void WorkerCommandManager::notify(engine::Notification *notification) {
  if (notification->isName(notification_review_worker_command_manager)) {
    // Remove finished worker tasks elements
    workerCommands.removeInMapIfFinished();

    // Review all WorkerCommand is necessary
    au::map<size_t, WorkerCommand>::iterator it_workerCommands;
    for (it_workerCommands = workerCommands.begin(); it_workerCommands != workerCommands.end(); it_workerCommands++) {
      it_workerCommands->second->Run();   // Execute if necessary
    }
    return;
  }
}

au::SharedPointer<gpb::Collection> WorkerCommandManager::GetCollectionOfWorkerCommands(
                                                                                       const Visualization& visualization) {
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name("worker_commands");

  au::map<size_t, WorkerCommand>::iterator it;
  for (it = workerCommands.begin(); it != workerCommands.end(); it++) {
    std::string command = it->second->command_;
    if (match(visualization.pattern(), command)) {
      it->second->fill(collection->add_record(), visualization);
    }
  }
  return collection;
}
}
