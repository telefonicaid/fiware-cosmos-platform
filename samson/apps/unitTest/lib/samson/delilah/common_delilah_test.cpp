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

  size_t memory = samson_setup->GetUInt64("general.memory");
  int num_cores = samson_setup->GetInt("general.num_processess");

  engine::Engine::InitEngine(num_cores, memory, 1);


  // Create a DelilahConsole once network is ready
  samson::DelilahConsole *delilahConsole = new samson::DelilahConsole();

  // Not connected since it depends on the test
  // delilahConsole->connect( "localhost" );

  return delilahConsole;
}

void close_delilah_test(samson::DelilahConsole *delilahConsole) {
  // Stop delilah console
  if (delilahConsole) {
    LOG_SM(("delilahConsole->stop()"));
    delilahConsole->StopConsole();
    delete delilahConsole;
    delilahConsole = NULL;
  }

  engine::Engine::StopEngine();

  // Wait all threads to finish
  LOG_SM(("From close_delilah_test, waiting all threads to finish"));
  au::Singleton<au::ThreadManager>::shared()->wait("Delilah test");

  lmCleanProgName();
}

