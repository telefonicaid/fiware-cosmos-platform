
#include "au/xml.h"         // au::xml...


#include "engine/EngineElement.h"   // Own interface

namespace engine {
EngineElement::EngineElement(std::string name) {
  name_ = name;    // Keep tha name of this element
  type_ = normal;  // Flag to indicate that this element will be executed just once
  description_ = "Engine element to be executed once";
  short_description_ = "Engine element to be executed once";
}

EngineElement::EngineElement(std::string name, int seconds) {
  name_ = name;  // Keep tha name of this element
  type_ = repeated;
  period_ = seconds;
  counter_ = 0;

  std::ostringstream txt;
  txt << "Engine element repeated every " << seconds  << " seconds";
  description_ = txt.str();
  short_description_ = txt.str();
}

// Reschedule action once executed

void EngineElement::SetAsExtra() {
  type_ = extra;
}

void EngineElement::Reschedule() {
  // Reset cronometer
  cronometer_.Reset();
  // Increse the counter to get an idea of the number of times a repeated task is executed
  counter_++;
}

double EngineElement::GetTimeToTrigger() {
  // Time for the next execution
  if (type_ == repeated) {
    double t = cronometer_.seconds();
    LM_T(LmtEngineTime, ("getTimeToTrigger: Period %d Cronometer: %f", period_, t ));
    return period_ - t;
  }
  return 0;
}

double EngineElement::period() const {
  return period_;
}

double EngineElement::GetWaitingTime() {
  // Time for the next execution
  return cronometer_.seconds();
}

std::string EngineElement::description() const {
  return description_;
}

std::string EngineElement::short_description() const {
  return short_description_;
}

void EngineElement::set_description(const std::string& description) {
  description_ = description;
}

void EngineElement::set_short_description(const std::string& short_description) {
  short_description_ = short_description;
}

std::string EngineElement::str() {
  if (type_ == repeated) {
    return au::str(
             "%s [ Engine element to be executed in %02.2f seconds ( repeat every %d secs , repeated %d times )] "
             , description_.c_str()
             , GetTimeToTrigger()
             , period_
             , counter_
             );
  } else if (type_ == extra) {
    return au::str("%s [ Engine element EXTRA ]", description_.c_str());
  } else {
    return au::str("%s [ Engine element ]", description_.c_str());
  }
}

std::string EngineElement::name() const {
  return name_;
}

bool EngineElement::IsRepeated() const {
  return (type_ == repeated);
}

bool EngineElement::IsExtra() const {
  return (type_ == extra);
}

bool EngineElement::IsNormal() const {
  return (type_ == normal);
}
}
