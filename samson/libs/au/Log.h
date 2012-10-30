
#ifndef AU_LOG_H
#define AU_LOG_H

#include <string>

#include "au/log/LogMain.h"

namespace au {
  
  class AULogChannels
  {
    
  public:
    
    AULogChannels()
    {
      // By default, channels are not registered
      registered_ = false;
      
      // Default to -1 to do not emit if not registered
      zoo = -1;
      
    };

    void RegisterChannels()
    {
      if( registered_ )
        return;
      registered_ = true;
      zoo = AU_LR( "au::zookeeper" , "ZK" );
    }
    
    // Channels definied in this library
    int zoo;

  private:
    bool registered_;
    
  };

  // Extern variable to logeverything
  extern AULogChannels logs;
  
}

#endif