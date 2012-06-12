
#include "au/ThreadManager.h"

#include "engine/Engine.h"
#include "engine/DiskManager.h"
#include "engine/ProcessManager.h"
#include "engine/MemoryManager.h"
#include "samson/module/ModulesManager.h"       // samson::ModulesManager
#include "au/mutex/LockDebugger.h"            // au::LockDebugger


#include "logMsg/logMsg.h"

#include "samson/delilah/DelilahConsole.h"

extern size_t delilah_random_code;

samson::DelilahConsole *init_delilah_test()
{
   // Random code for delilah
   delilah_random_code = au::code64_rand();

    // Make sure this singleton is created just once
    au::LockDebugger::shared();

    // Init samson setup with default values
    samson::SamsonSetup::init("","");

    engine::Engine::init();
    engine::MemoryManager::init(samson::SamsonSetup::shared()->getUInt64("general.memory"));
    engine::DiskManager::init(1);
    engine::ProcessManager::init(samson::SamsonSetup::shared()->getInt("general.num_processess"));

    samson::ModulesManager::init("delilah_test");         // Init the modules manager

    // Create a DelilahConsole once network is ready
    samson::DelilahConsole *delilahConsole = new samson::DelilahConsole();

	// Not connect since it depends on the test
    //delilahConsole->connect( "localhost" , 1234 , "anonymous" , "empty" );

    return delilahConsole;
}

void close_delilah_test(samson::DelilahConsole *delilahConsole)
{
    engine::Engine::stop();                  // Stop engine
    engine::DiskManager::stop();             // Stop disk manager
    engine::ProcessManager::stop();          // Stop process manager

    if( delilahConsole )
         delilahConsole->stop();
    
    // Wait all threads to finish
    LM_M(("From close_delilah_test, waiting all threads to finish"));
    au::ThreadManager::shared()->wait("Delilah test");

    // Clear google protocol buffers library
    //google::protobuf::ShutdownProtobufLibrary();

    if (delilahConsole != NULL)
    {
        delete delilahConsole;
        delilahConsole = NULL;
    }
    
    LM_W(("Finishing delilah"));
    au::LockDebugger::destroy();
    LM_W(("Calling to destroy ModulesManager"));
    samson::ModulesManager::destroy("delilah_test");
    engine::ProcessManager::destroy();
    engine::DiskManager::destroy();
    engine::MemoryManager::destroy();
    engine::Engine::destroy();
    samson::SamsonSetup::destroy();
    lmCleanProgName();
}
