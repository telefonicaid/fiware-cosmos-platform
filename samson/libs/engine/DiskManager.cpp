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

#include <sys/time.h>
#include <time.h>

#include "au/ThreadManager.h"
#include "au/containers/SharedPointer.h"
#include "au/mutex/TokenTaker.h"  // au::TokenTaker...
#include "au/singleton/Singleton.h"
#include "au/string/Descriptors.h"       // au::Descriptors
#include "au/string/xml.h"        // au::xml...

#include "engine/DiskManager.h"   // Own interface
#include "engine/DiskOperation.h"  // engine::DiskOperation
#include "engine/Engine.h"        // engine::Engine
#include "engine/EngineElement.h"  // engine::EngineElement
#include "engine/Logs.h"
#include "engine/Notification.h"  // engine::Notification
#include "engine/NotificationElement.h"      // engine::EngineNotificationElement
#include "engine/ProcessItem.h"   // engine::ProcessItem

#include "logMsg/logMsg.h"        // LM_X

namespace engine {
#pragma mark DiskManager

void *run_disk_manager_worker(void *p) {
  DiskManager *dm = reinterpret_cast<DiskManager *>(p);

  dm->run_worker();
  return NULL;
}

DiskManager::DiskManager(int max_num_disk_operations) :
  token_("engine::DiskManager"), quitting_(false),
  max_num_disk_operations_(max_num_disk_operations), num_disk_manager_workers_(0) {
  // Create as many threads as required
  CreateThreads();
}

void DiskManager::CreateThreads() {
  au::TokenTaker tt(&token_);   // Mutex protection


  // Create as many workers as necessary
  while (num_disk_manager_workers_ < max_num_disk_operations_) {
    LOG_D(logs.disk_manager, ("create threads %d/%d", num_disk_manager_workers_, max_num_disk_operations_));

    num_disk_manager_workers_++;

    pthread_t t;
    int rc = au::Singleton<au::ThreadManager>::shared()->AddThread("DiskManager_worker", &t, NULL,
                                                                   run_disk_manager_worker, this);
    if (rc) {
      LOG_SW(("Error creating background thread for disk operations..."));
      num_disk_manager_workers_--;
    }
  }

  LOG_D(logs.disk_manager, ("created %d threads", num_disk_manager_workers_));
}

DiskManager::~DiskManager() {
  Stop();
}

void DiskManager::Stop() {
  // Flag to indicate background threads to finish
  quitting_ = true;

  // Wait all background workers
  au::Cronometer cronometer;
  while (true) {
    if (num_disk_manager_workers_ == 0) {
      return;
    }

    usleep(100000);
    if (cronometer.seconds() > 1) {
      cronometer.Reset();
      LOG_SW(("Waiting for background threads of engine::DiskManager"));
    }
  }
}

void DiskManager::Add(au::SharedPointer<engine::DiskOperation> operation) {
  // Mutex protection
  au::TokenTaker tt(&token_);

  // Set the pointer to myself
  operation->diskManager = this;

  // Insert the operation in the queue of pending operations
  pending_operations_.Push(operation);
}

void DiskManager::Cancel(au::SharedPointer<engine::DiskOperation> operation) {
  au::TokenTaker tt(&token_);

  // If it is still in the pending queue
  if (pending_operations_.Contains(operation)) {
    // Operation is still retained at least by the argument provided
    pending_operations_.ExtractAll(operation);

    // Add a notification for this operation ( removed when delegate is notified )
    Notification *notification = new Notification(notification_disk_operation_request_response);

    // Add disk operation to the notification dictionary
    notification->dictionary().Set<DiskOperation> ("disk_operation", operation);

    notification->AddEngineListeners(operation->listeners);
    notification->environment().Add(operation->environment);
    // Recover the environment variables to identify this request
    Engine::shared()->notify(notification);
  }
}

void DiskManager::FinishDiskOperation(au::SharedPointer<engine::DiskOperation> operation) {
  // Callback received from background process

  // Mutex protection
  au::TokenTaker tt(&token_);

  // remove from the box of running operations
  running_operations_.Erase(operation);

  if (operation->getType() == DiskOperation::read) {
    rate_in_.Push(operation->getSize());
  }
  if (operation->getType() == DiskOperation::write) {
    rate_out_.Push(operation->getSize());
  }
  if (operation->getType() == DiskOperation::append) {
    rate_out_.Push(operation->getSize());
  }
  LOG_D(logs.disk_manager,
        ("DiskManager::finishDiskOperation erased and ready to send notification on file:%s",
         operation->fileName.c_str()));

  // Add a notification for this operation to the required target listener
  Notification *notification = new Notification(notification_disk_operation_request_response);

  notification->dictionary().Set<DiskOperation> ("disk_operation", operation);
  notification->AddEngineListeners(operation->listeners);
  notification->environment().Add(operation->environment);   // Recover the environment variables to identify this request

  LOG_D(logs.disk_manager,
        ("DiskManager::finishDiskOperation notification sent on file:%s and ready to share and checkDiskOperations",
         operation->fileName.c_str()));
  Engine::shared()->notify(notification);
}

au::SharedPointer<engine::DiskOperation> DiskManager::getNextDiskOperation() {
  // Mutex protection
  au::TokenTaker tt(&token_);

  if (pending_operations_.size() == 0) {
    on_off_monitor_.Set(!(running_operations_.size() == 0));
    return au::SharedPointer<engine::DiskOperation>(NULL);
  }

  // Extract the next operation
  au::SharedPointer<engine::DiskOperation> operation = pending_operations_.Pop();

  // Insert in the running list
  running_operations_.Insert(operation);

  // We are runnin one, so it is on
  on_off_monitor_.Set(true);

  return operation;
}

int DiskManager::num_disk_manager_workers() const {
  return num_disk_manager_workers_;
}

// Check if we can run more disk operations
void DiskManager::run_worker() {
  LOG_D(logs.disk_manager, ("Running worker...", num_disk_manager_workers_));

  while (true) {
    // If quitting or too many workers... just quit.
    if (quitting_ || (num_disk_manager_workers_ > max_num_disk_operations_)) {
      LOG_D(logs.disk_manager, ("Quitting worker...", num_disk_manager_workers_));
      au::TokenTaker tt(&token_);
      num_disk_manager_workers_--;
      return;
    }

    au::SharedPointer<engine::DiskOperation> operation = getNextDiskOperation();
    if (operation != NULL) {
      operation->run();
      FinishDiskOperation(operation);   // Process finish of this task
    } else {
      usleep(100000);
    }
  }
}

int DiskManager::num_disk_operations() const {
  // Mutex protection
  au::TokenTaker tt(&token_);

  return pending_operations_.size() + running_operations_.size();
}

void DiskManager::set_max_num_disk_operations(int max_num_disk_operations) {
  // Mutex protection
  au::TokenTaker tt(&token_);

  // Set a new limit
  max_num_disk_operations_ = max_num_disk_operations;
  // Create as many threads as required
  CreateThreads();
}

size_t DiskManager::rate_in() const {
  // Mutex protection
  au::TokenTaker tt(&token_);

  return rate_in_.rate();
}

size_t DiskManager::rate_out() const {
  // Mutex protection
  au::TokenTaker tt(&token_);

  return rate_out_.rate();
}

double DiskManager::rate_operations_in() const {
  // Mutex protection
  au::TokenTaker tt(&token_);

  return rate_in_.hit_rate();
}

double DiskManager::rate_operations_out() const {
  // Mutex protection
  au::TokenTaker tt(&token_);

  return rate_out_.hit_rate();
}

double DiskManager::on_off_activity() const {
  // Mutex protection
  au::TokenTaker tt(&token_);

  return on_off_monitor_.activity_percentadge();
}

double DiskManager::on_time() const {
  return on_off_monitor_.on_time();
}

double DiskManager::off_time() const {
  return on_off_monitor_.off_time();
}

int DiskManager::max_num_disk_operations() const {
  return max_num_disk_operations_;
}
}
