/* ****************************************************************************
*
* FILE            ProcessManager
*
* AUTHOR          Andreu Urruela
*
* DATE            July 2011
*
* DESCRIPTION
*
* Manager of background tasks to be executed by a set of cores.
* It creates a separate thread for each task and controlls them.
*
* ****************************************************************************/

#ifndef _H_SAMSON_PROCESS_MANAGER
#define _H_SAMSON_PROCESS_MANAGER

#include <iostream>                 // std::cout
#include <list>
#include <pthread.h>
#include <set>                      // std::set
#include <string>

#include "au/containers/Box.h"
#include "au/containers/Queue.h"
#include "au/mutex/Token.h"         // au::Token


#include "engine/Engine.h"     // engine::Engine
#include "engine/MemoryManager.h"   // engine::MemoryManager
#include "engine/Object.h"     // engine::EngineNotification
#include "engine/Object.h"          // engine::Object

#define notification_process_request_response "notification_process_request_response"

namespace engine {
class EngineElement;
class ProcessItem;
class DiskOperation;
class Notification;

/**
 * Class to manage background jobs
 */

class ProcessManager : engine::Object {
  static ProcessManager *processManager;    // Singleton Instance pointer

  // Private constructor to be a singleton
  ProcessManager(int _num_processes);

public:

  ~ProcessManager();

  // Generic notification system
  void notify(Notification *notification);

  // Function to add a Process. It will be notifier by delegate mechanism
  void Add(au::SharedPointer<ProcessItem> item, size_t listenerId);

  // Function to cancel a Process.
  void Cancel(au::SharedPointer<ProcessItem>item);

  // Get the number of running items or maximum items
  int num_used_procesors();
  int max_num_procesors();

private:

  // Stop backgroudn threads
  void Stop();

  // Method executed by background threads to run tasks
  void run_worker();

  // Function to run background threads
  friend void *ProcessManager_run_worker(void *p);

  // Mutex to protect different queues
  au::Token token_;

  au::Queue<ProcessItem> items_;            // List of items to be executed ( all priorities  )
  au::Box<ProcessItem> running_items_;      // Set of items currently being executed

  int num_procesors_;                       // Number of background workers to perform tasks
  int max_num_procesors_;                   // Maximum number of background processors

  friend class Engine;

  bool stopped_;
};
}

#endif  // ifndef _H_SAMSON_PROCESS_MANAGER
