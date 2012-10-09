
#ifndef _H_LOG_PLUGIN_CONSOLE
#define _H_LOG_PLUGIN_CONSOLE

#include "au/log/LogCommon.h"
#include "au/log/LogPlugin.h"

namespace au {
class LogPluginConsole : public LogPlugin {
public:

  LogPluginConsole(Console *console, const std::string& format = AU_LOG_DEFAULT_FORMAT)
    : LogPlugin("console")
      , log_formatter_(format) {
    console_ = console;
  }

  virtual void Emit(au::SharedPointer<Log>log) {
    std::string type = log->Get("channel_alias");

    if (( type == "E" ) || ( type == "X" )) {
      console_->writeErrorOnConsole("[LOG] " + log_formatter_.get(log) + "\n");
    } else if (type == "W") {
      console_->writeWarningOnConsole("[LOG] " + log_formatter_.get(log) + "\n");
    } else {
      console_->writeOnConsole("[LOG] " + log_formatter_.get(log) + "\n");
    }
  }

  virtual std::string status() {
    return "OK";  // nothing to show
  }

private:

  Console *console_;

  // Default method to print logs if added as plugin
  LogFormatter log_formatter_;
};
}

#endif // ifndef _H_LOG_PLUGIN_CONSOLE