/* ****************************************************************************
 *
 * FILE           SharedMemoryManager_test.cpp
 *
 * AUTHOR         Grant Croker
 *
 * DATE           May 2012
 *
 * DESCRIPTION
 *
 * Unit tests for the engine::SharedMemoryManager class
 *
 */

#include "gtest/gtest.h"

#include "au/ErrorManager.h"
#include "au/ThreadManager.h"
#include "samson/client/SamsonClient.h"
#include "samson/common/ports.h"

#include "engine/DiskManager.h"
#include "engine/Engine.h"
#include "engine/MemoryManager.h"
#include "engine/ProcessManager.h"
#include "samson/isolated/SharedMemoryManager.h"

#include "unitTest/common_engine_test.h"

class SharedMemoryManagerTest : public ::testing::Test {
protected:
  virtual void SetUp() {
    init_engine_test();
  }

  virtual void TearDown() {
    close_engine_test();
  }
};


// void SharedMemoryManager::init(
//          int _shared_memory_num_buffers , size_t _shared_memory_size_per_buffer )

TEST_F(SharedMemoryManagerTest, init) {
  engine::SharedMemoryManager *sharedMemoryManager = NULL;

  engine::SharedMemoryManager::init(1, 64000000);        // Allocate a single 64MB buffer
  sharedMemoryManager = engine::SharedMemoryManager::shared();
  EXPECT_TRUE(sharedMemoryManager != NULL) << "Error initializing the shared memory segment";
  engine::SharedMemoryManager::destroy();
}
