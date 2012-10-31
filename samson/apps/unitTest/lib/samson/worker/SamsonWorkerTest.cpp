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
#include <fcntl.h>

#include "au/ThreadManager.h"
#include "gtest/gtest.h"
#include "samson/worker/SamsonWorker.h"
#include "unitTest/TestClasses.h"

// Test disabled because of the thread count error, pending to be fixed
// TODO: @andreu Remove DISABLED when running threads be solved
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
  // TODO: @andreu read and write threads still running.
  // I have tried to stop worker network interface, but unsuccessfully
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());
}
