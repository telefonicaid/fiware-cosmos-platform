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


//Test void add( MemoryRequest *request );
TEST(memoryManagerTest, addTest) 
{
    engine::Engine::init();
    engine::MemoryManager::init(1000);

    engine::Buffer *buffer = engine::MemoryManager::shared()->newBuffer( "test_buffer" , "test" , 100 , 0.0 );
    EXPECT_TRUE( buffer != NULL ) << "Buffer was not creatd";
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 1) << "Memory request was not added";

    EXPECT_TRUE( buffer->getMaxSize() >= 100 ) << "Buffer size is not correct";
    
    // Destroy buffer
    engine::MemoryManager::shared()->destroyBuffer( buffer );

    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 0) << "Buffer is not removed correctly";
    
    engine::Engine::stop();                  // Stop engine
    au::ThreadManager::shared()->wait();     // Wait all threads to finsih
    engine::Engine::destroy();               // Destroy Engine
}    

//Test int getNumBuffers();
TEST(memoryManagerTest, getNumBuffersTest) 
{
    engine::Engine::init();
    engine::MemoryManager::init(1000);
    
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 0) << "Should be no buffers";
    
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  "test" , 100  );
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 1) << "Wrong number of buffers";
    
    engine::Buffer* buffer2 = engine::MemoryManager::shared()->newBuffer( "buffer2" ,  "test" , 100  );
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 2) << "Wrong number of buffers";
    
    engine::MemoryManager::shared()->destroyBuffer(buffer1);
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 1) << "Wrong number of buffers";
    
    engine::MemoryManager::shared()->destroyBuffer(buffer2);
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 0) << "Wrong number of buffers";
    
    engine::Engine::stop();                  // Stop engine
    au::ThreadManager::shared()->wait();     // Wait all threads to finsih
    engine::Engine::destroy();               // Destroy Engine

    
}

//Test size_t getUsedMemory();
//Test double getMemoryUsage();
TEST(memoryManagerTest, getUsedMemoryTest) 
{
    
    engine::Engine::init();
    engine::MemoryManager::init(1000);
    
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemory(), 0) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsage(), 0.0) << "Memory usage does not match";
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  "test" , 100  );
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemory(), 100) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsage(), 0.1) << "Memory usage does not match";
    engine::Buffer* buffer2 = engine::MemoryManager::shared()->newBuffer( "buffer2" , "test" , 100  );
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemory(), 200) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsage(), 0.2) << "Memory usage does not match";
    engine::MemoryManager::shared()->destroyBuffer(buffer1);
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemory(), 100) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsage(), 0.1) << "Memory usage does not match";
    engine::MemoryManager::shared()->destroyBuffer(buffer2);
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemory(), 0) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsage(), 0.0) << "Memory usage does not match";
 
    engine::Engine::stop();                  // Stop engine
    au::ThreadManager::shared()->wait();     // Wait all threads to finsih
    engine::Engine::destroy();               // Destroy Engine
    
}

    
    
//Test void runThread();        
    

