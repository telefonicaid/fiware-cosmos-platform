
#include "au/ThreadManager.h"

#include "au/mutex/LockDebugger.h"            // au::LockDebugger
#include "engine/DiskManager.h"
#include "engine/Engine.h"
#include "engine/MemoryManager.h"
#include "engine/ProcessManager.h"
#include "samson/module/ModulesManager.h"       // samson::ModulesManager


#include "logMsg/logMsg.h"

#include "samson/delilah/DelilahConsole.h"


samson::DelilahConsole *init_delilah_test() {
  // Init samson setup with default values
  samson::SamsonSetup *samson_setup = au::Singleton<samson::SamsonSetup>::shared();

  size_t memory = samson_setup->getUInt64("general.memory");
  int num_cores = samson_setup->getInt("general.num_processess");

  engine::Engine::InitEngine(num_cores, memory, 1);

  samson::ModulesManager::init("delilah_test");           // Init the modules manager

  // Create a DelilahConsole once network is ready
  samson::DelilahConsole *delilahConsole = new samson::DelilahConsole();

  // Not connected since it depends on the test
  // delilahConsole->connect( "localhost" );

  return delilahConsole;
}

void close_delilah_test(samson::DelilahConsole *delilahConsole) {
  engine::Engine::StopEngine();

  if (delilahConsole) {
    LM_M(("delilahConsole->stop()"));
    delilahConsole->stop();
  }

  // Wait all threads to finish
  LM_M(("From close_delilah_test, waiting all threads to finish"));
  au::Singleton<au::ThreadManager>::shared()->wait("Delilah test");

  // Clear google protocol buffers library
  // google::protobuf::ShutdownProtobufLibrary();

  if (delilahConsole != NULL) {
    delete delilahConsole;
    delilahConsole = NULL;
  }

  engine::Engine::DestroyEngine();

  LM_W(("Finishing delilah"));

  LM_W(("Calling to destroy ModulesManager"));
  samson::ModulesManager::destroy("delilah_test");
  lmCleanProgName();
}

