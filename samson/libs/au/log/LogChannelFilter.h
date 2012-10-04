
/*
 * class LogChannelFilter
 *
 * Filter to select what channels are activated or not
 *
 *
 */


#ifndef _H_LogChannelFilter
#define _H_LogChannelFilter

#include <vector>
#include "au/log/LogCommon.h"

namespace au {
  class LogChannelFilter
  {
    
  public:
    
    LogChannelFilter()
    {
      // By default all channels are open
      for (int i = 0 ; i < AU_LOG_MAX_CHANNELS ; i++)
        channels_[i] = true;
    }
    
    inline bool IsChannelActivated(int c)
    {
      if( c < 0)
        return false;
      return channels_[c];
    }
    
    inline void Add(int c)
    {
      if( c < 0)
        return;
      channels_[c] = true;
    }
    
    inline void Remove(int c)
    {
      if( c < 0)
        return;
      channels_[c] = false;
    }
    
    inline void Add( const std::vector<int>& channels)
    {
      for ( size_t i = 0 ; i  < channels.size() ; i++)
        channels_[ channels[i] ] = true;
    }
    inline void Remove( const std::vector<int>& channels)
    {
      for ( size_t i = 0 ; i  < channels.size() ; i++)
        channels_[ channels[i] ] = false;
    }
    
    
  private:
    
    bool channels_[AU_LOG_MAX_CHANNELS];
    
  };

  
} // end namespace au


#endif
