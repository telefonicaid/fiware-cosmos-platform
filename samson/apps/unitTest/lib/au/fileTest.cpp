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

TEST(au_file, simple){
  
  EXPECT_EQ(au::CreateDirectory("/tmp/testSamsonSetup"), au::OK)
  << "Error in createDirectory test";
  
  EXPECT_EQ(au::CreateFullDirectory("/tmp/level1/level2/level3/testSamsonSetup"), au::OK)
  << "Error in createFullDirectory test";
  
  EXPECT_EQ(au::GetCannonicalPath("/tmp/level1/level2/level3/") , "/tmp/level1/level2/level3")
  << "Error in cannonical_path test";
  
  EXPECT_EQ(au::path_from_directory("", "file"), "file") << "Error in path_from_directory() ";
  EXPECT_EQ(au::path_from_directory("/dir", "file"), "/dir/file") << "Error in path_from_directory() ";
  EXPECT_EQ(au::path_from_directory("/dir/", "file"), "/dir/file") << "Error in path_from_directory() ";
  
}
