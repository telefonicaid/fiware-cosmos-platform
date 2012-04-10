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

//#include <direct.h> // for getcwd
#include <stdlib.h>// for MAX_PATH

#include "gtest/gtest.h"

#include "engine/ReadFile.h"



//Test ReadFile( std::string _fileName );
TEST(readfileTest, openTest) 
{
    engine::ReadFile file1("badfile");
    EXPECT_TRUE(file1.isValid() == false);
       
    engine::ReadFile file2("test_data/testdata.txt");
    EXPECT_TRUE(file2.isValid());
}

//Test int read( char * read_buffer , size_t size );
TEST(readfileTest, readTest) {
    engine::ReadFile file("test_data/testdata.txt");
    ASSERT_TRUE(file.isValid());
    if(file.isValid())
    {
        char data[10];
        file.read(data, 5);
        data[5] = '\0';
        EXPECT_EQ(strcmp(data, "01234"), 0 ); 
    }
}

//Test int seek( size_t offset );
TEST(readfileTest, seekTest) {
    engine::ReadFile file("test_data/testdata.txt");
    ASSERT_TRUE(file.isValid());
    if(file.isValid())
    {
        char data[10];
        file.seek(2);
        file.read(data, 5);
        data[5] = '\0';
        EXPECT_EQ(strcmp(data, "23456"), 0 ); 
    }
    
    engine::ReadFile fileBad("badfile");
    EXPECT_EQ(fileBad.seek(2), 1) << "Trying to seek a bad file should return 1";
    

}
    
//Test bool isValid();
//Test void close();
TEST(readfileTest, closeTest) {
    engine::ReadFile file1("badfile");
    EXPECT_TRUE(file1.isValid() == false);
       
    engine::ReadFile file2("test_data/testdata.txt");
    ASSERT_TRUE(file2.isValid());
   
    if(file2.isValid())
    {
      file2.close();
      EXPECT_TRUE(file2.isValid() == false);
    }
    
}


