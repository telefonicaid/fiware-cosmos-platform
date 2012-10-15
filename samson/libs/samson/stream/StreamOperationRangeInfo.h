
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
    
    
    class StreamOperationRangeInfo {
    public:
      
      StreamOperationRangeInfo(SamsonWorker *samson_worker
                          , size_t stream_operation_id
                          , const std::string& string_operation_name
                          , const KVRange& range);
      ~StreamOperationRangeInfo();
      
      // Review this stream operation to compute priority rank
      void ReviewCurrentTask(  );
      
      // Review state of this stream operation for this range independently of it is running a task or not
      void Review( gpb::Data *data );
      
      // Get a new task for this stream operation ( or compute priority for next task )
      // If (task_id == -1) --> compute priority
      au::SharedPointer<WorkerTask> schedule_new_task( size_t task_id, gpb::Data *data );
      
      // Get a record for this element ( listing in delilah )
      void fill(samson::gpb::CollectionRecord *record, const Visualization& visualization);
      
      
      // Get information
      size_t priority_rank();
      std::string state();
      void set_state( const std::string& state );
      std::string str();
      au::SharedPointer<WorkerTask> worker_task();      // Accessor to worker task
      size_t stream_operation_id()
      {
        return stream_operation_id_;
      }
      
    private:
      
      // Set error and reset cronometer to count how much time since last error
      void SetError( const std::string error_message )
      {
        error_.set( error_message );
        cronometer_error_.Reset();
      }
      
      size_t stream_operation_id_;           // Unique identifier of the stream operation
      std::string stream_operation_name_;    // Name of the stream operation
      KVRange range_;                        // Range in this stream operation
      
      std::string state_input_queues_;       // String containing last state of input queues ( every review updates this )
      std::string state_;                    // String describing the state of this stream operation ( good for debugging )
      
      size_t pending_size_;                  // Size to be processes ( thrigerring task if > 0)
      size_t priority_rank_;                 // Priority number to scehdule a new task ( time * pending_size )
      
      au::Cronometer last_task_cronometer_;  // Last execution cronometer

      au::ErrorManager error_;               // Contains the last error of an operation
      au::Cronometer cronometer_error_;      // Time since the last error
      
      // Pointer to the worker task we have scheduled ( if any )
      au::SharedPointer<WorkerTask> worker_task_;
      
      // Pointer to samsonWorker to commit finished tasks
      SamsonWorker *samson_worker_;
    };
   
    // Check if this stream operation is valid as it is
    bool isStreamOperationValid(gpb::Data *data, const gpb::StreamOperation& stream_operation , au::ErrorManager *error);
    
    
  }
}  // end of namespace samson::stream

#endif  // ifndef _H_STREAM_OPERATION