/* ****************************************************************************
 *
 * FILE            commonTest.cpp
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
 * unit test of the file 'common.cpp' in the samson/common library
 *
 */
#include <utility>
#include <map>
#include <string>

#include "gtest/gtest.h"

#include "samson/common/common.h"
#include "samson/stream/BlockManager.h"
#include "samson/worker/BlockRequest.h"

#include "unitTest/common_engine_test.h"

// -----------------------------------------------------------------------------
// str_block_id -
//
TEST(samson_common_common, str_block_id) {
  const size_t  block_id = 14;
  std::string   block_id_as_string = samson::str_block_id(block_id);

  EXPECT_STREQ("B_14_0", block_id_as_string.c_str());
}

// -----------------------------------------------------------------------------
// GetTableFromCollection -
//
TEST(DISABLED_samson_common_common, GetTableFromCollection) {
  init_engine_test();
  samson::stream::BlockManager::init();

  samson::Visualization                       visualization;
  std::map<size_t, samson::BlockRequest*>     requests;
  samson::gpb::Collection                     collection;
  au::SharedPointer<samson::gpb::Collection>  collectionP =
     (au::SharedPointer<samson::gpb::Collection>) &collection;
  au::SharedPointer<au::tables::Table>        table       =
     samson::GetTableFromCollection(collectionP);

  samson::BlockRequest*                       blockRequest1 = new samson::BlockRequest(NULL, 0);
  samson::BlockRequest*                       blockRequest2 = new samson::BlockRequest(NULL, 0);

  requests.insert(std::pair<size_t, samson::BlockRequest*>(1, blockRequest1));
  requests.insert(std::pair<size_t, samson::BlockRequest*>(2, blockRequest2));

  collectionP = samson::GetCollectionForMap(std::string("Blocks"), requests, visualization);
  EXPECT_TRUE(collectionP != NULL);
  au::SharedPointer<au::tables::Table> table2 = samson::GetTableFromCollection(collectionP);

  close_engine_test();
}
