
#include "au/log/LogCentralChannels.h"  // Own interface

namespace au {

  LogCentralChannels::LogCentralChannels() {
    num_log_channels_ = 0;
    
    // Generic log channels
    RegisterChannel("system");
  };
  
  int LogCentralChannels::RegisterChannel(const std::string& name ) {
    names_[num_log_channels_] = name;   // Copy the name for debugging
    return num_log_channels_++;
  }
  
  int LogCentralChannels::channel(const std::string name) {
    for (int i = 0; i < AU_LOG_MAX_CHANNELS; i++) {
      if (names_[i] == name) {
        return i;
      }
    }
    return -1;
  }
  
  std::string LogCentralChannels::GetAllChannels() const
  {
    std::ostringstream output;
    for ( int i = 0 ; i < num_log_channels_ ; i++ )
      output << names_[i] << " ";
    return output.str();
  }
  
  std::string LogCentralChannels::channel_name(int c) {
    if (c < 0) {
      return "?";
    }
    if (c >= num_log_channels_) {
      return "?";
    }
    
    return names_[c];
  }
  
  // Commands to activate or dactivate channels
  std::vector<int> LogCentralChannels::Get(const std::string& str_pattern) {
    
    std::vector<int> channels;
    
    // Prepare pattern comparision
    SimplePattern pattern(str_pattern);
    
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
  
  bool LogCentralChannels::IsRegistered( int channel )
  {
    if (channel < 0) {
      return false;
    }
    if (channel >= num_log_channels_) {
      return false;
    }
    
    return true;
  }
  
  int LogCentralChannels::num_channels() const
  {
    return num_log_channels_;
  }
  
}