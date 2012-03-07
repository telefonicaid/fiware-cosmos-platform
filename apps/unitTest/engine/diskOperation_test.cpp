/* ****************************************************************************
*
* FILE            diskOperationTest.cpp
*
* AUTHOR          Javier Lois
*
* DATE            December 2011
*
* DESCRIPTION
*
* unit testing of the diskOperation class in the engine library
*
*/

#include "gtest/gtest.h"

#include "engine/DiskManager.h"

#include "xmlparser/xmlParser.h"

//Test void getInfo( std::ostringstream& output);
TEST(diskOperationTest, getInfoTest) {
    engine::Engine::init();
    engine::DiskManager::init(10);
    char charBuffer[1024*1024];
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( charBuffer , "test_data/test_data.txt" , 3 , 5, 0 );
    
    std::ostringstream info;
    operation->getInfo( info );
    
    //parse and check xml
    XMLNode xMainNode=XMLNode::parseString(info.str().c_str(),"disk_operation");
    EXPECT_EQ(std::string(xMainNode.getChildNode("type").getText()), "read") << "Error writing read tag";
    EXPECT_EQ(std::string(xMainNode.getChildNode("file_name").getText()), "test_data/test_data.txt") << "Error writing file_name tag";
    EXPECT_EQ(std::string(xMainNode.getChildNode("size").getText()), "5") << "Error writing size tag";
    EXPECT_EQ(std::string(xMainNode.getChildNode("offset").getText()), "3") << "Error writing offset tag";
}

//Test static DiskOperation* newReadOperation( char *data , std::string fileName , size_t offset , size_t size , size_t _listenerId  );
TEST(diskOperationTest, newReadOperationTest) {
    engine::Engine::init();
    engine::DiskManager::init(10);
    char buffer[1024*1024];
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( buffer , "test_data/test_data.txt" , 3 , 6, 2 );
    
    
    EXPECT_EQ(operation->getType(), engine::DiskOperation::read) << "Wrong type value";

    //parse and check xml for the rest of values
    std::ostringstream info;
    operation->getInfo( info );
    XMLNode xMainNode=XMLNode::parseString(info.str().c_str(),"disk_operation");
    EXPECT_EQ(std::string(xMainNode.getChildNode("file_name").getText()), "test_data/test_data.txt") << "Wrong file_name in constructor";
    EXPECT_EQ(std::string(xMainNode.getChildNode("size").getText()), "6") << "Wrong size in constructor";
    EXPECT_EQ(std::string(xMainNode.getChildNode("offset").getText()), "3") << "Wrong offset in constructor";
}

//Test static DiskOperation* newWriteOperation( Buffer* buffer ,  std::string fileName , size_t _listenerId  );
TEST(diskOperationTest, newWriteOperationTest) {
    engine::Engine::init();
    engine::DiskManager::init(10);
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    engine::DiskOperation* operation = engine::DiskOperation::newWriteOperation( buffer , "test_data/test_data.txt" , 2 );
    
    
    EXPECT_EQ(operation->getType(), engine::DiskOperation::write) << "Wrong type value";

    //parse and check xml for the rest of values
    std::ostringstream info;
    operation->getInfo( info );
    XMLNode xMainNode=XMLNode::parseString(info.str().c_str(),"disk_operation");
    EXPECT_EQ(std::string(xMainNode.getChildNode("file_name").getText()), "test_data/test_data.txt") << "Wrong file_name in constructor";
    EXPECT_EQ(std::string(xMainNode.getChildNode("size").getText()), "0") << "Wrong size in constructor";
    EXPECT_EQ(std::string(xMainNode.getChildNode("offset").getText()), "0") << "Wrong offset in constructor";
    
}

//Test static DiskOperation* newAppendOperation( Buffer* buffer ,  std::string fileName , size_t _listenerId  );
TEST(diskOperationTest, newAppendOperationTest) {
    engine::Engine::init();
    engine::DiskManager::init(10);
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    engine::DiskOperation* operation = engine::DiskOperation::newAppendOperation( buffer , "test_filename.txt" , 2 );
    
    
    EXPECT_EQ(operation->getType(), engine::DiskOperation::append) << "Wrong type value";

    //parse and check xml for the rest of values
    std::ostringstream info;
    operation->getInfo( info );
    XMLNode xMainNode=XMLNode::parseString(info.str().c_str(),"disk_operation");
    EXPECT_EQ(std::string(xMainNode.getChildNode("file_name").getText()), "test_filename.txt") << "Wrong file_name in constructor";
    EXPECT_EQ(std::string(xMainNode.getChildNode("size").getText()), "0") << "Wrong size in constructor";
    EXPECT_EQ(std::string(xMainNode.getChildNode("offset").getText()), "0") << "Wrong offset in constructor";
}

//Test static DiskOperation* newRemoveOperation( std::string fileName, size_t _listenerId );
TEST(diskOperationTest, newRemoveOperationTest) {
    engine::Engine::init();
    engine::DiskManager::init(10);
    engine::DiskOperation* operation = engine::DiskOperation::newRemoveOperation( "test_filename.txt" , 2 );
    
    EXPECT_EQ(operation->getType(), engine::DiskOperation::remove) << "Wrong type value";

    //parse and check xml for the rest of values
    std::ostringstream info;
    operation->getInfo( info );
    XMLNode xMainNode=XMLNode::parseString(info.str().c_str(),"disk_operation");
    EXPECT_EQ(std::string(xMainNode.getChildNode("file_name").getText()), "test_filename.txt") << "Wrong file_name in constructor";
    EXPECT_EQ(std::string(xMainNode.getChildNode("size").getText()), "0") << "Wrong size in constructor";
}

//Test static DiskOperation * newReadOperation( std::string _fileName , size_t _offset , size_t _size ,  SimpleBuffer simpleBuffer , size_t _listenerId );
TEST(diskOperationTest, newReadOperation2Test) {
    engine::Engine::init();
    engine::DiskManager::init(10);
    engine::MemoryManager::init(1000);
    char data[100];
    engine::SimpleBuffer buffer(data, 100);
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( "test_filename.txt" , 3 , 6, buffer, 2 );
    
    
    EXPECT_EQ(operation->getType(), engine::DiskOperation::read) << "Wrong type value";

    //parse and check xml for the rest of values
    std::ostringstream info;
    operation->getInfo( info );
    XMLNode xMainNode=XMLNode::parseString(info.str().c_str(),"disk_operation");
    EXPECT_EQ(std::string(xMainNode.getChildNode("file_name").getText()), "test_filename.txt") << "Wrong file_name in constructor";
    EXPECT_EQ(std::string(xMainNode.getChildNode("size").getText()), "6") << "Wrong size in constructor";
    EXPECT_EQ(std::string(xMainNode.getChildNode("offset").getText()), "3") << "Wrong offset in constructor";
}
    
    
//Test static std::string directoryPath( std::string path );
TEST(diskOperationTest, directoryPathTest) {
    EXPECT_EQ(engine::DiskOperation::directoryPath("/dirname1/dirname2/filename.txt"), "/dirname1/dirname2") << "Bad path extraction";
    EXPECT_EQ(engine::DiskOperation::directoryPath("dirname1/filename.txt"), "dirname1") << "Bad path extraction";
    EXPECT_EQ(engine::DiskOperation::directoryPath("filename.txt"), ".") << "No path specified, should return \".\"";

}

//Test void setError( std::string message );
TEST(diskOperationTest, setErrorTest) {
    engine::Engine::init();
    engine::DiskManager::init(10);
    char buffer[1024*1024];
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 3 , 6, 2 );

    operation->setError("Error test");
    EXPECT_EQ(operation->error.getMessage(), "Error test ( Read from file: 'test_filename.txt' Size:   6 B [6B] Offset:3 )");
}
    
//Test std::string getDescription();
TEST(diskOperationTest, getDescriptionTest) {
    engine::Engine::init();
    engine::DiskManager::init(10);
    char charBuffer[1024*1024];
    engine::MemoryManager::init(1000);
    engine::Buffer* engineBuffer = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    
    engine::DiskOperation* operation1 = engine::DiskOperation::newReadOperation( charBuffer , "test_filename.txt" , 3 , 6, 2 );
    engine::DiskOperation* operation2 = engine::DiskOperation::newWriteOperation( engineBuffer , "test_filename.txt" , 2 );
    engine::DiskOperation* operation3 = engine::DiskOperation::newAppendOperation( engineBuffer , "test_filename.txt" , 2 );
    engine::DiskOperation* operation4 = engine::DiskOperation::newRemoveOperation( "test_filename.txt" , 2 );
    
    EXPECT_EQ(operation1->getDescription(), "Read from file: 'test_filename.txt' Size:   6 B [6B] Offset:3") << "Description error";
    EXPECT_EQ(operation2->getDescription(), "Write to file: 'test_filename.txt' Size:   0 B") << "Description error";
    EXPECT_EQ(operation3->getDescription(), "Append to file: 'test_filename.txt' Size:   0 B") << "Description error";
    EXPECT_EQ(operation4->getDescription(), "Remove file: 'test_filename.txt'") << "Description error";
}

//Test std::string getShortDescription();
TEST(diskOperationTest, getShortDescriptionTest) {
    engine::Engine::init();
    engine::DiskManager::init(10);
    char charBuffer[1024*1024];
    engine::MemoryManager::init(1000);
    engine::Buffer* engineBuffer = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    
    engine::DiskOperation* operation1 = engine::DiskOperation::newReadOperation( charBuffer , "test_filename.txt" , 3 , 6, 2 );
    engine::DiskOperation* operation2 = engine::DiskOperation::newWriteOperation( engineBuffer , "test_filename.txt" , 2 );
    engine::DiskOperation* operation3 = engine::DiskOperation::newAppendOperation( engineBuffer , "test_filename.txt" , 2 );
    engine::DiskOperation* operation4 = engine::DiskOperation::newRemoveOperation( "test_filename.txt" , 2 );
    
    EXPECT_EQ(operation1->getShortDescription(), "R:   6 ") << "Description error";
    EXPECT_EQ(operation2->getShortDescription(), "W:   0 ") << "Description error";
    EXPECT_EQ(operation3->getShortDescription(), "A:   0 ") << "Description error";
    EXPECT_EQ(operation4->getShortDescription(), "X") << "Description error";
} 
  
//Test DiskOperationType getType()
TEST(diskOperationTest, getTypeTest) {
    engine::Engine::init();
    engine::DiskManager::init(10);
    char charBuffer[1024*1024];
    engine::MemoryManager::init(1000);
    engine::Buffer* engineBuffer = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    
    engine::DiskOperation* operation1 = engine::DiskOperation::newReadOperation( charBuffer , "test_filename.txt" , 3 , 6, 2 );
    engine::DiskOperation* operation2 = engine::DiskOperation::newWriteOperation( engineBuffer , "test_filename.txt" , 2 );
    engine::DiskOperation* operation3 = engine::DiskOperation::newAppendOperation( engineBuffer , "test_filename.txt" , 2 );
    engine::DiskOperation* operation4 = engine::DiskOperation::newRemoveOperation( "test_filename.txt" , 2 );
    
    EXPECT_EQ(operation1->getType(), engine::DiskOperation::read) << "Wrong type value";
    EXPECT_EQ(operation2->getType(), engine::DiskOperation::write) << "Wrong type value";
    EXPECT_EQ(operation3->getType(), engine::DiskOperation::append) << "Wrong type value";
    EXPECT_EQ(operation4->getType(), engine::DiskOperation::remove) << "Wrong type value";

}

//Test size_t getSize()
TEST(diskOperationTest, getSizeTest) {
    engine::Engine::init();
    engine::DiskManager::init(10);
    char charBuffer[1024*1024];
    engine::MemoryManager::init(1000);
    engine::Buffer* engineBuffer = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    
    engine::DiskOperation* operation1 = engine::DiskOperation::newReadOperation( charBuffer , "test_filename.txt" , 3 , 5, 2 );
    engine::DiskOperation* operation2 = engine::DiskOperation::newWriteOperation( engineBuffer , "test_filename.txt" , 2 );

    EXPECT_EQ(operation1->getSize(), 5) << "Error in getSize()";
    EXPECT_EQ(operation2->getSize(), 0) << "Error in getSize()";
}

//Test void destroyBuffer();
TEST(diskOperationTest, destroyBufferTest) {
    engine::Engine::init();
    engine::DiskManager::init(10);
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    engine::DiskOperation* operation = engine::DiskOperation::newWriteOperation( buffer , "test_filename.txt" , 2 );
  
    operation->destroyBuffer();

    EXPECT_EQ(engine::MemoryManager::shared()->getNumBuffers(), 0) << "Buffer was not destroyed";
}

//Test bool compare( DiskOperation *operation );
TEST(diskOperationTest, compareTest) {
    engine::Engine::init();
    engine::DiskManager::init(10);
     char charBuffer[1024*1024];
    engine::MemoryManager::init(1000);
    engine::Buffer* engineBuffer = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    
    engine::DiskOperation* operationRead1 = engine::DiskOperation::newReadOperation( charBuffer , "test_filename.txt" , 3 , 5, 2 );
    engine::DiskOperation* operationRead2 = engine::DiskOperation::newReadOperation( charBuffer , "test_filename.txt" , 3 , 5, 2 );
    engine::DiskOperation* operationWrite1 = engine::DiskOperation::newWriteOperation( engineBuffer , "test_filename.txt" , 2 );
    engine::DiskOperation* operationWrite2 = engine::DiskOperation::newWriteOperation( engineBuffer , "test_filename.txt" , 2 );
    engine::DiskOperation* operationAppend = engine::DiskOperation::newAppendOperation( engineBuffer , "test_filename.txt" , 2 );
    
    //with read operation always return false
    EXPECT_TRUE(operationRead1->compare(operationRead2)==false); 
    EXPECT_TRUE(operationRead1->compare(operationWrite1)==false); 
    EXPECT_TRUE(operationRead1->compare(operationAppend)==false); 
    //write/append operation returns true if compared operation is not write or append. Otherwise return false
    EXPECT_TRUE(operationWrite1->compare(operationRead1)); 
    EXPECT_TRUE(operationAppend->compare(operationRead1)); 
    EXPECT_TRUE(operationWrite1->compare(operationWrite2) == false); 
    EXPECT_TRUE(operationWrite1->compare(operationAppend) == false); 
    EXPECT_TRUE(operationAppend->compare(operationWrite1) == false); 
    
}    

//Test void addListener( size_t id )
    

