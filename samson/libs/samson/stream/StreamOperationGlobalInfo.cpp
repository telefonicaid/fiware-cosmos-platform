#include "samson/stream/StreamOperationGlobalInfo.h"      // Own interface
#include <algorithm>

#include "engine/Engine.h"
#include "engine/ProcessManager.h"
#include "samson/common/gpb_operations.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"
#include "samson/module/ModulesManager.h"
#include "samson/stream/StreamOperationRangeInfo.h"
#include "samson/stream/WorkerTask.h"
#include "samson/worker/SamsonWorker.h"

namespace samson {
namespace stream {

StreamOperationGlobalInfo::StreamOperationGlobalInfo(SamsonWorker *samson_worker, size_t stream_operation_id,
                                                     const std::string& stream_operation_name,
                                                     const std::vector<KVRange>& ranges) {
  samson_worker_ = samson_worker;
  stream_operation_id_ = stream_operation_id;
  stream_operation_name_ = stream_operation_name;

  // Create a StreamOperationInfoRange for each range
  for (size_t i = 0; i < ranges.size(); i++) {
    // Recover ( or create ) information for this stream operation
    StreamOperationRangeInfo * stream_operation_info = new StreamOperationRangeInfo(this, samson_worker,
                                                                                    stream_operation_id,
                                                                                    stream_operation_name, ranges[i]);

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

  // Information about input queues...
  // Scan all inputs
  for (int i = 0; i < stream_operation->inputs_size(); i++) {
    std::string input_queue = stream_operation->inputs(i);
    gpb::DataInfoForRanges info = gpb::get_data_info_for_ranges(data, input_queue, all_ranges);

    state_input_queues_ += au::str("[%s %s %s]", input_queue.c_str(), au::str(info.data_kvs_in_ranges, "kvs").c_str(),
                                   au::str(info.data_size_in_ranges, "B").c_str());

  }

  if (batch_operation)
    state_ += "[Batch]";
  if (reduce_forward)
    state_ += "[ReduceForward]";

}

void StreamOperationGlobalInfo::Review(gpb::Data *data) {

  // Review all data to be processed ( just debug information )
  ReviewIntern(data);

  au::vector<StreamOperationRangeInfo> new_ranges;
  while (stream_operations_range_info_.size() > 0) {
    // Extract the last element
    StreamOperationRangeInfo* info = stream_operations_range_info_.extract_back();
    info->Review(data);

    // If division is necessary, break in two and push back to the original vector
    // Otherwise push to the new vector

    if (info->range_division_necessary()) {
      KVRange range = info->range();
      int range_size = range.size();
      if (range_size == 1) {
        LM_X(1, ("Internal error, not possible to divide more this range"));
      }

      KVRange range1(range.hg_begin_, range.hg_begin_ + range_size / 2);
      KVRange range2(range.hg_begin_ + range_size / 2, range.hg_end_);

      StreamOperationRangeInfo *info1, *info2;
      info1 = new StreamOperationRangeInfo(this, samson_worker_, stream_operation_id_, stream_operation_name_, range1);
      info2 = new StreamOperationRangeInfo(this, samson_worker_, stream_operation_id_, stream_operation_name_, range2);

      delete info; // Remove original info structure

      stream_operations_range_info_.push_back(info1);
      stream_operations_range_info_.push_back(info2);
    } else {
      new_ranges.push_back(info);
    }

  }

  // Push the new elements
  for (size_t i = 0; i < new_ranges.size(); i++)
    stream_operations_range_info_.push_back(new_ranges[i]);

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
  for (size_t i = 0; i < stream_operations_range_info_.size(); i++) {
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
  for (size_t i = 0; i < stream_operations_range_info_.size(); ++i)
    all_ranges.push_back(stream_operations_range_info_[i]->range());
  return all_ranges;
}

std::vector<KVRange> StreamOperationGlobalInfo::GetDefragKVRanges() {

  KVRangeDivision kv_range_division;

  std::vector<KVRange> ranges = samson_worker_->worker_controller()->GetKVRanges();
  std::vector<KVRange> local_ranges = GetActiveRanges();

  kv_range_division.Force(ranges);
  kv_range_division.Force(local_ranges);

  return kv_range_division.ranges();
}

}
} // End of namespace samson::stream
