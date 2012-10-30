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
 * unit testing of the ReadFile class in the engine library
 *
 */

#include "engine/ReadFile.h"
#include "gtest/gtest.h"
#include <stdlib.h>  // for MAX_PATH

// Test ReadFile( std::string _fileName );
TEST(engine_ReadFile, openTest) {
  engine::ReadFile file1("badfile");

  EXPECT_TRUE(file1.IsValid() == false);

  engine::ReadFile file2("test_data/testdata.txt");
  EXPECT_TRUE(file2.IsValid());
}

// Test int read( char * read_buffer , size_t size );
TEST(readfileTest, readTest) {
  engine::ReadFile file("test_data/testdata.txt");

  ASSERT_TRUE(file.IsValid());
  if (file.IsValid()) {
    char data[10];
    file.Read(data, 5);
    data[5] = '\0';
    EXPECT_EQ(strcmp(data, "01234"), 0);
  }
}

// Test int seek( size_t offset );
TEST(engine_ReadFile, seekTest) {
  engine::ReadFile file("test_data/testdata.txt");

  ASSERT_TRUE(file.IsValid());
  if (file.IsValid()) {
    char data[10];
    file.Seek(2);
    file.Read(data, 5);
    data[5] = '\0';
    EXPECT_EQ(strcmp(data, "23456"), 0);
  }

  engine::ReadFile fileBad("badfile");
  EXPECT_EQ(fileBad.Seek(2), 1) << "Trying to seek a bad file should return 1";
}

// Test bool isValid();
// Test void close();
TEST(engine_ReadFile, closeTest) {
  engine::ReadFile file1("badfile");

  EXPECT_TRUE(file1.IsValid() == false);

  engine::ReadFile file2("test_data/testdata.txt");
  ASSERT_TRUE(file2.IsValid());

  if (file2.IsValid()) {
    file2.Close();
    EXPECT_FALSE(file2.IsValid());
  }
}


