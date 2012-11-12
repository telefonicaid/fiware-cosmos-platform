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
* unit testing of the dMemoryManager class in the engine library
*
*/

#include "gtest/gtest.h"

#include "au/ThreadManager.h"

#include "engine/MemoryManager.h"
#include "engine/MemoryRequest.h"

#include "xmlparser/xmlParser.h"

#include "unitTest/common_engine_test.h"

//Test void add( MemoryRequest *request );
TEST(memoryManagerTest, addTest) 
{

   init_engine_test();

    engine::Buffer *buffer = engine::MemoryManager::shared()->createBuffer( "test_buffer" , "test" , 100 , 0.0 );
    EXPECT_TRUE( buffer != NULL ) << "Buffer was not creatd";
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 1) << "Memory request was not added";

    EXPECT_TRUE( buffer->getMaxSize() >= 100 ) << "Buffer size is not correct";
    
    // Destroy buffer
    buffer->release();

    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 0) << "Buffer is not removed correctly";

	close_engine_test();

}    

//Test int getNumBuffers();
TEST(memoryManagerTest, getNumBuffersTest) 
{
	init_engine_test();
    
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 0) << "Should be no buffers";
    
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" ,  "test" , 100  );
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 1) << "Wrong number of buffers";
    
    engine::Buffer* buffer2 = engine::MemoryManager::shared()->createBuffer( "buffer2" ,  "test" , 100  );
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 2) << "Wrong number of buffers";

    buffer1->release();
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 1) << "Wrong number of buffers";
    
    buffer2->release();
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 0) << "Wrong number of buffers";

	close_engine_test();    
    
}

//Test size_t getUsedMemory();
//Test double getMemoryUsage();
TEST(memoryManagerTest, getUsedMemoryTest) 
{
   
   init_engine_test();    
       
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemory(), 0) << "Used memory does not match";
    
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" ,  "test" , 100  );

    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemory(), 100) << "Used memory does not match";
    
    engine::Buffer* buffer2 = engine::MemoryManager::shared()->createBuffer( "buffer2" , "test" , 100  );

    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemory(), 200) << "Used memory does not match";
    
    buffer1->release();

    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemory(), 100) << "Used memory does not match";

    buffer2->release();

    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemory(), 0) << "Used memory does not match";
 
	close_engine_test();

}

    
    
//Test void runThread();        
    

