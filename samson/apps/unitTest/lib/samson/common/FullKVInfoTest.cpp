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
#include <string>

#include "samson/common/MemoryCheck.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/status.h"
#include "samson/common/Rate.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/KVRange.h"
#include "samson/common/KVInfo.h"
#include "samson/common/FullKVInfo.h"
#include "samson/common/common.h"

#include "gtest/gtest.h"


// Test  FullKVInfo;
TEST(samson_common_FullKVInfo, test1) {
  samson::FullKVInfo info_1;
  
  EXPECT_EQ(info_1.str(), "    0 kvs/    0 B") << "Error/FullKVInfo isValid() for default constructor";
  info_1.set(static_cast<samson::uint32>(100), static_cast<samson::uint32>(10));
  EXPECT_EQ(info_1.str(), " 10.0 kvs/  100 B") << "Error/FullKVInfo isValid() for set";
  
  std::ostringstream output;
  info_1.getInfo(output);
  EXPECT_EQ(output.str(),
            "<kv_info><kvs>10</kvs><size>100</size></kv_info>") <<
            "Error/FullKVInfo getInfo";

  info_1.append(static_cast<samson::uint32>(1), static_cast<samson::uint32>(1));
  EXPECT_EQ(info_1.str(), " 11.0 kvs/  101 B") << "Error/FullKVInfo isValid() for append";
  info_1.remove(static_cast<samson::uint32>(2), static_cast<samson::uint32>(2));
  EXPECT_EQ(info_1.str(), " 9.00 kvs/ 99.0 B") << "Error/FullKVInfo isValid() for append";
  EXPECT_EQ(info_1.isEmpty(), false) << "Error/FullKVInfo isValid() for isEmpty false";
  info_1.clear();
  EXPECT_EQ(info_1.isEmpty(), true) << "Error/FullKVInfo isValid() for isEmpty true";
  
  samson::FullKVInfo info_2(2000, 20);
  EXPECT_EQ(" 20.0 kvs/ 2.00KB",info_2.str() ) << "Error/FullKVInfo for initialised constructor";
  info_1.append(info_2);
  EXPECT_EQ(" 20.0 kvs/ 2.00KB" , info_1.str()) << "Error/FullKVInfo append from KVInfo";
  
  info_1.set(static_cast<samson::uint64>(1000), static_cast<samson::uint64>(100));
  EXPECT_EQ("  100 kvs/ 1.00KB",info_1.str()) << "Error/FullKVInfo isValid() for set 64 bits";

  EXPECT_EQ(info_1.fitsInKVInfo(), true) << "Error/FullKVInfo fitsInKVInfo true";

  samson::uint64 kvs = 1024 * 1024;
  kvs *= 1024 * 8;
  samson::uint64 size = 1024 * 1024;
  size *= 1024 * 8;
  info_1.set(kvs, size);
  EXPECT_EQ(info_1.fitsInKVInfo(), false) << "Error/FullKVInfo fitsInKVInfo false";
}

// -----------------------------------------------------------------------------
// Test append(uint64 _size, uint64 _kvs)
// Test remove(uint64 _size, uint64 _kvs)
//
TEST(samson_common_FullKVInfo, appendRemove1) {
  samson::FullKVInfo  info;
  samson::uint64      size = 12;
  samson::uint64      kvs  = 3;

  info.append(size, kvs);
  EXPECT_EQ(info.str(), " 3.00 kvs/ 12.0 B") << "Error appending to samson::FullKVInfo";

  info.remove(size, kvs);
  EXPECT_EQ(info.str(), "    0 kvs/    0 B") << "Error removing from samson::FullKVInfo";
}

// -----------------------------------------------------------------------------
// Test append(FullKVInfo other)
// Test remove(FullKVInfo other)
//
TEST(samson_common_FullKVInfo, appendRemove2) {
  samson::FullKVInfo  from;
  samson::FullKVInfo  to;
  samson::uint64      size = 12;
  samson::uint64      kvs  = 3;

  from.append(size, kvs);

  to.append(from);
  EXPECT_EQ(to.str(), " 3.00 kvs/ 12.0 B") << "Error appending to samson::FullKVInfo";

  to.remove(from);
  EXPECT_EQ(to.str(), "    0 kvs/    0 B") << "Error removing from samson::FullKVInfo";
}

// -----------------------------------------------------------------------------
// Test append(KVInfo other)
// Test remove(KVInfo other)
//
TEST(samson_common_FullKVInfo, appendRemove3) {
  samson::KVInfo     from(12, 3);
  samson::FullKVInfo to;

  to.append(from);
  // NOT (  3.00 kvs in  12.0 bytes )
  EXPECT_EQ(to.str(), " 3.00 kvs/ 12.0 B") << "Error appending to samson::FullKVInfo";

  to.remove(from);
  EXPECT_EQ(to.str(), "    0 kvs/    0 B") << "Error removing from samson::FullKVInfo";
}

// -----------------------------------------------------------------------------
// Test set(KVInfo other)
//
TEST(samson_common_FullKVInfo, set) {
  samson::KVInfo     from(12, 3);
  samson::FullKVInfo to;

  to.set(from);
  EXPECT_EQ(to.str(), " 3.00 kvs/ 12.0 B") << "Error appending to samson::FullKVInfo";
}

// -----------------------------------------------------------------------------
// Test FullKVInfo::getKVInfo
//
TEST(samson_common_FullKVInfo, getKVInfo) {
  samson::FullKVInfo from(12, 3);
  samson::KVInfo     to = from.getKVInfo();

  EXPECT_EQ(to.str(), "(  3.00 kvs in  12.0 bytes )") << "Error getting FullKVInfo via getKVInfo";

  std::string details = from.strDetailed();
  EXPECT_STREQ(details.c_str(), "( 3 kvs [ 3.00 ] in 12 bytes [ 12.0 ] )");
}

// -----------------------------------------------------------------------------
// Test FullKVInfo::fitsInKVInfo
//
TEST(samson_common_FullKVInfo, fitsInKVInfo) {
  samson::FullKVInfo info;
  samson::uint64     kvs  = MAX_UINT_32 * 2;
  samson::uint64     size = 100;

  info.set(size, kvs);
  EXPECT_EQ(info.fitsInKVInfo(), false) << "Error in FullKVInfo fitsInKVInfo false";
}
