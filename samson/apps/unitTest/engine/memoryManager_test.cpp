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

/*
 *
 * //Test void add( MemoryRequest *request );
 * TEST(memoryManagerTest, addTest)
 * {
 *
 * init_engine_test();
 *
 *  engine::Buffer *buffer = engine::MemoryManager::shared()->createBuffer( "test_buffer" , "test" , 100 );
 *  EXPECT_TRUE( buffer != NULL ) << "Buffer was not creatd";
 *  EXPECT_EQ(engine::MemoryManager::shared()->num_buffers(), 1) << "Memory request was not added";
 *
 *  EXPECT_TRUE( buffer->getMaxSize() >= 100 ) << "Buffer size is not correct";
 *
 *  // Destroy buffer
 *  buffer->Release();
 *
 *  EXPECT_EQ(engine::MemoryManager::shared()->num_buffers(), 0) << "Buffer is not removed correctly";
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
 *  EXPECT_EQ(engine::MemoryManager::shared()->num_buffers(), 0) << "Should be no buffers";
 *
 *  engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" ,  "test" , 100  );
 *  EXPECT_EQ(engine::MemoryManager::shared()->num_buffers(), 1) << "Wrong number of buffers";
 *
 *  engine::Buffer* buffer2 = engine::MemoryManager::shared()->createBuffer( "buffer2" ,  "test" , 100  );
 *  EXPECT_EQ(engine::MemoryManager::shared()->num_buffers(), 2) << "Wrong number of buffers";
 *
 *  buffer1->Release();
 *  EXPECT_EQ(engine::MemoryManager::shared()->num_buffers(), 1) << "Wrong number of buffers";
 *
 *  buffer2->Release();
 *  EXPECT_EQ(engine::MemoryManager::shared()->num_buffers(), 0) << "Wrong number of buffers";
 *
 *      close_engine_test();
 *
 * }
 *
 * //Test size_t used_memory();
 * //Test double getMemoryUsage();
 * TEST(memoryManagerTest, used_memoryTest)
 * {
 *
 * init_engine_test();
 *
 *  EXPECT_EQ(engine::MemoryManager::shared()->used_memory(), 0) << "Used memory does not match";
 *
 *  engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" ,  "test" , 100  );
 *
 *  EXPECT_EQ(engine::MemoryManager::shared()->used_memory(), 100) << "Used memory does not match";
 *
 *  engine::Buffer* buffer2 = engine::MemoryManager::shared()->createBuffer( "buffer2" , "test" , 100  );
 *
 *  EXPECT_EQ(engine::MemoryManager::shared()->used_memory(), 200) << "Used memory does not match";
 *
 *  buffer1->Release();
 *
 *  EXPECT_EQ(engine::MemoryManager::shared()->used_memory(), 100) << "Used memory does not match";
 *
 *  buffer2->Release();
 *
 *  EXPECT_EQ(engine::MemoryManager::shared()->used_memory(), 0) << "Used memory does not match";
 *
 *      close_engine_test();
 *
 * }
 *
 *
 *
 * //Test void runThread();
 *
 *
 */
