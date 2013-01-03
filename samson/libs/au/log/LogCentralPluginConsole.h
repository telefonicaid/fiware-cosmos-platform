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

#include "au/console/Console.h"
#include "au/log/LogCentralPlugin.h"
#include "au/log/LogCommon.h"


namespace au {
/**
 * \brief Log plugin to show logs on a console
 */

class LogCentralPluginConsole : public LogCentralPlugin {
public:

  LogCentralPluginConsole(console::Console *console, const std::string& format = LOG_DEFAULT_FORMAT_CONSOLE,
                          bool log_to_screen = false)
    : LogCentralPlugin("console"),
      console_(console),
      log_formatter_(format, true),
      log_to_screen_(log_to_screen) {
  }

  virtual ~LogCentralPluginConsole() {
  };

  virtual void Emit(au::SharedPointer<Log>log) {
    if (console_) {
      console_->Write(log_formatter_.get(log) + "\n");
    } else if (log_to_screen_) {
      std::cout << log_formatter_.get(log) + "\n";
    }
  }

  /**
   * \brief Change console to print logs
   */

  void SetConsole(console::Console *console) {
    console_ = console;
  }

  virtual std::string status() {
    return "OK";  // nothing to show
  }

private:

  console::Console *console_;   /**< Console reference to print logs on it */
  LogFormatter log_formatter_;  /**< Formatter to create strings from logs */
  bool log_to_screen_;          /**< Log to screen if console_==NULL */
};
}

#endif  // ifndef _H_LOG_PLUGIN_CONSOLE