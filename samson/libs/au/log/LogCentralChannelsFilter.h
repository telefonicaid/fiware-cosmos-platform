
/*
 * class LogCentralChannelsFilter
 *
 * Filter to select what channels are activated or not
 *
 *
 */


#ifndef _H_LogCentralChannelsFilter
#define _H_LogCentralChannelsFilter

#include <vector>

#include "au/log/LogCommon.h"

namespace au {
class LogCentralChannelsFilter {
public:

  LogCentralChannelsFilter() {
    for (int i = 0; i < AU_LOG_MAX_CHANNELS; i++) {
      channels_level_[i] = AU_LOG_WARNING; // Warning and Errors by default
    }
  }

  inline int GetLevel(int channel ) const {
    if (channel < 0) {
      return 0;
    }
    if( channel >= AU_LOG_MAX_CHANNELS )
      return 0;
    
    return channels_level_[channel];
  }

  inline void SetLevel(int channel , int level ) {
    if (channel < 0) {
      return;
    }
    if( channel >= AU_LOG_MAX_CHANNELS )
      return;
    channels_level_[channel] = level;
  }
  
  inline void Set(const std::vector<int>& channels , int level ) {
    for (size_t i = 0; i  < channels.size(); i++) {
      SetLevel( channels[i] , level );
    }
  }

  inline bool IsLogAccepted( int channel , int level )
  {
    return ( GetLevel(channel) >= level );
  }
  
  std::string description();
  
private:

  int channels_level_[AU_LOG_MAX_CHANNELS];
};
}  // end namespace au


#endif  // ifndef _H_LogCentralChannelsFilter
