


#include "samson/common/Logs.h" // Own interface

#include "samson/module/samson.h"
#include "au/Log.h"
#include "engine/Logs.h"

namespace samson {

void RegisterLogChannels()
{
  // Register channels of au library
  au::logs.RegisterChannels();
  engine::logs.RegisterChannels();
  
  // Register channels for samson library
  logs.RegisterChannels();
  
  // Register channel for operations
  log_operations = LOG_REGISTER_CHANNEL( "samson::OP" , "samson library : Third party operations" );
  
}

// Channel logs
LogChannels logs;

}