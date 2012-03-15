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

#include "xmlparser/xmlParser.h"

// Tests engine's instantiation
TEST(memoryManagerTest, instantiationTest) {
    //access instance without initialise. Should return NULL.
    EXPECT_EQ(engine::MemoryManager::shared(), static_cast<engine::MemoryManager*>(NULL)) << "Uninitialized MemoryManager should be null";
    //call init() and then instance. Should return a valid one.
    engine::MemoryManager::init(1000);
    EXPECT_TRUE(engine::MemoryManager::shared() != static_cast<engine::MemoryManager*>(NULL)) << "MemoryManager instance should not be null after instantiation"; 
    
}

//Test void getInfo( std::ostringstream& output);
TEST(memoryManagerTest, getInfoTest) {
    //engine::Engine::init();
    engine::MemoryManager::init(1000);
  
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  "test", 100  );

    engine::MemoryRequest* request = new engine::MemoryRequest(10, 50.0, 1);
    engine::MemoryManager::shared()->add(request);

    //get info and check it is right
    std::ostringstream info;
    engine::MemoryManager::shared()->getInfo( info );

    XMLNode xMainNode=XMLNode::parseString(info.str().c_str(),"memory_manager");
 
    EXPECT_EQ(std::string(xMainNode.getChildNode("memory").getClear().lpszValue), "1000") << "Error in memory tag";    
    EXPECT_EQ(std::string(xMainNode.getChildNode("used_memory").getClear().lpszValue), "110") << "Error in used_memory tag";    
    EXPECT_EQ(std::string(xMainNode.getChildNode("num_buffers").getClear().lpszValue), "2") << "Error in num_buffers tag";    
    XMLNode buffersNode = xMainNode.getChildNode("buffers");
    //There has to be two buffers: "buffer from request" and "buffer1", but the order can vary, so we need to find out
    XMLNode buffer1Node = xMainNode.getChildNode("buffers").getChildNode("buffer", 0);
    ASSERT_TRUE(!buffer1Node.isEmpty());
    XMLNode buffer2Node = xMainNode.getChildNode("buffers").getChildNode("buffer", 1);
    ASSERT_TRUE(!buffer2Node.isEmpty());
    //If the order was not right, swap them
    if(strcmp(buffer1Node.getChildNode("name").getClear().lpszValue, "buffer1") != 0)
    {
        XMLNode tmp = buffer1Node;
        buffer1Node = buffer2Node;
        buffer2Node = tmp; 
    }
    EXPECT_EQ(std::string(buffer1Node.getChildNode("max_size").getClear().lpszValue), "100") << "Error writing max_size tag";
    EXPECT_EQ(std::string(buffer1Node.getChildNode("size").getClear().lpszValue), "0") << "Error writing size tag";
    EXPECT_EQ(std::string(buffer1Node.getChildNode("offset").getClear().lpszValue), "0") << "Error writing offset tag";
    EXPECT_EQ(std::string(buffer1Node.getChildNode("name").getClear().lpszValue), "buffer1") << "Error writing name tag";
    EXPECT_EQ(std::string(buffer2Node.getChildNode("max_size").getClear().lpszValue), "10") << "Error writing max_size tag";
    EXPECT_EQ(std::string(buffer2Node.getChildNode("size").getClear().lpszValue), "0") << "Error writing size tag";
    EXPECT_EQ(std::string(buffer2Node.getChildNode("offset").getClear().lpszValue), "0") << "Error writing offset tag";
    EXPECT_EQ(std::string(buffer2Node.getChildNode("name").getClear().lpszValue), "Buffer from general request") << "Error writing name tag";

    engine::MemoryManager::shared()->destroyBuffer(buffer1);

}

//Test void add( MemoryRequest *request );
TEST(memoryManagerTest, addTest) {
    //engine::Engine::init();
    engine::MemoryManager::init(1000);
    engine::MemoryRequest* request = new engine::MemoryRequest(0, 50.0, 1);
    engine::MemoryManager::shared()->add(request);
    
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 1) << "Memory request was not added";

    //get MemoryRequest info and check it is right
    std::ostringstream info;
    request->getInfo( info );

    XMLNode xMainNode=XMLNode::parseString(info.str().c_str(),"memory_request");
    EXPECT_TRUE(!xMainNode.isEmpty());
    
}    

//Test void cancel( MemoryRequest *request );
//Test Buffer *newBuffer( std::string name ,  size_t size , int tag );
//Test void destroyBuffer( Buffer *b );
TEST(memoryManagerTest, newBufferDestroyBufferTest) {
    //engine::Engine::init();
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  "test" , 100  );
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 1) << "Buffer was not created";
    engine::MemoryManager::shared()->destroyBuffer(buffer);
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 0) << "Buffer was not properly destroyed";
}    

//Test int getNumBuffers();
TEST(memoryManagerTest, getNumBuffersTest) {
    //engine::Engine::init();
    engine::MemoryManager::init(1000);
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 0) << "Should bew no buffers";
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  "test" , 100  );
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 1) << "Wrong number of buffers";
    engine::Buffer* buffer2 = engine::MemoryManager::shared()->newBuffer( "buffer2" ,  "test" , 100  );
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 2) << "Wrong number of buffers";
    engine::MemoryManager::shared()->destroyBuffer(buffer1);
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 1) << "Wrong number of buffers";
    engine::MemoryManager::shared()->destroyBuffer(buffer2);
    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 0) << "Wrong number of buffers";
    
}

//Test size_t getUsedMemory();
//Test double getMemoryUsage();
TEST(memoryManagerTest, getUsedMemoryTest) {
    //engine::Engine::init();
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
    
}

    
    
//Test void runThread();        
    

