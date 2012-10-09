#ifndef _H_AU_LOG_PLUGIN
#define _H_AU_LOG_PLUGIN

#include "au/containers/SharedPointer.h"
#include "au/log/Log.h"
#include "au/log/LogChannelFilter.h"
#include "au/log/LogFormatter.h"

namespace au {
class LogPlugin {

  public:

    LogPlugin(const std::string& name) {
      name_ = name;
      activated_ = true;
    }

    // Virtual destructor for abstract class
    virtual ~LogPlugin() {
    }
    ;

    // Main function to process incoming log
    void virtual Emit(au::SharedPointer<Log> log) = 0;

    // Function to check if a log is accepted
    bool Accept(au::SharedPointer<Log> log) {
      if (!activated_)
        return false;

      // Check emission channel
      int channel = log->log_data().channel;
      if (!log_channels_filter_.IsChannelActivated(channel))
        return false;

      return true;
    }

    // Status function
    std::string virtual status() = 0;

    // Manipulate channel filter
    LogChannelFilter& log_channel_filter() {
      return log_channels_filter_;
    }

    // Handy method to know if we have to emit this log here
    inline bool CheckLogChannel(int c) {
      return log_channels_filter_.IsChannelActivated(c);
    }

    bool activated() {
      return activated_;
    }

    void set_activated(bool activated) {
      activated_ = activated;
    }

    std::string name() {
      return name_;
    }

  private:

    std::string name_;
    bool activated_;
    LogChannelFilter log_channels_filter_;

};

}

#endif
