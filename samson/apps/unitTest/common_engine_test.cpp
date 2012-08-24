
#include "au/ThreadManager.h"

#include "engine/DiskManager.h"
#include "engine/Engine.h"
#include "engine/MemoryManager.h"
#include "engine/ProcessManager.h"

#include "samson/common/SamsonSetup.h"
#include "samson/common/samsonDirectories.h"

#include "samson/client/SamsonClient.h"

void init_engine_test() {
  std::string samson_home    = SAMSON_HOME_DEFAULT;
  std::string samson_working = SAMSON_WORKING_DEFAULT;
  char *env_samson_working = getenv("SAMSON_WORKING");
  char *env_samson_home = getenv("SAMSON_HOME");

  if (env_samson_working) {
    samson_working = env_samson_working;
  }
  if (env_samson_home) {
    samson_home = env_samson_home;
  }

  // Load setup and create default directories
  samson::SamsonSetup *samson_setup = au::Singleton<samson::SamsonSetup>::shared();
  samson_setup->SetWorkerDirectories(samson_home, samson_working);
  engine::Engine::InitEngine(4, 1000000, 1);
}

void close_engine_test() {
  engine::Engine::DestroyEngine();                      // Destroy engine
  au::ThreadManager::wait_all_threads("EngineTest");    // Wait all threads to finsih
}

samson::SamsonClient *init_samson_client_test() {
  // General init of the SamsonClient library
  size_t total_memory = 64 * 1024 * 1024;  // Use 64Mb for this test

  samson::SamsonClient::general_init(total_memory);
  LM_M(("general_init() returned"));

  // Create client connection
  LM_M(("creating samson_client"));
  samson::SamsonClient *samson_client = new samson::SamsonClient("SamsonClientTest");
  LM_M(("samson_client created"));

  if (samson_client->connect("localhost")) {
    LM_W(("Not possible to samson_client to  localhost"));  // SamsonClient to play with
  }
  return samson_client;
}

void close_samson_client_test(samson::SamsonClient *samson_client) {
  // Wait until all activity is finished
  samson_client->waitUntilFinish();

  LM_M(("waitUntilFinish finished"));

  // Disconnect from worker ( if previously connected )
  // samson_client->disconnect();

  LM_M(("client disconnected"));

  // Stop engine to avoid references to samson_client
  engine::Engine::DestroyEngine();

  LM_M(("engine  stopped"));

  // Remove the samson client instance
  delete samson_client;

  LM_M(("calling general_close()"));
  // General close of the SamsonClient library
  samson::SamsonClient::general_close();

  LM_M(("general_close() returned"));

  // Make sure no threads are pending to be finish
  au::ThreadManager::wait_all_threads("SamsonClientTest");    // Wait all threads to finish
}

