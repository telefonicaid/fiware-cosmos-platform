/* ****************************************************************************
 *
 * FILE            common_test.cpp
 *
 * AUTHOR         Gregorio Escalada
 *
 * DATE            May 2012
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
 *
 * DESCRIPTION
 *
 * unit testing of the common class in the samson  library
 *
 */
#include "gtest/gtest.h"

#include "au/ErrorManager.h"

#include "samson/common/KVHeader.h"
#include "samson/common/KVInfo.h"
#include "samson/common/KVRange.h"
#include "samson/common/MemoryCheck.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/Rate.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/status.h"

// Test  KVInfo;
TEST(samson_common_KVInfo, test1) {
  samson::KVInfo info_1;

  EXPECT_EQ(info_1.str(), "(     0 kvs in     0 bytes )") << "Error in KVInfo str() for default constructor";
  info_1.set(100, 10);
  EXPECT_EQ(info_1.str(), "(  10.0 kvs in   100 bytes )") << "Error in KVInfo str() for set";

  info_1.append(1, 1);
  EXPECT_EQ(info_1.str(), "(  11.0 kvs in   101 bytes )") << "Error in KVInfo str() for append";
  info_1.remove(2, 2);
  EXPECT_EQ(info_1.str(), "(  9.00 kvs in  99.0 bytes )") << "Error in KVInfo str() for append";
  EXPECT_EQ(info_1.isEmpty(), false) << "Error in KVInfo isValid() for isEmpty() false";
  info_1.clear();
  EXPECT_EQ(info_1.isEmpty(), true) << "Error in KVInfo isValid() for isEmpty() true";

  samson::KVInfo info_2(2000, 20);
  EXPECT_EQ(info_2.str(), "(  20.0 kvs in  2.00Kbytes )") << "Error in KVInfo for initialised constructor";
  EXPECT_EQ(info_1.canAppend(info_2), true) << "Error in KVInfo str() for canAppend";
  info_1.append(info_2);
  EXPECT_EQ(info_1.str(), "(  20.0 kvs in  2.00Kbytes )") << "Error in KVInfo append from KVInfo";
}

// -----------------------------------------------------------------------------
// Test KVInfo.canAppend
//
TEST(samson_common_KVInfo, canAppend) {
  samson::KVInfo toInfoWithHugeSize(0xFFFFFFFF, 5);
  samson::KVInfo fromInfoWithHugeSize(0xFFFFFFFF, 5);
  samson::KVInfo toInfoWithHugeKvs(5, 0xFFFFFFFF);
  samson::KVInfo fromInfoWithHugeKvs(5, 0xFFFFFFFF);

  EXPECT_EQ(false, toInfoWithHugeSize.canAppend(fromInfoWithHugeSize));
  EXPECT_EQ(false, toInfoWithHugeKvs.canAppend(fromInfoWithHugeKvs));
}

// -----------------------------------------------------------------------------
// Test append and remove
//
TEST(samson_common_KVInfo, appendRemove) {
  samson::KVInfo to(0, 0);
  samson::KVInfo from(1, 1);

  to.append(from);
  EXPECT_STREQ("(  1.00 kvs in  1.00 bytes )", to.str().c_str());

  to.remove(from);
  EXPECT_STREQ("(     0 kvs in     0 bytes )", to.str().c_str());
}

// -----------------------------------------------------------------------------
// Test creation of info vectors
//
TEST(samson_common_KVInfo, createKVInfoVector) {
  au::ErrorManager  errorMgr;  
  samson::KVInfo*   infoP = samson::createKVInfoVector(NULL, &errorMgr);
  EXPECT_EQ(NULL, infoP);

  char*             data = (char*) "a:1, b:2, c:3";
  infoP = samson::createKVInfoVector(data, &errorMgr);
  EXPECT_EQ(NULL, infoP);

  samson::KVFormat  format("samson:string", "system::string");
  samson::KVInfo    info(10, 30);
  samson::KVHeader  header;

  header.Init(format, info);

  infoP = samson::createKVInfoVector((char*) &header, &errorMgr);
  EXPECT_TRUE(infoP == NULL);
}
