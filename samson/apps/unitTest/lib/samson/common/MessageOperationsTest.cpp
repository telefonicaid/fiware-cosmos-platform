/* ****************************************************************************
 *
 * FILE            MessageOperationsTest.cpp
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
 * unit testing of the file MessageOperations.h in the samson common library
 *
 */
#include <string>

#include "gtest/gtest.h"

#include "au/containers/Dictionary.h"

#include "samson/common/MessagesOperations.h"  // template functions to test
#include "samson/common/Visualitzation.h"
#include "samson/common/samson.pb.h"           // CollectionRecord
#include "samson/stream/BlockManager.h"
#include "samson/worker/BlockRequest.h"

#include "unitTest/common_engine_test.h"

// -----------------------------------------------------------------------------
// Test the MessageOperations 'add' function
//
TEST(samson_common_MessageOperations, add) {
  samson::gpb::CollectionRecord  record;
  std::string                    test = "test";

  samson::add(&record, "test", test, "system.String");
  EXPECT_EQ(record.item_size(), 1);
}

// -----------------------------------------------------------------------------
// Test the MessageOperations 'name_match' function
//
TEST(samson_common_MessageOperations, name_match) {
  bool match   = samson::name_match("/tmp/a*", "/tmp/abc");
  bool nomatch = samson::name_match("/tmp/a*", "/tmp/bc");

  EXPECT_TRUE(match);
  EXPECT_FALSE(nomatch);
}

// -----------------------------------------------------------------------------
// Test the MessageOperations 'GetCollectionForMap' function
//
TEST(DISABLED_samson_common_MessageOperations, GetCollectionForMap) {
  init_engine_test();
  samson::stream::BlockManager::init();
  
  samson::Visualization                       visualization;
  std::map<size_t, samson::BlockRequest*>     requests;
  au::SharedPointer<samson::gpb::Collection>  collection;
  samson::BlockRequest*                       blockRequest1 = new samson::BlockRequest(NULL, 0);
  samson::BlockRequest*                       blockRequest2 = new samson::BlockRequest(NULL, 0);

  requests.insert(std::pair<size_t, samson::BlockRequest*>(1, blockRequest1));
  requests.insert(std::pair<size_t, samson::BlockRequest*>(2, blockRequest2));

  collection = samson::GetCollectionForMap(std::string("Blocks"), requests, visualization);
  EXPECT_TRUE(collection != NULL);

  close_engine_test();
}
