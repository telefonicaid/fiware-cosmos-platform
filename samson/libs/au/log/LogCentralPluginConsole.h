
#ifndef _H_LOG_PLUGIN_CONSOLE
#define _H_LOG_PLUGIN_CONSOLE

#include "au/log/LogCommon.h"
#include "au/log/LogCentralPlugin.h"


namespace au {
class LogCentralPluginConsole : public LogCentralPlugin {
public:

  LogCentralPluginConsole(Console *console, const std::string& format = AU_LOG_DEFAULT_FORMAT_CONSOLE)
  : LogCentralPlugin("console")
  , log_formatter_(format, true) // Output colored
  {
    console_ = console;
  }

  virtual ~LogCentralPluginConsole() {
  };

  virtual void Emit(au::SharedPointer<Log>log) {
    console_->writeOnConsole( log_formatter_.get(log) + "\n");
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

#endif  // ifndef _H_LOG_PLUGIN_CONSOLE