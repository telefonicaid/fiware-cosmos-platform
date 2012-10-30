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
#ifndef _H_STREAM_OPERATION_GLOBAL_INFO
#define _H_STREAM_OPERATION_GLOBAL_INFO

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
#include <vector>

#include "au/CommandLine.h"                 // au::CommandLine
#include "au/containers/map.h"              // au::map
#include "au/Environment.h"                 // au::Environment
#include "au/ErrorManager.h"                // au::ErrorManager
#include "au/string/StringUtilities.h"                      // au::Format
#include "engine/Buffer.h"                  // engine::Buffer
#include "engine/NotificationListener.h"    // engine::NotificationListener
#include "engine/NotificationListener.h"    // engine::NotificationListener
#include "samson/common/EnvironmentOperations.h"
#include "samson/common/NotificationMessages.h"
#include "samson/common/Rate.h"
#include "samson/common/samson.pb.h"        // network::...
#include "samson/module/Environment.h"      // samson::Environment
#include "samson/stream/BlockInfo.h"                      // struct BlockInfo
#include "samson/stream/BlockList.h"        // BlockList
#include "samson/stream/BlockListContainer.h"       // BlockListContainer ( parent class )
#include "samson/stream/WorkerTaskManager.h"  // samson::stream::WorkerTaskManager


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
    
    class StreamOperationGlobalInfo {
      
    public:
      
      StreamOperationGlobalInfo(SamsonWorker *samson_worker
                                , size_t stream_operation_id
                                , const std::string& stream_operation_name
                                , const std::vector<KVRange>& ranges );
      
      
      // Unique method to review everything related with this stream operation
      void Review( gpb::Data *data );
      
      // Accessors
      au::SharedPointer<WorkerTask> worker_task() const;
      std::string stream_operation_name() const {
        return stream_operation_name_;
      }

      // Detail about every range in this stream operation
      const au::vector<StreamOperationRangeInfo>& stream_operations_range_info()
      {
        return stream_operations_range_info_;
      }
      
      // Get a record for this element ( tables on delilah )
      void fill(samson::gpb::CollectionRecord *record, const Visualization& visualization);
      std::string str();      // Debut string
      
      
      // Get defrag ranges ( all ranges to defrag input data )
      std::vector<KVRange> GetDefragKVRanges();
      
    private:
      
      void ReviewIntern(gpb::Data *data);
      
      std::vector<KVRange> GetActiveRanges();
      
      void SetError(const std::string& error) {
        error_.set(error);
        cronometer_error_.Reset();
      }
      
      // Pointer to samson wokrer
      SamsonWorker *samson_worker_;
      
      // Detail about every range in this stream operation
      au::vector<StreamOperationRangeInfo> stream_operations_range_info_;
      
      // Information about stream operation
      size_t stream_operation_id_;
      std::string stream_operation_name_;
      
      // Global information for the stream operation
      std::string state_;
      std::string state_input_queues_;
      
      // Internal error
      au::ErrorManager error_;
      au::Cronometer cronometer_error_;
    };
  }
} // end of namespace samson::stream

#endif  // ifndef _H_STREAM_OPERATION
