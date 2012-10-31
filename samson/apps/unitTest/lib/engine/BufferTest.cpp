/* ****************************************************************************
 *
 * FILE            BufferTest.cpp
 *
 * AUTHOR          Javier Lois
 *
 * DATE            December 2011
 *
 * DESCRIPTION
 *
 * Unit testing of the Buffer class in the engine library
 *
 *
 * Telefonica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefonica Investigacion y Desarrollo S.A.U.
 * All rights reserved.
 */
#include <string>

#include "gtest/gtest.h"

#include "engine/Buffer.h"
#include "engine/MemoryManager.h"
#include "unitTest/common_engine_test.h"
#include "xmlparser/xmlParser.h"

// -----------------------------------------------------------------------------
// basic - buffer size tests
//
TEST(bufferTest, basic) {
  init_engine_test();

  {
    engine::BufferPointer buffer1 = engine::Buffer::Create("buffer1", "test", 15);
    EXPECT_EQ(0ULL, buffer1->size());
    EXPECT_EQ(15ULL, buffer1->max_size());

    buffer1->set_size(10);
    EXPECT_EQ(10ULL, buffer1->size()) << "Error in set_size()";

    au::Environment env = buffer1->environment();
    EXPECT_STREQ("{}", env.str().c_str());

    // Anything to check?

    // Erroneous set_size (size is bigger than allowed max size):
    buffer1->set_size(16);
    EXPECT_EQ(10ULL, buffer1->size()) << "Error in set_size()";

    buffer1->set_size(5);
    EXPECT_EQ(5ULL, buffer1->size()) << "Error in set_size()";

#define RETURN_STRING "[ Buffer (buffer1 / test) Size: 5/15 Read_offset 5 ]"
    EXPECT_EQ(RETURN_STRING, buffer1->str());
#undef RETURN_STRING
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// writeTest - write content updating the size variable coherently
//
// If it is not possible to write the entire block, it return false
// So, it never tries to write less than input_size bytes
//
TEST(bufferTest, writeTest) {
  init_engine_test();

  {
    engine::BufferPointer buffer1 = engine::Buffer::Create("buffer1", "test", 15);
    char data[21] = "01234567890123456789";
    buffer1->Write(data, 10);

    EXPECT_EQ(10ULL, buffer1->size()) << "wrong size after writing";

    char readChar;
    buffer1->Read(&readChar, 1);
    EXPECT_EQ(readChar, '0') << "read error after writing";
    // if we try to write past the buffer size, it will return false
    EXPECT_EQ(buffer1->Write(data, 20), false) << "should not write if size goes beyond size";
  }
  close_engine_test();
}

// -----------------------------------------------------------------------------
// skipWriteTest - skip some space without writing anything
//
TEST(bufferTest, skipWriteTest) {
  init_engine_test();

  {
    engine::BufferPointer buffer1 = engine::Buffer::Create("buffer1", "test", 15);
    char data[21] = "01234567890123456789";
    buffer1->SkipWrite(1);
    buffer1->Write(data, 10);
    char readBuffer[12];
    buffer1->Read(readBuffer, 11);
    readBuffer[11] = '\0';
    // the string "01234..." should start at position 1 instead of 0
    EXPECT_EQ(strcmp(readBuffer + 1, "0123456789"), 0) << "wrong data after skipWriting";
  }
  close_engine_test();
}

// -----------------------------------------------------------------------------
// ifstreamWriteTest - write the maximum possible amount of data to a buffer
//
TEST(bufferTest, ifstreamWriteTest) {
  init_engine_test();

  {
    std::string fileName = "test_data/testdata.txt";

    engine::BufferPointer buffer1 = engine::Buffer::Create("buffer1", "test", 15);

    std::ifstream file(fileName.c_str());
    EXPECT_EQ(file.is_open(), true) <<
       "Error opening test file test_data/testdata.txt at execution path. "
       "Copy it from the source directory.";
    buffer1->Write(file);

    EXPECT_EQ(buffer1->size(), 15ULL);

    char readBuffer[16];
    buffer1->Read(readBuffer, 15);
    readBuffer[15] = '\0';
    EXPECT_EQ(strcmp(readBuffer, "012345678901234"), 0) << "wrong data after writing from ifstream";
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// getAvailableWriteTest - get available space to write with "write call"
//
TEST(bufferTest, getAvailableWriteTest) {
  init_engine_test();

  {
    engine::BufferPointer buffer1 = engine::Buffer::Create("buffer1", "test", 15);
    char data[21] = "01234567890123456789";
    buffer1->Write(data, 10);
    EXPECT_EQ(5ULL, buffer1->GetAvailableSizeToWrite());
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// skipReadTest - skip some space without reading
//
TEST(bufferTest, skipReadTest) {
  init_engine_test();

  {
    engine::BufferPointer buffer1 = engine::Buffer::Create("buffer1", "test", 15);
    char data[21] = "0123456789";
    buffer1->Write(data, 10);
    char readBuffer[5];
    buffer1->SkipRead(3);
    buffer1->Read(readBuffer, 4);
    readBuffer[4] = '\0';
    // readbuffer should have started reading at 3 instead of 0
    EXPECT_EQ(strcmp(readBuffer, "3456"), 0) << "wrong data after skipRead";
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// readTest - read a Buffer into another Buffer
//
TEST(bufferTest, readTest) {
  init_engine_test();

  {
    engine::BufferPointer buffer1 = engine::Buffer::Create("buffer1", "test", 15);
    char data[21] = "0123456789";
    buffer1->Write(data, 10);
    char readBuffer[5];
    buffer1->Read(readBuffer, 4);
    readBuffer[4] = '\0';
    EXPECT_EQ(strcmp(readBuffer, "0123"), 0) << "Buffer::Read error";
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// getSizePendingReadTest - get pending bytes to be read
//
TEST(bufferTest, getSizePendingReadTest) {
  init_engine_test();

  {
    engine::BufferPointer buffer1 = engine::Buffer::Create("buffer1", "test", 15);
    char data[21] = "0123456789";
    buffer1->Write(data, 10);
    char readBuffer[5];
    buffer1->Read(readBuffer, 4);
    EXPECT_EQ(buffer1->GetAvailableSizeToRead(), 6ULL) << "Wrong pending read size";
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// getDataTest - Get a pointer to the data space
//
TEST(bufferTest, getDataTest) {
  init_engine_test();

  {
    engine::BufferPointer buffer1 = engine::Buffer::Create("buffer1", "test", 15);
    char data[21] = "0123456789";
    buffer1->Write(data, 10);
    char readBuffer[5];
    memcpy(readBuffer, buffer1->data(), 4);
    readBuffer[4] = '\0';
    EXPECT_EQ(strcmp(readBuffer, "0123"), 0) << "Error accesing buffer data pointer";
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// set_sizeTest - set used size manually
//
TEST(bufferTest, set_sizeTest) {
  init_engine_test();

  {
    engine::BufferPointer buffer1 = engine::Buffer::Create("buffer1", "test", 15);
    buffer1->set_size(1);
    EXPECT_EQ(buffer1->size(), 1ULL) << "Used size was not set correctly";
    char data[21] = "01234567890123456789";
    buffer1->Write(data, 10);
    char readBuffer[12];
    buffer1->Read(readBuffer, 11);
    readBuffer[11] = '\0';
    // the string "01234..." should start at position 1 instead of 0
    EXPECT_EQ(strcmp(readBuffer + 1, "0123456789"), 0) <<
       "wrong data after manually setting used size";
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// getSimpleBuffer - copy a Buffer using GetSimpleBuffer - check it's correct
//
TEST(bufferTest, getSimpleBufferTest) {
  init_engine_test();

  {
    engine::BufferPointer buffer1 = engine::Buffer::Create("buffer1", "test", 15);
    char data[21] = "0123456789";
    buffer1->Write(data, 10);

    // create the SimpleBuffer
    engine::SimpleBuffer simple = buffer1->GetSimpleBuffer();
    EXPECT_TRUE(simple.checkSize(buffer1->size())) <<
    "SimpleBuffer's size should be equal to the original Buffer's one";
    char readBuffer[5];
    memcpy(readBuffer, simple.data(), 4);
    readBuffer[4] = '\0';
    EXPECT_EQ(strcmp(readBuffer, "0123"), 0) << "Wrong data in the SimpleBuffer";
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// getSimpleBufferAtOffsetTest -
//
TEST(bufferTest, getSimpleBufferAtOffsetTest) {
  init_engine_test();

  {
    engine::BufferPointer buffer1 = engine::Buffer::Create("buffer1", "test", 15);
    char data[21] = "0123456789";
    buffer1->Write(data, 10);

    // create the SimpleBuffer
    engine::SimpleBuffer simple = buffer1->GetSimpleBufferAtOffset(2);
    EXPECT_TRUE(simple.checkSize(buffer1->size() - 2)) <<
       "SimpleBuffer's size should be equal to the original Buffer's one minus 2";

    char readBuffer[5];
    memcpy(readBuffer, simple.data(), 4);
    readBuffer[4] = '\0';
    // The data in the SimpleBuffer should start at 2 instead of 0
    EXPECT_EQ(strcmp(readBuffer, "2345"), 0) << "Wrong data in the SimpleBuffer";
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// removeLastUnfinishedLineTest - remove the last characters of an
//                                unfinished line and put them in buffer
//
TEST(bufferTest, removeLastUnfinishedLineTest) {
  init_engine_test();

  {
    engine::BufferPointer buffer1 = engine::Buffer::Create("buffer1", "test", 15);

    char data[21] = "0123\n0123\n012";
    buffer1->Write(data, 15);

    char *readBuffer = NULL;
    size_t bufferSize;
    buffer1->RemoveLastUnfinishedLine(readBuffer, bufferSize);
    // Check that data in the result buffer is okay
    EXPECT_EQ(strcmp(readBuffer, "012"), 0) <<
       "removeLastUnfinishedLine() returned wrong data in buffer";
    EXPECT_EQ(bufferSize, 5ULL) << "removeLastUnfinishedLine() returned wrong buffer size";

    // Check that the original buffer's data has been correctly modified
    char readBuffer2[15];
    memcpy(readBuffer2, buffer1->data(), buffer1->size());
    readBuffer2[buffer1->size()] = '\0';

    EXPECT_EQ(strcmp(readBuffer2, "0123\n0123\n"), 0) <<
       "Wrong data in buffer after removeLastUnfinishedLine call";
    EXPECT_EQ(buffer1->size(), 10ULL) <<
       "Wrong buffer size after removeLastUnfinishedLine call";

    if (readBuffer != NULL) {
      free(readBuffer);
    }
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// tagCollection - check various tag commands of the Buffer
//
TEST(bufferTest, tagCollection) {
  init_engine_test();

  {
    engine::BufferPointer buffer = engine::Buffer::Create("buffer1", "test", 15);

    buffer->SetTag("aTag");
    EXPECT_TRUE(buffer->contains_tag("aTag")) << "'aTag' not found!";

    std::string tagString = buffer->GetTagString();
    LM_M(("GetTagString returned this: '%s'", tagString.c_str()));

    buffer->RemoveTag("aTag");
    EXPECT_FALSE(buffer->contains_tag("aTag")) <<
       "Found 'aTag', which is an error - 'aTag' was just removed!";
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// name - check that the constructor sets name and type correctly
//
TEST(bufferTest, name) {
  init_engine_test();

  {
    engine::BufferPointer buffer = engine::Buffer::Create("buffer1", "test", 15);

    EXPECT_STREQ("buffer1", buffer->name().c_str());
    EXPECT_STREQ("test", buffer->type().c_str());
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// setNameAndType - set and change 'name' and 'type' - check changes are OK
//
TEST(bufferTest, setNameAndType) {
  init_engine_test();

  {
    engine::BufferPointer buf = engine::Buffer::Create("buf", "test", 15);

    buf->set_name_and_type("nameX", "typeX");

    EXPECT_STREQ("nameX", buf->name().c_str());
    EXPECT_STREQ("typeX", buf->type().c_str());

    buf->add_to_name("2");
    EXPECT_STREQ("nameX2", buf->name().c_str());
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// WriteFile - read in a file into a buffer and check the resulting buffer size
//
TEST(bufferTest, WriteFile) {
  init_engine_test();

  {
    au::ErrorManager       em;
    engine::BufferPointer  buf = engine::Buffer::Create("buf", "test", 150);

    buf->WriteFile("test_data/testdata.txt", em);
    EXPECT_EQ(93, buf->size());
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// badMaxSize - create a buffer with max size 0 and a too big max_size (> 1Gb)
//
TEST(bufferTest, badMaxSize) {
  init_engine_test();

  {
    au::ErrorManager       em;
    engine::BufferPointer  buf = engine::Buffer::Create("buf", "test", 0);

    EXPECT_EQ(0, buf->max_size());
    EXPECT_EQ(0, buf->size());

    buf->WriteFile("test_data/testdata.txt", em);
    EXPECT_EQ(0, buf->size());

    engine::BufferPointer  buf2 = engine::Buffer::Create("buf", "test", 2 * 1024ULL * 1024 * 1024);
    EXPECT_TRUE(buf2 == NULL);
  }

  close_engine_test();
}
