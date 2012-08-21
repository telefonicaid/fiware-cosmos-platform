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

#include "au/containers/ListMap.h"  // au::ListMap
#include "au/mutex/Token.h"         // au::Token

#include "au/OnOffMonitor.h"
#include "au/Rate.h"

#include "engine/Engine.h"
#include "engine/MemoryManager.h"   // engine::MemoryManager
#include "engine/Object.h"     // engine::EngineNotification

#include "engine/ReadFileManager.h"  // engine::ReadFileManager

#define notification_disk_operation_request_response "notification_disk_operation_request_response"

namespace engine {
class EngineElement;
class ProcessItem;
class DiskOperation;
class Notification;


class DiskManager {
  DiskManager(int _num_disk_operations);

public:

  // Singleton implementation
  static void init(int _num_disk_operations);
  static void stop();
  static void destroy();
  static DiskManager *shared();
  ~DiskManager();

  void Add(const au::SharedPointer< ::engine::DiskOperation>& operation);                               // Add a disk operation to be executed in the background
  void Cancel(const au::SharedPointer< ::engine::DiskOperation>& operation);                            // Cancel a disk operation already included

  // Setup the maimum number of paralel operations to be executed
  void setNumOperations(int _num_disk_operations);

  // Main function for the background worker
  // It is public only to be called form the thread-creation function
  // note: make it frind
  void run_worker();

  // Get information
  size_t get_rate_in();
  size_t get_rate_out();
  double get_rate_operations_in();
  double get_rate_operations_out();
  double get_on_off_activity();
  int getNumOperations();

private:

  // Notification that a disk operation has finished
  void finishDiskOperation(const au::SharedPointer< ::engine::DiskOperation >& operation);

  // Auxiliar function to get the next operation ( NULL if no more disk operations )
  au::SharedPointer< ::engine::DiskOperation > getNextDiskOperation();

  void add_worker();
  bool check_quit_worker();

  int get_num_disk_manager_workers();
  void createThreads();

  // Singleton instance
  static DiskManager *diskManager;

  // File manager ( containing all the open files )
  ReadFileManager fileManager;

  // Disk Operations
  au::Token token;

  bool quitting;                                  // Flag to indicate background processes to quit
  int num_disk_operations;                        // Number of paralell disk operations allowed
  int num_disk_manager_workers;                   // Number of parallel workers for Disk operations

  au::Queue<DiskOperation> pending_operations;    // Queue with pending operations
  au::Box<DiskOperation>   running_operations;    // Box of running operations

  // Information about rate and activity
  au::rate::Rate rate_in;
  au::rate::Rate rate_out;

public:
  // To be fixed to private

  au::OnOffMonitor on_off_monitor;

  friend class DiskOperation;
};
}

#endif  // ifndef _H_SAMSON_DISK_MANAGER
