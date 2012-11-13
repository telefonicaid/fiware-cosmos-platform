
#ifndef _H_AU_LOG_CHANNELS
#define _H_AU_LOG_CHANNELS

#include <string>
#include <vector>

#include "au/string/Pattern.h"
#include "au/log/LogCommon.h"

namespace au {
  
class LogCentralChannels {
public:

  LogCentralChannels();
  ~LogCentralChannels(){}

  int RegisterChannel(const std::string& name , const std::string& description );
  std::vector<int> Get(const std::string& str_pattern);  // Commands to activate or dactivate channels

  bool IsRegistered( int channel );
  int num_channels() const;
  int channel( const std::string name );
  std::string channel_name(int c);
  std::string channel_description(int c);
  std::string GetAllChannels()const;

private:

  // Names for each channel
  std::string names_[LOG_MAX_CHANNELS];
  std::string descriptions_[LOG_MAX_CHANNELS];

  // Number of currently defined channels
  int num_log_channels_;
};
}

#endif  // ifndef _H_AU_LOG_CHANNELS