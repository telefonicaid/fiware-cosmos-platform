#ifndef SAMSON_LIBS_ENGINE_ENGINEELEMENT_H_
#define SAMSON_LIBS_ENGINE_ENGINEELEMENT_H_

/* ****************************************************************************
* AUTHOR          Andreu Urruela
*
* DATE            July 2011
*
* DESCRIPTION
* Element defining a simple foreground task to be executed in the main thread
* This is the basic elements of the Engine library and can be scheduled using
* engine::Engine::shared()->add( . )
*
*
*
* Telefónica Digital - Product Development and Innovation
*
* THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
*
* Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
* All rights reserved.
*
*/
#include <sstream>                          // std::ostringstream
#include <string>
#include <unistd.h>                         // sleep

#include "logMsg/logMsg.h"      // Lmt
#include "logMsg/traceLevels.h"  // LmtEngine

#include "au/statistics/Cronometer.h"
#include "au/string/StringUtilities.h"                      // au::Format

namespace engine {
class EngineElement {
public:

  explicit EngineElement(std::string name);        // Constructor for single task or extra tasks
  EngineElement(std::string name, int seconds);    // Constructor for repeated task
  virtual ~EngineElement() {};                     // Virtual destructor necessary to destroy child-classes correctly

  virtual void run() = 0;                          // Run method to execute

  void SetAsExtra();  // Set as an extra element ( to be executed when nothing else has to be executed )
  void Reschedule();  // Reschedule action once executed ( for repeated )

  // Accessors
  bool IsRepeated() const;
  bool IsExtra() const;
  bool IsNormal() const;
  std::string name() const;
  double period() const;
  std::string description() const;
  void set_description(const std::string& description);
  std::string short_description() const;
  void set_short_description(const std::string& description);
  std::string str();  // Debug string

  double GetTimeToTrigger();  // Get time to be executed ( in repeated task )
  double GetWaitingTime();    // Get time that this element has been waiting

private:

  typedef enum {
    normal,                       // Executed once
    repeated,      // Repeated periodically
    extra,                        // Executed when nothing else is necessary to be executed before
  } Type;

  std::string name_;  // Name of the element ( to do some statistics over it )
  Type type_;      // Type of element

  // Fields in repeated type
  int counter_;                   // Number of times this element has been executed ( only in repeated )
  int period_;                    // Period of the execution

  au::Cronometer cronometer_;     // Cronometer since creation or last execution

  std::string description_;       // String for easy debugging
  std::string short_description_;   // Short description
};
}
#endif  // SAMSON_LIBS_ENGINE_ENGINEELEMENT_H_
