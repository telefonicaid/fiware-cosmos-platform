

#include <sys/time.h>
#include <time.h>

#include "logMsg/logMsg.h"        // LM_X

#include "au/Singleton.h"
#include "au/ThreadManager.h"
#include "au/containers/SharedPointer.h"
#include "au/mutex/TokenTaker.h"  // au::TokenTaker...
#include "au/xml.h"               // au::xml...

#include "au/Descriptors.h"       // au::Descriptors
#include "engine/DiskOperation.h"  // engine::DiskOperation
#include "engine/Engine.h"        // engine::Engine
#include "engine/EngineElement.h"  // engine::EngineElement
#include "engine/Notification.h"  // engine::Notification
#include "engine/NotificationElement.h"      // engine::EngineNotificationElement
#include "engine/ProcessItem.h"   // engine::ProcessItem

#include "engine/DiskManager.h"   // Own interface


namespace engine {
#pragma mark DiskManager

void *run_disk_manager_worker(void *p) {
  DiskManager *dm = (DiskManager *)p;

  dm->run_worker();
  return NULL;
}

DiskManager::DiskManager(int max_num_disk_operations) : token("engine::DiskManager") {
  // Number of parallel disk operations
  max_num_disk_operations_ = max_num_disk_operations;

  // Init counter for number of workers
  num_disk_manager_workers_ = 0;

  // Flag to indicate to backgroudn threads to finish
  quitting = false;

  // Create as many threads as required
  createThreads();
}

void DiskManager::createThreads() {
  au::TokenTaker tt(&token);      // Mutex protection


  // Create as many workers as necessary
  while (num_disk_manager_workers_ < max_num_disk_operations_) {
    LM_T(LmtEngineDiskManager, ("create threads %d/%d", num_disk_manager_workers_, max_num_disk_operations_ ));

    num_disk_manager_workers_++;

    pthread_t t;
    int rc =
      au::Singleton<au::ThreadManager>::shared()->addThread("DiskManager_worker", &t, NULL, run_disk_manager_worker,
                                                            this);
    if (rc) {
      LM_W(("Error creating background thread for disk operations..."));
      num_disk_manager_workers_--;
    }
  }

  LM_T(LmtEngineDiskManager, ("created %d threads", num_disk_manager_workers_ ));
}

DiskManager::~DiskManager() {
  Stop();
}

void DiskManager::Stop() {
  quitting = true;

  // Wait all background workers
  au::Cronometer cronometer;
  while (true) {
    if (num_disk_manager_workers_ == 0) {
      return;
    }

    usleep(100000);
    if (cronometer.seconds() > 1) {
      cronometer.Reset();
      LM_W(("Waiting for background threads of engine::DiskManager"));
    }
  }
}

void DiskManager::Add(const au::SharedPointer< ::engine::DiskOperation >& operation) {
  // Mutex protection
  au::TokenTaker tt(&token);

  // Set the pointer to myself
  operation->diskManager = this;

  // Insert the operation in the queue of pending operations
  pending_operations.Push(operation);
}

void DiskManager::Cancel(const au::SharedPointer<engine::DiskOperation>& operation) {
  au::TokenTaker tt(&token);

  // If it is still in the pending queue
  if (pending_operations.Contains(operation)) {
    // Operation is still retained at least by the argument provided
    pending_operations.ExtractAll(operation);

    // Add a notification for this operation ( removed when delegate is notified )
    Notification *notification = new Notification(notification_disk_operation_request_response);

    au::SharedPointer<NotificationObject> notification_object;
    notification_object = operation.static_pointer_cast<NotificationObject>();
    // notification_object = operation;

    notification->dictionary().Set("disk_operation",  notification_object);
    notification->AddEngineListeners(operation->listeners);
    notification->environment().Add(operation->environment);
    // Recover the environment variables to identify this request
    Engine::shared()->notify(notification);
  }
}

void DiskManager::finishDiskOperation(const au::SharedPointer< ::engine::DiskOperation >& operation) {
  // Callback received from background process

  // Mutex protection
  au::TokenTaker tt(&token);

  // remove from the box of running operations
  running_operations.Erase(operation);

  if (operation->getType() == DiskOperation::read) {
    rate_in.Push(operation->getSize());
  }
  if (operation->getType() == DiskOperation::write) {
    rate_out.Push(operation->getSize());
  }
  if (operation->getType() == DiskOperation::append) {
    rate_out.Push(operation->getSize());
  }
  LM_T(LmtDisk,
       (
         "DiskManager::finishDiskOperation erased and ready to send notification on file:%s",
         operation->fileName.c_str()
       ));

  // Add a notification for this operation to the required target listener
  Notification *notification = new Notification(notification_disk_operation_request_response);

  au::SharedPointer<NotificationObject> notification_object;
  notification_object = operation.static_pointer_cast<NotificationObject>();

  notification->dictionary().Set("disk_operation", notification_object);
  notification->AddEngineListeners(operation->listeners);
  notification->environment().Add(operation->environment);              // Recover the environment variables to identify this request

  LM_T(LmtDisk,
       ("DiskManager::finishDiskOperation notification sent on file:%s and ready to share and checkDiskOperations",
        operation->fileName.c_str()));
  Engine::shared()->notify(notification);
}

au::SharedPointer< ::engine::DiskOperation > DiskManager::getNextDiskOperation() {
  // Mutex protection
  au::TokenTaker tt(&token);

  if (pending_operations.size() == 0) {
    on_off_monitor.Set(!(running_operations.size() == 0));
    return au::SharedPointer< ::engine::DiskOperation >(NULL);
  }

  // Extract the next operation
  au::SharedPointer< ::engine::DiskOperation > operation = pending_operations.Pop();

  // Insert in the running list
  running_operations.Insert(operation);

  // We are runnin one, so it is on
  on_off_monitor.Set(true);

  return operation;
}

int DiskManager::num_disk_manager_workers() {
  return num_disk_manager_workers_;
}

// Check if we can run more disk operations
void DiskManager::run_worker() {
  LM_T(LmtEngineDiskManager, ("Running worker...", num_disk_manager_workers_ ));

  while (true) {
    // If quitting or too many workers... just quit.
    if (quitting || (num_disk_manager_workers_ > max_num_disk_operations_)) {
      LM_T(LmtEngineDiskManager, ("Quitting worker...", num_disk_manager_workers_ ));
      au::TokenTaker tt(&token);
      num_disk_manager_workers_--;
      return;
    }

    au::SharedPointer< ::engine::DiskOperation > operation = getNextDiskOperation();
    if (operation != NULL) {
      operation->run();
      finishDiskOperation(operation);        // Process finish of this task
    } else {
      usleep(100000);
    }
  }
}

int DiskManager::getNumOperations() {
  // Mutex protection
  au::TokenTaker tt(&token);

  return pending_operations.size() + running_operations.size();
}

void DiskManager::setNumOperations(int max_num_disk_operations) {
  // Mutex protection
  au::TokenTaker tt(&token);

  // Set a new limit
  max_num_disk_operations_ = max_num_disk_operations;
  // Create as many threads as required
  createThreads();
}

size_t DiskManager::get_rate_in() {
  // Mutex protection
  au::TokenTaker tt(&token);

  return rate_in.rate();
}

size_t DiskManager::get_rate_out() {
  // Mutex protection
  au::TokenTaker tt(&token);

  return rate_out.rate();
}

double DiskManager::get_rate_operations_in() {
  // Mutex protection
  au::TokenTaker tt(&token);

  return rate_in.hit_rate();
}

double DiskManager::get_rate_operations_out() {
  // Mutex protection
  au::TokenTaker tt(&token);

  return rate_out.hit_rate();
}

double DiskManager::get_on_off_activity() {
  // Mutex protection
  au::TokenTaker tt(&token);

  return on_off_monitor.activity_percentadge();
}

int DiskManager::max_num_disk_operations() {
  return max_num_disk_operations_;
}
}
