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

#include "au/ThreadManager.h"

#include "engine/Engine.h"
#include "engine/DiskManager.h"
#include "engine/ProcessManager.h"
#include "engine/MemoryManager.h"

#include "samson/common/samsonDirectories.h"
#include "samson/common/SamsonSetup.h"

#include "samson/client/SamsonClient.h"

void init_engine_test()
{
   
   std::string samson_home    = SAMSON_HOME_DEFAULT;
   std::string samson_working = SAMSON_WORKING_DEFAULT;
   char *env_samson_working = getenv("SAMSON_WORKING");
   char *env_samson_home = getenv("SAMSON_HOME");
   if( env_samson_working )
	  samson_working = env_samson_working;
   if( env_samson_home )
	  samson_home = env_samson_home;
   
   samson::SamsonSetup::init(samson_home, samson_working);          // Load setup and create default directories

   
   engine::Engine::init();
   engine::DiskManager::init(1);
   engine::ProcessManager::init(4);
   engine::MemoryManager::init(1000000);
}


void close_engine_test()
{
   engine::Engine::stop();                  // Stop engine
   engine::DiskManager::stop();             // Stop disk manager
   engine::ProcessManager::stop();          // Stop process manager
   
   au::ThreadManager::wait_all_threads("EngineTest");   // Wait all threads to finsih
   
   engine::DiskManager::destroy();          // Destroy Disk manager
   engine::MemoryManager::destroy();        // Destroy Memory manager
   engine::ProcessManager::destroy();       // Destroy Process manager
   engine::Engine::destroy();               // Destroy Engine

   samson::SamsonSetup::destroy();

}


samson::SamsonClient* init_samson_client_test()
{
   
   // General init of the SamsonClient library
   size_t total_memory = 64*1024*1024; // Use 64Mb for this test
   samson::SamsonClient::general_init( total_memory );
   LM_M(("general_init() returned"));

   // Create client connection
   LM_M(("creating samson_client"));
   samson::SamsonClient* samson_client = new samson::SamsonClient("SamsonClientTest");
   LM_M(("samson_client created"));

   // SamsonClient to play with
   return samson_client;

}


void close_samson_client_test( samson::SamsonClient* samson_client  )
{
   // Wait until all activity is finished
   samson_client->waitUntilFinish();

   LM_M(("waitUntilFinish finished"));

   // Disconnect from worker ( if previously connected )
   samson_client->disconnect();

   LM_M(("client disconnected"));

   // Stop engine to avoid references to samson_client
   engine::Engine::stop();

   LM_M(("engine  stopped"));

   // Remove the samson client instance
   delete samson_client;

   LM_M(("calling general_close()"));
   // General close of the SamsonClient library
   samson::SamsonClient::general_close();

   LM_M(("general_close() returned"));

   // Make sure no threads are pending to be finish
   au::ThreadManager::wait_all_threads("SamsonClientTest");   // Wait all threads to finish

}
