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

#include "samson/common/MemoryCheck.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/status.h"
#include "samson/common/Rate.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/KVRange.h"

#include "gtest/gtest.h"


// Test  KVRange;
TEST(samson_common_KVRange, test1) {
  samson::KVRange range_1;
  
  EXPECT_EQ(range_1.isValid(), true) << "Error in KVRange isValid() for default constructor";
  range_1.set(0, 10);
  EXPECT_EQ(range_1.isValid(), true) << "Error in KVRange isValid() for set operation";
  range_1.set(-1, 10);
  EXPECT_EQ(range_1.isValid(), false) << "Error in KVRange isValid() for wrong set operation";
  range_1.set(0, -1);
  EXPECT_EQ(range_1.isValid(), false) << "Error in KVRange isValid() for wrong set operation";
  range_1.set(1, 0);
  EXPECT_EQ(range_1.isValid(), false) << "Error in KVRange isValid() for wrong set operation";
  range_1.set(0, 20);
  
  samson::KVRange range_2(1, 10);
  EXPECT_EQ(range_1.isValid(), true) << "Error in KVRange isValid() for initialised constructor";
  
  
  EXPECT_EQ(range_2.str(), "[00001 00010)") << "Error in KVRange str";
  EXPECT_EQ(range_2.size(), 9) << "Error in KVRange getNumHashGroups";
  
  EXPECT_EQ(range_1.IsOverlapped(range_2), true) << "Error in KVRange overlap true";
  EXPECT_EQ(range_2.IsOverlapped(range_1), true) << "Error in KVRange overlap true";
  samson::KVRange range_3(10, 30);
  EXPECT_EQ(range_3.IsOverlapped(range_1), true) << "Error in KVRange overlap true";
  EXPECT_EQ(range_3.IsOverlapped(range_2), false) << "Error in KVRange overlap false";
  
  EXPECT_EQ(range_1.Includes(range_2), true) << "Error in KVRange includes true";
  EXPECT_EQ(range_2.Includes(range_1), false) << "Error in KVRange includes false";
  EXPECT_EQ(range_1.Contains(15), true) << "Error in KVRange contains true";
  EXPECT_EQ(range_1.Contains(20), false) << "Error in KVRange contains false";
  EXPECT_EQ(range_1.Contains(range_2), true) << "Error in KVRange includes true";
  EXPECT_EQ(range_2.Contains(range_1), false) << "Error in KVRange includes false";
  
  EXPECT_EQ((range_1 < range_2), false) << "Error in KVRange operator < false";
  range_1.set(11, 20);
  EXPECT_EQ((range_2 < range_1), true) << "Error in KVRange operator < true";
  
  EXPECT_EQ((range_1 == range_2), false) << "Error in KVRange operator == false";
  range_1.set(1, 10);
  EXPECT_EQ((range_2 == range_1), true) << "Error in KVRange operator == true";
  
  EXPECT_EQ((range_1 != range_2), false) << "Error in KVRange operator != false";
  range_1.set(1, 11);
  EXPECT_EQ((range_2 != range_1), true) << "Error in KVRange operator != true";
}
