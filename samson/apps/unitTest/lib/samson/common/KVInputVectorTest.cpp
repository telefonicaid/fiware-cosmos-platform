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

#include "samson/common/KVInputVector.h"

// -----------------------------------------------------------------------------
// addKVs - test the addition of key-values
//
TEST(samson_common_KVInputVector, addKVs) {
  samson::Operation      op("TestOperation", samson::Operation::map);
  samson::KVInputVector  inputVec(&op);
  samson::KVInfo         info(100, 10);

  // samson::KV             kvV[12];
  // inputVec.addKVs(12, info, &kvV[0]);
}

// -----------------------------------------------------------------------------
// sort - test the sorting of key-values
//
TEST(samson_common_KVInputVector, sort) {
  samson::Operation      op("TestOperation", samson::Operation::map);
  samson::KVInputVector  inputVec(&op);

  inputVec.sort();
}
