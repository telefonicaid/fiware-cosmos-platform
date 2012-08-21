

#include "engine/ProcessManager.h"



#include "logMsg/logMsg.h"         // LM_X
#include <sys/time.h>
#include <time.h>

#include "au/Descriptors.h"        // au::Descriptors
#include "au/ThreadManager.h"
#include "au/containers/SharedPointer.h"
#include "au/mutex/TokenTaker.h"   // au::TokenTake
#include "au/xml.h"                // au::xml...

#include "engine/DiskOperation.h"  // engine::DiskOperation
#include "engine/Engine.h"         // engine::Engine
#include "engine/EngineElement.h"  // engine::EngineElement
#include "engine/Notification.h"   // engine::Notification
#include "engine/NotificationElement.h"       // engine::EngineNotificationElement
#include "engine/ProcessItem.h"    // engine::ProcessItem
#include "engine/ProcessManager.h"  // engine::Process

namespace engine {
void *ProcessManager_run_worker(void *p) {
  ProcessManager *process_manager = (ProcessManager *)p;

  process_manager->run_worker();
  return NULL;
}

// Initialise singleton instance pointer
ProcessManager *ProcessManager::processManager = NULL;

void ProcessManager::init(int _num_processes) {
  LM_V(("ProcessManager init with %d processes",  _num_processes ));

  if (processManager) {
    LM_W(("Please, init processManager only once.Ignoring..."));
    return;
  }
  processManager = new ProcessManager(_num_processes);
}

void ProcessManager::stop() {
  LM_V(("ProcessManager stop"));

  // Set the maximum number of process to 0 makes all background threads to quit
  if (processManager)
    processManager->max_num_procesors_ = 0;
}

void ProcessManager::destroy() {
  LM_V(("ProcessManager destroy"));

  if (!processManager)
    LM_RVE(("attempt to destroy uninitialized process manager")); delete processManager;
  processManager = NULL;
}

ProcessManager *ProcessManager::shared() {
  if (!processManager)
    LM_X(1, ("ProcessManager not initialiazed")); return processManager;
}

ProcessManager::ProcessManager(int max_num_procesors) : token_("engine::ProcessManager") {
  // Defined maximum number of background threads
  num_procesors_ = 0;
  max_num_procesors_ = max_num_procesors;
}

ProcessManager::~ProcessManager() {
}

void ProcessManager::notify(Notification *notification) {
  LM_X(1,
       ("Wrong notification at ProcessManager [Listener %lu] %s", getEngineId(), notification->getDescription().c_str()));
}

void ProcessManager::Add(au::SharedPointer<ProcessItem> item, size_t listenerId) {
  // Protect multi-thread access
  au::TokenTaker tt(&token_);

  // We make sure items always come at least one listener id
  item->addListenerId(listenerId);

  // Insert in the list of items
  items_.Push(item);

  // Check number of background processors
  while (num_procesors_ < max_num_procesors_) {
    pthread_t t;
    au::ThreadManager::shared()->addThread("background_worker"
                                           , &t
                                           , 0
                                           , ProcessManager_run_worker
                                           , this);
    num_procesors_++;
  }

  // Wake up all background threads if necessary
  tt.WakeUpAll();
}

void ProcessManager::Cancel(au::SharedPointer<ProcessItem> item) {
  // We can only cancel items if they are in the list of pending items
  if (items_.Contains(item)) {
    // If still in the queue of items
    items_.ExtractAll(item);

    // Set this process with an error
    item->error().set("ProcessItem canceled");

    // Notify this using the notification Mechanism
    Notification *notification = new Notification(notification_process_request_response);
    notification->AddEngineListeners(item->listeners());

    au::SharedPointer<NotificationObject> notification_object;
    notification_object = item.static_pointer_cast<NotificationObject>();

    notification->dictionary().Set("process_item", notification_object);

    notification->environment().Add(item->environment());
    notification->environment().set("error", "Canceled");

    Engine::shared()->notify(notification);
  }
}

int ProcessManager::num_used_procesors() {
  au::TokenTaker tt(&token_);

  return (int)running_items_.size();
}

int ProcessManager::max_num_procesors() {
  return max_num_procesors_;
}

void ProcessManager::run_worker() {
  while (true) {
    // Check if too many background workers are running
    {
      au::TokenTaker tt(&token_);
      if (num_procesors_ > max_num_procesors_) {
        num_procesors_--;
        return;
      }
    }

    // Take the next item to be executed
    au::SharedPointer<ProcessItem> item;
    {
      au::TokenTaker tt(&token_);
      item = items_.Pop();
      if (item != NULL) {
        running_items_.Insert(item);
      } else {
        tt.Stop();   // Block until main thread wake me up
        continue;
      }
    }

    // Init cronometer for this process item
    item->StartCronometer();

    // Run the process
    item->run();

    // Stop cronometer for this process item
    item->StopCronometer();

    // Remove from the box of running elements
    {
      au::TokenTaker tt(&token_);
      running_items_.Erase(item);
    }

    // Notify this using the notification Mechanism
    Notification *notification = new Notification(notification_process_request_response);

    // Add item itself as an object inside the notification
    au::SharedPointer<NotificationObject> notification_object;
    notification_object = item.static_pointer_cast<NotificationObject>();
    notification->dictionary().Set("process_item", notification_object);

    // Add targets to be notified
    notification->AddEngineListeners(item->listeners());

    // Add enviroment variables
    notification->environment().Add(item->environment());

    // Extra error environment if necessary
    if (item->error().IsActivated()) {
      notification->environment().set("error", item->error().GetMessage());  // Add the notification to the main engine
    }
    Engine::shared()->notify(notification);
  }
}
}
