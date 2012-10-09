
#ifndef _H_AU_LOG_CHANNELS
#define _H_AU_LOG_CHANNELS

#include <string>
#include <vector>

#include "au/Pattern.h"
#include "au/log/LogCommon.h"

namespace au {
// Class to manage all registeres logs
// Keep the name
class LogChannels {
public:

  LogChannels() {
    num_log_channels_ = 0;

    // Generic log channels
    RegisterChannel("Message", "M");
    RegisterChannel("Warning", "W");
    RegisterChannel("Error", "E");

    RegisterChannel("Exit", "X");

    RegisterChannel("Verbose", "V");
    RegisterChannel("Verbose2", "V");
    RegisterChannel("Verbose3", "V");
    RegisterChannel("Verbose4", "V");
    RegisterChannel("Verbose5", "V");
  };

  // Register a channel
  int RegisterChannel(const std::string& name) {
    return RegisterChannel(name, name);
  }

  int RegisterChannel(const std::string& name, const std::string& alias) {
    names_[num_log_channels_] = name;   // Copy the name for debugging
    alias_[num_log_channels_] = alias;   // Copy the name for debugging
    return num_log_channels_++;
  }

  bool IsRegistered(int c) {
    return names_[c] != "";
  }

  int channel(const std::string name) {
    for (int i = 0; i < AU_LOG_MAX_CHANNELS; i++) {
      if (names_[i] == name) {
        return i;
      }
    }
    return -1;
  }

  std::string channel_name(int c) {
    if (c < 0) {
      return "[?]";
    }
    if (c >= num_log_channels_) {
      return "[?]";
    }

    return names_[c];
  }

  std::string channel_alias(int c) {
    if (c < 0) {
      return "[?]";
    }
    if (c >= num_log_channels_) {
      return "[?]";
    }
    return alias_[c];
  }

  // Commands to activate or dactivate channels
  std::vector<int> Get(const std::string& str_pattern) {
    std::vector<int> channels;


    if (str_pattern == "std") {
      channels.push_back(AU_LOG_M);
      channels.push_back(AU_LOG_W);
      channels.push_back(AU_LOG_X);
      channels.push_back(AU_LOG_E);
      return channels;
    }

    // Prepare pattern comparision
    au::ErrorManager error;
    Pattern pattern(str_pattern, error);
    if (error.IsActivated()) {
      return channels;
    }

    // Search for channels
    for (int i = 0; i < AU_LOG_MAX_CHANNELS; i++) {
      if (names_[i].length() > 0) {
        if (pattern.match(names_[i])) {
          channels.push_back(i);
        }
      }
    }
    return channels;
  }

private:

  // Names for each channel
  std::string names_[AU_LOG_MAX_CHANNELS];
  std::string alias_[AU_LOG_MAX_CHANNELS];

  // Number of currently defined channels
  int num_log_channels_;
};
}

#endif  // ifndef _H_AU_LOG_CHANNELS