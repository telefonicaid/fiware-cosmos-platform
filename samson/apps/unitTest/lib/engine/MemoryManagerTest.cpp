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
 * FILE            memoryManager_Test.cpp
 *
 * AUTHOR          Javier Lois
 *
 * DATE            December 2011
 *
 * DESCRIPTION
 *
 * Unit testing of the MemoryManager class in the engine library
 *
 *
 * Telefonica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefonica Investigacion y Desarrollo S.A.U.
 * All rights reserved.
 */
#include "gtest/gtest.h"

#include "au/ThreadManager.h"
#include "au/tables/Table.h"
#include "engine/Buffer.h"
#include "engine/MemoryManager.h"
#include "samson/client/BufferContainer.h"
#include "unitTest/common_engine_test.h"
#include "xmlparser/xmlParser.h"

// -----------------------------------------------------------------------------
// used_memory
//
TEST(memoryManagerTest, used_memory) {
  init_engine_test();

  {
    samson::BufferContainer bufContainer;
    engine::BufferPointer buf1 =
      engine::Buffer::Create("buf1", 3 * 1024);   // Added to memory_manager

    EXPECT_TRUE(buf1 != NULL);

    bufContainer.Push("testqueue", buf1);
    size_t usedMemory = engine::Engine::memory_manager()->used_memory();
    EXPECT_EQ(3 * 1024, usedMemory);

    int bufs = engine::Engine::memory_manager()->num_buffers();
    EXPECT_EQ(1, bufs);

    double musage = engine::Engine::memory_manager()->memory_usage();
    EXPECT_NE(0, musage);

    bufContainer.Pop("testqueue");
    usedMemory = engine::Engine::memory_manager()->used_memory();
    EXPECT_EQ(3 * 1024, usedMemory);  // Actually, I would like it to be empty ... :-(
  }

  close_engine_test();  // Can't kill memory manager until after block ends ...
}

// -----------------------------------------------------------------------------
// table
//
TEST(memoryManagerTest, table) {
  init_engine_test();

  {
    engine::BufferPointer buf1   =
      engine::Buffer::Create("buf1", 3 * 1024);   // Added to memory_manager
    au::tables::Table table  =
      engine::Engine::memory_manager()->getTableOfBuffers();

    EXPECT_EQ(1, table.getNumRows());
    EXPECT_EQ(4, table.getNumColumns());
  }

  close_engine_test();
}
