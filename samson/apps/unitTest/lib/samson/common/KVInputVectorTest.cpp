/* ****************************************************************************
 *
 * FILE            KVInputVectorTest.cpp
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
 *
 * DESCRIPTION
 *
 * unit test of the class 'KVInputVector' in the samson/common library
 *
 */
#include "gtest/gtest.h"

#include "engine/Buffer.h"
#include "engine/Engine.h"
#include "engine/MemoryManager.h"

#include "samson/module/ModulesManager.h"
#include "samson/common/KVHeader.h"
#include "samson/common/KVInputVector.h"

#include "unitTest/common_engine_test.h"

namespace samson {
  extern bool         compareKV(KV* kv1, KV* kv2);
  extern std::string  str_kv(KV* kv);
  extern bool         equalKV(KV *kv1, KV *kv2);
}

// -----------------------------------------------------------------------------
// Init - test the Init method
//
TEST(samson_common_KVInputVector, Init) {
  samson::KVInputVector inputVec = samson::KVInputVector(4);

  inputVec.Init();
  EXPECT_TRUE(inputVec.GetNext() == NULL);
}

// -----------------------------------------------------------------------------
// addAndSort - test the addition of key-values and sort them
// Create a KVFile, and use the kvP from it
//
TEST(samson_common_KVInputVector, addAndSort) {
  samson::KVInputVector inputVec = samson::KVInputVector(4);
  samson::KVInfo        info(4, 4);
  samson::KV            kvs[4];

  for (unsigned int ix = 0; ix < sizeof(kvs) / sizeof(kvs[0]); ++ix) {
     kvs[ix].key         = strdup("KEY_X");
     kvs[ix].key[4]      = '0' + (9 - ix);
     kvs[ix].key_size    = 5;
     kvs[ix].value       = strdup("VALUE_X");
     kvs[ix].value[6]    = '0' + (9 - ix);
     kvs[ix].value_size  = 7;
  }

  inputVec.prepareInput(4);
  inputVec.addKVs(1, info, kvs);
  EXPECT_EQ(inputVec.num_kvs, 4);
  EXPECT_STREQ(inputVec.kv[0].key, "KEY_9");
  EXPECT_STREQ(inputVec.kv[1].key, "KEY_8");
  EXPECT_STREQ(inputVec.kv[2].key, "KEY_7");
  EXPECT_STREQ(inputVec.kv[3].key, "KEY_6");

  inputVec.sort();
  EXPECT_STREQ(inputVec._kv[0]->key, "KEY_6");
  EXPECT_STREQ(inputVec._kv[1]->key, "KEY_7");
  EXPECT_STREQ(inputVec._kv[2]->key, "KEY_8");
  EXPECT_STREQ(inputVec._kv[3]->key, "KEY_9");
  
  // 'unsorting' first two - reparing for sortAndMerge(2) ...
  inputVec._kv[0]->key[4]   = '7';
  inputVec._kv[0]->value[6] = '7';
  inputVec._kv[1]->key[4]   = '6';
  inputVec._kv[1]->value[6] = '6';
  EXPECT_STREQ(inputVec._kv[0]->key, "KEY_7");
  EXPECT_STREQ(inputVec._kv[1]->key, "KEY_6");
  inputVec.sortAndMerge(2);
  EXPECT_STREQ(inputVec._kv[0]->key, "KEY_6");
  EXPECT_STREQ(inputVec._kv[1]->key, "KEY_7");
  
  // cleanup
  for (unsigned int ix = 0; ix < sizeof(kvs) / sizeof(kvs[0]); ++ix) {
    free(kvs[ix].key);
    free(kvs[ix].value);
  }
}

// -----------------------------------------------------------------------------
// operationConstructor - 
//
TEST(DISABLED_samson_common_KVInputVector, operationConstructor) {
  init_engine_test();

  au::ErrorManager         errorMgr;
  samson::ModulesManager*  mm   = au::Singleton<samson::ModulesManager>::shared();
  mm->AddModulesFromDirectory("test_modules", errorMgr);

  samson::Operation     operation("testOperation", samson::Operation::map);
  samson::KVFormat      format1("system.String", "system.UInt");
  samson::KVFormat      format2("system.String", "system.UInt");
  samson::KVFormat      format3("system.String", "system.UInt");

  operation.inputFormats.push_back(format1);
  operation.inputFormats.push_back(format2);
  operation.inputFormats.push_back(format3);

  samson::KVInputVector inputVec = samson::KVInputVector(&operation);
  samson::KVInfo        info(4, 4);

  close_engine_test();
}

// -----------------------------------------------------------------------------
// addKVs - test the addition of key-values
//
TEST(DISABLED_samson_common_KVInputVector, addKVs) {
  init_engine_test();

  au::ErrorManager         errorMgr;
  samson::ModulesManager*  mm   = au::Singleton<samson::ModulesManager>::shared();
  mm->AddModulesFromDirectory("test_modules", errorMgr);

  samson::Operation       op("TestOperation", samson::Operation::map);
  samson::KVFormat        format1("system.String", "system.UInt");
  samson::KVFormat        format2("system.String", "system.UInt");
  samson::KVFormat        format3("system.String", "system.UInt");

  op.inputFormats.push_back(format1);
  op.inputFormats.push_back(format2);
  op.inputFormats.push_back(format3);

  samson::KVInputVector   inputVec(&op);
  samson::KVInfo          info(100, 10);
  samson::KVFormat        format("system.String", "system.UInt");

  samson::Data*           dataKeyP   = mm->GetData("system.String");
  samson::Data*           dataValueP = mm->GetData("system.UInt");

  EXPECT_TRUE(dataKeyP   != NULL);
  EXPECT_TRUE(dataValueP != NULL);

  engine::BufferPointer buffer         = 
     (engine::BufferPointer) engine::Buffer::Create("testbuffer",  10000);
  samson::DataInstance* keyInstanceP   = dataKeyP->getInstance();  
  samson::DataInstance* valueInstanceP = dataValueP->getInstance();  
  char*                 dataPosition   = buffer->data();

  EXPECT_TRUE(keyInstanceP != NULL);
  EXPECT_TRUE(valueInstanceP != NULL);

  size_t offset = sizeof(samson::KVHeader);
  for (unsigned int ix = 0; ix < 100; ++ix) {
    keyInstanceP->setFromString(strdup("123sss"));
    valueInstanceP->setFromString(strdup("12"));

    int size = keyInstanceP->serialize(&dataPosition[offset]);
    offset    += size;
    info.size += size;

    size = valueInstanceP->serialize(&dataPosition[offset]);
    offset    += size;
    info.size += size;

    ++info.kvs;
  }

  samson::KV kvV[100];
  for (unsigned int ix = 0; ix < sizeof(kvV) / sizeof(kvV[0]); ++ix) {
     kvV[ix].key   = strdup("12");
     kvV[ix].value = strdup("twelve");
  }

  inputVec.prepareInput(10);
  inputVec.addKVs(9, info, kvV);
  EXPECT_EQ(100, inputVec.num_kvs) << "input vector should have 100 KVs";

  for (unsigned int ix = 0; ix < sizeof(kvV) / sizeof(kvV[0]); ++ix) {
    free(kvV[ix].key);
    free(kvV[ix].value);
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// prepareInput - 
//
TEST(samson_common_KVInputVector, prepareInput) {
  samson::Operation      op("TestOperation", samson::Operation::map);
  samson::KVInputVector  inputVec(&op);

  inputVec.prepareInput(3);
  EXPECT_EQ(3, inputVec.max_num_kvs);
  inputVec.prepareInput(4);
  EXPECT_EQ(4, inputVec.max_num_kvs);
}

// -----------------------------------------------------------------------------
// str_kv - 
//
TEST(samson_common_KVInputVector, str_kv) {
  samson::KV kv;
  kv.key        = (char*) "123";
  kv.key_size   = 4;
  kv.value      = (char*) "456";
  kv.value_size = 4;

  std::string kv_str = samson::str_kv(&kv);
  EXPECT_STREQ(kv_str.c_str(), "KV[4][4]");
}

// -----------------------------------------------------------------------------
// equalKV - test comparing key-values
//
TEST(samson_common_KVInputVector, equalKV) {
  samson::KV pair1;
  samson::KV pair2;

  pair1.key        = (char*) "123";
  pair1.key_size   = 3;
  pair1.value      = (char*) "456";
  pair1.value_size = 3;

  pair2.key        = (char*) "1234";
  pair2.key_size   = 4;
  pair2.value      = (char*) "456";
  pair2.value_size = 3;

  bool kvsAreEqual = samson::equalKV(&pair1, &pair2);
  EXPECT_FALSE(kvsAreEqual);

  pair2.key        = (char*) "124";
  pair2.key_size   = 3;
  kvsAreEqual = samson::equalKV(&pair1, &pair2);
  EXPECT_FALSE(kvsAreEqual);

  pair2.key = (char*) "123";
  kvsAreEqual = samson::equalKV(&pair1, &pair2);
  EXPECT_TRUE(kvsAreEqual);
}

// -----------------------------------------------------------------------------
// compareKV - test comparing key-values
//
TEST(samson_common_KVInputVector, compareKV) {
  samson::KV pair1;
  samson::KV pair2;

  pair1.key        = (char*) "123";
  pair1.key_size   = 4;
  pair1.value      = (char*) "456";
  pair1.value_size = 4;
  pair1.input      = 1;

  pair2.key        = (char*) "123";
  pair2.key_size   = 4;
  pair2.value      = (char*) "456";
  pair2.value_size = 4;
  pair2.input      = 1;

  // 1. EQUAL
  bool kvsComparison = samson::compareKV(&pair1, &pair2);
  EXPECT_FALSE(kvsComparison);

  // 2. pair1.key_size < pair2.key_size
  pair1.key_size = 3;  
  kvsComparison = samson::compareKV(&pair1, &pair2);
  EXPECT_TRUE(kvsComparison);
  pair1.key_size = 4;

  // 3. pair1.key_size > pair2.key_size
  pair1.key_size = 5;
  kvsComparison = samson::compareKV(&pair1, &pair2);
  EXPECT_FALSE(kvsComparison);
  pair1.key_size = 4;

  // 4. number of input differs
  pair1.input    = 2;
  kvsComparison = samson::compareKV(&pair1, &pair2);
  EXPECT_FALSE(kvsComparison);
  pair1.input = 1;

  // 5. kv1->key[i] < kv2->key[i]
  pair1.key = (char*) "012";
  kvsComparison = samson::compareKV(&pair1, &pair2);
  EXPECT_TRUE(kvsComparison);
  pair1.key = (char*) "123";

  // 6. kv1->key[i] > kv2->key[i]
  pair1.key = (char*) "234";
  kvsComparison = samson::compareKV(&pair1, &pair2);
  EXPECT_FALSE(kvsComparison);
  pair1.key = (char*) "123";

  // 7. kv1->value_size < kv2->value_size
  pair1.value_size = 3;
  kvsComparison = samson::compareKV(&pair1, &pair2);
  EXPECT_TRUE(kvsComparison);
  pair1.value_size = 4;

  // 8. kv1->value_size > kv2->value_size
  pair1.value_size = 5;
  kvsComparison = samson::compareKV(&pair1, &pair2);
  EXPECT_FALSE(kvsComparison);
  pair1.value_size = 4;

  // 9. kv1->value[i] < kv2->value[i]
  pair1.value = (char*) "345";
  kvsComparison = samson::compareKV(&pair1, &pair2);
  EXPECT_TRUE(kvsComparison);
  pair1.value = (char*) "456";

  // 10. kv1->value[i] > kv2->value[i]
  pair1.value = (char*) "567";
  kvsComparison = samson::compareKV(&pair1, &pair2);
  EXPECT_FALSE(kvsComparison);
  pair1.value = (char*) "456";
}

// -----------------------------------------------------------------------------
// GetNext - 
//
TEST(DISABLED_samson_common_KVInputVector, GetNext) {
  init_engine_test();
  au::ErrorManager         errorMgr;
  samson::ModulesManager*  mm   = au::Singleton<samson::ModulesManager>::shared();
  mm->AddModulesFromDirectory("test_modules", errorMgr);

  samson::Operation     operation("testOperation", samson::Operation::map);
  samson::KVFormat      format1("system.String", "system.UInt");
  samson::KVFormat      format2("system.String", "system.UInt");
  samson::KVFormat      format3("system.String", "system.UInt");

  operation.inputFormats.push_back(format1);
  operation.inputFormats.push_back(format2);
  operation.inputFormats.push_back(format3);

  samson::KVInputVector inputVec = samson::KVInputVector(&operation);
  samson::KVInfo        info(10, 10);
  samson::KV            kvs[10];

  for (unsigned int ix = 0; ix < sizeof(kvs) / sizeof(kvs[0]); ++ix) {
    kvs[ix].key         = (ix < 5)? strdup("KEY_X") : strdup("KEY_Y");
    kvs[ix].key_size    = 5;
    kvs[ix].value       = (ix < 5)? strdup("VALUE_X") : strdup("VALUE_Y");
    kvs[ix].value_size  = 7;
  }

  inputVec.prepareInput(10);
  inputVec.addKVs(1, info, kvs);
  EXPECT_EQ(inputVec.num_kvs, 10);

  samson::KVSetStruct* set = inputVec.GetNext();
  EXPECT_TRUE(set == NULL);
  // EXPECT_EQ(set->num_kvs, 5);
  
  // cleanup
  for (unsigned int ix = 0; ix < sizeof(kvs) / sizeof(kvs[0]); ++ix) {
    free(kvs[ix].key);
    free(kvs[ix].value);
  }

  close_engine_test();
}
