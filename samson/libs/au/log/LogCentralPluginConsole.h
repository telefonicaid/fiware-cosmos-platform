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

#ifndef _H_LOG_PLUGIN_CONSOLE
#define _H_LOG_PLUGIN_CONSOLE

#include "au/log/LogCentralPlugin.h"
#include "au/log/LogCommon.h"


namespace au {
class LogCentralPluginConsole : public LogCentralPlugin {
public:

  LogCentralPluginConsole(console::Console::Console *console, const std::string& format = LOG_DEFAULT_FORMAT_CONSOLE)
    : LogCentralPlugin("console")
      , log_formatter_(format, true) {  // Output colored
    console_ = console;
  }

  virtual ~LogCentralPluginConsole() {
  };

  virtual void Emit(au::SharedPointer<Log>log) {
    console_->writeOnConsole(log_formatter_.get(log) + "\n");
  }

  virtual std::string status() {
    return "OK";  // nothing to show
  }

private:

  console::Console *console_;            // Console reference to print logs on it
  LogFormatter log_formatter_;  // Formatter to create strings from logs
};
}

#endif  // ifndef _H_LOG_PLUGIN_CONSOLE