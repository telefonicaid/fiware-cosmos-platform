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

//#include <direct.h> // for getcwd
#include <stdlib.h>// for MAX_PATH

#include "gtest/gtest.h"

#include "engine/MemoryManager.h"
#include "engine/Buffer.h"

#include "xmlmarkup/xmlmarkup.h"


//Test size_t getMaxSize();
TEST(BufferTest, getMaxSizeTest) {
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    
    EXPECT_EQ(buffer1->getMaxSize(), 15);
    
    engine::MemoryManager::shared()->destroyBuffer(buffer1);
}
    
    
// Get used size of this buffer ( not necessary the maximum )
//Test getSize() and setSize();
TEST(BufferTest, getandSetSizeTest) {
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );

    EXPECT_EQ(buffer1->getSize(), 0) << "Error in getSize()";
    
    buffer1->setSize(10);
   
    EXPECT_EQ(buffer1->getSize(), 10) << "Error in setSize()";

    engine::MemoryManager::shared()->destroyBuffer(buffer1);

}
   
// Get a description of the buffer ( debugging )
//Test std::string str();
TEST(BufferTest, strTest) {
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    buffer1->setSize(5);

    EXPECT_EQ(buffer1->str(), "[ Buffer (buffer1) MaxSize: 15 Size: 5 Offset 0 ]") << "Error in str()";

    engine::MemoryManager::shared()->destroyBuffer(buffer1);

}
    
/**
Function to write content updating the size variable coherently
If it is not possible to write the entire block, it return false
So, it never try to write less than input_size bytes
*/
//Test bool write( char * input_buffer , size_t input_size );
TEST(BufferTest, writeTest) {
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    char data[21] ="01234567890123456789";
    buffer1->write(data, 10);
    
    EXPECT_EQ(buffer1->getSize(), 10) << "wrong size after writing";

    char readChar;
    buffer1->read(&readChar, 1);
    EXPECT_EQ(readChar, '0') << "read error after writing";
    
    //if we try to write past the buffer size, it will return false
    EXPECT_EQ(buffer1->write(data, 20), false) << "should not write if size goes beyond size";
    

    engine::MemoryManager::shared()->destroyBuffer(buffer1);
}
    
// Skip some space without writing anything
//Test bool skipWrite( size_t size );
TEST(BufferTest, skipWriteTest) {
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    char data[21] ="01234567890123456789";
    buffer1->skipWrite(1);
    buffer1->write(data, 10);
    char readBuffer[12];
    buffer1->read(readBuffer, 11);
    readBuffer[11] = '\0';
    //the string "01234..." should start at position 1 instead of 0    
    EXPECT_EQ(strcmp(readBuffer+1, "0123456789"), 0) << "wrong data after skipWriting";

    engine::MemoryManager::shared()->destroyBuffer(buffer1);
}    

// Write on the buffer the maximum possible ammount of data
//Test void write( std::ifstream &inputStream );
TEST(BufferTest, ifstreamWriteTest) {
    std::string fileName = "./testdata.txt";

    engine::MemoryManager::init(1000);
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );

    std::ifstream file(fileName.c_str());
    ASSERT_TRUE(file.is_open()) << "Error opening test file ./testdata.txt at execution path. Copy it from the source directory.";
    buffer1->write(file);
    
    EXPECT_EQ(buffer1->getSize(), 15);
    
    char readBuffer[16];
    buffer1->read(readBuffer,15);
    readBuffer[15] = '\0';
    EXPECT_EQ(strcmp(readBuffer, "012345678901234"), 0) << "wrong data after writing from ifstream";

    engine::MemoryManager::shared()->destroyBuffer(buffer1);
}    

// Get available space to write with "write call"
//Test size_t getAvailableWrite();
TEST(BufferTest, getAvailableWriteTest) {
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    char data[21] ="01234567890123456789";
    buffer1->write(data, 10);
    EXPECT_EQ(buffer1->getAvailableWrite(), 5);

    engine::MemoryManager::shared()->destroyBuffer(buffer1);
}    

// Skip some space without reading
//Test size_t skipRead( size_t size);
TEST(BufferTest, skipReadTest) {
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    char data[21] ="0123456789";
    buffer1->write(data, 10);
    char readBuffer[5];
    buffer1->skipRead(3);
    buffer1->read(readBuffer, 4);
    readBuffer[4] = '\0';
    //std::cout << "readBuffer: " << readBuffer << std::endl;
    //readbuffer should have started reading at 3 instead of 0
    EXPECT_EQ(strcmp(readBuffer, "3456"), 0) << "wrong data after skipRead";

    engine::MemoryManager::shared()->destroyBuffer(buffer1);
}

//Test size_t read( char *output_buffer, size_t output_size);
TEST(BufferTest, readTest) {
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    char data[21] ="0123456789";
    buffer1->write(data, 10);
    char readBuffer[5];
    buffer1->read(readBuffer, 4);
    readBuffer[4] = '\0';
    //std::cout << "readBuffer: " << readBuffer << std::endl;
    EXPECT_EQ(strcmp(readBuffer, "0123"), 0) << "Reading error";

    engine::MemoryManager::shared()->destroyBuffer(buffer1);
}
    
// Get pending bytes to be read
//Test size_t getSizePendingRead();
TEST(BufferTest, getSizePendingReadTest) {
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    char data[21] ="0123456789";
    buffer1->write(data, 10);
    char readBuffer[5];
    buffer1->read(readBuffer, 4);
    //std::cout << "readBuffer: " << readBuffer << std::endl;
    EXPECT_EQ(buffer1->getSizePendingRead(), 6) << "Wrong pending read size";

    engine::MemoryManager::shared()->destroyBuffer(buffer1);
}
    
// Get a pointer to the data space
//Test char *getData();
TEST(BufferTest, getDataTest) {
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    char data[21] ="0123456789";
    buffer1->write(data, 10);
    char readBuffer[5];
    memcpy(readBuffer, buffer1->getData(), 4);
    readBuffer[4] = '\0';
    //std::cout << "readBuffer: " << readBuffer << std::endl;
    EXPECT_EQ(strcmp(readBuffer, "0123"), 0) << "Error accesing buffer data pointer";
    
    engine::MemoryManager::shared()->destroyBuffer(buffer1);
}    
// Set used size manually
//Test void setSize( size_t size );
TEST(BufferTest, setSizeTest) {
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    buffer1->setSize(1);
    EXPECT_EQ(buffer1->getSize(), 1) << "Used size was not set correctly";
    
    char data[21] ="01234567890123456789";
    buffer1->write(data, 10);
    char readBuffer[12];
    buffer1->read(readBuffer, 11);
    readBuffer[11] = '\0';
    //the string "01234..." should start at position 1 instead of 0    
    EXPECT_EQ(strcmp(readBuffer+1, "0123456789"), 0) << "wrong data after manually setting used size";

    engine::MemoryManager::shared()->destroyBuffer(buffer1);
}    
    
//Test SimpleBuffer getSimpleBuffer();
TEST(BufferTest, getSimpleBufferTest) {
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    char data[21] ="0123456789";
    buffer1->write(data, 10);
    
    //create the SimpleBuffer
    engine::SimpleBuffer simple = buffer1->getSimpleBuffer();
    EXPECT_TRUE(simple.checkSize(buffer1->getSize())) << "SimpleBuffer's size should be equal to the original Buffer's one";
    char readBuffer[5];
    memcpy(readBuffer, simple.getData(), 4);
    readBuffer[4] = '\0';
    //std::cout << "readBuffer: " << readBuffer << std::endl;
    EXPECT_EQ(strcmp(readBuffer, "0123"), 0) << "Wrong data in the SimpleBuffer";
    
    engine::MemoryManager::shared()->destroyBuffer(buffer1);
}    

//Test SimpleBuffer getSimpleBufferAtOffset(size_t offset);
TEST(BufferTest, getSimpleBufferAtOffsetTest) {
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    char data[21] ="0123456789";
    buffer1->write(data, 10);
    
    //create the SimpleBuffer
    engine::SimpleBuffer simple = buffer1->getSimpleBufferAtOffset(2);
    EXPECT_TRUE(simple.checkSize(buffer1->getSize()-2)) << "SimpleBuffer's size should be equal to the original Buffer's one minus 2";
    char readBuffer[5];
    memcpy(readBuffer, simple.getData(), 4);
    readBuffer[4] = '\0';
    //std::cout << "readBuffer: " << readBuffer << std::endl;
    //The dta in the SimpleBuffer should start at 2 instead of 0
    EXPECT_EQ(strcmp(readBuffer, "2345"), 0) << "Wrong data in the SimpleBuffer";
}    

// Remove the last characters of an unfinished line and put them in buffer.
//Test int removeLastUnfinishedLine( char ** buffer , size_t* buffer_size);
TEST(BufferTest, removeLastUnfinishedLineTest) {
    //std::string fileName = "./testdata.txt";

    engine::MemoryManager::init(1000);
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );

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
}    
    
// get xml information
//Test void getInfo( std::ostringstream& output);
TEST(BufferTest, getInfoTest) {
    engine::MemoryManager::init(1000);
    engine::Buffer* buffer1 = engine::MemoryManager::shared()->newBuffer( "buffer1" ,  15 , 1 );
    char data[21] ="0123456789";
    buffer1->write(data, 10);
    buffer1->skipRead(2);
    
    std::ostringstream info;
    buffer1->getInfo( info );
    //std::cout << info.str() << std::endl;
    
    //read and check xml
    CMarkup xmlData( info.str() );
    xmlData.FindElem();
    xmlData.IntoElem();
    xmlData.FindElem("max_size");
    EXPECT_EQ(xmlData.GetData(), "15") << "Error writing max_size tag";
    xmlData.FindElem("size");
    EXPECT_EQ(xmlData.GetData(), "10") << "Error writing size tag";
    xmlData.FindElem("offset");
    EXPECT_EQ(xmlData.GetData(), "2") << "Error writing offset tag";
    xmlData.FindElem("name");
    EXPECT_EQ(xmlData.GetData(), "buffer1") << "Error writing name tag";

}

