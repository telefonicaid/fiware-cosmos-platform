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
 * FILE            readFile_test.cpp
 *
 * AUTHOR          Javier Lois
 *
 * DATE            January 2012
 *
 * DESCRIPTION
 *
 * Unit testing of the ReadFile class in the engine library
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
#include <stdlib.h>  // for MAX_PATH

#include <string>

#include "gtest/gtest.h"

#include "engine/ReadFile.h"

// -----------------------------------------------------------------------------
// openTest - test to open a file that doesn't exist and another one that does
//
TEST(engine_ReadFile, openTest) {
  engine::ReadFile file1("badfile");

  EXPECT_TRUE(file1.IsValid() == false);

  engine::ReadFile file2("test_data/testdata.txt");
  EXPECT_TRUE(file2.IsValid());
}

// -----------------------------------------------------------------------------
// readTest -
//
TEST(readfileTest, readTest) {
  engine::ReadFile file("test_data/testdata.txt");

  EXPECT_TRUE(file.IsValid());

  if (file.IsValid()) {
    char data[10];
    file.Read(data, 5);
    data[5] = '\0';
    EXPECT_EQ(strcmp(data, "01234"), 0);
  }
}

// -----------------------------------------------------------------------------
// seekTest - ok and not-ok seeks tested
//

TEST(engine_ReadFile, DISABLED_seekTest) {
  engine::ReadFile file("test_data/testdata.txt");

  EXPECT_TRUE(file.IsValid());

  if (file.IsValid()) {
    char data[10];
    int  x;

    file.Seek(2);
    file.Read(data, 5);
    data[5] = '\0';
    EXPECT_EQ(strcmp(data, "23456"), 0);

    x = file.Read(data, 50000);
    EXPECT_EQ(x, 1);

    x = file.Seek(-2);
    EXPECT_EQ(2, x);
  }

  engine::ReadFile fileBad("badfile");
  EXPECT_EQ(fileBad.Seek(2), 1) << "Trying to seek a bad file should return 1";
}

// -----------------------------------------------------------------------------
// closeTest - testing validity and closing of ReadFile
//
TEST(engine_ReadFile, closeTest) {
  engine::ReadFile file1("badfile");

  EXPECT_TRUE(file1.IsValid() == false);

  engine::ReadFile file2("test_data/testdata.txt");
  EXPECT_TRUE(file2.IsValid());

  if (file2.IsValid()) {
    file2.Close();
    EXPECT_FALSE(file2.IsValid());
  }
}

// -----------------------------------------------------------------------------
// seekSamePositionTest - test a seek to the same position
//
TEST(engine_ReadFile, seekSamePositionTest) {
  engine::ReadFile  file("test_data/testdata.txt");
  int               pos;

  EXPECT_TRUE(file.IsValid());

  pos = file.Seek(2);
  EXPECT_EQ(pos, 0);
  pos = file.Seek(2);
  EXPECT_EQ(pos, 0);
}

// -----------------------------------------------------------------------------
// misc - test offset and a read of zero bytes
//
TEST(engine_ReadFile, misc) {
  engine::ReadFile  file("test_data/testdata.txt");

  std::string name = file.file_name();
  EXPECT_STREQ(name.c_str(), "test_data/testdata.txt");

  size_t offset = file.offset();
  EXPECT_EQ(0, offset);

  char buf[3];
  int s = file.Read(buf, 0);
  EXPECT_EQ(0, s);
}
