/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

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
      zoo = LOG_REGISTER_CHANNEL( "au::ZOO" , "au library : Zookeeper wrapper" );
      gpb = LOG_REGISTER_CHANNEL( "au::GPB" , "au library : Google Protocol Buffers wrapper");
    }
    
    // Channels definied in this library
    int zoo;
    int gpb;

  private:
    bool registered_;
    
  };

  // Extern variable to logeverything
  extern AULogChannels logs;
  
}

#endif