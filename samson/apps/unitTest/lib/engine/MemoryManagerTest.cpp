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
 * unit testing of the dMemoryManager class in the engine library
 *
 */
#include "gtest/gtest.h"
#include "au/ThreadManager.h"
#include "engine/MemoryManager.h"
#include "xmlparser/xmlParser.h"
#include "unitTest/common_engine_test.h"
#include "engine/Buffer.h"
#include "samson/client/BufferContainer.h"
#include "au/tables/Table.h"


/*
 *
 * // Old Test void add( MemoryRequest *request ); (MemoryRequest disappeared
 * TEST(memoryManagerTest, addTest)
 * {
 *
 * init_engine_test();
 *
 *  engine::Buffer *buffer = engine::Engine::memory_manager()->createBuffer( "test_buffer" , "test" , 100 );
 *  EXPECT_TRUE( buffer != NULL ) << "Buffer was not creatd";
 *  EXPECT_EQ(engine::Engine::memory_manager()->num_buffers(), 1) << "Memory request was not added";
 *
 *  EXPECT_TRUE( buffer->max_size() >= 100 ) << "Buffer size is not correct";
 *
 *  // Destroy buffer
 *  buffer->Release();
 *
 *  EXPECT_EQ(engine::Engine::memory_manager()->num_buffers(), 0) << "Buffer is not removed correctly";
 *
 *      close_engine_test();
 *
 * }
 *
 * //Test int num_buffers();
 * TEST(memoryManagerTest, num_buffersTest)
 * {
 *      init_engine_test();
 *
 *  EXPECT_EQ(engine::Engine::memory_manager()->num_buffers(), 0) << "Should be no buffers";
 *
 *  engine::Buffer* buffer1 = engine::Engine::memory_manager()->createBuffer( "buffer1" ,  "test" , 100  );
 *  EXPECT_EQ(engine::Engine::memory_manager()->num_buffers(), 1) << "Wrong number of buffers";
 *
 *  engine::Buffer* buffer2 = engine::Engine::memory_manager()->createBuffer( "buffer2" ,  "test" , 100  );
 *  EXPECT_EQ(engine::Engine::memory_manager()->num_buffers(), 2) << "Wrong number of buffers";
 *
 *  buffer1->Release();
 *  EXPECT_EQ(engine::Engine::memory_manager()->num_buffers(), 1) << "Wrong number of buffers";
 *
 *  buffer2->Release();
 *  EXPECT_EQ(engine::Engine::memory_manager()->num_buffers(), 0) << "Wrong number of buffers";
 *
 *      close_engine_test();
 *
 * }
 */



//
// used_memory
//
TEST(memoryManagerTest, used_memory) {
  init_engine_test();

  {
    samson::BufferContainer* bc   = new samson::BufferContainer();
    engine::BufferPointer    buf1 = engine::Buffer::Create("buf1", "ram", 3 * 1024);  // Added to memory_manager

    EXPECT_TRUE(buf1 != NULL);

    bc->Push("testqueue", buf1);
    size_t usedMemory = engine::Engine::memory_manager()->used_memory();
    EXPECT_EQ(3 * 1024, usedMemory);

    int bufs = engine::Engine::memory_manager()->num_buffers();
    EXPECT_EQ(1, bufs);

    double musage = engine::Engine::memory_manager()->memory_usage();
    LM_M(("memory usage: %02d%%", (int) (musage * 100)));

    bc->Pop("testqueue");
    usedMemory = engine::Engine::memory_manager()->used_memory();
    EXPECT_EQ(3 * 1024, usedMemory);  // Actually, I would like it to be empty ... :-(

    delete bc;
  }

  close_engine_test(); // Can't kill memory manager until after block ends ...
}



//
// table
//
TEST(memoryManagerTest, table) {
  init_engine_test();

  {
    engine::BufferPointer    buf1   = engine::Buffer::Create("buf1", "ram", 3 * 1024);  // Added to memory_manager
    au::tables::Table        table  = engine::Engine::memory_manager()->getTableOfBuffers();
  }

  close_engine_test();
}
