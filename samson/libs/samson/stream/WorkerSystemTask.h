

#ifndef _H_STREAM_SYSTEM_QUEUE_TASK
#define _H_STREAM_SYSTEM_QUEUE_TASK

#include <sstream>
#include <string>

#include "au/ErrorManager.h"
#include "au/containers/map.h"                  // au::map
#include "au/string.h"                          // au::Format
#include "au/utils.h"

#include "engine/NotificationListener.h"                      // engine::NotificationListener
#include "engine/ProcessItem.h"                 // engibe::ProcessItem

#include "engine/NotificationListener.h"                      // engine::NotificationListener
#include "engine/ProcessItem.h"                 // engine::ProcessItem


#include "samson/common/Visualitzation.h"
#include "samson/common/coding.h"               // KVRange
#include "samson/common/samson.pb.h"
#include "samson/common/samson.pb.h"            // network::...

#include "samson/stream/WorkerTaskBase.h"       // samson::stream::WorkerTaskBase

namespace samson {
class SamsonWorker;

namespace stream {
class Queue;
class BlockList;
class Block;
class StreamManager;

// System queue-task ( like queuTask but whithout the isolation )

class WorkerSystemTask : public engine::ProcessItem, public WorkerTaskBase {
  std::string concept_;

public:

  // Constructor
  WorkerSystemTask(size_t id, std::string concept);
  ~WorkerSystemTask();

  // Virtual methods from WorkerTaskBase
  void fill(samson::gpb::CollectionRecord *record, const Visualization& visualization);

  // Get information
  std::string concept();
};


class BlockRequestTask : public WorkerSystemTask {
public:

  BlockRequestTask(SamsonWorker *samson_worker, size_t id, size_t block_id, size_t worker_id);

  // Virtual method from engine::ProcessItem
  virtual void run();

  // Virtual method of WorkerTaskBase
  virtual std::string str() {
    return au::str("Task %lu: Request for block %lu from worker %lu", get_id(), block_id_, worker_id_);
  }

private:

  SamsonWorker *samson_worker_;
  size_t block_id_;
  size_t worker_id_;
  BlockPointer block_;
};

class PopBlockRequestTask : public WorkerSystemTask {
public:

  PopBlockRequestTask(SamsonWorker *samson_worker
                      , size_t id
                      , size_t block_id
                      , const gpb::KVRanges& ranges
                      , size_t delialh_id
                      , size_t delilah_component_id
                      , size_t pop_id);

  // Virtual method from engine::ProcessItem
  virtual void run();

  // Virtual method of WorkerTaskBase
  virtual std::string str() {
    return au::str("Task %lu: Pop Request for block %lu Ranges %s from delilah %s ( component %d pop_id %d)"
                   , get_id()
                   , block_id_
                   , KVRanges(ranges_).str().c_str()
                   , au::code64_str(delilah_id_).c_str()
                   , delilah_component_id_
                   , pop_id_);
  }

private:


  void sent_response(engine::BufferPointer buffer);

  SamsonWorker *samson_worker_;
  size_t block_id_;
  gpb::KVRanges ranges_;
  BlockPointer block_;

  size_t delilah_id_;
  size_t delilah_component_id_;
  size_t pop_id_;
};
}
}

#endif  // ifndef _H_STREAM_SYSTEM_QUEUE_TASK
