
#ifndef _H_AU_LOG_PLUGIN
#define _H_AU_LOG_PLUGIN

#include "au/containers/SharedPointer.h"
#include "au/log/Log.h"
#include "au/log/LogCentralChannelsFilter.h"
#include "au/log/LogCounter.h"
#include "au/log/LogFormatter.h"

namespace au {

class LogCentralPlugin {
public:

  explicit LogCentralPlugin(const std::string& name) {
    name_ = name;
    activated_ = true;
  }
  virtual ~LogCentralPlugin() {};  // Virtual destructor for abstract class

  void Process(au::SharedPointer<Log> log) {
    log_counter_.Process(log);
    Emit(log);
  }

  // Pure virtual interface to be implemented by plugins
  void virtual Emit(au::SharedPointer<Log> log) = 0;  // Main function to process incoming log
  std::string virtual status() = 0;  // Status function
  
  // Function to check if a log is accepted based on the channels filter for this plugin
  bool IsLogAccepted(au::SharedPointer<Log> log) const;
  bool IsLogAccepted( int channel , int level );  // Handy method to know if we have to emit this log here

  // Manipulate channel filter
  LogCentralChannelsFilter& log_channel_filter() {
    return log_channels_filter_;
  }

  bool activated() const {
    return activated_;
  }

  void set_activated(bool activated) {
    activated_ = activated;
  }

  std::string name() {
    return name_;
  }

  // Single line info  for display in tables
  virtual std::string str_info() {
    return "";
  }

  LogCounter& log_counter() {
    return log_counter_;
  }

private:

  std::string name_;
  bool activated_;
  LogCentralChannelsFilter log_channels_filter_;   // Filter of logs to be generated for this plugin
  LogCounter log_counter_;                         // Counter of logs
};
}

#endif  // ifndef _H_AU_LOG_PLUGIN