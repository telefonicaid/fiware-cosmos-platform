/* ****************************************************************************
 *
 * FILE            gpb_operationsTest.cpp
 *
 * AUTHOR          Ken Zangelin
 *
 * DATE            Nov 2012
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
#include <set>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "samson/common/gpb_operations.h"
#include "samson/common/samson.pb.h"
#include "samson/stream/BlockManager.h"
#include "samson/worker/BlockRequest.h"

#include "unitTest/common_engine_test.h"


namespace samson {
namespace gpb {
    extern size_t range_overlap(int hg_begin1, int hg_end1, int hg_begin2, int hg_end2);
    extern bool string_starts_with(const std::string& s, const std::string& prefix);
    extern void AddBlockIds(const gpb::Queue&  queue,
                            const std::vector<samson::KVRange>&ranges, std::set<size_t>& block_ids);
  }
}

// -----------------------------------------------------------------------------
// getStreamOperation -
//
TEST(samson_common_gpb_operations, getStreamOperation) {
  samson::gpb::Data              data;
  samson::gpb::StreamOperation*  op  = data.add_operations();
  samson::gpb::StreamOperation*  op1 = samson::gpb::getStreamOperation(&data, "test");
  samson::gpb::StreamOperation*  op2 = samson::gpb::getStreamOperation(&data, 18);

  EXPECT_TRUE(op  != NULL);
  EXPECT_TRUE(op1 == NULL);
  EXPECT_TRUE(op2 == NULL);

  op->set_name("op1");
  op1 = samson::gpb::getStreamOperation(&data, "op1");
  EXPECT_TRUE(op1 != NULL);

  op  = data.add_operations();
  op->set_stream_operation_id(18);
  op->set_name("op2");
  op2 = samson::gpb::getStreamOperation(&data, 18);
  EXPECT_TRUE(op2 != NULL);

  samson::gpb::removeStreamOperation(&data, "op1");

  samson::gpb::Queue* queue = data.add_queue();
  queue->set_name("mQueue0");

  samson::gpb::QueueConnection* qc1 = data.add_queue_connections();
  samson::gpb::QueueConnection* qc2 = data.add_queue_connections();
  EXPECT_TRUE(qc1 != NULL);
  EXPECT_TRUE(qc2 != NULL);

  samson::gpb::BatchOperation* bo1 = data.add_batch_operations();
  samson::gpb::BatchOperation* bo2 = data.add_batch_operations();
  EXPECT_TRUE(bo1 != NULL);
  EXPECT_TRUE(bo2 != NULL);

  samson::gpb::reset_stream_operations(&data);
  EXPECT_EQ(0, data.mutable_operations()->size());
}

// -----------------------------------------------------------------------------
// DataInfoForRanges -
//
TEST(samson_common_gpb_operations, DataInfoForRanges) {
  samson::gpb::DataInfoForRanges dataInfo;

  EXPECT_EQ(dataInfo.data_size, 0);
}

// -----------------------------------------------------------------------------
// properties - test setProperty, getProperty, unsetProperty ...
//
TEST(samson_common_gpb_operations, setProperty) {
  samson::gpb::Environment env;

  // Just set a property
  samson::gpb::setProperty(&env, "prop1", "97");
  EXPECT_STREQ(samson::gpb::getProperty(&env, "prop1", "not 97").c_str(), "97");

  // Set the property again, to exercise the lookup part of setProperty
  samson::gpb::setProperty(&env, "prop1", "98");
  EXPECT_STREQ(samson::gpb::getProperty(&env, "prop1", "not 97").c_str(), "98");

  // Looking up a non-existing property
  EXPECT_STREQ(samson::gpb::getProperty(&env, "prop2", "not 97").c_str(), "not 97");

  // Unsetting a property
  samson::gpb::unsetProperty(&env, "prop1");
  EXPECT_STREQ(samson::gpb::getProperty(&env, "prop1", "not 97").c_str(), "not 97");

  // Set an integer property
  samson::gpb::setPropertyInt(&env, "prop3", 97);
  samson::gpb::setPropertyInt(&env, "prop4", 98);
  std::string envString = samson::gpb::str(env);
  EXPECT_STREQ(envString.c_str(), "prop3=97 prop4=98 ");
  EXPECT_EQ(samson::gpb::getPropertyInt(&env, "prop3", 98), 97);

  // Lookup a non-existing integer property
  EXPECT_EQ(samson::gpb::getPropertyInt(&env, "prop5", 98), 98);
}

// -----------------------------------------------------------------------------
// isWorkerIncluded -
//
TEST(samson_common_gpb_operations, isWorkerIncluded) {
  samson::gpb::ClusterInfo clusterInfo;

  EXPECT_TRUE(samson::gpb::isWorkerIncluded(&clusterInfo, 1) == false);

  samson::gpb::ClusterWorker* worker = clusterInfo.add_workers();
  worker->set_worker_id(1);

  EXPECT_TRUE(samson::gpb::isWorkerIncluded(&clusterInfo, 1) == true);
}

// -----------------------------------------------------------------------------
// GetNumKVRanges -
//
TEST(samson_common_gpb_operations, GetNumKVRanges) {
  samson::gpb::ClusterInfo clusterInfo;
  samson::gpb::ClusterWorker* worker = clusterInfo.add_workers();
  worker->set_worker_id(1);
  EXPECT_EQ(samson::gpb::GetNumKVRanges(&clusterInfo, 1), 0);

  samson::gpb::ProcessUnit* pu = clusterInfo.add_process_units();
  pu->set_hg_begin(1);
  pu->set_hg_end(8);
  pu->set_worker_id(1);

  EXPECT_EQ(samson::gpb::GetNumKVRanges(&clusterInfo, 1), 1);
}

// -----------------------------------------------------------------------------
// reset_data -
//
TEST(samson_common_gpb_operations, reset_data) {
  samson::gpb::Data data;

  samson::gpb::Queue* queue = data.add_queue();
  queue->set_name(".modules");

  samson::gpb::reset_data(&data);
  EXPECT_EQ(data.IsInitialized(), false);
}

// -----------------------------------------------------------------------------
// get_or_create_queue -
//
TEST(samson_common_gpb_operations, get_or_create_queue) {
  samson::gpb::Data data;
  au::ErrorManager  eManager;
  samson::KVFormat  kvFormat;

  // Queue created OK
  samson::gpb::Queue* queue = samson::gpb::get_or_create_queue(&data, "qname", kvFormat, eManager);
  EXPECT_TRUE(queue != NULL);

  // Queue returned OK - it already exists ...
  samson::gpb::Queue* queue2 = samson::gpb::get_or_create_queue(&data, "qname", kvFormat, eManager);
  EXPECT_TRUE(queue2 != NULL);
  EXPECT_TRUE(queue == queue2);

  // Queue not OK, as error manager has errors
  eManager.AddError("Not really an error");
  queue = samson::gpb::get_or_create_queue(&data, "qname", kvFormat, eManager);
  EXPECT_TRUE(queue == NULL);
}

// -----------------------------------------------------------------------------
// get_queue -
//
TEST(samson_common_gpb_operations, get_queue) {
  samson::gpb::Data data;
  au::ErrorManager  eManager;
  samson::KVFormat  kvFormat("kf",   "vf");
  samson::KVFormat  kvFormat2("kf2", "vf");
  samson::KVFormat  kvFormat3("kf",  "vf2");

  // Queue not found
  samson::gpb::Queue* queue = samson::gpb::get_queue(&data, "qname", kvFormat, eManager);
  EXPECT_TRUE(queue == NULL);

  // Queue created OK
  samson::gpb::Queue* queue2 =
    samson::gpb::get_or_create_queue(&data, "qname", kvFormat, eManager);
  EXPECT_TRUE(queue2 != NULL);
  queue = samson::gpb::get_queue(&data, "qname", kvFormat, eManager);
  EXPECT_TRUE(queue != NULL);

  // Queue not found as kv key formats differ
  queue = samson::gpb::get_queue(&data, "qname", kvFormat2, eManager);
  EXPECT_TRUE(queue == NULL);

  // Queue not found as kv value formats differ
  queue = samson::gpb::get_queue(&data, "qname", kvFormat3, eManager);
  EXPECT_TRUE(queue == NULL);
}

// -----------------------------------------------------------------------------
// remove_queue -
//
TEST(samson_common_gpb_operations, remove_queue) {
  samson::gpb::Data data;
  au::ErrorManager  eManager;
  samson::KVFormat  kvFormat("kf",   "vf");

  // Create a Queue
  samson::gpb::Queue* queue = samson::gpb::get_or_create_queue(&data, "qname", kvFormat, eManager);
  EXPECT_TRUE(queue != NULL);

  // Now remove it
  samson::gpb::removeQueue(&data, "qname");
  queue = samson::gpb::get_queue(&data, "qname", kvFormat, eManager);
  EXPECT_TRUE(queue == NULL);
}

// -----------------------------------------------------------------------------
// getQueueInfo -
//
TEST(samson_common_gpb_operations, getQueueInfo) {
  samson::gpb::Data  data;
  au::ErrorManager   eManager;
  samson::KVFormat   kvFormat("kf", "vf");

  // Create a Queue
  samson::gpb::Queue* queue = samson::gpb::get_or_create_queue(&data, "qname", kvFormat, eManager);
  EXPECT_TRUE(queue != NULL);

  // Get info
  size_t num_blocks;
  size_t kvs;
  size_t size;
  samson::gpb::getQueueInfo(*queue, &num_blocks, &kvs, &size);
  EXPECT_EQ(num_blocks, 0);
  EXPECT_EQ(kvs,        0);
  EXPECT_EQ(size,       0);

  // Add blocks to queue
  samson::gpb::Block* block = queue->add_blocks();
  EXPECT_TRUE(block != NULL);
  samson::gpb::getQueueInfo(*queue, &num_blocks, &kvs, &size);
  EXPECT_EQ(num_blocks, 1);
  EXPECT_EQ(kvs,        0);
  EXPECT_EQ(size,       0);
}

// -----------------------------------------------------------------------------
// getKVInfoForQueue -
//
TEST(samson_common_gpb_operations, getKVInfoForQueue) {
  samson::gpb::Data  data;
  au::ErrorManager   eManager;
  samson::KVFormat   kvFormat("kf", "vf");

  // Create a Queue
  samson::gpb::Queue* queue = samson::gpb::get_or_create_queue(&data, "qname", kvFormat, eManager);
  EXPECT_TRUE(queue != NULL);

  samson::FullKVInfo fullKvInfo    = samson::gpb::getKVInfoForQueue(*queue);
  std::string        fullKvInfoStr = fullKvInfo.str();
  EXPECT_STREQ("    0 kvs/    0 B", fullKvInfoStr.c_str());

  // Add a block to the queue
  samson::gpb::Block* block = queue->add_blocks();
  EXPECT_TRUE(block != NULL);

  block->set_block_size(23);
  fullKvInfo    = samson::gpb::getKVInfoForQueue(*queue);
  fullKvInfoStr = fullKvInfo.str();
  EXPECT_STREQ("    0 kvs/    0 B", fullKvInfoStr.c_str());
}

// -----------------------------------------------------------------------------
// getBlockKVInfoForQueue -
//
TEST(samson_common_gpb_operations, getBlockKVInfoForQueue) {
  samson::gpb::Data  data;
  au::ErrorManager   eManager;
  samson::KVFormat   kvFormat("kf", "vf");

  // Create a Queue
  samson::gpb::Queue* queue = samson::gpb::get_or_create_queue(&data, "qname", kvFormat, eManager);
  EXPECT_TRUE(queue != NULL);

  samson::KVRange      range(0, 256);
  samson::BlockKVInfo  blockKvInfo = samson::gpb::getBlockKVInfoForQueue(*queue, range);
  EXPECT_EQ(blockKvInfo.num_blocks, 0);

  samson::KVInfo info(2, 1);
  samson::gpb::add_block(&data, "qname", 2, 1, kvFormat, range, info, 1, eManager);
  blockKvInfo = samson::gpb::getBlockKVInfoForQueue(*queue, range);
  EXPECT_EQ(blockKvInfo.num_blocks, 1);
}

// -----------------------------------------------------------------------------
// add_block -
//
TEST(samson_common_gpb_operations, add_block) {
  samson::gpb::Data  data;
  au::ErrorManager   eManager;
  samson::KVFormat   kvFormat("kf", "vf");

  // Create a Queue
  samson::gpb::Queue* queue =
    samson::gpb::get_or_create_queue(&data, "newqueue", kvFormat, eManager);
  EXPECT_TRUE(queue != NULL);

  // Try to add a block with errors in error manager 
  samson::KVRange      range(0, 256);
  samson::BlockKVInfo  blockKvInfo = samson::gpb::getBlockKVInfoForQueue(*queue, range);
  EXPECT_EQ(blockKvInfo.num_blocks, 0);
  samson::KVInfo info(2, 1);
  eManager.AddError("not really en error");
  EXPECT_TRUE(eManager.HasErrors());
  samson::gpb::add_block(&data, "newqueue", 2, 1, kvFormat, range, info, 1, eManager);

  // Looking up queue 'newqueue'
  queue = get_or_create_queue(&data, "newqueue", kvFormat, eManager);
  EXPECT_TRUE(queue == NULL);
}

// -----------------------------------------------------------------------------
// rm_block -
//
TEST(samson_common_gpb_operations, rm_block) {
  samson::gpb::Data  data;
  au::ErrorManager   eManager;
  samson::KVFormat   kvFormat("kf", "vf");

  // Create a Queue
  samson::gpb::Queue* queue =
    samson::gpb::get_or_create_queue(&data, "newqueue", kvFormat, eManager);
  EXPECT_TRUE(queue != NULL);

  // Prepare for block add/remove functions
  samson::KVRange      range(0, 256);
  samson::BlockKVInfo  blockKvInfo = samson::gpb::getBlockKVInfoForQueue(*queue, range);
  EXPECT_EQ(blockKvInfo.num_blocks, 0);
  samson::KVInfo info(2, 1);

  // Try to remove an unexisting block
  samson::gpb::rm_block(&data, "newqueue", 2, kvFormat, range, info, 1, eManager);
  EXPECT_FALSE(eManager.HasErrors());

  // Add a block and remove it
  samson::gpb::add_block(&data, "newqueue", 2, 1, kvFormat, range, info, 1, eManager);
  EXPECT_FALSE(eManager.HasErrors());
  samson::gpb::rm_block(&data, "newqueue", 2, kvFormat, range, info, 1, eManager);
  EXPECT_FALSE(eManager.HasErrors());

  // Remove the block again to exercise error handling in rm_block
  samson::gpb::rm_block(&data, "noqueue", 2, kvFormat, range, info, 1, eManager);
  EXPECT_TRUE(eManager.HasErrors());
}

// -----------------------------------------------------------------------------
// range_overlap -
//
TEST(samson_common_gpb_operations, range_overlap) {
  int hg_begin1  = 0;
  int hg_end1    = 5;
  int hg_begin2  = 1;
  int hg_end2    = 6;
  int hg_begin3  = 6;
  int hg_end3    = 8;

  size_t result = samson::gpb::range_overlap(hg_begin1, hg_end1, hg_begin2, hg_end2);
  EXPECT_EQ(result, 4);
  result = samson::gpb::range_overlap(hg_begin1, hg_end1, hg_begin3, hg_end3);
  EXPECT_EQ(result, 0);
}

// -----------------------------------------------------------------------------
// data_queue_connections -
//
TEST(samson_common_gpb_operations, data_queue_connections) {
  samson::gpb::Data  data;
  au::ErrorManager   eManager;
  samson::KVFormat   kvFormat("kf", "vf");

  // Creating two queues
  samson::gpb::Queue* queue1 =
    samson::gpb::get_or_create_queue(&data, "queue1", kvFormat, eManager);
  EXPECT_TRUE(queue1 != NULL);
  samson::gpb::Queue* queue2 =
    samson::gpb::get_or_create_queue(&data, "queue2", kvFormat, eManager);
  EXPECT_TRUE(queue2 != NULL);

  // Connecting the queues
  samson::gpb::data_create_queue_connection(&data, "queue1", "queue2");
  EXPECT_TRUE(samson::gpb::data_exist_queue_connection(&data, "queue1", "queue2"));

  // Disconnecting the queues
  samson::gpb::data_remove_queue_connection(&data, "queue1", "queue2");
  EXPECT_FALSE(samson::gpb::data_exist_queue_connection(&data, "queue1", "queue2"));

  // Creating another queue and connecting queue 2 and 3 to queue 1
  samson::gpb::Queue* queue3 =
    samson::gpb::get_or_create_queue(&data, "queue3", kvFormat, eManager);
  EXPECT_TRUE(queue3 != NULL);
  samson::gpb::data_create_queue_connection(&data, "queue1", "queue2");
  samson::gpb::data_create_queue_connection(&data, "queue1", "queue3");
  au::StringVector    qVec = data_get_queues_connected(&data, "queue1");
  EXPECT_EQ(qVec.size(), 2);
}

// -----------------------------------------------------------------------------
// batch_operation_is_finished -
//
TEST(samson_common_gpb_operations, batch_operation_is_finished) {
  samson::gpb::Data            data;
  samson::gpb::BatchOperation  batchOp;

  batchOp.set_delilah_id(191);
  EXPECT_EQ(batchOp.delilah_id(), 191);

  batchOp.set_delilah_component_id(192);
  EXPECT_EQ(batchOp.delilah_component_id(), 192);

  batchOp.add_inputs();
  batchOp.add_inputs();
  EXPECT_EQ(batchOp.inputs_size(), 2);

  samson::gpb::KVInfo* input_info = batchOp.add_input_info();
  EXPECT_TRUE(input_info != NULL);

  batchOp.set_operation("op1");
  batchOp.set_inputs(1, "input1");
  batchOp.set_inputs(1, "input2");
  batchOp.set_inputs(1, "input3");
  bool is_finished = samson::gpb::batch_operation_is_finished(&data, batchOp);
  EXPECT_TRUE(is_finished);
}

// -----------------------------------------------------------------------------
// string_starts_with -
//
TEST(samson_common_gpb_operations, string_starts_with) {
  std::string prefix     = "post";
  std::string string     = "postfix";
  std::string string2    = "prefix";
  std::string longPrefix = "longPrefix";

  EXPECT_FALSE(samson::gpb::string_starts_with(string, longPrefix));
  EXPECT_FALSE(samson::gpb::string_starts_with(string2, prefix));
  EXPECT_TRUE(samson::gpb::string_starts_with(string, prefix));
}

// -----------------------------------------------------------------------------
// remove_finished_operation -
//
TEST(samson_common_gpb_operations, remove_finished_operation) {
  samson::gpb::Data            data;

  samson::gpb::BatchOperation* bo1 = data.add_batch_operations();
  EXPECT_TRUE(bo1 != NULL);

  samson::gpb::QueueConnection* qc1 = data.add_queue_connections();
  EXPECT_TRUE(qc1 != NULL);

  samson::gpb::Queue* queue = data.add_queue();
  queue->set_name(".00000000_0_");

  qc1 = data.add_queue_connections();
  EXPECT_TRUE(qc1 != NULL);

  samson::gpb::BatchOperation* batchOp = data.add_batch_operations();
  EXPECT_TRUE(batchOp != NULL);
  batchOp->set_delilah_id(191);
  EXPECT_EQ(batchOp->delilah_id(), 191);
  batchOp->set_delilah_component_id(192);
  EXPECT_EQ(batchOp->delilah_component_id(), 192);
  batchOp->add_inputs();

  samson::gpb::StreamOperation* so = data.add_operations();
  EXPECT_TRUE(so != NULL);
  so->set_name(".00000000_0_streamOperation01");
  so->add_inputs();

  samson::gpb::remove_finished_operation(&data, false);

  bo1 = data.add_batch_operations();
  EXPECT_TRUE(bo1 != NULL);
  qc1 = data.add_queue_connections();
  EXPECT_TRUE(qc1 != NULL);
  samson::gpb::remove_finished_operation(&data, true);
  EXPECT_EQ(0, data.operations_size());
}

// -----------------------------------------------------------------------------
// AddBlockIds1 -
//
TEST(samson_common_gpb_operations, AddBlockIds1) {
  samson::gpb::Queue   queue;
  samson::gpb::Block*  block = queue.add_blocks();
  EXPECT_TRUE(block != NULL);
  block->set_block_id(31);
  block->set_block_size(14);

  samson::gpb::KVRange* range = block->mutable_range();
  EXPECT_TRUE(range != NULL);
  range->set_hg_begin(0);
  range->set_hg_end(10);

  std::vector<samson::KVRange> rangeV;
  samson::KVRange              range1(0, 2);
  samson::KVRange              range2(0, 2);
  rangeV.push_back(range1);
  rangeV.push_back(range2);

  std::set<size_t> blockIdV;

  blockIdV.insert(1);
  blockIdV.insert(2);
  blockIdV.insert(3);

  samson::gpb::AddBlockIds(queue, rangeV, blockIdV);
  EXPECT_EQ(blockIdV.size(), 4);
}

// -----------------------------------------------------------------------------
// AddStateBlockIds -
//
TEST(samson_common_gpb_operations, AddStateBlockIds) {
  samson::gpb::Data              data;
  std::vector<samson::KVRange>   rangeV;
  std::set<size_t>               blockIdV;

  // Add  a stream operation
  samson::gpb::StreamOperation*  op  = data.add_operations();
  EXPECT_TRUE(op != NULL);
  op->set_stream_operation_id(1);
  op->set_name("op1");

  // Calling AddStateBlockIds with just one operation
  samson::gpb::AddStateBlockIds(&data, rangeV, blockIdV);
  EXPECT_EQ(blockIdV.size(), 0);

  // Add a Batch operation
  data.add_batch_operations();

  // Add another Stream operation, and get it a queue
  samson::gpb::Queue* queue1 = data.add_queue();
  EXPECT_TRUE(queue1 != NULL);
  queue1->set_name("op4");

  samson::gpb::Queue* queue2 = data.add_queue();
  EXPECT_TRUE(queue2 != NULL);
  queue2->set_name("q2");

  op  = data.add_operations();
  EXPECT_TRUE(op != NULL);
  op->set_stream_operation_id(4);
  op->set_name("op4");
  op->add_inputs("op4");
  op->add_inputs("op5");

  // Calling AddStateBlockIds with enough operations to get forward
  samson::gpb::AddStateBlockIds(&data, rangeV, blockIdV);
  EXPECT_EQ(blockIdV.size(), 0);
}

// -----------------------------------------------------------------------------
// AddBlockIds2 -
//
TEST(samson_common_gpb_operations, AddBlockIds2) {
  samson::gpb::Data              data;
  std::vector<samson::KVRange>   rangeV;
  std::set<size_t>               blockIdV;
  samson::gpb::Queue*            queue = data.add_queue();

  EXPECT_TRUE(queue != NULL);
  queue->set_name("q1");
  queue->add_blocks();
  samson::gpb::AddBlockIds(&data, rangeV, blockIdV);
  EXPECT_EQ(blockIdV.size(), 0);
}

// -----------------------------------------------------------------------------
// AddBlockIds3 -
//
TEST(samson_common_gpb_operations, AddBlockIds3) {
  samson::gpb::Data              data;
  std::set<size_t>               blockIdV;
  samson::gpb::Queue*            queue = data.add_queue();

  EXPECT_TRUE(queue != NULL);
  queue->set_name("q1");
  queue->add_blocks();
  samson::gpb::AddBlockIds(&data, blockIdV);
  EXPECT_EQ(blockIdV.size(), 1);
}

// -----------------------------------------------------------------------------
// GetFullKVInfo -
//
TEST(samson_common_gpb_operations, GetFullKVInfo) {
  samson::gpb::Data              data;
  samson::gpb::Queue*            queue = data.add_queue();

  EXPECT_TRUE(queue != NULL);
  queue->set_name("q1");
  queue->add_blocks();

  samson::FullKVInfo fullKvInfo = samson::gpb::GetFullKVInfo(&data);
  EXPECT_EQ(fullKvInfo.size, 0);
}

// -----------------------------------------------------------------------------
// get_data_info_for_ranges -
//
TEST(samson_common_gpb_operations, get_data_info_for_ranges) {
  samson::gpb::Data              data;
  std::vector<samson::KVRange>   rangeV;

  samson::gpb::DataInfoForRanges dataRangeInfo =
    samson::gpb::get_data_info_for_ranges(&data, "q1", rangeV);
  EXPECT_EQ(dataRangeInfo.data_size, 0);
}

// -----------------------------------------------------------------------------
// get_data_info_for_ranges2
//
TEST(samson_common_gpb_operations, get_data_info_for_ranges2) {
  samson::gpb::Data              data;
  std::vector<std::string>       queueV;
  std::vector<samson::KVRange>   rangeV;
  samson::gpb::Queue*            queue = data.add_queue();

  EXPECT_TRUE(queue != NULL);
  queue->set_name("q1");

  // Add blocks to queue 'q1'
  samson::gpb::Block* block = queue->add_blocks();
  EXPECT_TRUE(block != NULL);
  EXPECT_EQ(queue->blocks_size(), 1);

  block->set_block_size(300);
  samson::gpb::KVRange* mutable_range = block->mutable_range();
  mutable_range->set_hg_begin(1);
  mutable_range->set_hg_end(100);

  block = queue->add_blocks();
  EXPECT_TRUE(block != NULL);
  EXPECT_EQ(queue->blocks_size(), 2);
  block->set_block_size(700);
  mutable_range = block->mutable_range();
  mutable_range->set_hg_begin(5);
  mutable_range->set_hg_end(30);

  // Add queues
  queueV.push_back("FakeQueue");
  queueV.push_back("q1");

  // Add ranges
  samson::KVRange      range1(0,  10);
  samson::KVRange      range2(2, 30);
  samson::KVRange      range3(40, 50);
  rangeV.push_back(range1);
  rangeV.push_back(range2);
  rangeV.push_back(range3);

  // Call get_data_info_for_ranges
  samson::gpb::DataInfoForRanges info = get_data_info_for_ranges(&data, queueV, rangeV);
  EXPECT_EQ(info.data_size, 0);
}

// -----------------------------------------------------------------------------
// limit_last_commits
//
TEST(samson_common_gpb_operations, limit_last_commits) {
  samson::gpb::Data  data;

  // if (data->last_commits_size() < 20) ...
  samson::gpb::limit_last_commits(&data);

  // Add 30 commits
  for (int ix = 0; ix < 30; ix++)
     data.add_last_commits("commit X");
  EXPECT_EQ(data.last_commits_size(), 30);

  samson::gpb::limit_last_commits(&data);
  EXPECT_EQ(data.last_commits_size(), 20);
}

// -----------------------------------------------------------------------------
// Get
//
TEST(samson_common_gpb_operations, Get) {
  samson::gpb::CollectionRecord  cRec;
  std::string                    testString = "test";

  samson::add(&cRec, std::string("test"), testString, std::string("system.String"));
  std::string emptyString = samson::gpb::Get(cRec, "test1");
  std::string test = samson::gpb::Get(cRec, "test");

  EXPECT_STREQ("", emptyString.c_str());
  EXPECT_STREQ("test", test.c_str());
}

// -----------------------------------------------------------------------------
// Sort
//
TEST(samson_common_gpb_operations, Sort) {
  samson::gpb::Collection  collection;
  samson::gpb::CollectionRecord* record1 = collection.add_record();
  samson::gpb::CollectionRecord* record2 = collection.add_record();
  samson::gpb::CollectionRecord* record3 = collection.add_record();
  EXPECT_TRUE(record1 != NULL);
  EXPECT_TRUE(record2 != NULL);
  EXPECT_TRUE(record3 != NULL);
  EXPECT_EQ(collection.record_size(), 3);

  samson::gpb::CollectionItem*   item1   = record1->add_item();
  samson::gpb::CollectionItem*   item2   = record2->add_item();
  samson::gpb::CollectionItem*   item3   = record3->add_item();
  EXPECT_TRUE(item1 != NULL);
  EXPECT_TRUE(item2 != NULL);
  EXPECT_TRUE(item3 != NULL);

  item1->set_name("test");
  item1->set_value("testA");
  item2->set_name("test");
  item2->set_value("testC");
  item3->set_name("test");
  item3->set_value("testB");

  samson::gpb::Sort(&collection, "test");

  samson::gpb::CollectionRecord rec1 = collection.record(0);
  samson::gpb::CollectionRecord rec2 = collection.record(1);
  samson::gpb::CollectionRecord rec3 = collection.record(2);
  samson::gpb::CollectionItem i1   = rec1.item(0);
  samson::gpb::CollectionItem i2   = rec2.item(0);
  samson::gpb::CollectionItem i3   = rec3.item(0);

  EXPECT_STREQ(i1.value().c_str(), "testA");
  EXPECT_STREQ(i2.value().c_str(), "testB");
  EXPECT_STREQ(i3.value().c_str(), "testC");
}

// -----------------------------------------------------------------------------
// UpdateEnvironment -
//
TEST(samson_common_gpb_operations, UpdateEnvironment) {
  samson::gpb::Environment  environment;
  std::string               envVar = "samsonCommonLib=almostDone";
  au::ErrorManager          eManager;

  samson::gpb::UpdateEnvironment(&environment, envVar, eManager);
  EXPECT_FALSE(eManager.HasErrors());
}
