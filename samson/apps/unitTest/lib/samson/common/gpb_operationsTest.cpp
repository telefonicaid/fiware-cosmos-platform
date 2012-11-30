/* ****************************************************************************
 *
 * FILE            gpb_operationsTest.cpp
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
 * unit test of the file 'gpb_operations.cpp' in the samson/common library
 *
 */
#include "gtest/gtest.h"

#include "samson/common/gpb_operations.h"
#include "samson/common/samson.pb.h"
#include "samson/stream/BlockManager.h"
#include "samson/worker/BlockRequest.h"

#include "unitTest/common_engine_test.h"

// -----------------------------------------------------------------------------
// getStreamOperation - 
//
TEST(samson_common_gpb_operations, getStreamOperation) {
  samson::gpb::Data              data;
  samson::gpb::StreamOperation*  op  = data.add_operations();
  samson::gpb::StreamOperation*  op1 = samson::gpb::getStreamOperation(&data, "test");
  samson::gpb::StreamOperation*  op2 = samson::gpb::getStreamOperation(&data, 1);

  EXPECT_TRUE(op  != NULL);
  EXPECT_TRUE(op1 == NULL);
  EXPECT_TRUE(op2 == NULL);

  samson::gpb::reset_stream_operations(&data);
  samson::gpb::removeStreamOperation(&data, "nada");

}

// -----------------------------------------------------------------------------
// DataInfoForRanges - 
//
TEST(samson_common_gpb_operations, DataInfoForRanges) {
  samson::gpb::DataInfoForRanges dataInfo;

  EXPECT_EQ(dataInfo.data_size, 0);
}
