
#include "au/log/LogCentralChannelsFilter.h"  // Own interface


#include "au/log/LogCentral.h"

namespace au {
  
  
  std::string LogCentralChannelsFilter::description()
  {
    std::ostringstream output;
    
    for ( int c = 0 ; c < LOG_MAX_CHANNELS ; c++ )
    {
      if( !log_central.log_channels().IsRegistered(c) )
        continue;
      
      int level = GetLevel(c);
      if( level > 0 )
      {
        std::string name = log_central.log_channels().channel_name(c);
        output << name << ":" << Log::GetLogLevel(level) << " ";
      }
    }
    
    return output.str();
  }

  
}
