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

#ifndef _H_AU_LOG_PLUGIN_SCREEN
#define _H_AU_LOG_PLUGIN_SCREEN

#include "au/containers/SharedPointer.h"
#include "au/log/Log.h"
#include "au/log/LogCentralChannelsFilter.h"
#include "au/log/LogCentralPlugin.h"
#include "au/log/LogFormatter.h"

namespace au {
class LogCentralPluginScreen : public LogCentralPlugin {
public:
  LogCentralPluginScreen(const std::string& format_definition = LOG_DEFAULT_FORMAT, bool output_stderror = false)
    : LogCentralPlugin("Screen")
      , log_formatter_(format_definition, true) {
    output_stderror_ = output_stderror;
  }

  virtual ~LogCentralPluginScreen() {
  }

  virtual void Emit(au::SharedPointer<Log> log) {
    if (output_stderror_) {
      fprintf(stdout, "%s\n", log_formatter_.get(log).c_str());
    } else {
      fprintf(stderr, "%s\n", log_formatter_.get(log).c_str());
    }
  }

  virtual std::string status() {
    std::ostringstream output;

    if (output_stderror_) {
      output << "[STDERR]";
    }
    output << log_formatter_.definition();
    return output.str();
  }

  void SetFormat(const std::string format, bool color = false) {
    log_formatter_.SetFormat(format, color);
  }

private:
  bool output_stderror_;
  LogFormatter log_formatter_;
};
}

#endif  // ifndef _H_AU_LOG_PLUGIN_SCREEN