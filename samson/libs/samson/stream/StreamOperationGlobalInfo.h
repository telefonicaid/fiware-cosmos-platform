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
#include "au/string.h"                      // au::Format
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
    StreamOperationGlobalInfo(SamsonWorker *samson_worker, size_t stream_operation_id,
                              const std::string& stream_operation_name, const std::vector<KVRange>& ranges);


    // main review function based on current data model
    void Review(gpb::Data *data , int num_running_operations );

    // Get a record for this element ( tables on delilah )
    void fill(samson::gpb::CollectionRecord *record, const Visualization& visualization);

    // Debut string
    std::string str();

    // Accessors
    au::SharedPointer<WorkerTask> worker_task() const;
    std::string stream_operation_name() const {
      return stream_operation_name_;
    }

    std::vector<KVRange> ranges() const {
      // Ranges to consider in operations based on current division factor
      return ranges_for_division_factor(division_factor_);
    }

    int division_factor() {
      return division_factor_;
    }

    bool execute_range_operations() {
      return execute_range_operations_;
    }

  private:

  // Schedule defrag operation ( called from Review )
  void schedule_defrag(gpb::Data *data);
  void schedule_defrag(gpb::Data* data, const std::string& queue_name);
  
    // Compute ranges given the current division factor
    std::vector<KVRange> ranges_for_division_factor(int division_factor) const {
      if (division_factor_ == 1)
        return ranges_;
      std::vector<KVRange> all_ranges;
      for (size_t i = 0; i < ranges_.size(); i++) {
        std::vector<KVRange> tmp_ranges = ranges_[i].divide(division_factor);
        for (size_t j = 0; j < tmp_ranges.size(); j++)
          all_ranges.push_back(tmp_ranges[j]);
      }
      return all_ranges;
    }

    void ReviewCurrentTasks();

    void SetError(const std::string& error) {
      error_.set(error);
      cronometer_error_.Reset();
    }

    // Pointer to samson wokrer
    SamsonWorker *samson_worker_;

    // Information about stream operation
    size_t stream_operation_id_;
    std::string stream_operation_name_;

    // Ranges I should process in this worker
    std::vector<KVRange> ranges_;
    int division_factor_; // Overdivision of the ranges if too mush data is present

    // List of scheduled tasks
    std::vector<au::SharedPointer<DefragTask> > defrag_tasks_;

    // State in the last review...
    std::string state_;
    std::string state_input_queues_;

    // flag to indicate that operations for ranges shoudl be executed
    bool execute_range_operations_;

    // Internal error
    au::ErrorManager error_;
    au::Cronometer cronometer_error_;
};
}
} // end of namespace samson::stream

#endif  // ifndef _H_STREAM_OPERATION
