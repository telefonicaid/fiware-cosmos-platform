

#include "GlobalBlockSortInfo.h"  // Own interface

namespace samson {
void BlockSortInfo::NotifyInputForStreamOperation(const std::string&  stream_operation_name
                                                  , const std::string& queue_name
                                                  , bool state
                                                  , size_t pos) {
  if (so_is_state_ && !state) {
    return;   // State queues have higher priority
  }
  if (( so_pos_ != SIZE_T_UNDEFINED ) && ( so_pos_ < pos )) {
    return;
  }
  // Update information
  so_is_state_ = state;
  so_pos_ =  pos;
  so_name_ = stream_operation_name;
  so_queue_ = queue_name;
}

void BlockSortInfo::NotifyInputForExternStreamOperation(const std::string&  stream_operation_name
                                                        , const std::string& queue_name
                                                        , size_t pos) {
  if (( extern_so_pos_ != SIZE_T_UNDEFINED ) && ( extern_so_pos_ < pos )) {
    return;
  }
  // Update information
  extern_so_pos_ =  pos;
  extern_so_name_ = stream_operation_name;
  extern_so_queue_ = queue_name;
}

std::string BlockSortInfo::State() const {
  std::ostringstream output;

  // Inform about involved tasks
  if (tasks_.size() > 0) {
    output << "WT" << au::str(tasks_) << " ";
  }

  // Infomration about involved queues
  if (queues_.size() > 0) {
    output << "Q" << au::str(queues_) << " ";
  }

  // Involved in a stream operation in this worker
  if (so_pos_ != SIZE_T_UNDEFINED) {
    output << "SO[ " << so_name_ << " ";
    output << (so_is_state_ ? "[STATE] " : "") << so_queue_ << " " << au::str(so_pos_) << "]";
  } else if (extern_so_pos_ != SIZE_T_UNDEFINED) {
    output << "EXTERN SO[ " << extern_so_name_ << " ";
    output << extern_so_queue_ << " " << au::str(extern_so_pos_) << "]";
  }

  // If nothing else to show, inform about old data model belonging
  if (( tasks_.size() == 0) && ( queues_.size() == 0 )  && ( so_pos_ == SIZE_T_UNDEFINED )) {
    if (old_queues_.size() > 0) {
      output << "OLD Q" << au::str(old_queues_) << " ";
    }
  }
  return output.str();
}

bool BlockSortInfo::Compare(BlockSortInfo *other) {
  // Higher priority to blocks involved in sheduled tasks
  size_t min_task_id_1 = min_task_id_;
  size_t min_task_id_2 = other->min_task_id_;

  if (min_task_id_1 != min_task_id_2) {
    return (min_task_id_1 < min_task_id_2);
  }

  // Higher priority to the blocks involved in stream operations for this worker
  if (so_is_state_ && !other->so_is_state_) {
    return true;
  }
  if (!so_is_state_ && other->so_is_state_) {
    return false;
  }
  if (so_pos_ != other->so_pos_) {
    if (( so_pos_ != SIZE_T_UNDEFINED ) && ( other->so_pos_ == SIZE_T_UNDEFINED )) {
      return true;
    }
    if (( so_pos_ == SIZE_T_UNDEFINED ) && ( other->so_pos_ != SIZE_T_UNDEFINED )) {
      return false;
    }
    if (( so_pos_ != SIZE_T_UNDEFINED ) && ( other->so_pos_ != SIZE_T_UNDEFINED )) {
      // Order in bytes
      return ( so_pos_ < other->so_pos_ );
    }
  }

  // higher priority to blocks involved in stream operations in other workers
  if (extern_so_pos_ != other->extern_so_pos_) {
    if (( extern_so_pos_ != SIZE_T_UNDEFINED ) && ( other->extern_so_pos_ == SIZE_T_UNDEFINED )) {
      return true;
    }
    if (( extern_so_pos_ == SIZE_T_UNDEFINED ) && ( other->extern_so_pos_ != SIZE_T_UNDEFINED )) {
      return false;
    }
    if (( extern_so_pos_ != SIZE_T_UNDEFINED ) && ( other->extern_so_pos_ != SIZE_T_UNDEFINED )) {
      // Order in bytes
      return ( extern_so_pos_ < other->extern_so_pos_ );
    }
  }

  // Modules go to the end
  bool modules = IsOnlyInModulesQueue();
  bool other_modules = other->IsOnlyInModulesQueue();
  if (modules != other_modules) {
    return !modules;
  }

  if (( queues_.size() > 0 ) && ( other->queues_.size() == 0 )) {
    return true;
  }
  if (( queues_.size() == 0 ) && ( other->queues_.size() > 0 )) {
    return false;
  }

  // Put first blocks present in more current queues
  if (queues_.size() != other->queues_.size()) {
    return queues_.size() > other->queues_.size();
  }


  // Put first blocks present in old data-models
  if (old_queues_.size() != other->old_queues_.size()) {
    return old_queues_.size() > other->old_queues_.size();
  }

  // Same order
  return true;
}

bool BlockSortInfo::IsOnlyInModulesQueue() {
  if (queues_.size() != 1) {
    return false;
  }
  return( *queues_.begin() == ".modules" );
}
}
