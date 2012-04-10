
#include "au/ThreadManager.h"

#include "engine/Engine.h"
#include "engine/DiskManager.h"
#include "engine/ProcessManager.h"
#include "engine/MemoryManager.h"

void init_engine_test()
{
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
    
    au::ThreadManager::wait_all_threads();   // Wait all threads to finsih
    
    engine::DiskManager::destroy();          // Destroy Disk manager
    engine::MemoryManager::destroy();        // Destroy Memory manager
    engine::ProcessManager::destroy();       // Destroy Process manager
    engine::Engine::destroy();               // Destroy Engine
}
