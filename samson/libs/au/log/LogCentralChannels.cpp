
#include "au/log/LogCentralChannels.h"  // Own interface

#include "au/string/StringUtilities.h"

namespace au {

  LogCentralChannels::LogCentralChannels() {
    num_log_channels_ = 0;
    
    // Generic log channels
    RegisterChannel("system" , "System channel");
  };
  
  int LogCentralChannels::RegisterChannel(const std::string& name , const std::string& description ) {
    names_[num_log_channels_] = name;   // Copy the name for debugging
    descriptions_[num_log_channels_] = description;   // Copy the name for debugging
    return num_log_channels_++;
  }
  
  int LogCentralChannels::channel(const std::string name) {
    for (int i = 0; i < LOG_MAX_CHANNELS; i++) {
      if (names_[i] == name) {
        return i;
      }
    }
    return -1;
  }
  
  std::string LogCentralChannels::GetAllChannels() const
  {
    
    std::vector<std::string> channels;
    for ( int i = 0 ; i < num_log_channels_ ; i++ )
      channels.push_back( names_[i] );

    return au::str_grouped( channels );
    
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
  std::string LogCentralChannels::channel_description(int c) {
    if (c < 0) {
      return "?";
    }
    if (c >= num_log_channels_) {
      return "?";
    }
    
    return descriptions_[c];
  }

  
  // Commands to activate or dactivate channels
  std::vector<int> LogCentralChannels::Get(const std::string& str_pattern) {
    
    std::vector<int> channels;
    
    // Prepare pattern comparision
    SimplePattern pattern(str_pattern);
    
    // Search for channels
    for (int i = 0; i < LOG_MAX_CHANNELS; i++) {
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