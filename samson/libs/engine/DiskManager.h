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
/* ****************************************************************************
*
* FILE            DiskManager
*
* AUTHOR          Andreu Urruela
*
* DATE            July 2011
*
* DESCRIPTION
*
* DiskManager is the entity in charge of schedulling all the read/write/remove acetions over file system
*
* ****************************************************************************/

#ifndef _H_SAMSON_DISK_MANAGER
#define _H_SAMSON_DISK_MANAGER

#include <iostream>                 // std::cout
#include <list>
#include <pthread.h>
#include <set>                      // std::set
#include <string.h>
#include <string>

#include "au/containers/Box.h"
#include "au/containers/Queue.h"
#include "au/containers/SharedPointer.h"
#include "au/containers/ListMap.h"  // au::ListMap
#include "au/mutex/Token.h"         // au::Token

#include "au/statistics/OnOffMonitor.h"
#include "au/statistics/Rate.h"

#include "engine/Engine.h"
#include "engine/MemoryManager.h"   // engine::MemoryManager
#include "engine/NotificationListener.h"     // engine::EngineNotification
#include "engine/ReadFileManager.h"  // engine::ReadFileManager

#define notification_disk_operation_request_response "notification_disk_operation_request_response"

namespace engine {
class EngineElement;
class ProcessItem;
class DiskOperation;
class Notification;


class DiskManager {
public:

  ~DiskManager();

  // Add a disk operation to be executed in the background
  void Add(const au::SharedPointer< ::engine::DiskOperation>& operation);

  // Cancel a disk operation already included
  void Cancel(const au::SharedPointer< ::engine::DiskOperation>& operation);

  // Main function for the background worker
  // It is public only to be called form the thread-creation function
  // note: make it frind
  void run_worker();

  // Get information
  size_t rate_in() const;
  size_t rate_out() const;
  double rate_operations_in() const;
  double rate_operations_out() const;
  double on_off_activity() const;
  double on_time() const;
  double off_time() const;
  int num_disk_operations() const;
  int max_num_disk_operations() const;
  void set_max_num_disk_operations(int _num_disk_operations);  // Setup the maimum number of paralel operations to be executed
  int num_disk_manager_workers() const;  // Get the number of workers running on the background

private:

  // Private constructor ( see engine::Engine::InitEngine() )
  DiskManager(int max_num_disk_operations);

  // Stop background threads
  void Stop();

  // Notification that a disk operation has finished
  void FinishDiskOperation(const au::SharedPointer< ::engine::DiskOperation >& operation);

  // Auxiliary function to get the next operation ( NULL if no more disk operations )
  au::SharedPointer< ::engine::DiskOperation > getNextDiskOperation();

  // Create threads
  void CreateThreads();

  // Disk Operations
  mutable au::Token token_;

  // File manager ( containing all the open files for read operations )
  ReadFileManager fileManager_;

  bool quitting_;                                 // Flag to indicate background processes to quit
  int max_num_disk_operations_;                   // Number of paralell disk operations allowed
  int num_disk_manager_workers_;                  // Number of parallel workers for Disk operations

  au::Queue<DiskOperation> pending_operations_;    // Queue with pending operations
  au::Box<DiskOperation>   running_operations_;    // Box of running operations

  // Information about rate and activity
  au::rate::Rate rate_in_;
  au::rate::Rate rate_out_;

  // Monitor to count how much time we are on
  au::OnOffMonitor on_off_monitor_;

  friend class DiskOperation;
  friend class Engine;
};
}

#endif  // ifndef _H_SAMSON_DISK_MANAGER
