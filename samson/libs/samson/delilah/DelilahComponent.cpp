


#include "au/CommandLine.h"                      // samson::CommandLine

#include "engine/Buffer.h"                       // engine::Buffer
#include "engine/Notification.h"

#include "samson/common/EnvironmentOperations.h"  // copyEnviroment()
#include "samson/common/NotificationMessages.h"     // notification_delilah_review_repeat_tasks

#include "samson/network/Packet.h"               // samson::Packet

#include "DelilahComponent.h"                    // Own interface
#include "samson/delilah/Delilah.h"              // samson::Delilah
#include "samson/delilah/DelilahConsole.h"       // samson::DelilahConsole


namespace samson {
DelilahComponent::DelilahComponent(DelilaComponentType _type) {
  component_finished =  false;
  type = _type;

  cronometer.Start();

  concept = "Unknown";
  progress = 0;

  hidden = false;
  print_output_at_finish = false;          // By default, foreground process waits for component to finish
}

void DelilahComponent::setId(Delilah *_delilah,  size_t _id) {
  delilah = _delilah;
  id = _id;
}

std::string DelilahComponent::getTypeName() {
  switch (type) {
    case push:                  return "[ Push    ]";

    case pop:                   return "[ Pop     ]";

    case worker_command:        return "[ Comamnd ]";
  }

  LM_X(1, ("Impossible error"));
  return "";
}

std::string DelilahComponent::getStatusDescription() {
  std::ostringstream output;

  if (error.IsActivated()) {
    output << "ERROR";
  } else {
    if (component_finished) {
      output << "FINISHED";
    } else {
      output << "RUNNING ";
      output << "[ Progress: " << au::str_percentage(progress) << " ] ";
      output << "[ Time: " << cronometer << " ]";
    }
  }
  return output.str();
}

std::string DelilahComponent::getIdAndConcept() {
  std::ostringstream output;

  output << "[ " << id << " ] " << "'" << concept << "'";
  return output.str();
}

bool DelilahComponent::isComponentFinished() {
  return component_finished;
}

void DelilahComponent::setComponentFinished() {
  // Only mark as finished once
  if (component_finished)
    return;

  // LM_M(("Set component finish (%s)" , concept.c_str() ));

  component_finished = true;
  cronometer.Stop();

  // Show ourput on screen
  if (print_output_at_finish)
    delilah->showMessage(output.str()); delilah->delilahComponentFinishNotification(this);
}

void DelilahComponent::setComponentFinishedWithError(std::string error_message) {
  // Only mark as finished once
  if (component_finished)
    return;

  component_finished = true;
  cronometer.Stop();

  error.set(error_message);
  delilah->delilahComponentFinishNotification(this);
}

void DelilahComponent::setConcept(std::string _concept) {
  concept = _concept;
}

void DelilahComponent::setProgress(double p) {
  progress = p;
}
}

