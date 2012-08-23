
/* ****************************************************************************
*
* FILE            Engine.h
*
* AUTHOR          Andreu Urruela
*
* DATE            July 2011
*
* DESCRIPTION
*
* Engine: Singlenton object with all the main funcitonalities of the engine library
*
* ****************************************************************************/

#ifndef _H_SAMSON_ENGINE
#define _H_SAMSON_ENGINE

#include <iostream>                         // std::cout
#include <list>
#include <math.h>
#include <pthread.h>
#include <set>                              // std::set
#include <string>

#include "au/statistics/ActivityMonitor.h"
#include "au/tables/Table.h"

#include "au/Cronometer.h"                  // au::Cronometer
#include "au/containers/list.h"             // au::list
#include "au/mutex/Token.h"                 // au::Token
// NAMESPACE_BEGIN & }
#include "au/containers/vector.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#include "engine/EngineElementCollection.h"
#include "engine/Object.h"                  // engine::EngineNotification
#include "engine/ObjectsManager.h"          // engine::ObjectsManager

namespace au {
class Error;
class Token;
}

namespace engine {
class EngineElement;
class ProcessItem;
class DiskOperation;
class DiskManager;
class ProcessManager;
class MemoryManager;
class Notification;

// ---------------------------------------------------
//
// EngineElementCollection
//
// Main Engine platform
// ---------------------------------------------------


class Engine {
public:

  ~Engine();

  // Static methods to init and close engine ( all included )
  static void InitEngine(int num_cores, size_t memory, int num_disk_operations);
  static void StopEngine();
  static void DestroyEngine();

  static Engine *shared();
  static DiskManager *disk_manager();
  static MemoryManager *memory_manager();
  static ProcessManager *process_manager();

  // Function to add a simple foreground tasks
  void add(EngineElement *element);

  // Methods to add notifictions
  void notify(Notification *notification);
  void notify(Notification *notification, int seconds);   // Repeated notification
  void notify_extra(Notification *notification);

  // Get an object by its registry names
  Object *objectByName(const char *name);

  // Debug information
  int GetNumElementsInEngineStack();
  double GetMaxWaitingTimeInEngineStack();
  std::string GetTableOfEngineElements();      // Get information about current elements in engine
  au::statistics::ActivityMonitor *activity_monitor();      // Return activity monitor to print some statistics

private:

  // Private constructor ( see Init static method )
  Engine();

  // Stop backgroudn threads
  void Stop();

  // Methods only executed from the thread-creation-functions ( never use directly )
  void run();

  // Methods to register and unregister objects
  void register_object(Object *object);
  void register_object_for_channel(Object *object, const char *channel);
  void unregister_object(Object *object);

  // Really sent a notification to the targets
  void send(Notification *notification);

  // Run a particular engine element
  void runElement(EngineElement *running_element);

  // Statistics
  au::statistics::ActivityMonitor activity_monitor_;

  // Collection of items
  EngineElementCollection engine_element_collection;

  // Management of all objects
  ObjectsManager objectsManager;

  // Counter of EngineElement processed
  size_t counter;

  pthread_t thread_id_;       // Thread to run the engine in background ( if necessary )
  bool running_thread_;       // Flag to indicate that background thread is running
  bool quitting_thread_;      // Flag to indicate to the backgroudn thread to quit

  au::Cronometer uptime;                              // Total up time
  double last_uptime_mark;                            // Last mark used to spent time

  // Common engine instance
  static Engine *engine_;
  static MemoryManager *memory_manager_;
  static DiskManager *disk_manager_;
  static ProcessManager *process_manager_;

  friend class Object;
  friend class NotificationElement;
  friend void *runEngineBackground(void *);   // Backgrount function
};
}
#endif  // ifndef _H_SAMSON_ENGINE
