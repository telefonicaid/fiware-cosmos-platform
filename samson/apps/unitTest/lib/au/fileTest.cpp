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

#include "au/file.h"
#include "gtest/gtest.h"

TEST(au_file, simple) {
  std::string test_dir = au::GetRandomDirectory();

  EXPECT_EQ(au::CreateDirectory(test_dir), au::OK)
  << "Error in createDirectory test";

  EXPECT_EQ(au::OK, au::CreateFullDirectory(test_dir + "/a/b/c/"))
  << "Error in createFullDirectory test";

  EXPECT_EQ(test_dir + "/a/b/c", au::GetCannonicalPath(test_dir + "/a/b/c/"))
  << "Error in cannonical_path test";

  EXPECT_EQ(au::path_from_directory("", "file"), "file") << "Error in path_from_directory() ";
  EXPECT_EQ(au::path_from_directory("/dir", "file"), "/dir/file") << "Error in path_from_directory() ";
  EXPECT_EQ(au::path_from_directory("/dir/", "file"), "/dir/file") << "Error in path_from_directory() ";

  au::ErrorManager error;
  au::RemoveDirectory(test_dir, error);
  EXPECT_FALSE(error.IsActivated());
}
