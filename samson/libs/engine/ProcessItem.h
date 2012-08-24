/* ****************************************************************************
*
* FILE            ProcessItem
*
* AUTHOR          Andreu Urruela
*
* DATE            July 2011
*
* DESCRIPTION
*
* ProcessItem defined the task to be executed by ProcessManager
* It should be an independent task ( no external access while running )
* It is managed by the process manager
*
* ****************************************************************************/

#ifndef _H_PROCESS_ITEM
#define _H_PROCESS_ITEM


#include <sstream>
#include <string>

#include "au/CronometerSystem.h"
#include "au/Environment.h"             // au::Environment
#include "au/ErrorManager.h"            // au::ErrorManager
#include "au/mutex/Token.h"             // au::Token

#include "engine/Notification.h"

#define PI_PRIORITY_NORMAL_OPERATION 1
#define PI_PRIORITY_NORMAL_COMPACT   5  // Compact operation
#define PI_PRIORITY_BUFFER_PREWRITE  10 // Processing an output buffer to generate what is stored on disk

/**
 *
 * ProcessItem
 *
 * item inside the ProcessManager
 *
 * An item is a task to do without any IO operation in the middle.
 * It only requires processing time
 */

namespace engine {
class ProcessItem;
class ProcessManagerDelegate;
class ProcessManager;

// Note: NotificationObject is an empty class base to allo us to include a process item in a notification

class ProcessItem : public NotificationObject {
public:

  // Constructor with priority
  ProcessItem(int _priority = 5);
  virtual ~ProcessItem();

  // What to do when processor is available
  virtual void run() = 0;

  // Add additional listsners
  void AddListener(size_t _listenerId);

  // Accessorts
  std::set<size_t> listeners() const;
  std::string process_item_description() const;
  std::string process_item_current_task_description() const;
  std::string process_item_status() const;  // Status management
  bool running() const;  // Check status of this process item
  double progress() const;  // Get progress of this task

  // Debug string
  std::string str() const;


  // Methods to indicate that we are starting running this process
  void StartCronometer();
  void StopCronometer();

  au::Environment& environment();
  au::ErrorManager& error();

  const au::CronometerSystem& cronometer();

protected:

  // Interface to monitor operations performance
  void set_progress(double p);
  void set_process_item_description(const std::string& message);
  void set_process_item_current_task_description(const std::string& message);

  // Error management
  au::ErrorManager error_;

  // Environment variables
  au::Environment environment_;

private:

  // Operation name for statistics
  std::string process_item_description_;
  std::string process_item_current_task_description_;

  // Priority level ( 0 = low priority ,  10 = high priority )
  int priority_;

  // Cronometer to measure time running
  au::CronometerSystem cronometer_;

  // Identifiers of the listeners that should be notified when operation is finished
  std::set<size_t> listeners_;

  // Flag to indivate if this process is running
  bool running_;

  // Progress of the operation ( if internally reported somehow )
  double progress_;
};
}

#endif  // ifndef _H_PROCESS_ITEM
