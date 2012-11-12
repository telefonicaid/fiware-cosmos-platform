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
#include "samson/stream/StreamOperationGlobalInfo.h"      // Own interface
#include <algorithm>

#include "engine/Engine.h"
#include "engine/ProcessManager.h"
#include "samson/common/KVRangeDivision.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/gpb_operations.h"
#include "samson/module/ModulesManager.h"
#include "samson/stream/StreamOperationRangeInfo.h"
#include "samson/stream/WorkerTask.h"
#include "samson/worker/SamsonWorker.h"

namespace samson {
namespace stream {
StreamOperationGlobalInfo::StreamOperationGlobalInfo(SamsonWorker *samson_worker
                                                     , size_t stream_operation_id
                                                     , const std::string& stream_operation_name
                                                     , const std::vector<KVRange>& ranges) {
  samson_worker_ = samson_worker;
  stream_operation_id_ = stream_operation_id;
  stream_operation_name_ = stream_operation_name;

  // Create a StreamOperationInfoRange for each range
  for (size_t i = 0; i < ranges.size(); ++i) {
    // Recover ( or create ) information for this stream operation
    StreamOperationRangeInfo *stream_operation_info = new StreamOperationRangeInfo(this
                                                                                   , samson_worker
                                                                                   , stream_operation_id
                                                                                   , stream_operation_name
                                                                                   , ranges[i]);

    stream_operations_range_info_.push_back(stream_operation_info);
  }
}

void StreamOperationGlobalInfo::ReviewIntern(gpb::Data *data) {
  // Review all data to be processed ( just debug information )
  gpb::StreamOperation *stream_operation = gpb::getStreamOperation(data, stream_operation_id_);

  bool batch_operation = stream_operation->batch_operation();
  bool reduce_forward = stream_operation->reduce_forward();

  state_ = "";
  state_input_queues_ = "";

  // All ranges currently in use
  std::vector<KVRange> all_ranges = GetActiveRanges();

  // Scan all inputs queues
  for (int i = 0; i < stream_operation->inputs_size(); ++i) {
    std::string input_queue = stream_operation->inputs(i);
    gpb::DataInfoForRanges info = gpb::get_data_info_for_ranges(data, input_queue, all_ranges);

    state_input_queues_ += au::str("[%s %s %s]", input_queue.c_str(), au::str(info.data_kvs_in_ranges, "kvs").c_str(),
                                   au::str(info.data_size_in_ranges, "B").c_str());
  }

  if (batch_operation) {
    state_ += "[Batch]";
  }
  if (reduce_forward) {
    state_ += "[ReduceForward]";
  }
}

void StreamOperationGlobalInfo::Review(gpb::Data *data) {
  // Review all data to be processed ( just debug information )
  ReviewIntern(data);

  // Review all ranges
  for (size_t i = 0; i < stream_operations_range_info_.size(); i++) {
    stream_operations_range_info_[i]->Review(data);
  }
}

std::string StreamOperationGlobalInfo::str() {
  std::ostringstream output;

  output << "StreamOperation " << stream_operation_id_;
  output << " " << stream_operation_name_;
  return output.str();
}

void StreamOperationGlobalInfo::fill(samson::gpb::CollectionRecord *record, const Visualization& visualization) {
  ::samson::add(record, "id", stream_operation_id_, "different");
  ::samson::add(record, "name", stream_operation_name_, "different");
  ::samson::add(record, "KVRanges", stream_operations_range_info_.size(), "different");

  std::ostringstream output;
  output << "(" << stream_operations_range_info_.size() << " ranges)";
  for (size_t i = 0; i < stream_operations_range_info_.size(); ++i) {
    output << stream_operations_range_info_[i]->short_state();
  }

  if (visualization.get_flag("state")) {
    ::samson::add(record, "state", state_, "different");
    return;
  }

  ::samson::add(record, "States for each range", output.str(), "different");

  ::samson::add(record, "inputs", state_input_queues_, "different");
  ::samson::add(record, "state", state_, "different");
}

std::vector<KVRange> StreamOperationGlobalInfo::GetActiveRanges() {
  std::vector<KVRange> all_ranges;
  for (size_t i = 0; i < stream_operations_range_info_.size(); ++i) {
    all_ranges.push_back(stream_operations_range_info_[i]->range());
  }
  return all_ranges;
}
}
}  // End of namespace samson::stream
