
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
#include "au/string.h"                      // au::Format

#include "engine/Buffer.h"                  // engine::Buffer
#include "engine/NotificationListener.h"    // engine::NotificationListener
#include "engine/NotificationListener.h"    // engine::NotificationListener

#include "samson/common/EnvironmentOperations.h"
#include "samson/common/NotificationMessages.h"
#include "samson/common/Rate.h"
#include "samson/common/samson.pb.h"        // network::...

#include "samson/module/Environment.h"      // samson::Environment

#include "samson/stream/BlockList.h"        // BlockList
#include "samson/stream/BlockListContainer.h"       // BlockListContainer ( parent class )
#include "samson/stream/WorkerTaskManager.h"  // samson::stream::WorkerTaskManager

#include "BlockInfo.h"                      // struct BlockInfo

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


class StreamOperationInfo {
public:

  StreamOperationInfo(SamsonWorker *samson_worker
                      , size_t stream_operation_id
                      , const KVRange& range
                      , const gpb::StreamOperation& stream_operation);
  ~StreamOperationInfo();

  // Review this stream operation to compute priority rank
  void review();

  // Get a new  task for this stream operation
  au::SharedPointer<WorkerTask> schedule_new_task(size_t task_id, gpb::Data *data);

  // Get information
  void RecomputePriorityRank(gpb::Data *data);
  size_t priority_rank();
  std::string state();

  // Get a record for this element ( listing in delilah )
  void fill(samson::gpb::CollectionRecord *record, const Visualization& visualization);

  // Reset all pending worker_tasks
  void Reset();
  void ResetWithError(const std::string& error_message);

  // Debug string
  std::string str();

  // Accessor to worker task
  au::SharedPointer<WorkerTask> worker_task();

private:

  // Check if this stream operation is valid as it is
  bool isValid(gpb::Data *data, au::ErrorManager *error);


  size_t stream_operation_id_;          // Unique identifier of the stream operation
  KVRange range_;                       // Range in this stream operation

  std::string state_;                   // String describing the state of this stream operation ( good for debugging )

  size_t pending_size_;                 // Pending size to be processed
  size_t priority_rank_;                // Last Priority number computed

  au::SharedPointer<gpb::StreamOperation> stream_operation_;      // Copy of the stream operation definition

  au::Cronometer last_task_cronometer_;      // Last execution cronometer

  // Error management
  au::ErrorManager error_;              // Contains the last error of an operation
  au::Cronometer cronometer_error_;      // Time since the last error

  // Pointer to the worker task we have scheduled ( if any )
  au::SharedPointer<WorkerTask> worker_task_;

  // Pointer to samsonWorker to commit finished tasks
  SamsonWorker *samson_worker_;
};
}
}  // end of namespace samson::stream

#endif  // ifndef _H_STREAM_OPERATION