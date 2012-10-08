
#include "au/ErrorManager.h"                // au::ErrorManager
#include "au/ExecesiveTimeAlarm.h"
#include "au/ThreadManager.h"
#include "au/mutex/TokenTaker.h"            // au::TokenTake
#include "au/xml.h"                         // au::xml...

#include "engine/Engine.h"                  // engine::Engine
#include "engine/ProcessItem.h"             // Own interface
#include "engine/ProcessManager.h"          // engine::ProcessManager

namespace engine {
#pragma mark ----

ProcessItem::ProcessItem(int _priority) {
  // Default values
  priority_ = _priority;
  progress_ = 0;                // Initial progress to "0"
  running_ = false;
  finished_ = false;

  process_item_description_ = "unknown";
  process_item_current_task_description_ = "";
  
  waiting_cronometer_.Start(); // Start counting as waiting since creation
}

ProcessItem::~ProcessItem() {
}

std::string ProcessItem::process_item_status() const {
  std::ostringstream o;

  if (running_) {
    // Show progress is any
    int p = progress_ * 100.0;
    if (p > 0) {
      o << "[" << au::str_percentage(p) << "]";
    }
    o << "Running (" << running_cronometer_.str_seconds() << ")";
  } if ( finished_ )
    o << "Finished";
    else {
      o << "Queued waiting " << waiting_cronometer_.str_seconds() ;
  } o << " : " << priority_;

  if (process_item_current_task_description_.length() > 0) {
    o << std::string(" : ") << process_item_current_task_description_;
  }

  return o.str();
}

std::string ProcessItem::str() const {
  return au::str("Process Item '%s' Status: %s"
                 , process_item_description_.c_str()
                 , process_item_status().c_str()
                 );
}

std::string ProcessItem::process_item_description() const {
  return process_item_description_;
}

void ProcessItem::AddListener(size_t _listenerId) {
  listeners_.insert(_listenerId);
}

  bool ProcessItem::finished() const
  {
    return finished_;
  }
  bool ProcessItem::running() const
  {
    return running_;
  }

void ProcessItem::StartActivity() {
  running_ = true;
  waiting_cronometer_.Stop();
  running_cronometer_.Start();
}

void ProcessItem::StopActivity() {
  running_ = false;
  finished_ = true;
  running_cronometer_.Stop();
}

std::set<size_t> ProcessItem::listeners() const {
  return listeners_;
}

std::string ProcessItem::process_item_current_task_description() const {
  return process_item_current_task_description_;
}

// Interface to monitor operations performance
void ProcessItem::set_process_item_description(const std::string& message) {
  process_item_description_ = message;
}

void ProcessItem::set_process_item_current_task_description(const std::string& message) {
  process_item_current_task_description_ = message;
}

au::Environment& ProcessItem::environment() {
  return environment_;
}

au::ErrorManager& ProcessItem::error() {
  return error_;
}

void ProcessItem::set_progress(double p) {
  progress_ = p;
}

double ProcessItem::progress() const {
  return progress_;
}

const au::CronometerSystem& ProcessItem::running_cronometer() const {
  return running_cronometer_;
}
  const au::CronometerSystem& ProcessItem::waiting_cronometer() const {
    return running_cronometer_;
  }
}
