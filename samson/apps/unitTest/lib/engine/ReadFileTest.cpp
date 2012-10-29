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
#include <stdlib.h>  // for MAX_PATH

#include "gtest/gtest.h"

#include "engine/ReadFile.h"



//
// Test ReadFile( std::string _fileName );
//
TEST(engine_ReadFile, openTest) {
  engine::ReadFile file1("badfile");

  EXPECT_TRUE(file1.IsValid() == false);

  engine::ReadFile file2("test_data/testdata.txt");
  EXPECT_TRUE(file2.IsValid());
}



//
// Test int read( char * read_buffer , size_t size );
//
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



//
// Test int seek( size_t offset );
//
TEST(engine_ReadFile, seekTest) {
  engine::ReadFile file("test_data/testdata.txt");

  ASSERT_TRUE(file.IsValid());

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



//
// Test bool IsValid();
// Test void Close();
//
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



//
// Test a seek to the same position
//
TEST(engine_ReadFile, seekSamePositionTest) {
  engine::ReadFile  file("test_data/testdata.txt");
  int               pos;

  ASSERT_TRUE(file.IsValid());
  
  pos = file.Seek(2);
  EXPECT_EQ(pos, 0);
  pos = file.Seek(2);
  EXPECT_EQ(pos, 0);
}



//
// gets - 
//
TEST(engine_ReadFile, gets) {
  engine::ReadFile  file("test_data/testdata.txt");

  std::string name = file.file_name();
  EXPECT_STREQ(name.c_str(), "test_data/testdata.txt");

  size_t offset = file.offset();
  EXPECT_EQ(0, offset);

  char buf[3];
  int s = file.Read(buf, 0);
  EXPECT_EQ(0, s);
}
