#include <fcntl.h>

#include "au/ThreadManager.h"
#include "engine/DiskManager.h"
#include "engine/Engine.h"
#include "engine/MemoryManager.h"
#include "engine/ProcessManager.h"
#include "engine/NotificationListener.h"
#include "gtest/gtest.h"
#include "unitTest/TestClasses.h"
#include "unitTest/common_engine_test.h"



//
// upDown
//
TEST(engine_Engine, upDown) {
  engine::Engine::InitEngine(4, 1000000, 2);

  engine::Engine::DestroyEngine();
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());  // Make sure no background threads are left there
}



//
// managers
//
TEST(engine_Engine, managers) {
  EXPECT_EQ(NULL, engine::Engine::shared());
  EXPECT_EQ(NULL, engine::Engine::memory_manager());
  EXPECT_EQ(NULL, engine::Engine::disk_manager());
  EXPECT_EQ(NULL, engine::Engine::process_manager());

  engine::Engine::InitEngine(4, 1000000, 2);

  EXPECT_EQ(1000000ULL, engine::Engine::memory_manager()->memory());
  EXPECT_EQ(2,          engine::Engine::disk_manager()->max_num_disk_operations());
  EXPECT_EQ(4,          engine::Engine::process_manager()->max_num_procesors());

  engine::Engine::DestroyEngine();
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());  // Make sure no background threads are left there

  EXPECT_EQ(NULL, engine::Engine::memory_manager());
  EXPECT_EQ(NULL, engine::Engine::disk_manager());
  EXPECT_EQ(NULL, engine::Engine::process_manager());
}



//
// threads
//
TEST(engine_Engine, threads) {
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());

  engine::Engine::InitEngine(4, 1000000, 2);

  engine::Engine::DestroyEngine();
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());  // Make sure no background threads are left there
}



//
// gets
//
TEST(engine_Engine, gets) {
  engine::Engine* engine;

  engine::Engine::InitEngine(4, 1000000, 2);

  engine = engine::Engine::shared();

  std::string ss = engine->GetTableOfEngineElements();
  EXPECT_TRUE(ss.c_str() != NULL);

  au::statistics::ActivityMonitor* amP = engine->activity_monitor();
  EXPECT_TRUE(amP != NULL);
  EXPECT_EQ(1, engine->GetNumElementsInEngineStack());
  EXPECT_EQ(0, engine->GetMaxWaitingTimeInEngineStack());

  engine::Engine::DestroyEngine();
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());  // Make sure no background threads are left there
}



//
// notifications
//
TEST(engine_Engine, notifications) {
  engine::Engine::InitEngine(4, 1000000, 2);

  engine::notify("notification");
  engine::notify("notification with period", 1.0);
  engine::notify_extra("notification extra");

  // engine::Engine*               engine;
  // engine::NotificationListener  nl;
  //
  // engine = engine::Engine::shared();
  // AddListenerToChannel ...

  engine::Engine::DestroyEngine();
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());  // Make sure no background threads are left there
}



//
// restart
//
TEST(engine_Engine, restart) {
  engine::Engine::InitEngine(4, 1000000, 2);
  engine::Engine::InitEngine(4, 1000000, 2); // Second init is ignored and causes a warning

  engine::Engine::DestroyEngine();
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());  // Make sure no background threads are left there

  // Destroying the engine again is ignored and causes a warning
  engine::Engine::DestroyEngine();
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());  // Make sure no background threads are left there
}
