/* ****************************************************************************
 *
 * FILE            BlockKVInfoTest.cpp
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
 * unit testing of the common class in the samson  library
 *
 */
#include "gtest/gtest.h"

#include "samson/common/FullKVInfo.h"
#include "samson/common/BlockKVInfo.h"

// ----------------------------------------------------------------------------
// constructor - 
//
TEST(samson_common_BlockKVInfo, constructor) {
  samson::BlockKVInfo blockInfo;
  EXPECT_EQ(blockInfo.size, 0);
  EXPECT_EQ(blockInfo.kvs, 0);
  EXPECT_EQ(blockInfo.num_blocks, 0);
}

// ----------------------------------------------------------------------------
// AppendBlock - 
//
TEST(samson_common_BlockKVInfo, AppendBlock) {
  samson::BlockKVInfo blockInfo;
  samson::FullKVInfo  fullKvInfo(20, 10);

  blockInfo.AppendBlock(fullKvInfo);
  EXPECT_EQ(blockInfo.size, 20);
  EXPECT_EQ(blockInfo.kvs, 10);
  EXPECT_EQ(blockInfo.num_blocks, 1);
  EXPECT_STREQ(" 1.00 Bl/ 10.0 kvs/ 20.0 B", blockInfo.str().c_str());
}

// ----------------------------------------------------------------------------
// Append - 
//
TEST(samson_common_BlockKVInfo, Append) {
  samson::BlockKVInfo blockInfo;
  samson::BlockKVInfo blockInfo2;
  samson::FullKVInfo  fullKvInfo(20, 10);

  blockInfo.AppendBlock(fullKvInfo);
  blockInfo2.Append(blockInfo);
  EXPECT_EQ(blockInfo2.size, 20);
  EXPECT_EQ(blockInfo2.kvs, 10);
  EXPECT_EQ(blockInfo2.num_blocks, 1);
}
