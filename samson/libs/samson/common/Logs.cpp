


#include "samson/common/Logs.h" // Own interface

#include "samson/module/samson.h"
#include "au/Log.h"

namespace samson {

void RegisterLogChannels()
{
  // Register channels of au library
  au::logs.RegisterChannels();
  
  // Register channels for samson library
  logs.RegisterChannels();
  
  // Register channel for operations
  log_operations = AU_REGISTER_CHANNEL( "samson::OP" );
  
}

// Channel logs
LogChannels logs;

}