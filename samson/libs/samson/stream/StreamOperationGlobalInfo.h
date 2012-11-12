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


/**
 * \brief \ref StreamOperationGlobalInfo is the manager for a stream-operation
 */

class StreamOperationGlobalInfo {
public:


  /**
   * \brief Constructor
   * \param samson_worker Pointer to the global samson worker
   * \param stream_operation_id Identifier of the stream operation to manage
   * \param stream_operation_name Name of the stream operation to manage
   * \param ranges Set of ranges to manage ( only a subset of all ranges )
   */
  StreamOperationGlobalInfo(SamsonWorker *samson_worker
                            , size_t stream_operation_id
                            , const std::string& stream_operation_name
                            , const std::vector<KVRange>& ranges);

  /**
   * \brief Unique method to review everything related with this stream operation
   */
  void Review(gpb::Data *data);

  /**
   * \brief Get name of the stream operation
   */
  std::string stream_operation_name() const {
    return stream_operation_name_;
  }

  /**
   * \brief Get a vector of individual managers for each range
   */
  const au::vector<StreamOperationRangeInfo>& stream_operations_range_info() {
    return stream_operations_range_info_;
  }

  /**
   * \brief Fill provided record with information avout this element
   */
  void fill(samson::gpb::CollectionRecord *record, const Visualization& visualization);

  /**
   * \brief Debug string
   */
  std::string str();

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
}  // end of namespace samson::stream

#endif  // ifndef _H_STREAM_OPERATION
