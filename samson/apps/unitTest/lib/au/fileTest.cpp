
#include "au/file.h"
#include "gtest/gtest.h"

TEST(au_file, simple){
  
  std::string test_dir = au::GetRandomDirectory();
  
  EXPECT_EQ(au::CreateDirectory(test_dir), au::OK)
  << "Error in createDirectory test";
  
  EXPECT_EQ(au::CreateFullDirectory( test_dir + "/a/b/c/" ), au::OK)
  << "Error in createFullDirectory test";
  
  EXPECT_EQ(test_dir + "/a/b/c",au::GetCannonicalPath(test_dir + "/a/b/c/"))
  << "Error in cannonical_path test";
  
  EXPECT_EQ(au::path_from_directory("", "file"), "file") << "Error in path_from_directory() ";
  EXPECT_EQ(au::path_from_directory("/dir", "file"), "/dir/file") << "Error in path_from_directory() ";
  EXPECT_EQ(au::path_from_directory("/dir/", "file"), "/dir/file") << "Error in path_from_directory() ";
  
}
