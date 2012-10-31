/* ****************************************************************************
 *
 * FILE            DiskOperationTest.cpp
 *
 * AUTHOR          Javier Lois
 *
 * DATE            December 2011
 *
 * DESCRIPTION
 *
 * Unit testing of the DiskOperation class in the engine library
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
#include "gtest/gtest.h"

#include "engine/DiskManager.h"
#include "engine/DiskOperation.h"
#include "unitTest/common_engine_test.h"
#include "xmlparser/xmlParser.h"

// ----------------------------------------------------------------------------
// setError -
//
TEST(engine_DiskOperation, setError) {
  std::ostringstream  out;
  const char*         expected =
     "<disk_operation>\n"
     "<type>read</type>\n"
     "<file_name>test_data/test_data.txt</file_name>\n"
     "<size>5</size>\n"
     "<offset>3</offset>\n"
     "</disk_operation>\n";

  init_engine_test();

  char charBuffer[1024*1024];
  engine::DiskOperation* operation =
     engine::DiskOperation::newReadOperation(charBuffer, "test_data/test_data.txt", 3, 5, 0);

  operation->setError("Just a test");
  operation->getInfo(out);
  EXPECT_STREQ(expected, out.str().c_str());

  close_engine_test();
}

// ----------------------------------------------------------------------------
// getInfoTest -
//
TEST(engine_DiskOperation, getInfoTest) {
  std::ostringstream out;

  init_engine_test();

  char charBuffer[1024*1024];
  engine::DiskOperation* operation =
     engine::DiskOperation::newReadOperation(charBuffer, "test_data/test_data.txt", 3, 5, 0);
  EXPECT_EQ(operation->getType(), engine::DiskOperation::read) << "Wrong type value";
  operation->getInfo(out);
  // operation->run();

  close_engine_test();
}

// -----------------------------------------------------------------------------
// newWriteOperationTest -
//
TEST(engine_DiskOperation, newWriteOperationTest) {
  std::ostringstream out;

  init_engine_test();

  engine::BufferPointer buffer = engine::Buffer::Create("buffer1", "test", 15);
  engine::DiskOperation* operation =
     engine::DiskOperation::newWriteOperation(buffer, "test_data/test_data.txt", 2);

  EXPECT_EQ(operation->getType(), engine::DiskOperation::write) << "Wrong type value";
  operation->getInfo(out);
  operation->run();

  close_engine_test();
}

// -----------------------------------------------------------------------------
// newAppendOperationTest -
//
TEST(engine_DiskOperation, newAppendOperationTest) {
  std::ostringstream out;

  init_engine_test();

  engine::BufferPointer  buffer    = engine::Buffer::Create("buffer1", "test", 15);
  engine::DiskOperation* operation =
     engine::DiskOperation::newAppendOperation(buffer, "test_filename.txt", 2);

  EXPECT_EQ(operation->getType(), engine::DiskOperation::append) << "Wrong type value";
  operation->getInfo(out);
  operation->run();

  close_engine_test();
}

// -----------------------------------------------------------------------------
// newRemoveOperationTest -
//
TEST(engine_DiskOperation, newRemoveOperationTest) {
  std::ostringstream out;

  init_engine_test();

  engine::DiskOperation* operation =
     engine::DiskOperation::newRemoveOperation("test_data/test_data.txt", 1);

  EXPECT_EQ(operation->getType(), engine::DiskOperation::remove) << "Wrong type value";
  operation->getInfo(out);
  operation->run();
  close_engine_test();
}

// -----------------------------------------------------------------------------
// newReadOperation2Test -
//
TEST(engine_DiskOperation, newReadOperation2Test) {
  std::ostringstream out;
  char data[100];

  init_engine_test();
  engine::SimpleBuffer buffer(data, 100);

  engine::DiskOperation* operation =
     engine::DiskOperation::newReadOperation("test_filename.txt", 3, 6, buffer, 2);

  EXPECT_EQ(operation->getType(), engine::DiskOperation::read) << "Wrong type value";
  close_engine_test();
}

// -----------------------------------------------------------------------------
// getDescriptionTest -
//
TEST(engine_DiskOperation, getDescriptionTest) {
  init_engine_test();

  char charBuffer[1024*1024];
  engine::BufferPointer buffer = engine::Buffer::Create("buffer1", "test", 15);

  engine::DiskOperation* operation1 =
     engine::DiskOperation::newReadOperation(charBuffer, "test_filename.txt", 3, 6, 2);
  engine::DiskOperation* operation2 =
     engine::DiskOperation::newWriteOperation(buffer,    "test_filename.txt", 2);
  engine::DiskOperation* operation3 =
     engine::DiskOperation::newAppendOperation(buffer,   "test_filename.txt", 2);
  engine::DiskOperation* operation4 =
     engine::DiskOperation::newRemoveOperation("test_filename.txt", 2);

  EXPECT_EQ(operation1->getDescription(),
            "Read from file: 'test_filename.txt' Size: 6.00 B [6B] Offset:3") <<
     "Description error (Read)";

  EXPECT_EQ(operation2->getDescription(),
            "Write to file: 'test_filename.txt' Size:    0 B") <<
     "Description error (Write)";

  EXPECT_EQ(operation3->getDescription(),
            "Append to file: 'test_filename.txt' Size:    0 B") <<
     "Description error (Append)";

  EXPECT_EQ(operation4->getDescription(),
            "Remove file: 'test_filename.txt'") <<
     "Description error (Remove)";

  EXPECT_EQ(operation1->getShortDescription(), "R: 6.00 ") << "Description error (E)";
  EXPECT_EQ(operation2->getShortDescription(), "W:    0 ") << "Description error (W)";
  EXPECT_EQ(operation3->getShortDescription(), "A:    0 ") << "Description error (A)";
  EXPECT_EQ(operation4->getShortDescription(), "X")        << "Description error (X)";

  close_engine_test();
}

// -----------------------------------------------------------------------------
// getSizeTest -
//
TEST(engine_DiskOperation, getSizeTest) {
  init_engine_test();

  char charBuffer[1024*1024];
  engine::BufferPointer buffer = engine::Buffer::Create("buffer1", "test", 15);

  engine::DiskOperation* operation1 =
     engine::DiskOperation::newReadOperation(charBuffer, "test_filename.txt", 3, 5, 2);
  engine::DiskOperation* operation2 =
     engine::DiskOperation::newWriteOperation(buffer, "test_filename.txt", 2);

  EXPECT_EQ(operation1->getSize(), 5) << "Error in getSize()";
  EXPECT_EQ(operation2->getSize(), 0) << "Error in getSize()";

  close_engine_test();
}
