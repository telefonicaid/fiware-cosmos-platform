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
#include "engine/DiskOperation.h"
#include "xmlparser/xmlParser.h"

#include "unitTest/common_engine_test.h"

/*
 *
 * //Test void getInfo( std::ostringstream& output);
 * TEST(diskOperationTest, getInfoTest)
 * {
 *  init_engine_test();
 *
 *  char charBuffer[1024*1024];
 *  engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( charBuffer , "test_data/test_data.txt" , 3 , 5, 0 );
 *  EXPECT_EQ(operation->getType(), engine::DiskOperation::read) << "Wrong type value";
 *
 *  close_engine_test();
 *
 * }
 *
 * //Test static DiskOperation* newWriteOperation( Buffer* buffer ,  std::string fileName , size_t _listenerId  );
 * TEST(diskOperationTest, newWriteOperationTest)
 * {
 *  init_engine_test();
 *
 *  engine::Buffer* buffer = engine::MemoryManager::shared()->createBuffer( "buffer1" , "test" , 15 );
 *  engine::DiskOperation* operation = engine::DiskOperation::newWriteOperation( buffer , "test_data/test_data.txt" , 2 );
 *  buffer->Release();
 *
 *  EXPECT_EQ(operation->getType(), engine::DiskOperation::write) << "Wrong type value";
 *
 *  close_engine_test();
 *
 * }
 *
 * //Test static DiskOperation* newAppendOperation( Buffer* buffer ,  std::string fileName , size_t _listenerId  );
 * TEST(diskOperationTest, newAppendOperationTest)
 * {
 *  init_engine_test();
 *
 *  engine::Buffer* buffer = engine::MemoryManager::shared()->createBuffer( "buffer1" , "test",  15 );
 *  engine::DiskOperation* operation = engine::DiskOperation::newAppendOperation( buffer , "test_filename.txt" , 2 );
 *  buffer->Release();
 *
 *  EXPECT_EQ(operation->getType(), engine::DiskOperation::append) << "Wrong type value";
 *
 *  close_engine_test();
 * }
 *
 * //Test static DiskOperation* newRemoveOperation( std::string fileName, size_t _listenerId );
 * TEST(diskOperationTest, newRemoveOperationTest)
 * {
 *  init_engine_test();
 *
 *  engine::DiskOperation* operation = engine::DiskOperation::newRemoveOperation( "test_data/test_data.txt", 1 );
 *
 *  EXPECT_EQ(operation->getType(), engine::DiskOperation::remove) << "Wrong type value";
 *
 *  close_engine_test();
 * }
 *
 * //Test static DiskOperation * newReadOperation( std::string _fileName , size_t _offset , size_t _size ,  SimpleBuffer simpleBuffer , size_t _listenerId );
 * TEST(diskOperationTest, newReadOperation2Test)
 * {
 *  init_engine_test();
 *
 *  char data[100];
 *  engine::SimpleBuffer buffer(data, 100);
 *  engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( "test_filename.txt" , 3 , 6, buffer, 2 );
 *
 *  EXPECT_EQ(operation->getType(), engine::DiskOperation::read) << "Wrong type value";
 *
 *  close_engine_test();
 * }
 *
 * //Test std::string getDescription();
 * TEST(diskOperationTest, getDescriptionTest)
 * {
 *
 *  init_engine_test();
 *
 *  char charBuffer[1024*1024];
 *  engine::Buffer* engineBuffer = engine::MemoryManager::shared()->createBuffer( "buffer1" , "test",  15 );
 *
 *  engine::DiskOperation* operation1 = engine::DiskOperation::newReadOperation( charBuffer , "test_filename.txt" , 3 , 6, 2 );
 *  engine::DiskOperation* operation2 = engine::DiskOperation::newWriteOperation( engineBuffer , "test_filename.txt" , 2 );
 *  engine::DiskOperation* operation3 = engine::DiskOperation::newAppendOperation( engineBuffer , "test_filename.txt" , 2 );
 *  engine::DiskOperation* operation4 = engine::DiskOperation::newRemoveOperation( "test_filename.txt" , 2 );
 *
 *  EXPECT_EQ(operation1->getDescription(), "Read from file: 'test_filename.txt' Size: 6.00 B [6B] Offset:3") << "Description error";
 *  EXPECT_EQ(operation2->getDescription(), "Write to file: 'test_filename.txt' Size:    0 B") << "Description error";
 *  EXPECT_EQ(operation3->getDescription(), "Append to file: 'test_filename.txt' Size:    0 B") << "Description error";
 *  EXPECT_EQ(operation4->getDescription(), "Remove file: 'test_filename.txt'") << "Description error";
 *
 *  engineBuffer->Release();
 *
 *  close_engine_test();
 * }
 *
 * //Test std::string getShortDescription();
 * TEST(diskOperationTest, getShortDescriptionTest)
 * {
 *  init_engine_test();
 *
 *  char charBuffer[1024*1024];
 *  engine::Buffer* engineBuffer = engine::MemoryManager::shared()->createBuffer( "buffer1" , "test", 15 );
 *
 *  engine::DiskOperation* operation1 = engine::DiskOperation::newReadOperation( charBuffer , "test_filename.txt" , 3 , 6, 2 );
 *  engine::DiskOperation* operation2 = engine::DiskOperation::newWriteOperation( engineBuffer , "test_filename.txt" , 2 );
 *  engine::DiskOperation* operation3 = engine::DiskOperation::newAppendOperation( engineBuffer , "test_filename.txt" , 2 );
 *  engine::DiskOperation* operation4 = engine::DiskOperation::newRemoveOperation( "test_filename.txt" , 2 );
 *
 *  EXPECT_EQ(operation1->getShortDescription(), "R: 6.00 ") << "Description error";
 *  EXPECT_EQ(operation2->getShortDescription(), "W:    0 ") << "Description error";
 *  EXPECT_EQ(operation3->getShortDescription(), "A:    0 ") << "Description error";
 *  EXPECT_EQ(operation4->getShortDescription(), "X") << "Description error";
 *
 *  engineBuffer->Release();
 *
 *  close_engine_test();
 * }
 *
 *
 * //Test size_t getSize()
 * TEST(diskOperationTest, getSizeTest)
 * {
 *
 *  init_engine_test();
 *
 *  char charBuffer[1024*1024];
 *  engine::Buffer* engineBuffer = engine::MemoryManager::shared()->createBuffer( "buffer1" ,"test",  15 );
 *
 *  engine::DiskOperation* operation1 = engine::DiskOperation::newReadOperation( charBuffer , "test_filename.txt" , 3 , 5, 2 );
 *  engine::DiskOperation* operation2 = engine::DiskOperation::newWriteOperation( engineBuffer , "test_filename.txt" , 2 );
 *
 *  EXPECT_EQ(operation1->getSize(), 5) << "Error in getSize()";
 *  EXPECT_EQ(operation2->getSize(), 0) << "Error in getSize()";
 *
 *  engineBuffer->Release();
 *
 *  close_engine_test();
 * }
 *
 *
 * //Test void addListener( size_t id )
 *
 *
 */
