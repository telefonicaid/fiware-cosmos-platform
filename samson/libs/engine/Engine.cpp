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



#include <signal.h>   // SIGKILL
#include <sys/time.h>
#include <time.h>

#include "logMsg/logMsg.h"             // LM_X

#include "au/ErrorManager.h"      // au::ErrorManager
#include "au/ThreadManager.h"
#include "au/mutex/Token.h"       // au::Token
#include "au/mutex/TokenTaker.h"  // au::TokenTake
#include "au/singleton/Singleton.h"
#include "au/string/StringUtilities.h"  // au::xml_...
#include "au/string/xml.h"             // au::xml...


#include "Notification.h"              // engine::Notification

#include "engine/DiskManager.h"
#include "engine/DiskOperation.h"  // engine::DiskOperation
#include "engine/EngineElement.h"  // engine::EngineElement
#include "engine/Logs.h"
#include "engine/MemoryManager.h"
#include "engine/NotificationElement.h"       // engine::EngineNotificationElement
#include "engine/ProcessItem.h"   // engine::ProcessItem
#include "engine/ProcessManager.h"

#include "engine/Engine.h"        // Own interface

// Goyo. From 60 to 60000
#define ENGINE_MAX_RUNNING_TIME 60000

namespace engine {
void *runEngineBackground(void *);

// Static variables
Engine *Engine::engine_ = NULL;
MemoryManager *Engine::memory_manager_ = NULL;
DiskManager *Engine::disk_manager_ = NULL;
ProcessManager *Engine::process_manager_ = NULL;

void Engine::InitEngine(int num_cores, size_t memory, int num_disk_operations) {
  LOG_M(logs.engine, ("Engine init"));
  if (engine_) {
    LOG_W(logs.engine, ("Init engine twice... just ignoring"));
    return;
  }

  // Create the unique engine entity
  engine_ = new Engine();
  disk_manager_ = new DiskManager(num_disk_operations);
  memory_manager_ = new MemoryManager(memory);
  process_manager_ = new ProcessManager(num_cores);

  engine_->StartThread();  // Start thread in background
}

bool Engine::IsEngineWorking() {
  return (engine_ != NULL);
}

void Engine::StopEngine() {
  if (engine_) {
    engine_->StopThread();  // Stop background thread if necessary
  }
  if (disk_manager_ != NULL) {
    disk_manager_->Stop();
    delete disk_manager_;
    disk_manager_ = NULL;
  }
  if (process_manager_ != NULL) {
    process_manager_->Stop();
    delete process_manager_;
    process_manager_ = NULL;
  }
  if (memory_manager_ != NULL) {
    delete memory_manager_;
    memory_manager_ = NULL;
  }
  if (engine_ != NULL) {
    delete engine_;
    engine_ = NULL;
  }
}

Engine::Engine() : au::Thread("engine") {
  // Add a simple periodic element to not die inmediatelly
  EngineElement *element = new NotificationElement(new Notification("alive"), 10);

  engine_element_collection_.Add(element);
}

Engine::~Engine() {
}

void Engine::RunElement(EngineElement *running_element) {
  au::Cronometer cronometer;

  InternRunElement(running_element);
  if (cronometer.seconds() > 3) {
    LOG_W(logs.engine, ("EngineElement %s has being running for %s"
                        , running_element->str().c_str(), au::str_time(cronometer.seconds()).c_str()));
  }
}

void Engine::InternRunElement(EngineElement *running_element) {
  activity_monitor_.StartActivity(running_element->name());

  // Execute the item selected as running_element
  LOG_M(logs.engine, ("[START] Engine:  executing %s", running_element->str().c_str()));

  // Print traces for debugging strange situations
  int waiting_time = running_element->GetWaitingTime();
  if (waiting_time > 10) {
    LOG_SW(("Engine is running an element that has been waiting %d seconds", waiting_time));

    if (waiting_time > 100) {
      // Print all elements with traces for debuggin...
      engine_element_collection_.PrintElements();
    }
  }


  {
    // Run the item controlling excesive time
    au::Cronometer c;

    // Run the running element ;)
    running_element->run();

    int execution_time = c.seconds();
    if (execution_time > 10) {
      LOG_SW(("Engine has executed an item in %d seconds.", execution_time));
      LOG_SW(("Engine Executed item: %s", running_element->str().c_str()));
    }
  }

  LOG_M(logs.engine, ("[DONE] Engine:  executing %s", running_element->str().c_str()));

  // Collect information about this execution
  activity_monitor_.StartActivity("engine_management");
}

/**
 *
 * RunMainLoop
 *
 * \breif Main loop to process engine-elements ( normal, periodic ande extra )
 *
 * Try to get the next element in the repeat_elements list
 * if not there , try normal elements...
 * if not, run extra elements and loop again...
 *
 */


void Engine::RunThread() {
  LOG_M(logs.engine, ("Engine run"));

  counter_ = 0;  // Init the counter to elements managed by this run-time

  while (true) {
    counter_++;  // Keep a total counter of loops

    if (IsThreadQuiting()) {
      return;    // Finish this thread if necessary ( a call to StopThread is done )
    }

    // Check if there are elements in the list
    if (engine_element_collection_.IsEmpty()) {
      LOG_D(logs.engine, ("SamsonEngine: No more elements to process in the engine. Quitting ..."));
      return;
    }

    // Warning if we have a lot of elements in the engine stack
    size_t num_engine_elements = engine_element_collection_.GetNumEngineElements();
    LOG_D(logs.engine, ("Number of elements in the engine stack %lu", num_engine_elements));

    if (num_engine_elements > 10000) {
      LOG_SW(("Execesive number of elements in the engine stack %lu", num_engine_elements));
    }

    // Try if next repeated element is ready to be executed
    EngineElement *element = engine_element_collection_.NextRepeatedEngineElement();
    if (element) {
      RunElement(element);
      element->Reschedule();         // Reschedule this item
      Add(element);
      continue;
    }

    // Try next normal item
    element = engine_element_collection_.NextNormalEngineElement();
    if (element) {
      RunElement(element);
      delete element;         // Remove this normal element
      continue;
    }

    // Get a vector with all extra elements to be executed
    std::vector<EngineElement *> extra_elements = engine_element_collection_.ExtraElements();

    // Run all the elements
    for (size_t i = 0; i < extra_elements.size(); i++) {
      EngineElement *element = extra_elements[i];
      RunElement(element);
      element->Reschedule();         // reinit internal counters...
    }

    // If normal elements to be executed, do not sleep
    size_t num_normal_elements =  engine_element_collection_.GetNumNormalEngineElements();
    if (num_normal_elements > 0) {
      LOG_D(logs.engine, ("Do not sleep since it seems there are %lu elements in the engine",
                          num_normal_elements));
      continue;         // Do not sleep here
    }

    // If next repeated elements is close, do not sleep
    double t_next_repeated_elements = engine_element_collection_.TimeForNextRepeatedEngineElement();
    LOG_D(logs.engine, ("Engine: Next repeated item in %.2f secs ...", t_next_repeated_elements));
    if (t_next_repeated_elements < 0.01) {
      continue;
    }
    activity_monitor_.StartActivity("sleep");

    usleep(300000);

    activity_monitor_.StartActivity("engine_management");
  }
}

int Engine::GetNumElementsInEngineStack() {
  return engine_element_collection_.GetNumEngineElements();
}

double Engine::GetMaxWaitingTimeInEngineStack() {
  return engine_element_collection_.GetMaxWaitingTimeInEngineStack();
}

#pragma mark ----

// Functions to register objects ( general and for a particular notification )
void Engine::AddListener(NotificationListener *object) {
  notification_listeners_manager_.Add(object);
}

void Engine::AddListenerToChannel(NotificationListener *object, const char *channel) {
  notification_listeners_manager_.AddToChannel(object, channel);
}

// Generic method to unregister an object
void Engine::RemoveListener(NotificationListener *object) {
  notification_listeners_manager_.Remove(object);
}

// Run a particular notification
// Only executed from friend class "NotificationElement"
void Engine::Send(Notification *notification) {
  notification_listeners_manager_.Send(notification);
}

// Add a notification
void Engine::notify(Notification *notification) {
  // Push a notification element with the notification
  Add(new NotificationElement(notification));
}

void Engine::notify_extra(Notification *notification) {
  // Push a notification element with the notification
  NotificationElement *notification_element = new NotificationElement(notification);

  notification_element->SetAsExtra();
  Add(notification_element);
}

void Engine::notify(Notification *notification, int seconds) {
  // Push a notification element with the notification ( in this case with periodic time )
  Add(new NotificationElement(notification, seconds));
}

// Function to add a simple foreground tasks
void Engine::Add(EngineElement *element) {
  // Add a new item in the engine_element_collection
  engine_element_collection_.Add(element);
}

std::string Engine::GetTableOfEngineElements() {
  return engine_element_collection_.GetTableOfEngineElements();
}

au::statistics::ActivityMonitor *Engine::activity_monitor() {
  return &activity_monitor_;
}

Engine *Engine::shared() {
  if (!engine_) {
    LOG_W(logs.engine, ("Engine was not initialised."));
  }
  return engine_;
}

DiskManager *Engine::disk_manager() {
  if (!disk_manager_) {
    LM_E(("DiskManager was not initialised."));
  }
  return disk_manager_;
}

MemoryManager *Engine::memory_manager() {
  if (!memory_manager_) {
    LM_E(("MemoryManager was not initialised."));
  }
  return memory_manager_;
}

ProcessManager *Engine::process_manager() {
  if (!process_manager_) {
    LM_E(("ProcessManager was not initialised."));
  }
  return process_manager_;
}

void notify(const char *notification_name) {
  Engine::shared()->notify(new Notification(notification_name));
}

void notify(const char *notification_name, double period) {
  Engine::shared()->notify(new Notification(notification_name), period);
}

void notify_extra(const char *notification_name) {
  Engine::shared()->notify_extra(new Notification(notification_name));
}
}
