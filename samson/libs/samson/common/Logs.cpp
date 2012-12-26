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