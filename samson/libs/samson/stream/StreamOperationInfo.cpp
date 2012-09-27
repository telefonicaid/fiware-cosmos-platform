

#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/gpb_operations.h"
#include "samson/module/ModulesManager.h"
#include "samson/worker/SamsonWorker.h"

#include "WorkerTask.h"

#include "samson/stream/StreamOperationInfo.h"      // Own interface

namespace samson {
namespace stream {
StreamOperationInfo::StreamOperationInfo(SamsonWorker *samson_worker
                                         , size_t stream_operation_id
                                         , const KVRange& range
                                         , const gpb::StreamOperation& stream_operation) {
  // Keep identifiers & operation range
  stream_operation_id_ = stream_operation_id;
  range_ = range;

  // Keep a pointer to samson worker
  samson_worker_ = samson_worker;

  // Duplicate stream operation information
  stream_operation_ = new gpb::StreamOperation();
  stream_operation_->CopyFrom(stream_operation);

  // Default values
  priority_rank_ = 0;
  state_ = "No info";

  worker_task_ = NULL;                         // No task by default
}

StreamOperationInfo::~StreamOperationInfo() {
}

void StreamOperationInfo::review() {
  // Init pending size and priority to be recomputed
  pending_size_ = 0;
  priority_rank_ = 0;

  if (worker_task_ != NULL) {
    // If we are running a task, let see if it is finished
    if (worker_task_->is_finished()) {
      const au::ErrorManager& worker_task_error = worker_task_->error();          // If there is an error, reset
      if (worker_task_error.IsActivated()) {
        ResetWithError(worker_task_error.GetMessage());
        return;
      } else {
        // Commit changes and release task
        std::string commit_command = worker_task_->commit_command();
        std::string caller = au::str("task %lu // %s"
                                     , worker_task_->get_id()
                                     , str().c_str());
        au::ErrorManager error;
        samson_worker_->data_model()->Commit(caller, commit_command, &error);
        if (error.IsActivated()) {
          ResetWithError(au::str("Error commiting tasks: %s", error.GetMessage().c_str()));
        }

        // Release our copy of this task
        worker_task_ = NULL;
      }
    }
  }
}

size_t StreamOperationInfo::priority_rank() {
  return priority_rank_;
}

void StreamOperationInfo::RecomputePriorityRank(gpb::Data *data) {
  // Init pending size and priority to be recomputed
  pending_size_ = 0;
  priority_rank_ = 0;

  // If operation is paused, do not consider then...
  if (stream_operation_->paused()) {
    state_ = "Operation paused";
    return;
  }

  // Check if the operatio is valid
  au::ErrorManager error;
  if (!isValid(data, &error)) {
    state_ = au::str("E[%s]: ", error.GetMessage().c_str());
    return;
  }

  // Compute aproximate pending size
  std::string input_queue = stream_operation_->inputs(0);
  gpb::Queue *queue = ::samson::gpb::get_queue(data, input_queue);
  if (!queue) {
    state_ = au::str("Queue %s not present in data model", input_queue.c_str());
    return;
  }
  for (int i = 0; i < queue->blocks_size(); i++) {
    const gpb::Block& block = queue->blocks(i);
    KVRanges ranges = block.ranges();     // Implicit conversion
    double overlap_factor = ranges.GetOverlapFactor(range_);
    if (overlap_factor > 1) {
      LM_X(1, ("Internal error"));
    }
    pending_size_ += ( overlap_factor * block.size());
  }

  if (pending_size_ == 0) {
    state_ = au::str("Queue %s has no data to be processed in the range %s"
                     , input_queue.c_str()
                     , range_.str().c_str());
    return;
  }

  // Compute priority rank based on time and size
  priority_rank_ = pending_size_ * last_task_cronometer_.seconds();
  state_ = au::str("ready to schedule a new task %lu", priority_rank_);
}

au::SharedPointer<WorkerTask> StreamOperationInfo::schedule_new_task(size_t task_id, gpb::Data *data) {
  if (worker_task_ != NULL) {
    LM_X(1, ("Internal error"));
  }
  Operation *operation = au::Singleton<ModulesManager>::shared()->getOperation(stream_operation_->operation());

  if (!operation) {
    LM_X(1, ("Internal error: Operation not found"));                           // Create a new task
  }
  worker_task_ = new WorkerTask(samson_worker_, task_id, *stream_operation_, operation, range_);

  // Recover main input queue
  std::string input_queue = stream_operation_->inputs(0);
  gpb::Queue *queue = ::samson::gpb::get_queue(data, input_queue);

  if (!queue) {
    LM_X(1, ("Internal error: Queue not found"));                           // This cannot happen since I would never be called
  }
  // Compute the limit block to start packaging blocks in the task
  size_t accumulated_size = 0;
  au::Uint64Set block_ids;

  for (int i = 0; i < queue->blocks_size(); i++) {
    const gpb::Block& block = queue->blocks(i);

    size_t block_id = block.block_id();

    const gpb::KVRanges ranges = block.ranges();
    for (int b = 0; b < ranges.range_size(); b++) {
      KVRange range = ranges.range(b);                           // Implicit conversion
      KVRange intersection = range.Intersection(range_);

      if (intersection.size() > 0) {
        // This block should be considered
        BlockPointer real_block = BlockManager::shared()->getBlock(block.block_id());
        if (real_block == NULL) {
          LM_X(1,
               ("Internal error: Block %lu not found.",
                block.block_id()));
        }
        worker_task_->add_input(0, real_block, intersection, KVInfo(0,
                                                                    0));

        // Accumulate size if the block was not considered before
        if (!block_ids.contains(block_id)) {
          accumulated_size += real_block->getKVInfo().size;
          block_ids.insert(block_id);
        }
      }
    }

    // Stop if we need to much memory for this task
    if (accumulated_size > 200000000 ) {
      break;                           // No more accumulation for this operation
    }
  }

  // Reset cronometer
  last_task_cronometer_.Reset();

  // Add environment varialble to identify this stream_operation_id
  worker_task_->environment().Set("system.stream_operation_id", stream_operation_id_);

  LM_W(("Scheduling a new task %lu for range %s", task_id, range_.str().c_str()));

  // Return the newly generated worker task to be really shcedulled in the worker task manager
  return worker_task_;
}

std::string StreamOperationInfo::state() {
  return state_;
}

// Reset all pending worker_tasks
void StreamOperationInfo::Reset() {
  // Release current working task ( if any )
  worker_task_ = NULL;
}

void StreamOperationInfo::ResetWithError(const std::string& error_message) {
  error_.set(error_message);
  cronometer_error_.Reset();
  Reset();
}

std::string StreamOperationInfo::str() {
  std::ostringstream output;

  output << "StreamOperation " << stream_operation_id_;
  output << " " << stream_operation_->name();
  output << " " << range_.str();
  return output.str();
}

void StreamOperationInfo::fill(samson::gpb::CollectionRecord *record, const Visualization& visualization) {
  // Common columns
  ::samson::add(record, "name", stream_operation_->name(), "different");
  ::samson::add(record, "id", stream_operation_id_, "different");
  ::samson::add(record, "range", range_.str().c_str(), "different");

  if (visualization.get_flag("state")) {
    ::samson::add(record, "state", state_, "different");
    return;
  }

  if (visualization.get_flag("tasks")) {
    if (worker_task_ != NULL) {
      ::samson::add(record, "tasks", au::str("%lu: %s", worker_task_->get_id(),
                                             worker_task_->task_state().c_str()), "different");
    } else {
      ::samson::add(record, "tasks", "none", "different");
    } return;
  }

  // Default view

  ::samson::add(record, "pending_size", pending_size_, "sum,f=uint64");
  ::samson::add(record, "priority rank", priority_rank(), "different");

  if (worker_task_ != NULL) {
    ::samson::add(record, "tasks", worker_task_->get_id(), "different");
  } else {
    ::samson::add(record, "tasks", "none", "different");
  } ::samson::add(record, "state", state_,"different,left");
}

bool StreamOperationInfo::isValid(gpb::Data *data, au::ErrorManager *error) {
  // Reset error in timeout 60

  if ((error_.IsActivated() && (cronometer_error_.seconds() > 60))) {
    error_.Reset();
  }

  // Transfer the error if any
  if (error_.IsActivated()) {
    error->set(au::str("Error ( time %s ) %s"
                       , au::S(cronometer_error_).str().c_str()
                       , error_.GetMessage().c_str()));
    return false;
  }

  std::string operation_name = stream_operation_->operation();
  Operation *operation = au::Singleton<ModulesManager>::shared()->getOperation(operation_name);

  if (!operation) {
    error->set(au::str("Operation %s not found", operation_name.c_str()));
    return false;
  }

  // Check at least an input is defined
  if (stream_operation_->inputs_size() == 0) {
    error->set("No inputs defined for this operation");
    return false;
  }

  // Check number of inputs and outputs
  if (operation->getNumInputs() != stream_operation_->inputs_size()) {
    error->set(au::str("Operation %s needs %d inputs and %d are provided"
                       , operation_name.c_str()
                       , operation->getNumInputs()
                       , stream_operation_->inputs_size()));
    return false;
  }
  if (operation->getNumOutputs() != stream_operation_->outputs_size()) {
    error->set(au::str("Operation %s needs %d output and %d are provided"
                       , operation_name.c_str()
                       , operation->getNumOutputs()
                       , stream_operation_->outputs_size()));
    return false;
  }

  // Check the format of all input queues
  for (int i = 0; i < operation->getNumInputs(); i++) {
    std::string queue_name = stream_operation_->inputs(i);
    gpb::Queue *queue = get_queue(data, queue_name);
    if (queue) {                           // If queue does not exist, there is no problem. I will be automatically created
      KVFormat format(queue->key_format(), queue->value_format());
      if (format != operation->inputFormats[i]) {
        error->set(au::str("%d-th input for %s ( queue %s ) should be %s and it is %s "
                           , i + 1
                           , operation_name.c_str()
                           , queue_name.c_str()
                           , operation->inputFormats[i].str().c_str()
                           , format.str().c_str()));
        return false;
      }
    }
  }

  // Check the format of all output queues
  for (int i = 0; i < operation->getNumOutputs(); i++) {
    std::string queue_name = stream_operation_->outputs(i);
    gpb::Queue *queue = get_queue(data, queue_name);
    if (queue) {                           // If queue does not exist, there is no problem. I will be automatically created
      KVFormat format(queue->key_format(), queue->value_format());
      if (format != operation->outputFormats[i]) {
        error->set(au::str("%d-th output for %s ( queue %s ) should be %s and it is %s "
                           , i + 1
                           , operation_name.c_str()
                           , queue_name.c_str()
                           , operation->outputFormats[i].str().c_str()
                           , format.str().c_str()));
        return false;
      }
    }
  }


  // Additional checks for reduce operations....
  // TO BE COMPLETED


  return true;
}

au::SharedPointer<WorkerTask> StreamOperationInfo::worker_task() {
  return worker_task_;
}
}
}                         // End of namespace samson::stream
