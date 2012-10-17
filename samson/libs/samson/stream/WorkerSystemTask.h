
#ifndef _H_STREAM_SYSTEM_QUEUE_TASK
#define _H_STREAM_SYSTEM_QUEUE_TASK

#include <sstream>
#include <string>
#include <vector>

#include "au/log/LogMain.h"
#include "au/ErrorManager.h"
#include "au/containers/map.h"                  // au::map
#include "au/string.h"                          // au::Format
#include "au/utils.h"
#include "au/log/LogCommon.h"
#include "au/log/LogCentral.h"

#include "engine/NotificationListener.h"        // engine::NotificationListener
#include "engine/ProcessItem.h"                 // engibe::ProcessItem

#include "samson/common/Visualitzation.h"
#include "samson/common/coding.h"               // KVRange
#include "samson/common/KVRange.h"
#include "samson/common/samson.pb.h"            // network::...
#include "samson/stream/WorkerTaskBase.h"       // samson::stream::WorkerTaskBase

namespace samson {
class SamsonWorker;

namespace stream {
class Queue;
class BlockList;
class Block;
class StreamManager;

/*

 Tasks executed by the platform
 Not that this tasks derive from WorkerTaskBase so they execute in foreground


 BlockDistributionTask: Distribute a particular block to a set of workers
 PopBlockRequestTask:   Distribute a particular block to a delilah client
 DefragTask:            Defrag a set of blocks in a queue to generate better alternatives to this block in the same queue
 
 */

class BlockDistributionTask : public WorkerTaskBase {
  public:
    BlockDistributionTask(SamsonWorker *samson_worker, size_t id, size_t block_id,
                          const std::vector<size_t>& worker_ids);

    // Virtual method from engine::ProcessItem
    virtual void run();

    // Virtual method of WorkerTaskBase
    virtual std::string str() {
      std::ostringstream output;
      output << au::str("Task %lu: Distributing block %lu to workers ", worker_task_id(), block_id_);
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

class DefragTask : public WorkerTaskBase {
  public:
  DefragTask(SamsonWorker *samson_worker, const std::string& queue_name, size_t id , const std::vector<KVRange>& ranges ) :
      WorkerTaskBase(samson_worker, id, "Defrag Operation") {
      samson_worker_ = samson_worker;
        queue_name_ = queue_name;
        ranges_ = ranges;
        AU_LM_W( ("DEFRAG over ranges %s" , ::samson::str(ranges_).c_str() ));
    }

    // Virtual method from engine::ProcessItem
    virtual void run();

    // Generate the commit command necessary to update data model
    std::string commit_command();

    // Virtual method of WorkerTaskBase
    virtual std::string str() {
      return au::str("Task %lu: Defrag.. ", worker_task_id());
    }

  private:
  
    void AddOutputBuffer(engine::BufferPointer buffer);

    SamsonWorker *samson_worker_;
    std::string queue_name_;   // Name of the queue we are processing
  std::vector<KVRange> ranges_;
};
}
}

#endif  // ifndef _H_STREAM_SYSTEM_QUEUE_TASK
