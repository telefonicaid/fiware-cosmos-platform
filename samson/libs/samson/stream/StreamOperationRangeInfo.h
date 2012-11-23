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

#ifndef _H_STREAM_OPERATION
#define _H_STREAM_OPERATION

/* ****************************************************************************
 *
 * FILE                      StreamOperation.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * Definition of the stream operation for automatic queue processing
 *
 */

#include <sstream>
#include <string>

#include "au/CommandLine.h"                 // au::CommandLine
#include "au/Environment.h"                 // au::Environment
#include "au/ErrorManager.h"                // au::ErrorManager
#include "au/containers/map.h"              // au::map
#include "au/string/StringUtilities.h"      // au::Format

#include "engine/Buffer.h"                  // engine::Buffer
#include "engine/NotificationListener.h"    // engine::NotificationListener
#include "engine/NotificationListener.h"    // engine::NotificationListener

#include "samson/common/EnvironmentOperations.h"
#include "samson/common/NotificationMessages.h"
#include "samson/common/Rate.h"
#include "samson/common/samson.pb.h"        // network::...

#include "samson/module/Environment.h"      // samson::Environment

#include "samson/stream/BlockInfo.h"        // struct BlockInfo
#include "samson/stream/BlockList.h"        // BlockList
#include "samson/stream/BlockListContainer.h"  // BlockListContainer ( parent class )
#include "samson/stream/WorkerTaskManager.h"   // samson::stream::WorkerTaskManager

namespace samson {
class SamsonWorker;
class Info;
class Operation;
}

namespace samson {
namespace stream {
class Queue;
class WorkerTask;
class Block;
class BlockList;
class WorkerCommand;
class PopQueue;


/**
 * StreamOperationRangeInfo
 * \brief Manager for a stream-operation in a particular range
 */
class StreamOperationRangeInfo {
public:

  StreamOperationRangeInfo(StreamOperationGlobalInfo *stream_operation_global_info
                           , SamsonWorker *samson_worker
                           , size_t stream_operation_id
                           , const std::string& string_operation_name
                           , const KVRange& range);
  ~StreamOperationRangeInfo() {
    if (worker_task_ != NULL) {
      worker_task_->environment().Set("system.canceled_task", "yes");  // Make sure current task is never commited
    }
  }

  /**
   * \brief Review state of this stream operation for this range independently of it is running a task or not
   */
  void Review(gpb::Data *data);

  /**
   * \brief Schedule a new task for this stream operation ( task id is the identifier of the new task )
   */
  au::SharedPointer<WorkerTask> schedule_new_task(size_t task_id, gpb::Data *data);

  /**
   * \brief Fill provided record with information about this element
   */
  void fill(samson::gpb::CollectionRecord *record, const Visualization& visualization) const;

  /**
   * \brief Get priority rank to be compare with the rest of pending tasks
   */
  size_t priority_rank() const {
    return priority_rank_;
  }

  /**
   * \brief Get state string for debugging
   */
  std::string state() const {
    return state_;
  }

  /**
   * \brief Get a short version of the state to be displayed toguether with information from other ranges
   */
  std::string short_state() const {
    return short_state_;
  }

  /**
   * \brief Debug string
   */
  std::string str() const;
  KVRange range() const {
    return range_;
  }

  /**
   * \brief Get pointer to the currently running task
   */
  au::SharedPointer<WorkerTask> worker_task() const {
    return worker_task_;
  }

  /**
   * \brief Get id of the stream operation
   */
  size_t stream_operation_id() const {
    return stream_operation_id_;
  }

private:

  /**
   * \brief Review this stream operation to compute priority rank
   */
  void ReviewCurrentTask();

  /**
   * \brief Set error and reset chronometer to count how much time since last error
   */
  void SetError(const std::string error_message) {
    error_.set(error_message);
    cronometer_error_.Reset();
    state_ = au::str("Error [%s]: %s", cronometer_error_.str().c_str(), error_.GetMessage().c_str());
    short_state_ = "[E]";
  }

  /**
   * \brief Check if this stream operation is valid as it is defined
   */

  static bool IsStreamOperationValid(gpb::Data *data
                                     , const gpb::StreamOperation& stream_operation
                                     , au::ErrorManager *error);

  size_t stream_operation_id_;               // Unique identifier of the stream operation
  std::string stream_operation_name_;        // Name of the stream operation
  KVRange range_;                            // Range in this stream operation

  std::string state_input_queues_;           // String containing last state of input queues ( every review updates this )
  std::string state_;                        // String describing the state of this stream operation ( good for debugging )
  std::string short_state_;

  size_t pending_size_;                      // Pending size to be processed
  size_t priority_rank_;                     // Priority number to schedule a new task ( time * pending_size )

  au::Cronometer last_task_cronometer_;      // Last execution chronometer

  au::ErrorManager error_;                   // Contains the last error of an operation
  au::Cronometer cronometer_error_;          // Time since the last error

  au::SharedPointer<WorkerTask> worker_task_;  // Pointer to the worker task we have scheduled ( if any )

  SamsonWorker *samson_worker_;  // Pointer to samsonWorker to commit finished tasks

  StreamOperationGlobalInfo *stream_operation_global_info_;  // Pointer to my parent global infoma
};
}
}  // end of namespace samson::stream

#endif  // ifndef _H_STREAM_OPERATION
