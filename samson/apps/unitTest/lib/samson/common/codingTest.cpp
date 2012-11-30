/* ****************************************************************************
 *
 * FILE            codingTest.cpp
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
 * unit test of the file 'coding.cpp' in the samson/common library
 *
 */
#include "gtest/gtest.h"

#include "logMsg/logMsg.h"

#include "samson/common/coding.h"

// -----------------------------------------------------------------------------
// operators - 
//
TEST(samson_common_coding, operators) {
  samson::KVFormat formatAB("a", "b"); 
  samson::KVFormat formatAC("a", "c"); 
  samson::KVFormat formatBB("b", "b"); 

  EXPECT_TRUE(formatAB == formatAB);
  EXPECT_FALSE(formatAB != formatAB);
  EXPECT_FALSE(formatAB == formatBB);
  EXPECT_TRUE(formatAB != formatBB);
  EXPECT_TRUE(formatAB != formatAC);
  EXPECT_FALSE(formatAB == formatAC);
}

// -----------------------------------------------------------------------------
// HashGroupOutput -
//
TEST(samson_common_coding, HashGroupOutput) {
  samson::HashGroupOutput outputGroup;
  outputGroup.init();
  EXPECT_EQ(outputGroup.first_node, KV_NODE_UNASIGNED);
  EXPECT_EQ(outputGroup.last_node, KV_NODE_UNASIGNED);
}

// -----------------------------------------------------------------------------
// OutputChannel -
//
TEST(samson_common_coding, OutputChannel) {
   samson::OutputChannel outputChannel;
   outputChannel.init();
   EXPECT_EQ(outputChannel.hg[0].first_node, KV_NODE_UNASIGNED);
   EXPECT_EQ(outputChannel.hg[0].last_node, KV_NODE_UNASIGNED);
}

// -----------------------------------------------------------------------------
// NodeBuffer -
//
TEST(samson_common_coding, NodeBuffer) {
  samson::NodeBuffer nodeBuf;
  nodeBuf.init();
  EXPECT_EQ(nodeBuf.size, 0);
  EXPECT_EQ(nodeBuf.next, KV_NODE_UNASIGNED);

  // Exercise 'setNext'
  samson::uint32 next = nodeBuf.next;
  nodeBuf.setNext(77);
  EXPECT_EQ(77, nodeBuf.next);
  nodeBuf.setNext(next);
  EXPECT_EQ(next, nodeBuf.next);

  // Write OK
  nodeBuf.write((char*) "12345", 5);
  EXPECT_EQ(nodeBuf.availableSpace(), KV_NODE_SIZE - 5);
  LM_M(("nodeBuf.availableSpace: %d", nodeBuf.availableSpace()));
  EXPECT_FALSE(nodeBuf.isFull());

  // Fill buffer to make it overrun its initial size
  size_t space   = KV_NODE_SIZE - nodeBuf.size;
  nodeBuf.write((char*) "12345", space + 1);
  EXPECT_EQ(nodeBuf.availableSpace(), 0);
}

// -----------------------------------------------------------------------------
// KeyValueHash - 
//
TEST(samson_common_coding, KeyValueHash) {
  samson::KeyValueHash kvHash;

  EXPECT_EQ(kvHash.key_hash, 0);
  EXPECT_EQ(kvHash.value_hash, 0);
}
