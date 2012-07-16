/* ****************************************************************************
*
* FILE            bufferTest.cpp
*
* AUTHOR          Javier Lois
*
* DATE            December 2011
*
* DESCRIPTION
*
* unit testing of the buffer class in the engine library
*
*/

//#include <stdlib.h>// for MAX_PATH

#include "gtest/gtest.h"

#include "engine/MemoryManager.h"
#include "engine/Buffer.h"

#include "xmlparser/xmlParser.h"

#include "unitTest/common_engine_test.h"

//Test size_t getMaxSize();
TEST(bufferTest, getMaxSizeTest) 
{
    init_engine_test();
    
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" ,  "test" , 15  );
    EXPECT_TRUE(buffer1->getMaxSize() >= 15);
    buffer1->release();
    
    close_engine_test();
}
    
    
// Get used size of this buffer ( not necessary the maximum )
//Test getSize() and setSize();
TEST(bufferTest, getandSetSizeTest) 
{
    init_engine_test();
    
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" ,  "test" , 15 );

    EXPECT_EQ(buffer1->getSize(), 0) << "Error in getSize()";
    buffer1->setSize(10);
    EXPECT_EQ(buffer1->getSize(), 10) << "Error in setSize()";

    buffer1->release();

    close_engine_test();
}
   
// Get a description of the buffer ( debugging )
//Test std::string str();
TEST(bufferTest, strTest) 
{
    init_engine_test();
    
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" ,  "test" , 15 );
    buffer1->setSize(5);

#define RETURN_STRING "[ Buffer (buffer1 / test) Size: 5/15 Read_offset 5 Retain Counter 0"
    EXPECT_EQ(buffer1->str().compare(0,strlen(RETURN_STRING), RETURN_STRING), 0) << "Error in str()" << ". Should be: '" << RETURN_STRING << "', not :'" << buffer1->str() << "'";
#undef RETURN_STRING

    buffer1->release();

    close_engine_test();
}
    
/**
Function to write content updating the size variable coherently
If it is not possible to write the entire block, it return false
So, it never try to write less than input_size bytes
*/
//Test bool write( char * input_buffer , size_t input_size );
TEST(bufferTest, writeTest) 
{
    
    init_engine_test();
    
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" ,  "test" , 15 );
    char data[21] ="01234567890123456789";
    buffer1->write(data, 10);
    
    EXPECT_EQ(buffer1->getSize(), 10) << "wrong size after writing";

    char readChar;
    buffer1->read(&readChar, 1);
    EXPECT_EQ(readChar, '0') << "read error after writing";
    
    //if we try to write past the buffer size, it will return false
    EXPECT_EQ(buffer1->write(data, 20), false) << "should not write if size goes beyond size";
 
    buffer1->release();

    close_engine_test();
}
    
// Skip some space without writing anything
//Test bool skipWrite( size_t size );
TEST(bufferTest, skipWriteTest) 
{
    init_engine_test();
    
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" , "test" , 15 );
    char data[21] ="01234567890123456789";
    buffer1->skipWrite(1);
    buffer1->write(data, 10);
    char readBuffer[12];
    buffer1->read(readBuffer, 11);
    readBuffer[11] = '\0';
    //the string "01234..." should start at position 1 instead of 0    
    EXPECT_EQ(strcmp(readBuffer+1, "0123456789"), 0) << "wrong data after skipWriting";

    buffer1->release();

    close_engine_test();
}    

// Write on the buffer the maximum possible ammount of data
//Test void write( std::ifstream &inputStream );
TEST(bufferTest, ifstreamWriteTest) 
{
    init_engine_test();

    std::string fileName = "test_data/testdata.txt";

    engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" ,  "test" , 15 );

    std::ifstream file(fileName.c_str());
    ASSERT_TRUE(file.is_open()) << "Error opening test file test_data/testdata.txt at execution path. Copy it from the source directory.";
    buffer1->write(file);
    
    EXPECT_EQ(buffer1->getSize(), 15);
    
    char readBuffer[16];
    buffer1->read(readBuffer,15);
    readBuffer[15] = '\0';
    EXPECT_EQ(strcmp(readBuffer, "012345678901234"), 0) << "wrong data after writing from ifstream";

    buffer1->release();
    
    close_engine_test();
}    

// Get available space to write with "write call"
//Test size_t getAvailableWrite();
TEST(bufferTest, getAvailableWriteTest) 
{
    init_engine_test();
    
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" ,  "test" , 15 );
    char data[21] ="01234567890123456789";
    buffer1->write(data, 10);
    EXPECT_EQ(buffer1->getAvailableWrite(), 5);

    buffer1->release();

    close_engine_test();

}    

// Skip some space without reading
//Test size_t skipRead( size_t size);
TEST(bufferTest, skipReadTest) 
{
    init_engine_test();
    
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" , "test" , 15 );
    char data[21] ="0123456789";
    buffer1->write(data, 10);
    char readBuffer[5];
    buffer1->skipRead(3);
    buffer1->read(readBuffer, 4);
    readBuffer[4] = '\0';
    //readbuffer should have started reading at 3 instead of 0
    EXPECT_EQ(strcmp(readBuffer, "3456"), 0) << "wrong data after skipRead";

    buffer1->release();
    
    close_engine_test();
}

//Test size_t read( char *output_buffer, size_t output_size);
TEST(bufferTest, readTest) 
{
    init_engine_test();
    
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" ,  "test" , 15 );
    char data[21] ="0123456789";
    buffer1->write(data, 10);
    char readBuffer[5];
    buffer1->read(readBuffer, 4);
    readBuffer[4] = '\0';
    EXPECT_EQ(strcmp(readBuffer, "0123"), 0) << "Reading error";

    buffer1->release();
    
    close_engine_test();
}
    
// Get pending bytes to be read
//Test size_t getSizePendingRead();
TEST(bufferTest, getSizePendingReadTest) 
{
    init_engine_test();
    
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" ,  "test" , 15 );
    char data[21] ="0123456789";
    buffer1->write(data, 10);
    char readBuffer[5];
    buffer1->read(readBuffer, 4);
    EXPECT_EQ(buffer1->getSizePendingRead(), 6) << "Wrong pending read size";

    buffer1->release();
    
    close_engine_test();
}
    
// Get a pointer to the data space
//Test char *getData();
TEST(bufferTest, getDataTest) 
{
    init_engine_test();
    
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" ,  "test" , 15 );
    char data[21] ="0123456789";
    buffer1->write(data, 10);
    char readBuffer[5];
    memcpy(readBuffer, buffer1->getData(), 4);
    readBuffer[4] = '\0';
    EXPECT_EQ(strcmp(readBuffer, "0123"), 0) << "Error accesing buffer data pointer";
    
    buffer1->release();
    
    close_engine_test();
}    
// Set used size manually
//Test void setSize( size_t size );
TEST(bufferTest, setSizeTest) 
{
    init_engine_test();
    
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" ,  "test" , 15 );
    buffer1->setSize(1);
    EXPECT_EQ(buffer1->getSize(), 1) << "Used size was not set correctly";
    
    char data[21] ="01234567890123456789";
    buffer1->write(data, 10);
    char readBuffer[12];
    buffer1->read(readBuffer, 11);
    readBuffer[11] = '\0';
    //the string "01234..." should start at position 1 instead of 0    
    EXPECT_EQ(strcmp(readBuffer+1, "0123456789"), 0) << "wrong data after manually setting used size";

    buffer1->release();

    close_engine_test();
}    
    
//Test SimpleBuffer getSimpleBuffer();
TEST(bufferTest, getSimpleBufferTest) 
{
    init_engine_test();
    
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" ,  "test" , 15 );
    char data[21] ="0123456789";
    buffer1->write(data, 10);
    
    //create the SimpleBuffer
    engine::SimpleBuffer simple = buffer1->getSimpleBuffer();
    EXPECT_TRUE(simple.checkSize(buffer1->getSize())) << "SimpleBuffer's size should be equal to the original Buffer's one";
    char readBuffer[5];
    memcpy(readBuffer, simple.getData(), 4);
    readBuffer[4] = '\0';
    EXPECT_EQ(strcmp(readBuffer, "0123"), 0) << "Wrong data in the SimpleBuffer";
    
    buffer1->release();
    
    close_engine_test();
}    

//Test SimpleBuffer getSimpleBufferAtOffset(size_t offset);
TEST(bufferTest, getSimpleBufferAtOffsetTest) 
{
    init_engine_test();
    
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" ,  "test" , 15);
    char data[21] ="0123456789";
    buffer1->write(data, 10);
    
    //create the SimpleBuffer
    engine::SimpleBuffer simple = buffer1->getSimpleBufferAtOffset(2);
    EXPECT_TRUE(simple.checkSize(buffer1->getSize()-2)) << "SimpleBuffer's size should be equal to the original Buffer's one minus 2";
    char readBuffer[5];
    memcpy(readBuffer, simple.getData(), 4);
    readBuffer[4] = '\0';
    //The dta in the SimpleBuffer should start at 2 instead of 0
    EXPECT_EQ(strcmp(readBuffer, "2345"), 0) << "Wrong data in the SimpleBuffer";

    buffer1->release();

    close_engine_test();
}    

// Remove the last characters of an unfinished line and put them in buffer.
//Test int removeLastUnfinishedLine( char ** buffer , size_t* buffer_size);
TEST(bufferTest, removeLastUnfinishedLineTest) 
{
    init_engine_test();
    //std::string fileName = "./testdata.txt";

    engine::Buffer* buffer1 = engine::MemoryManager::shared()->createBuffer( "buffer1" ,  "test" , 15 );

    /*std::ifstream file(fileName.c_str());
    ASSERT_TRUE(file.is_open()) << "Error opening test file ./testdata.txt at execution path";
    buffer1->write(file);*/
    
    char data[21] ="0123\n0123\n012";
    buffer1->write(data, 15);


    char* readBuffer = NULL;
    size_t bufferSize;
    buffer1->removeLastUnfinishedLine(readBuffer, bufferSize);
    //Check that data in the result buffer is okay
    EXPECT_EQ(strcmp(readBuffer, "012"), 0) << "removeLastUnfinishedLine() returned wrong data in buffer";    
    EXPECT_EQ(bufferSize, 5) << "removeLastUnfinishedLine() returned wrong buffer size";    
    
    //Check that the original buffer's data has been correctly modified
    char readBuffer2[15];
    memcpy(readBuffer2, buffer1->getData(), buffer1->getSize());
    readBuffer2[buffer1->getSize()] = '\0';

    EXPECT_EQ(strcmp(readBuffer2, "0123\n0123\n"), 0) << "Wrong data in buffer after removeLastUnfinishedLine call";    
    EXPECT_EQ(buffer1->getSize(), 10) << "Wrong buffer size after removeLastUnfinishedLine call";
    
   if (readBuffer!= NULL)
    {
        free(readBuffer);
    }
    
    buffer1->release();
    
    close_engine_test();
}    
    
