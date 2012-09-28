#include <fcntl.h>

#include "au/ThreadManager.h"
#include "gtest/gtest.h"
#include "samson/worker/SamsonWorker.h"
#include "unitTest/TestClasses.h"


TEST(DISABLED_samson_worker_SamsonWorker, basic) {
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());


  // Init engine
  engine::Engine::InitEngine(4, 10000000, 1);

  // Create a worker
  samson::SamsonWorker *worker = new samson::SamsonWorker("no_zk_host", 9000, 9001);

  // Stop engine
  engine::Engine::StopEngine();

  // Remove worker
  delete worker;

  // Destroying engine
  engine::Engine::DestroyEngine();

  // Make sure no background threads are left there
  // TODO: read and write threads still running.
  // I have tried to stop worker network interface, but unsuccessfully
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());
}
