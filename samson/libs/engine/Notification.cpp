#include "engine/Notification.h"   // Own interface

#include "au/string/StringUtilities.h"      // au::Format

#include "engine/NotificationListener.h"         // engine::NotificationListener

#include "logMsg/logMsg.h"      // Log system
#include "logMsg/traceLevels.h"  // LmtEngine

namespace engine {
// Simples constructor
Notification::Notification(const char *name) {
  name_ = name;
}

Notification::~Notification() {
}

void Notification::AddEngineListener(size_t listener_id) {
  targets_.insert(listener_id);
}

void Notification::AddEngineListeners(const std::set<size_t>& _listeners_id) {
  // Insert this as the first listener to receive this notification
  targets_.insert(_listeners_id.begin(), _listeners_id.end());
}

const char *Notification::name() {
  return name_;
}

au::GeneralDictionary& Notification::dictionary() {
  return dictionary_;
}

au::Environment& Notification::environment() {
  return environment_;
}

bool Notification::isName(const char *name) {
  return strcmp(name_, name) == 0;
}

const std::set<size_t>& Notification::targets() {
  return targets_;
}

std::string Notification::GetDescription() {
  std::ostringstream output;

  output << name_ << " [ Notification " << name_ << " ";
  output << "Targets: (";
  std::set<size_t>::iterator iterator_listener_id;
  for (iterator_listener_id = targets_.begin(); iterator_listener_id != targets_.end(); iterator_listener_id++) {
    output << *iterator_listener_id << " ";
  }
  output << ") ";
  output << environment_ << " ]";
  return output.str();
}

std::string Notification::GetShortDescription() {
  return au::str("[ Not: %s ]", name_);
}
}
