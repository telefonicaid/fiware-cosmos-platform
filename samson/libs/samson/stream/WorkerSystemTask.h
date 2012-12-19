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

#ifndef _H_STREAM_SYSTEM_QUEUE_TASK
#define _H_STREAM_SYSTEM_QUEUE_TASK

#include <sstream>
#include <string>
#include <vector>

#include "au/ErrorManager.h"
#include "au/containers/map.h"                  // au::map
#include "au/log/LogCentral.h"
#include "au/log/LogCommon.h"
#include "au/log/LogMain.h"
#include "au/string/StringUtilities.h"          // au::Format
#include "au/utils.h"

#include "engine/NotificationListener.h"        // engine::NotificationListener
#include "engine/ProcessItem.h"                 // engibe::ProcessItem

#include "samson/common/KVRange.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/coding.h"               // KVRange
// network::...
#include "samson/stream/WorkerTaskBase.h"       // samson::stream::WorkerTaskBase

namespace samson {
class SamsonWorker;

namespace stream {
class Queue;
class BlockList;
class Block;
class StreamManager;

/*
 *
 * Tasks executed by the platform
 * Not that this tasks derive from WorkerTaskBase so they execute in foreground
 *
 *
 * BlockDistributionTask: Distribute a particular block to a set of workers
 * PopBlockRequestTask:   Distribute a particular block to a delilah client
 *
 */

class BlockRequestTask : public WorkerTaskBase {
public:
  BlockRequestTask(SamsonWorker *samson_worker, size_t id, size_t block_id, const std::vector<size_t>& worker_ids);

  // Virtual method from engine::ProcessItem
  virtual void run();

  // Virtual method of WorkerTaskBase
  virtual std::string str() {
    std::ostringstream output;

    output << au::str("Task %lu: Distributing block %s to workers ", worker_task_id(), str_block_id(block_id_).c_str());
    output << au::str(worker_ids_);
    return output.str();
  }

private:
  size_t block_id_;
  std::vector<size_t> worker_ids_;
  BlockPointer block_;
};

class PopBlockRequestTask : public WorkerTaskBase {
public:
  PopBlockRequestTask(SamsonWorker *samson_worker
                      , size_t id
                      , size_t block_id
                      , size_t delialh_id
                      , size_t delilah_component_id
                      , size_t pop_id);

  // Virtual method from engine::ProcessItem
  virtual void run();

  // Virtual method of WorkerTaskBase
  virtual std::string str() {
    return au::str("Task %lu: Pop Request for block %lu from delilah %s ( component %d pop_id %d)",
                   worker_task_id()
                   , block_id_
                   , au::code64_str(delilah_id_).c_str()
                   , delilah_component_id_, pop_id_);
  }

private:

  size_t block_id_;
  BlockPointer block_;

  size_t delilah_id_;
  size_t delilah_component_id_;
  size_t pop_id_;
};
}
}

#endif  // ifndef _H_STREAM_SYSTEM_QUEUE_TASK
