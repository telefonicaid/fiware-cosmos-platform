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
#include "samson/stream/StreamOperationRangeInfo.h"      // Own interface
#include <string>

#include "engine/Engine.h"
#include "engine/ProcessManager.h"
#include "samson/common/Logs.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/gpb_operations.h"
#include "samson/module/ModulesManager.h"
#include "samson/stream/StreamOperationGlobalInfo.h"
#include "samson/stream/WorkerTask.h"
#include "samson/worker/SamsonWorker.h"

namespace samson {
namespace stream {
class InputData {
public:

  InputData() :
    num_blocks_(0) {
  }

  std::string str() const {
    // Build string to inform about this
    std::ostringstream output;

    output << "[ " << au::str(num_blocks_, "Bl") << " ";
    output << total_info_.str() << " ";
    output << "R: " << au::str_simple_percentage(ready_info_.size, total_info_.size) << " ";
    output << "P: " << au::str_simple_percentage(processing_info_.size, total_info_.size) << " ";
    output << " ]";
    return output.str();
  }

  void AppendToTotal(double factor, const KVInfo& info) {
    num_blocks_++;
    total_info_.Append(factor, info);
  }

  void AppendToReady(double factor, const KVInfo& info) {
    ready_info_.Append(factor, info);
  }

  void AppendToProcessing(double factor, const KVInfo& info) {
    processing_info_.Append(factor, info);
  }

  FullKVInfo total_info() const {
    return total_info_;
  };

  FullKVInfo ready_info() const {
    return ready_info_;
  }

  bool IsFullyReady() const {
    return (total_info_.size == ready_info_.size);
  }

private:

  int num_blocks_;
  FullKVInfo total_info_;
  FullKVInfo ready_info_;
  FullKVInfo processing_info_;
};

class InputsData {
public:

  explicit InputsData(int num_inputs) {
    for (int i = 0; i < num_inputs; ++i) {
      inputs_data_.push_back(InputData());
    }
  }

  void AppendToTotal(int input,  double factor, const KVInfo& info) {
    inputs_data_[input].AppendToTotal(factor, info);
  }

  void AppendToReady(int input, double factor, const KVInfo& info) {
    inputs_data_[input].AppendToReady(factor, info);
  }

  void AppendToProcessing(int input, double factor, const KVInfo& info) {
    inputs_data_[input].AppendToProcessing(factor, info);
  }

  std::string str() const {
    std::ostringstream output;

    for (size_t i = 0; i < inputs_data_.size(); i++) {
      output << inputs_data_[i].str();
    }
    return output.str();
  }

  size_t GetReadySize() const {
    size_t total = 0;

    for (size_t i = 0; i < inputs_data_.size(); ++i) {
      total +=  inputs_data_[i].ready_info().size;
    }
    return total;
  }

  size_t GetReadySizeForAllInputsExceptLastOne() const {
    size_t total = 0;

    for (size_t i = 0; i < inputs_data_.size() - 1; ++i) {
      total +=  inputs_data_[i].ready_info().size;
    }
    return total;
  }

  size_t GetTotalSize() const {
    size_t total = 0;

    for (size_t i = 0; i < inputs_data_.size(); ++i) {
      total +=  inputs_data_[i].total_info().size;
    }
    return total;
  }

  size_t GetLastInputTotalSize() const {
    return inputs_data_[inputs_data_.size() - 1].total_info().size;
  }

  bool AreAllInputFullyReady() {
    for (size_t i = 0; i < inputs_data_.size(); ++i) {
      if (!inputs_data_[i].IsFullyReady()) {
        return false;
      }
    }
    return true;
  }

  bool IsLastInputFullyReady() const {
    return inputs_data_[inputs_data_.size() - 1].IsFullyReady();
  }

private:

  std::vector<InputData> inputs_data_;
};

StreamOperationRangeInfo::StreamOperationRangeInfo(StreamOperationGlobalInfo *stream_operation_global_info,
                                                   SamsonWorker *samson_worker, size_t stream_operation_id,
                                                   const std::string& stream_operation_name, const KVRange& range)
  : stream_operation_id_(stream_operation_id)
    , stream_operation_name_(stream_operation_name)
    , range_(range)
    , state_input_queues_("No info")
    , state_("No info")
    , short_state_("No info")
    , priority_rank_(0)
    , last_task_cronometer_()
    , error_()
    , cronometer_error_()
    , worker_task_(NULL)  // No task by default
    , samson_worker_(samson_worker)  // Keep a pointer to samson worker
    , stream_operation_global_info_(stream_operation_global_info) {  // Keep a reference to my parent global information
}

/*
 * Dynamic inputs
 * ----------------------------
 * map,parse,...     All inputs are dynamic (1 dynamic input)
 * Batch operation:  All inputs are non dynamic ( all input required )
 * Stream operation: All inputs are dynamic except last ( state ) input
 * Reduce forward:   All inputs are dynamic except last ( auxiliary data-set ) input
 *
 * Inputs to count for pending size
 * ----------------------------
 * map,parse,...     Single input ( counts )
 * Batch operation:  All inputs
 * Stream operation: N-1 ( last one is state and it is not considered pending.size)
 */

// Get number of inputs that can be taken in small blocks
int GetNumberOfDynamicInputs(gpb::StreamOperation *stream_operation) {
  std::string operation_name = stream_operation->operation();
  Operation *operation = au::Singleton<ModulesManager>::shared()->GetOperation(operation_name);

  if (!operation) {
    LM_X(1, ("Internal error"));
  }

  bool batch_operation = stream_operation->batch_operation();
  bool reduce_operation = (operation->getType() == Operation::reduce);

  if (reduce_operation && batch_operation) {
    return 0;
  }

  if (reduce_operation) {
    return operation->getNumInputs() - 1;
  }

  // No reduce... it is always 1
  return 1;
}

// Get the number of inputs to take into account for triggering tasks
int GetNumberOfInputsForThrigering(gpb::StreamOperation *stream_operation) {
  std::string operation_name = stream_operation->operation();
  Operation *operation = au::Singleton<ModulesManager>::shared()->GetOperation(operation_name);

  if (!operation) {
    LM_X(1, ("Internal error"));
  }

  bool batch_operation = stream_operation->batch_operation();
  bool reduce_operation = (operation->getType() == Operation::reduce);

  if (reduce_operation && batch_operation) {
    return operation->getNumInputs();
  }

  if (reduce_operation) {
    return operation->getNumInputs() - 1;
  }

  // No reduce... it is always 1
  return 1;
}

void StreamOperationRangeInfo::Review(gpb::Data *data) {
  // Reset strings of states
  state_input_queues_ = "";
  state_ = "";
  short_state_ = "";

  // Init pending size and priority to be recomputed
  pending_size_ = 0;
  priority_rank_ = 0;

  // Recover stream operation from data
  gpb::StreamOperation *stream_operation = gpb::getStreamOperation(data, stream_operation_id_);
  if (!stream_operation) {
    SetError(au::str("stream operation %s does not exist"));
    worker_task_ = NULL;       // Cancel task if any
    return;
  }

  // Check if the operation is valid
  au::ErrorManager error;
  if (!IsStreamOperationValid(data, *stream_operation, &error)) {
    SetError(au::str("Error validating stream operation: %s", error.GetMessage().c_str()));
    worker_task_ = NULL;       // Cancel task if any
    return;
  }

  // Get operation to be executed
  std::string operation_name = stream_operation->operation();
  Operation *operation = au::Singleton<ModulesManager>::shared()->GetOperation(operation_name);
  if (!operation) {
    SetError(au::str("Operation %s not found", operation_name.c_str()));
    return;
  }

  // Flag to detect reduce and batch operations
  bool reduce_operation = (operation->getType() == Operation::reduce);
  bool batch_operation  = stream_operation->batch_operation();

  // Get all information about data at input queues
  InputsData inputs_data(stream_operation->inputs_size());

  for (int i = 0; i < stream_operation->inputs_size(); ++i) {
    std::string input_queue = stream_operation->inputs(i);     // name of the i-th input queue
    gpb::Queue *queue = gpb::get_queue(data, input_queue);

    if (!queue) {
      continue;
    }

    // Take all the blocks intersecting block "range_"
    bool ready = true;     // Flag to indicate that data is ready so far
    for (int b = 0; b < queue->blocks_size(); ++b) {
      const gpb::Block& block = queue->blocks(b);
      KVRange range = block.range();     // Implicit conversion
      double overlap_factor = range.GetOverlapFactor(range_);
      if (overlap_factor > 0) {
        KVInfo block_info(block.size(), block.kvs());

        inputs_data.AppendToTotal(i, overlap_factor, block_info);

        // If this block has to be defrag by others, ignore
        if (range.hg_begin_ < range_.hg_begin_) {
          continue;
        }

        if (range.hg_end_ > range_.hg_end_) {
          // If I have to defrag, consider not ready...
          ready = false;
        }

        if (stream::BlockManager::shared()->GetBlock(block.block_id()) == NULL) {
          // Block not local ( data has not been requested? )
          ready = false;
          samson_worker_->worker_block_manager()->RequestBlock(block.block_id());
        }

        if (ready) {
          inputs_data.AppendToReady(i, overlap_factor, block_info);
        }

        // See if we are processing this data
        if (worker_task_ != NULL) {
          if (worker_task_->ContainsBlockAtInput(i, block.block_id())) {
            inputs_data.AppendToProcessing(i, overlap_factor, block_info);
          }
        }
      }
    }
  }

  // Inform about content in in input queues
  state_input_queues_ +=  inputs_data.str();

  // Reset error in timeout 60
  if ((error_.IsActivated() && (cronometer_error_.seconds() > 10))) {
    error_.Reset();
  }
  if (error_.IsActivated()) {
    // If we had an error, wait some time...
    state_ = au::str("Error [%s]: %s", cronometer_error_.str().c_str(), error_.GetMessage().c_str());
    short_state_ = "[E]";
    return;
  }

  // If operation is paused, do not consider then...
  if (stream_operation->paused()) {
    state_ = "Operation paused";
    short_state_ = "[P]";
    return;
  }

  // If state is too large, do not process it
  if ((worker_task_ == NULL)) {
    size_t total_input_size = inputs_data.GetTotalSize();
    size_t total_input_state_size = inputs_data.GetLastInputTotalSize();

    // Memory size limits in this platform
    size_t memory = engine::Engine::shared()->memory_manager()->memory();
    size_t max_memory = 0.5 * memory;

    // In batch reduce operations, range will be divided if all data (all inputs) for this range is larger than max memory size per task
    if (reduce_operation && batch_operation && (total_input_size > max_memory)) {
      SetError(au::str("Not possible to process this range (%s)", au::str(total_input_size).c_str()));
      return;
    }

    if (reduce_operation && !batch_operation && total_input_state_size > max_memory) {
      SetError(au::str("Not possible to process this range (%s)", au::str(total_input_state_size).c_str()));
      return;
    }
  }

  // ---------------------------------------------------------------------
  // Schedule a defrag operation if required and we are not running one
  // ---------------------------------------------------------------------

  for (int i = 0; i < stream_operation->inputs_size(); ++i) {
    std::string input_queue = stream_operation->inputs(i);
    gpb::Queue *queue = gpb::get_queue(data, input_queue);

    if (!queue) {
      continue;     // No queue, no problem...
    }

    for (int b = 0; b < queue->blocks_size(); ++b) {
      const gpb::Block& block = queue->blocks(b);
      KVRange range = block.range();

      if (!range.IsOverlapped(range_)) {
        continue;       // No problem with this...
      }

      if (range_.Includes(range)) {
        continue;     // This block has no problem since it fits inside my range
      }
      size_t block_id = block.block_id();
      BlockPointer real_block = BlockManager::shared()->GetBlock(block_id);
      if (real_block == NULL) {
        continue;     // We cannot defrag since it still not in this worker
      }
      // Add defrag operation
      std::vector<KVRange> defrag_ranges = samson_worker_->worker_controller()->GetKVRanges();
      samson_worker_->worker_block_manager()->AddBlockBreak(input_queue, block_id, defrag_ranges);
    }
  }

  // Check if we can really execute this range
  if (reduce_operation && batch_operation && !inputs_data.AreAllInputFullyReady()) {
    state_ = "Waiting for all data to be ready";
    short_state_ = "[WR]";
    return;
  }

  if (reduce_operation && !batch_operation && !inputs_data.IsLastInputFullyReady()) {
    state_ = "Waiting for state to be ready";
    short_state_ = "[WS]";
    return;
  }


  if (worker_task_ != NULL) {
    state_ = "Running task" + worker_task_->str();
    ReviewCurrentTask();       // Review if this task finished...
    short_state_ = "[*]";
    return;
  }

  // Compute pending to be process size
  if (reduce_operation && !batch_operation) {
    pending_size_ = inputs_data.GetReadySizeForAllInputsExceptLastOne();
  } else {
    pending_size_ = inputs_data.GetReadySize();
  }

  // Eval pending size to schedule new tasks
  if (pending_size_ > 0) {
    // Compute priority based on pending size and time
    size_t time = 1 + last_task_cronometer_.seconds();
    priority_rank_ = pending_size_ * time;
    state_ = "Ready to schedule a new task";
    short_state_ = "[R]";
  } else {
    state_ = "No data to be processed";
    short_state_ = "[N]";
    last_task_cronometer_.Reset();
    // Reset the chronometer if not data since otherwise it will count a lot of time when some data appears
  }
}

void StreamOperationRangeInfo::ReviewCurrentTask() {
  if ((worker_task_ != NULL) && (worker_task_->IsWorkerTaskFinished())) {
    if (worker_task_->error().IsActivated()) {
      SetError(worker_task_->error().GetMessage());
    }
    worker_task_ = NULL;
  }
}

au::SharedPointer<WorkerTask> StreamOperationRangeInfo::schedule_new_task(size_t task_id, gpb::Data *data) {
  // Everything is checked by Rreview command, so no error is accepted
  if (priority_rank_ == 0) {
    SetError("Scheduling a task with priority_rank=0");
    return au::SharedPointer<WorkerTask>(NULL);
  }

  // Get information about the stream operation
  gpb::StreamOperation *stream_operation = gpb::getStreamOperation(data, stream_operation_id_);
  if (!stream_operation) {
    SetError(au::str("Error recovering stream operation %s", stream_operation_name_.c_str()));
    return au::SharedPointer<WorkerTask>(NULL);
  }
  if (!IsStreamOperationValid(data, *stream_operation, &error_)) {
    return au::SharedPointer<WorkerTask>(NULL);
  }

  size_t memory = engine::Engine::shared()->memory_manager()->memory();
  size_t max_memory_per_task = memory / 2;
  std::string operation_name = stream_operation->operation();  // Get the operation to be executed
  Operation *operation = au::Singleton<ModulesManager>::shared()->GetOperation(operation_name);

  if (error_.IsActivated()) {
    SetError("Error scheduling a task with a previous error");
    return au::SharedPointer<WorkerTask>(NULL);
  }

  if (stream_operation->paused()) {
    SetError("Scheduling a task in a paused stream operation");
    return au::SharedPointer<WorkerTask>(NULL);
  }
  if (!operation) {
    SetError(au::str("Error finding operation  %s", operation_name.c_str()));
    return au::SharedPointer<WorkerTask>(NULL);
  }

  // Get number of input-channels with dynamic input ( we can take more or less data to be processed )
  int num_dynamic_input = GetNumberOfDynamicInputs(stream_operation);

  // Create candidate task
  worker_task_.Reset(new WorkerTask(samson_worker_, task_id, *stream_operation, operation, range_));

  // Accumulated memory used for this task ( we will limit taken blocks with this element )
  size_t accumulated_size = 0;     // Accumulated size required to be in memory for this operation
  au::Uint64Set block_ids;         // Set of Block ids required so far

  // Scan all inputs ( in reverse order to include first state.... )
  for (int i = stream_operation->inputs_size() - 1; i >= 0; --i) {
    std::string input_queue = stream_operation->inputs(i);
    gpb::Queue *queue = ::samson::gpb::get_queue(data, input_queue);

    if (!queue) {
      continue;     // No data at this input
    }

    for (int b = 0; b < queue->blocks_size(); ++b) {
      const gpb::Block& block = queue->blocks(b);
      size_t block_id = block.block_id();
      KVRange range = block.range();
      KVInfo info(block.size(), block.kvs());
      // If this block is not in our range of interest, just skip it
      if (!range.IsOverlapped(range_)) {
        continue;
      }

      if (!range_.Includes(range)) {
        if (i < num_dynamic_input) {
          break;     // In dynamic data,
        }
        LM_X(1, ("Internal error. Block %lu (%s) in queue %s shoudl be contained in range %s"
                 , block_id
                 , range.str().c_str()
                 , input_queue.c_str()
                 , range_.str().c_str()));
      }

      BlockPointer real_block = BlockManager::shared()->GetBlock(block.block_id());
      if (real_block == NULL) {
        break;     // We cannot include more blocks since they are not in memory
      }

      // Accumulate size if the block was not considered before
      if (!block_ids.contains(block_id)) {
        accumulated_size += real_block->getSize();     // Accumulate size of the block
        block_ids.insert(block_id);
      }
      // Add input to the task
      worker_task_->AddInput(i, real_block, range_, info);

      // Stop if we need too much memory for this task
      if (i < num_dynamic_input) {
        if (accumulated_size > max_memory_per_task) {
          break;     // No more data from this input
        }
      }
    }
  }

  if (accumulated_size > 2 * max_memory_per_task) {
    worker_task_ = NULL;
    SetError(au::str("Error: Memory footprint of %s for previous operation.", au::str(accumulated_size).c_str()));
    return au::SharedPointer<WorkerTask>(NULL);
  }

  // Reset chronometer
  last_task_cronometer_.Reset();

  // Add environment variable to identify this stream_operation_id
  worker_task_->environment().Set("system.stream_operation_id", stream_operation_id_);

  return worker_task_;
}

std::string StreamOperationRangeInfo::str() const {
  std::ostringstream output;

  output << "StreamOperation " << stream_operation_id_;
  output << " " << stream_operation_name_;
  output << " " << range_.str();
  return output.str();
}

void StreamOperationRangeInfo::fill(samson::gpb::CollectionRecord *record, const Visualization& visualization) const {
  // Common columns
  ::samson::add(record, "id", stream_operation_id_, "different");
  ::samson::add(record, "name", stream_operation_name_, "different");
  ::samson::add(record, "range", range_.str().c_str(), "different");

  if (visualization.get_flag("state")) {
    ::samson::add(record, "state", state_, "different");
    return;
  }

  if (visualization.get_flag("tasks")) {
    if (worker_task_ != NULL) {
      ::samson::add(record, "tasks", worker_task_->str_short(), "different");
    } else {
      ::samson::add(record, "tasks", "none", "different");
    }
    return;
  }

  // Default view
  ::samson::add(record, "inputs", state_input_queues_, "different");
  ::samson::add(record, "time", au::str_time(last_task_cronometer_.seconds()), "different");
  ::samson::add(record, "priority", priority_rank(), "f=uint64,different");

  if (worker_task_ != NULL) {
    ::samson::add(record, "tasks", worker_task_->id(), "different");
  } else {
    ::samson::add(record, "tasks", "none", "different");
  }

  ::samson::add(record, "state", state_, "different,left");
}

bool StreamOperationRangeInfo::IsStreamOperationValid(gpb::Data *data
                                                      , const gpb::StreamOperation& stream_operation
                                                      , au::ErrorManager *error) {
  std::string operation_name = stream_operation.operation();
  Operation *operation = au::Singleton<ModulesManager>::shared()->GetOperation(operation_name);

  if (!operation) {
    error->set(au::str("Operation %s not found", operation_name.c_str()));
    return false;
  }

  // Check at least an input is defined
  if (stream_operation.inputs_size() == 0) {
    error->set("No inputs defined for this operation");
    return false;
  }

  // Check number of inputs and outputs
  if (operation->getNumInputs() != stream_operation.inputs_size()) {
    error->set(
      au::str("Operation %s needs %d inputs and %d are provided", operation_name.c_str(),
              operation->getNumInputs(), stream_operation.inputs_size()));
    return false;
  }
  if (operation->getNumOutputs() != stream_operation.outputs_size()) {
    error->set(
      au::str("Operation %s needs %d output and %d are provided", operation_name.c_str(),
              operation->getNumOutputs(), stream_operation.outputs_size()));
    return false;
  }

  // Check the format of all input queues
  for (int i = 0; i < operation->getNumInputs(); ++i) {
    std::string queue_name = stream_operation.inputs(i);
    gpb::Queue *queue = get_queue(data, queue_name);
    if (queue) {     // If queue does not exist, there is no problem. I will be automatically created
      KVFormat format(queue->key_format(), queue->value_format());
      if (format != operation->inputFormats[i]) {
        error->set(
          au::str("%d-th input for %s ( queue %s ) should be %s and it is %s ", i + 1, operation_name.c_str(),
                  queue_name.c_str(), operation->inputFormats[i].str().c_str(), format.str().c_str()));
        return false;
      }
    }
  }

  // Check the format of all output queues
  for (int i = 0; i < operation->getNumOutputs(); ++i) {
    std::string queue_name = stream_operation.outputs(i);
    gpb::Queue *queue = get_queue(data, queue_name);
    if (queue) {     // If queue does not exist, there is no problem. I will be automatically created
      KVFormat format(queue->key_format(), queue->value_format());
      if (format != operation->outputFormats[i]) {
        error->set(
          au::str("%d-th output for %s ( queue %s ) should be %s and it is %s ", i + 1,
                  operation_name.c_str(), queue_name.c_str(), operation->outputFormats[i].str().c_str(),
                  format.str().c_str()));
        return false;
      }
    }
  }

  // Additional checks for reduce operations....
  // TO BE COMPLETED

  return true;
}
}
}  // End of namespace samson::stream
