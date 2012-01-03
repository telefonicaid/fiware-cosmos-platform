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

#include "engine/MemoryManager.h"
#include "engine/MemoryRequest.h"

#include "xmlmarkup/xmlmarkup.h"

// Tests engine's instantiation
TEST(memorymanagertest, instantiationTest) {
    //access instance without initialise. Should return NULL.
    EXPECT_EQ(engine::MemoryManager::shared(), static_cast<engine::MemoryManager*>(NULL)) << "Uninitialized MemoryManager should be null";
    //call init() and then instance. Should return a valid one.
    engine::MemoryManager::init(1000);
    EXPECT_TRUE(engine::MemoryManager::shared() != static_cast<engine::MemoryManager*>(NULL)) << "MemoryManager instance should not be null after instantiation"; 
    
}

//Test void add( MemoryRequest *request );
TEST(memorymanagertest, addTest) {
    engine::Engine::init();
    engine::MemoryManager::init(1000);
    engine::MemoryRequest* request = new engine::MemoryRequest(10, 50.0, 1);
    engine::MemoryManager::shared()->add(request);
    
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 1) << "Memory request was not added";
}    

//Test void cancel( MemoryRequest *request );
//Test Buffer *newBuffer( std::string name ,  size_t size , int tag );
//Test void destroyBuffer( Buffer *b );
TEST(memorymanagertest, newBufferDestroyBufferTest) {
    engine::Engine::init();
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  100 , 1 );
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 1) << "Buffer was not created";
    engine::MemoryManager::shared()->destroyBuffer(buffer);
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 0) << "Buffer was not properly destroyed";
}    

//Test int getNumBuffers();
TEST(memorymanagertest, getNumBuffersTest) {
    engine::Engine::init();
    engine::MemoryManager::init(1000);
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 0) << "Should bew no buffers";
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  100 , 1 );
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 1) << "Wrong number of buffers";
    engine::Buffer* buffer2 = engine::MemoryManager::shared()->newBuffer( "buffer2" ,  100 , 1 );
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 2) << "Wrong number of buffers";
    engine::MemoryManager::shared()->destroyBuffer(buffer1);
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 1) << "Wrong number of buffers";
    engine::MemoryManager::shared()->destroyBuffer(buffer2);
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 0) << "Wrong number of buffers";
    
}

//Test size_t getUsedMemory();
//Test double getMemoryUsage();
TEST(memorymanagertest, getUsedMemoryTest) {
    engine::Engine::init();
    engine::MemoryManager::init(1000);
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemory(), 0) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsage(), 0.0) << "Memory usage does not match";
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  100 , 1 );
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemory(), 100) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsage(), 0.1) << "Memory usage does not match";
    engine::Buffer* buffer2 = engine::MemoryManager::shared()->newBuffer( "buffer2" ,  200 , 1 );
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemory(), 300) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsage(), 0.3) << "Memory usage does not match";
    engine::MemoryManager::shared()->destroyBuffer(buffer1);
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemory(), 200) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsage(), 0.2) << "Memory usage does not match";
    engine::MemoryManager::shared()->destroyBuffer(buffer2);
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemory(), 0) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsage(), 0.0) << "Memory usage does not match";
    
}

    
    
//Test int getNumBuffersByTag( int tag );
//Test size_t getUsedMemoryByTag( int tag );
//Test double getMemoryUsageByTag( int tag );
TEST(memorymanagertest, getNumBuffersByTagTest) {
    engine::Engine::init();
    engine::MemoryManager::init(1000);
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemoryByTag(1), 0) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsageByTag(1), 0.0) << "Memory usage does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffersByTag(1), 0) << "Number of buffers does not match";
  
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  100 , 1 );
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemoryByTag(1), 100) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsageByTag(1), 0.1) << "Memory usage does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffersByTag(1), 1) << "Number of buffers does not match";
    engine::Buffer* buffer2 = engine::MemoryManager::shared()->newBuffer( "buffer2" ,  200 , 2 );
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemoryByTag(1), 100) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsageByTag(1), 0.1) << "Memory usage does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffersByTag(1), 1) << "Number of buffers does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemoryByTag(2), 200) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsageByTag(2), 0.2) << "Memory usage does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffersByTag(2), 1) << "Number of buffers does not match";
    engine::Buffer* buffer3 = engine::MemoryManager::shared()->newBuffer( "buffer3" ,  200 , 1 );
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemoryByTag(1), 300) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsageByTag(1), 0.3) << "Memory usage does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffersByTag(1), 2) << "Number of buffers does not match";
    engine::MemoryManager::shared()->destroyBuffer(buffer1);
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemoryByTag(1), 200) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsageByTag(1), 0.2) << "Memory usage does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffersByTag(1), 1) << "Number of buffers does not match";
    engine::MemoryManager::shared()->destroyBuffer(buffer2);
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemoryByTag(1), 200) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsageByTag(1), 0.2) << "Memory usage does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffersByTag(1), 1) << "Number of buffers does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemoryByTag(2), 0) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsageByTag(2), 0.0) << "Memory usage does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffersByTag(2), 0) << "Number of buffers does not match";
    engine::MemoryManager::shared()->destroyBuffer(buffer3);
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemoryByTag(1), 0) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsageByTag(1), 0.0) << "Memory usage does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffersByTag(1), 0) << "Number of buffers does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getUsedMemoryByTag(2), 0) << "Used memory does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getMemoryUsageByTag(2), 0.0) << "Memory usage does not match";
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffersByTag(2), 0) << "Number of buffers does not match";
}


    
//Test void runThread();        
    
//Test void getInfo( std::ostringstream& output);
TEST(memorymanagertest, getInfoTest) {
    engine::Engine::init();
    engine::MemoryManager::init(1000);
  
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  100 , 1 );

    engine::MemoryRequest* request = new engine::MemoryRequest(10, 50.0, 1);
    engine::MemoryManager::shared()->add(request);

    //get info and check it is right
    std::ostringstream info;
    engine::MemoryManager::shared()->getInfo( info );
    //std::cout << info.str() << std::endl;

    CMarkup xmlData( info.str() );
    xmlData.FindElem();
    xmlData.IntoElem();
 
    //First engine_element will be notification test_notification1
    xmlData.FindElem("memory");
    EXPECT_EQ(xmlData.GetData(), "1000") << "Error in memory tag";    
    xmlData.FindElem("used_memory");
    EXPECT_EQ(xmlData.GetData(), "110") << "Error in used_memory tag";    
    xmlData.FindElem("num_buffers");
    EXPECT_EQ(xmlData.GetData(), "2") << "Error in num_buffers tag";    
    xmlData.FindElem("buffers");
    xmlData.IntoElem(); 
    xmlData.FindElem("buffer");
    xmlData.IntoElem(); 
    xmlData.FindElem("max_size");
    EXPECT_EQ(xmlData.GetData(), "100") << "Error writing max_size tag";
    xmlData.FindElem("size");
    EXPECT_EQ(xmlData.GetData(), "0") << "Error writing size tag";
    xmlData.FindElem("offset");
    EXPECT_EQ(xmlData.GetData(), "0") << "Error writing offset tag";
    xmlData.FindElem("name");
    EXPECT_EQ(xmlData.GetData(), "buffer1") << "Error writing name tag";
    xmlData.OutOfElem();
    xmlData.FindElem("buffer");
    xmlData.IntoElem(); 
    xmlData.FindElem("max_size");
    EXPECT_EQ(xmlData.GetData(), "10") << "Error writing max_size tag";
    xmlData.FindElem("size");
    EXPECT_EQ(xmlData.GetData(), "0") << "Error writing size tag";
    xmlData.FindElem("offset");
    EXPECT_EQ(xmlData.GetData(), "0") << "Error writing offset tag";
    xmlData.FindElem("name");
    EXPECT_EQ(xmlData.GetData(), "Buffer from request") << "Error writing name tag";

    engine::MemoryManager::shared()->destroyBuffer(buffer1);

}


