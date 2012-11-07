
#ifndef ENGINE_LOG_H
#define ENGINE_LOG_H

#include <string>

#include "au/log/LogMain.h"

namespace engine {
  
  class EngineLogChannels
  {
    
  public:
    
    EngineLogChannels()
    {
      // By default, channels are not registered
      registered_ = false;
      
      // Default to -1 to do not emit if not registered
      buffer = -1;
      
    };
    
    void RegisterChannels()
    {
      if( registered_ )
        return;
      registered_ = true;
      
      engine = LOG_REGISTER_CHANNEL( "engine"    , "engine lib: General channel");;
      buffer = LOG_REGISTER_CHANNEL( "engine::B" , "engine lib: Memory Buffer");
      notifications = LOG_REGISTER_CHANNEL( "engine::N" , "engine lib: Notifications" );;
      disk_manager = LOG_REGISTER_CHANNEL( "engine::DM" , "engine lib: Disk Manager");;
      memory_manager = LOG_REGISTER_CHANNEL( "engine::MM" , "engine lib: Memory Manager" );;
      process_manager = LOG_REGISTER_CHANNEL( "engine::PM", "engine lib: ProcessManager" );;
      
    }
    
    // Channels definied in this library
    int engine;
    int buffer;
    int notifications;
    int disk_manager;
    int memory_manager;
    int process_manager;
    
  private:
    bool registered_;
    
  };
  
  // Extern variable to logeverything
  extern EngineLogChannels logs;
  
}

#endif