#include <fcntl.h>

#include "au/ThreadManager.h"
#include "engine/DiskManager.h"
#include "engine/Engine.h"
#include "engine/MemoryManager.h"
#include "engine/ProcessManager.h"
#include "gtest/gtest.h"
#include "unitTest/TestClasses.h"


TEST(engine_Engine, basic) {
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());
  engine::Engine::InitEngine(4, 1000000, 2);
  EXPECT_EQ(1000000, engine::Engine::memory_manager()->memory());
  EXPECT_EQ(2, engine::Engine::disk_manager()->max_num_disk_operations());
  EXPECT_EQ(4, engine::Engine::process_manager()->max_num_procesors());
  engine::Engine::DestroyEngine();
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());  // Make sure no background threads are left there

  // Repat to make sure we can re-start engine
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());
  engine::Engine::InitEngine(4, 1000000, 2);
  EXPECT_EQ(1000000, engine::Engine::memory_manager()->memory());
  EXPECT_EQ(2, engine::Engine::disk_manager()->max_num_disk_operations());
  EXPECT_EQ(4, engine::Engine::process_manager()->max_num_procesors());
  engine::Engine::DestroyEngine();
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());  // Make sure no background threads are left there
}
