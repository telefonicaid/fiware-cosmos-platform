
#include "au/ThreadManager.h"

#include "engine/Engine.h"
#include "engine/DiskManager.h"
#include "engine/ProcessManager.h"
#include "engine/MemoryManager.h"

#include "samson/common/samsonDirectories.h"
#include "samson/common/SamsonSetup.h"

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
