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
/* ****************************************************************************
 *
 * FILE            EngineTest.cpp
 *
 * AUTHOR          Javier Lois
 *
 * DATE            January 2012
 *
 * DESCRIPTION
 *
 * Unit testing of the Engine class in the engine library
 *
 *
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefonica Investigacion y Desarrollo S.A.U.
 * All rights reserved.
 */
#include <fcntl.h>
#include <string>

#include "gtest/gtest.h"

#include "au/ThreadManager.h"
#include "engine/DiskManager.h"
#include "engine/Engine.h"
#include "engine/MemoryManager.h"
#include "engine/NotificationListener.h"
#include "engine/ProcessManager.h"
#include "unitTest/TestClasses.h"
#include "unitTest/common_engine_test.h"

// -----------------------------------------------------------------------------
// upDown -
//
TEST(engine_Engine, upDown) {
  engine::Engine::InitEngine(4, 1000000, 2);

  engine::Engine::DestroyEngine();
  // Make sure no background threads are left
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());
}

// -----------------------------------------------------------------------------
// managers - quick test of the managers for memory, disk and process
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
  // Make sure no background threads are left there
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());

  EXPECT_EQ(NULL, engine::Engine::memory_manager());
  EXPECT_EQ(NULL, engine::Engine::disk_manager());
  EXPECT_EQ(NULL, engine::Engine::process_manager());
}

// -----------------------------------------------------------------------------
// threads -
//
TEST(engine_Engine, threads) {
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());

  engine::Engine::InitEngine(4, 1000000, 2);

  engine::Engine::DestroyEngine();
  // Make sure no background threads are left
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());
}

// -----------------------------------------------------------------------------
// gets -
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
  // Make sure no background threads are left
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());
}

// -----------------------------------------------------------------------------
// notifications -
//
TEST(engine_Engine, notifications) {
  engine::Engine::InitEngine(4, 1000000, 2);

  engine::notify("notification");
  engine::notify("notification with period", 1.0);
  engine::notify_extra("notification extra");

  engine::Engine::DestroyEngine();
  // Make sure no background threads are left there
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());
}

// -----------------------------------------------------------------------------
// restart - initialize engine twice, destroy it twice - should work
//
TEST(engine_Engine, restart) {
  engine::Engine::InitEngine(4, 1000000, 2);
  engine::Engine::InitEngine(4, 1000000, 2);  // Second init is ignored and causes a warning

  engine::Engine::DestroyEngine();

  // Make sure no background threads are left there
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());

  // Destroying the engine again is ignored and causes a warning
  engine::Engine::DestroyEngine();
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());
}
