

#include "engine/ProcessManager.h"

#include <sys/time.h>
#include <time.h>

#include "au/containers/SharedPointer.h"
#include "au/Descriptors.h"        // au::Descriptors
#include "au/mutex/TokenTaker.h"   // au::TokenTake
#include "au/Singleton.h"
#include "au/ThreadManager.h"
#include "au/xml.h"                // au::xml...

#include "engine/DiskOperation.h"  // engine::DiskOperation
#include "engine/Engine.h"         // engine::Engine
#include "engine/EngineElement.h"  // engine::EngineElement
#include "engine/Notification.h"   // engine::Notification
#include "engine/NotificationElement.h"       // engine::EngineNotificationElement
#include "engine/ProcessItem.h"    // engine::ProcessItem
#include "engine/ProcessManager.h"  // engine::Process

#include "logMsg/logMsg.h"         // LM_X
#include "logMsg/traceLevels.h"

namespace engine {
void *ProcessManager_run_worker(void *p) {
  ProcessManager *process_manager = (ProcessManager *)p;

  process_manager->run_worker();
  return NULL;
}

ProcessManager::ProcessManager(int max_num_procesors) : token_("engine::ProcessManager") {
  stopped_ = false;

  // Defined maximum number of background threads
  num_procesors_ = 0;
  max_num_procesors_ = max_num_procesors;
}

ProcessManager::~ProcessManager() {
}

void ProcessManager::Stop() {
  {
    au::TokenTaker tt(&token_);
    items_.Clear();
  }
  stopped_ = true;   // Flag to notify all background process to finish

  // Wait all background workers
  au::Cronometer cronometer;
  while (true) {
    if (num_procesors_ == 0) {
      return;
    }
    usleep(100000);
    {
      // We are getting blocked here, because run_worker() is slept waiting for items
      au::TokenTaker tt(&token_);
      tt.WakeUpAll();
    }
    if (cronometer.seconds() > 1) {
      cronometer.Reset();
      LM_W(("Waiting for background threads of engine::ProcessManager, still %d num_procesors_", num_procesors_));
    }
  }
}

void ProcessManager::notify(Notification *notification) {
  LM_X(1, ("Wrong notification at ProcessManager [Listener %lu] %s",
           engine_id(), notification->GetDescription().c_str()));
}

void ProcessManager::Add(au::SharedPointer<ProcessItem> item, size_t listenerId) {
  // Protect multi-thread access
  au::TokenTaker tt(&token_);

  // We make sure items always come at least one listener id
  item->AddListener(listenerId);

  // Insert in the list of items
  items_.Push(item);

  // Check number of background processors
  if (!stopped_) {
    while (num_procesors_ < max_num_procesors_) {
      pthread_t t;
      au::Singleton<au::ThreadManager>::shared()->addThread("background_worker"
                                                            , &t
                                                            , 0
                                                            , ProcessManager_run_worker
                                                            , this);
      num_procesors_++;
    }
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
    notification->environment().Set("error", "Canceled");

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
  LM_T(LmtCleanup, ("run_worker() started"));
  while (true) {
    // Check if too many background workers are running
    {
      au::TokenTaker tt(&token_);
      if (stopped_ || (num_procesors_ > max_num_procesors_)) {
        num_procesors_--;
        LM_T(LmtCleanup, ("run_worker exits because stopped or num_processors(%d) > max_num_procesors(%d)",
                          num_procesors_, max_num_procesors_));
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
        LM_T(LmtCleanup, ("run_worker() sleeps on tt waiting to be woken up"));
        tt.Stop();   // Block until main thread wake me up
        continue;
      }
    }

    // Init cronometer for this process item
    item->StartActivity();

    // Run the process
    item->run();

    // Stop cronometer for this process item
    item->StopActivity();

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
      notification->environment().Set("error", item->error().GetMessage());  // Add the notification to the main engine
    }
    Engine::shared()->notify(notification);
  }
}
}
