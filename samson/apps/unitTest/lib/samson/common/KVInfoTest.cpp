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

#include "engine/Buffer.h"
#include "engine/Engine.h"
#include "engine/MemoryManager.h"

#include "samson/common/KVFile.h"
#include "samson/common/KVHeader.h"
#include "samson/common/KVInfo.h"
#include "samson/module/Data.h"
#include "samson/module/ModulesManager.h"

#include "unitTest/common_engine_test.h"

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

  // EXPECT_EQ(toInfoWithHugeSize.canAppend(fromInfoWithHugeSize), false);
  // EXPECT_EQ(toInfoWithHugeKvs.canAppend(fromInfoWithHugeKvs), false);
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
TEST(DISABLED_samson_common_KVInfo, createKVInfoVector) {
  au::ErrorManager  errorMgr;  
  samson::KVInfo*   infoP = samson::createKVInfoVector(NULL, &errorMgr);
  EXPECT_EQ(NULL, infoP);

  char*             data = (char*) "a:1, b:2, c:3";
  infoP = samson::createKVInfoVector(data, &errorMgr);
  EXPECT_EQ(NULL, infoP);

  samson::KVFormat  format("samson:string", "system::string");
  samson::KVInfo    info(10, 30);
  samson::KVHeader  badHeader;

  badHeader.Init(format, info);

  infoP = samson::createKVInfoVector((char*) &badHeader, &errorMgr);
  EXPECT_TRUE(infoP == NULL);

  init_engine_test();
  {
    au::ErrorManager         errorMgr;
    samson::ModulesManager*  mm = au::Singleton<samson::ModulesManager>::shared();
    mm->AddModulesFromDirectory("test_modules", errorMgr);

    samson::Data*          dataKeyP   = mm->GetData("system.String");
    samson::Data*          dataValueP = mm->GetData("system.UInt");

    EXPECT_TRUE(dataKeyP   != NULL);
    EXPECT_TRUE(dataValueP != NULL);

    samson::KVFormat       format2("system.String", "system.UInt");
    samson::KVInfo         info2(0, 0);
    engine::BufferPointer  buffer         =
       (engine::BufferPointer) engine::Buffer::Create("testbuffer",  10000);
    samson::KVHeader*      goodHeader     = reinterpret_cast<samson::KVHeader*>(buffer->data());
    samson::DataInstance*  keyInstanceP   = dataKeyP->getInstance();
    samson::DataInstance*  valueInstanceP = dataValueP->getInstance();
    char*                  dataPosition   = buffer->data();

    EXPECT_TRUE(keyInstanceP != NULL);
    EXPECT_TRUE(valueInstanceP != NULL);

    size_t offset = sizeof(samson::KVHeader);
    for (unsigned int ix = 0; ix < 100; ++ix) {
       keyInstanceP->setFromString("123sss");
       valueInstanceP->setFromString("12");

       int size = keyInstanceP->serialize(&dataPosition[offset]);
       offset     += size;
       info2.size += size;

       size = valueInstanceP->serialize(&dataPosition[offset]);
       offset     += size;
       info2.size += size;

       ++info2.kvs;
    }

    goodHeader->Init(format2, info2);
    infoP = samson::createKVInfoVector((char*) &goodHeader, &errorMgr);
    LM_M(("Error: '%s'", errorMgr.GetLastError().c_str()));
    EXPECT_TRUE(infoP == NULL);
  }

  close_engine_test();
}
