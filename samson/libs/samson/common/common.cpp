

#include "common.h" // Own interface


namespace samson {

  void RegisterLogChannels()
  {
    // Register channels of au library
    au::logs.RegisterChannels();
    
    // Register channels for samson library
    logs.RegisterChannels();
  }

  // Channel logs
  LogChannels logs;
  
}