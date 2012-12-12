/* ****************************************************************************
 *
 * FILE            SamsonDataSetTest.cpp
 *
 * AUTHOR          Ken Zangelin
 *
 * DATE            Nov 2012
 *
 *
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 *
 * DESCRIPTION
 * unit testing of the class SamsonDataSet in the samson common library
 *
 */
#include "gtest/gtest.h"
#include "logMsg/logMsg.h"

#include "samson/common/SamsonDataSet.h"
#include "unitTest/common_engine_test.h"


// -----------------------------------------------------------------------------
// Test the SamsonDataSet create method
//
TEST(samson_common_SamsonDataSet, constructor) {
  init_engine_test();

  // Test on Empty directory
  int status = mkdir("test_data/EmptyDir", 0777);
  EXPECT_EQ(0, status);

  au::ErrorManager                          eManager;
  au::SharedPointer<samson::SamsonDataSet>  dataSetP = samson::SamsonDataSet::create("test_data/EmptyDir", eManager);
  EXPECT_TRUE(eManager.HasErrors());
  EXPECT_STREQ(eManager.GetLastError().c_str(), "No content");

  // Test on Non-existing directory
  eManager.Reset();
  dataSetP = samson::SamsonDataSet::create("/tmp/not a directory", eManager);
  EXPECT_TRUE(eManager.HasErrors());
  EXPECT_STREQ(eManager.GetLastError().c_str(), "/tmp/not a directory doesn't exist");

  // Test on correct directory with a correct samson data file inside
  eManager.Reset();
  // dataSetP = samson::SamsonDataSet::create("test_data/SamsonDataSetTest", eManager);
  // EXPECT_STREQ(eManager.GetLastError().c_str(), "Unknown data type for key: system.String");
  // EXPECT_STREQ(eManager.GetLastError().c_str(), "No errors");

  status = rmdir("test_data/EmptyDir");
  EXPECT_EQ(0, status);

  close_engine_test();
}
