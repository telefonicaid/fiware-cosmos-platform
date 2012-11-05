#include "samson/stream/StreamOperationRangeInfo.h"      // Own interface
#include <string>

#include "engine/Engine.h"
#include "engine/ProcessManager.h"
#include "samson/common/gpb_operations.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"
#include "samson/module/ModulesManager.h"
#include "samson/stream/StreamOperationGlobalInfo.h"
#include "samson/stream/WorkerTask.h"
#include "samson/worker/SamsonWorker.h"
#include "samson/common/Logs.h"

namespace samson {
  namespace stream {
    
    class InputData
    {
      
    public:
      
      std::string str()
      {
        // Build string to inform about this
        std::ostringstream output;
        output << "[ ";
        output << total_info_.str() << " ";
        output << "Ready: " << au::str_percentage( ready_info_.size , total_info_.size );
        output << " ]";
        return output.str();
      }
      
      void AppendToTotal( double factor , KVInfo info )
      {
        total_info_.Append(factor, info);
      }

      void AppendToReady( double factor , KVInfo info )
      {
        ready_info_.Append(factor, info);
      }

      FullKVInfo total_info()
      {
        return total_info_;
      };
      
      FullKVInfo ready_info()
      {
        return ready_info_;
      }


      bool IsFullyReady()
      {
        return (total_info_.size == ready_info_.size);
      }
      
    private:
      
      FullKVInfo total_info_;
      FullKVInfo ready_info_;
      
    };
    
    class InputsData
    {
      
    public:
      
      InputsData( int num_inputs )
      {
        for ( int i = 0 ; i < num_inputs ; i++ )
        {
          inputs_data.push_back( InputData() );
        }
      }
      
      void AppendToTotal( int input ,  double factor , KVInfo info )
      {
        inputs_data[input].AppendToTotal( factor , info );
      }
      
      void AppendToReady( int input , double factor , KVInfo info )
      {
        inputs_data[input].AppendToReady( factor , info );
      }
      
      std::string str()
      {
        std::ostringstream output;
        for (size_t i = 0 ; i < inputs_data.size() ; i++ )
          output << inputs_data[i].str();
        return output.str();
      }
      
      size_t GetReadySize()
      {
        size_t total = 0;
        for (size_t i = 0; i < inputs_data.size(); ++i) {
          total +=  inputs_data[i].ready_info().size;
        }
        return total;
      }

      size_t GetReadySizeForAllInputsExceptLastOne()
      {
        size_t total = 0;
        for (size_t i = 0; i < inputs_data.size()-1  ; ++i) {
          total +=  inputs_data[i].ready_info().size;
        }
        return total;
      }
      
      
      size_t GetTotalSize()
      {
        size_t total = 0;
        for (size_t i = 0; i < inputs_data.size(); ++i) {
          total +=  inputs_data[i].total_info().size;
        }
        return total;
      }
      
      size_t GetLastInputTotalSize()
      {
        return  inputs_data[inputs_data.size()-1].total_info().size;
      }
      
      bool AreAllInputFullyReady()
      {
        for (size_t i = 0; i < inputs_data.size(); ++i)
          if( !inputs_data[i].IsFullyReady() )
            return false;
        return true;
      }
      
      bool IsLastInputFullyReady()
      {
        return inputs_data[inputs_data.size()-1].IsFullyReady();
      }
      
      
    private:
      
      std::vector<InputData> inputs_data;
      
    };
    
    StreamOperationRangeInfo::StreamOperationRangeInfo(StreamOperationGlobalInfo * stream_operation_global_info,
                                                       SamsonWorker *samson_worker, size_t stream_operation_id,
                                                       const std::string& stream_operation_name, const KVRange& range)
    : stream_operation_id_(stream_operation_id)
    , stream_operation_name_(stream_operation_name)
    , range_(range)
    , state_input_queues_("No info")
    , state_("No info")
    , short_state_("No info")
    , priority_rank_(0)
    , range_division_necessary_(false)
    , last_task_cronometer_()
    , error_()
    , cronometer_error_()
    , worker_task_(NULL)  // No task by default
    , samson_worker_(samson_worker)  // Keep a pointer to samson worker
    , stream_operation_global_info_(stream_operation_global_info)  // Keep a reference to my parent global information
    {
    }
    
    /*
     Dynamic inputs
     ----------------------------
     map,parse,...     All inputs are dynamic (1 dynamic input)
     Batch operation:  All inputs are non dynamic ( all input required )
     Stream operation: All inputs are dynamic except last ( state ) input
     Reduce forward:   All inputs are dynamic except last ( auxiliary data-set ) input
     
     Inputs to count for pending size
     ----------------------------
     map,parse,...     Single input ( counts )
     Batch operation:  All inputs
     Stream operation: N-1 ( last one is state and it is not considered pending.size)
     */
    
    // Get number of inputs that can be taken in small blocks
    int GetNumberOfDynamicInputs(gpb::StreamOperation* stream_operation) {
      std::string operation_name = stream_operation->operation();
      Operation *operation = au::Singleton<ModulesManager>::shared()->getOperation(operation_name);
      
      if (!operation) {
        LM_X(1, ("Internal error"));
      }
      
      bool batch_operation = stream_operation->batch_operation();
      bool reduce_operation = (operation->getType() == Operation::reduce);
      
      if (reduce_operation && batch_operation) {
        return 0;
      }
      
      if (reduce_operation) {
        return operation->getNumInputs() - 1;
      }
      
      // No reduce... it is always 1
      return 1;
      
    }
    
    // Get the number of inputs to take into account for triggering tasks
    int GetNumberOfInputsForThrigering(gpb::StreamOperation* stream_operation) {
      std::string operation_name = stream_operation->operation();
      Operation *operation = au::Singleton<ModulesManager>::shared()->getOperation(operation_name);
      
      if (!operation) {
        LM_X(1, ("Internal error"));
      }
      
      bool batch_operation = stream_operation->batch_operation();
      bool reduce_operation = (operation->getType() == Operation::reduce);
      
      if (reduce_operation && batch_operation) {
        return operation->getNumInputs();
      }
      
      if (reduce_operation) {
        return operation->getNumInputs() - 1;
      }
      
      // No reduce... it is always 1
      return 1;
    }
    
    void StreamOperationRangeInfo::Review(gpb::Data *data) {
      
      // Reset strings of states
      state_input_queues_ = "";
      state_ = "";
      short_state_ = "";
      range_division_necessary_ = false;
      
      // Init pending size and priority to be recomputed
      pending_size_ = 0;
      priority_rank_ = 0;
      
      // Recover stream operation from data
      gpb::StreamOperation *stream_operation = gpb::getStreamOperation(data, stream_operation_id_);
      if (!stream_operation) {
        SetError(au::str("stream operation %s does not exist"));
        worker_task_ = NULL;   // Cancel task if any
        return;
      }
      
      // Check if the operation is valid
      au::ErrorManager error;
      if (!isStreamOperationValid(data, *stream_operation, &error)) {
        SetError(au::str("Error validating stream operation: %s", error.GetMessage().c_str()));
        worker_task_ = NULL;   // Cancel task if any
        return;
      }
      
      // Get operation to be executed
      std::string operation_name = stream_operation->operation();
      Operation *operation = au::Singleton<ModulesManager>::shared()->getOperation(operation_name);
      if (!operation)
      {
        SetError(au::str("Operation %s not found" , operation_name.c_str() ));
        return;
      }
      
      // Flag to detect reduce and batch operations
      bool reduce_operation = (operation->getType() == Operation::reduce);
      bool batch_operation  = stream_operation->batch_operation();
      
      // Scan all inputs ( schedule defrag operations if required )
      InputsData inputs_data( stream_operation->inputs_size() );
      
      for (int i = 0; i < stream_operation->inputs_size(); ++i) {
        std::string input_queue = stream_operation->inputs(i); // name of the i-th input queue
        gpb::Queue* queue = gpb::get_queue(data, input_queue );
                
        if( queue )
        {
          // Take all the blocks intersecting block "range_"
          bool ready = true; // Flag to indicate that data is ready so far
          for ( int b = 0 ; b < queue->blocks_size() ; b++ )
          {
            const gpb::Block& block = queue->blocks(b);
            KVRange range = block.range(); // Implicit conversion
            double overlap_factor = range.GetOverlapFactor( range_ );
            if ( overlap_factor > 0 )
            {
              KVInfo block_info( block.size() , block.kvs() );
              
              inputs_data.AppendToTotal( i , overlap_factor , block_info);
              
              // If this block has to be defrag by others, ignore
              if( range.hg_begin_ < range_.hg_begin_ )
                continue;
              
              if( range.hg_end_ > range_.hg_end_ )
              {
                // If I have to defrag, consider not ready...
                ready = false;
              }
              
              if( stream::BlockManager::shared()->GetBlock( block.block_id() ) == NULL )
              {
                // Block not local ( data has not been requested )
                ready = false;
                samson_worker_->worker_block_manager()->RequestBlock( block.block_id() );
              }
              
              if ( ready )
                inputs_data.AppendToReady(i, overlap_factor, block_info );
              
            }
          }
        }
        
      }
      
      // Inform about content in in input queues
      state_input_queues_ +=  inputs_data.str();
      
      // Reset error in timeout 60
      if ((error_.IsActivated() && (cronometer_error_.seconds() > 10))) {
        error_.Reset();
      }
      if (error_.IsActivated()) {
        // If we had an error, wait some time...
        state_ = au::str("Error [%s]: %s", cronometer_error_.str().c_str(), error_.GetMessage().c_str());
        short_state_ = "[E]";
        return;
      }
      
      // If operation is paused, do not consider then...
      if (stream_operation->paused()) {
        state_ = "Operation paused";
        short_state_ = "[P]";
        return;
      }
      
      // Decide if the range has to be divided ( only if no tasks are being executed )
      if( ( worker_task_ == NULL) && ( defrag_task_ == NULL ) )
      {
        
        // In batch reduce operations, range will be divided if all data (all inputs) for this range is larger than max memory size per task
        if (reduce_operation && batch_operation && ( inputs_data.GetTotalSize() > GetMaxMemoryPerTask() )) {
          range_division_necessary_ = true;
          return;
        }
        
        if (reduce_operation && !batch_operation && inputs_data.GetLastInputTotalSize() > (GetMaxMemoryPerTask() / 2)) {
          range_division_necessary_ = true;
          return;
        }
      }
      
      // ---------------------------------------------------------------------
      // Schedule a defrag operation if required and we are not running one
      // ---------------------------------------------------------------------
      
      if( defrag_task_ != NULL )
        ReviewCurrentDefragTask();
      else
      {
        for (int i = 0; i < stream_operation->inputs_size(); ++i)
        {
          
          std::string input_queue = stream_operation->inputs(i);
          gpb::Queue* queue = gpb::get_queue(data, input_queue);
          
          if (!queue) {
            continue; // No queue, no problem...
          }
          
          for (int b = 0; b < queue->blocks_size(); ++b) {
            const gpb::Block& block = queue->blocks(b);
            KVRange range = block.range();
            
            if (!range.IsOverlapped(range_)) {
              continue;   // we are not interested in this block, no problem with this...
            }
            
            if (range.hg_begin_ < range_.hg_begin_) {
              continue; // This is not our responsability
            }
            
            if (range.hg_end_ > range_.hg_end_)
            {

              BlockPointer real_block = BlockManager::shared()->GetBlock(block.block_id());
              KVInfo info(block.size(), block.kvs());

              if( real_block != NULL ) // Local block
              {
                std::vector<KVRange> defrag_ranges = stream_operation_global_info_->GetDefragKVRanges();
                defrag_task_ = new DefragTask(  samson_worker_
                                              , input_queue
                                              , samson_worker_->task_manager()->getNewId()
                                              , defrag_ranges );
                
                defrag_task_->AddInput(0, real_block, range, info);
                samson_worker_->task_manager()->Add( defrag_task_.dynamic_pointer_cast<WorkerTaskBase>() );
                break;
              }
            }
          }
        }
      }
      
      // Check if we can really execute this range
      if( reduce_operation && batch_operation && !inputs_data.AreAllInputFullyReady() )
      {
        state_ = "Waiting for all data to be ready";
        short_state_ = "[WR]";
        return;
      }
      
      if( reduce_operation && !batch_operation && !inputs_data.IsLastInputFullyReady() )
      {
        state_ = "Waiting for state to be ready";
        short_state_ = "[WS]";
        return;
      }

      
      if (worker_task_ != NULL) {
        state_ = "Running task" + worker_task_->str();
        ReviewCurrentTask();   // Review if this task finished...
        short_state_ = "[*]";
        return;
      }
      
      // Compute pending to be process size
      if( reduce_operation && !batch_operation )
        pending_size_ = inputs_data.GetReadySizeForAllInputsExceptLastOne();
      else
        pending_size_ = inputs_data.GetReadySize();
      
      // Eval pending size to schedule new tasks
      if (pending_size_ > 0) {
        // Compute priority based on pending size and time
        size_t time = 1 + last_task_cronometer_.seconds();
        priority_rank_ = pending_size_ * time;
        state_ = "Ready to schedule a new task";
        short_state_ = "[R]";
      } else {
        state_ = "No data to be processed";
        short_state_ = "[N]";
        last_task_cronometer_.Reset();
        // Reset the chronometer if not data since otherwise it will count a lot of time when some data appears
      }
    }
    
    void StreamOperationRangeInfo::ReviewCurrentTask() {
      
      if (worker_task_ != NULL) {
        
        // Process possibly generated output buffers
        worker_task_->processOutputBuffers();
        
        // If we are running a task, let see if it is finished
        if (worker_task_->IsWorkerTaskFinished()) {

          
          // Process outputs generated by this task
          worker_task_->processOutputBuffers();
          
          if (worker_task_->error().IsActivated()) {
            std::string error_message = worker_task_->error().GetMessage();
            AU_M(logs.task_manager, ("Error in task %lu (%s)" , worker_task_->id() , error_message.c_str() ));
            SetError(error_message);
            worker_task_ = NULL;
            return;
          } else {

            AU_M(logs.task_manager, ("Commiting task %lu (%s)" , worker_task_->id() , worker_task_->str().c_str() ));
            // Commit changes and release task
            std::string commit_command = worker_task_->commit_command();
            std::string caller = au::str("task %lu // %s", worker_task_->worker_task_id(), str().c_str());
            au::ErrorManager error;
            samson_worker_->data_model()->Commit(caller, commit_command, error);
            if (error.IsActivated())
              SetError(error.GetMessage());
            
            worker_task_ = NULL; // Release our copy of this task
            return;
          }
        }
      }
    }
    void StreamOperationRangeInfo::ReviewCurrentDefragTask()
    {
      if (defrag_task_ != NULL) {
        // If we are running a task, let see if it is finished
        if (defrag_task_->IsWorkerTaskFinished()) {
          
          
          if (defrag_task_->error().IsActivated()) {
            std::string error_message = worker_task_->error().GetMessage();
            AU_M(logs.task_manager, ("Error in defrag task %lu (%s)" , worker_task_->id() , worker_task_->str().c_str() ));
            SetError(error_message);
            defrag_task_ = NULL;
            return;
          } else {
            // Commit changes and release task
            AU_M(logs.task_manager, ("Commiting defrag task %lu (%s)" , worker_task_->id() , worker_task_->str().c_str() ));
            std::string commit_command = defrag_task_->commit_command();
            std::string caller = au::str("defrag task %lu // %s", defrag_task_->worker_task_id(), str().c_str());
            au::ErrorManager error;
            samson_worker_->data_model()->Commit(caller, commit_command, error);
            
            if (error.IsActivated())
              SetError(error.GetMessage());
            defrag_task_ = NULL; // Release our copy of this task
            return;
          }
        }
      }
    }
    
    au::SharedPointer<WorkerTask> StreamOperationRangeInfo::schedule_new_task(size_t task_id, gpb::Data *data) {
      
      // Get the maximum allowed memory for a single task
      size_t max_memory_per_task = GetMaxMemoryPerTask();
      
      // Get information about the stream operation
      gpb::StreamOperation *stream_operation = gpb::getStreamOperation(data, stream_operation_id_);
      
      // Get the operation to be executed
      std::string operation_name = stream_operation->operation();
      Operation *operation = au::Singleton<ModulesManager>::shared()->getOperation(operation_name);
      
      // Everything is checked by Rreview command, so no error is accepted
      if (priority_rank_ == 0)
      {
        SetError("Scheduling a task with priority_rank=0");
        return au::SharedPointer<WorkerTask>(NULL);
      }
      if (!stream_operation)
      {
        SetError(au::str("Error recovering stream operation %s" , stream_operation_name_.c_str() ));
        return au::SharedPointer<WorkerTask>(NULL);
      }
      if (error_.IsActivated())
      {
        SetError("Error scheduling a task with a previous error");
        return au::SharedPointer<WorkerTask>(NULL);
      }
      if (!isStreamOperationValid(data, *stream_operation, &error_) )
        return au::SharedPointer<WorkerTask>(NULL);
      
      if (stream_operation->paused())
      {
        SetError("Scheduling a task in a paused stream operation");
        return au::SharedPointer<WorkerTask>(NULL);
      }
      if (!operation)
      {
        SetError(au::str("Error finding operation  %s", operation_name.c_str() ) );
        return au::SharedPointer<WorkerTask>(NULL);
      }
      
      // Get number of input-channels with dynamic input ( we can take more or less data to be processed )
      int num_dynamic_input = GetNumberOfDynamicInputs(stream_operation);
      
      // Create candidate task
      worker_task_ = new WorkerTask(samson_worker_, task_id, *stream_operation, operation, range_);
      
      // Accumulated memory used for this task ( we will limit taken blocks with this element )
      size_t accumulated_size = 0; // Accumulated size required to be in memory for this operation
      au::Uint64Set block_ids;     // Set of Block ids required so far
      
      // Scan all inputs ( in reverse order to include first state.... )
      for (int i = stream_operation->inputs_size() - 1; i >= 0; --i) {
        std::string input_queue = stream_operation->inputs(i);
        gpb::Queue *queue = ::samson::gpb::get_queue(data, input_queue);
        
        if (!queue) {
          continue; // No data at this input
        }
        
        for (int b = 0; b < queue->blocks_size(); ++b) {
          const gpb::Block& block = queue->blocks(b);
          size_t block_id = block.block_id();
          KVRange range = block.range();
          KVInfo info(block.size(), block.kvs());
          // If this block is not in our range of interest, just skip it
          if (!range.IsOverlapped(range_))
            continue;
          
          if (!range_.Includes(range))
          {
            if (i < num_dynamic_input)
              break; // In dynamic data,
            
            LM_X(1, ("Internal error. Block %lu (%s) in queue %s shoudl be contained in range %s"
                     , block_id
                     , range.str().c_str()
                     , input_queue.c_str()
                     , range_.str().c_str() ));
          }
          
          BlockPointer real_block = BlockManager::shared()->GetBlock(block.block_id());
          if (real_block == NULL) {
            break; // We cannot include more blocks since they are not in memory
          }
          
          // Accumulate size if the block was not considered before
          if (!block_ids.contains(block_id)) {
            accumulated_size += real_block->getSize(); // Accumulate size of the block
            block_ids.insert(block_id);
          }
          // Add input to the task
          worker_task_->AddInput(i, real_block, range_, info);
          
          // Stop if we need too much memory for this task
          if (i < num_dynamic_input)
            if (accumulated_size > max_memory_per_task) {
              break; // No more data from this input
            }

        }
        
      }
      
      if (accumulated_size > 2 * max_memory_per_task) {
        worker_task_ = NULL;
        SetError(au::str("Error: Memory footprint of %s for previous operation.", au::str(accumulated_size).c_str()));
        return au::SharedPointer<WorkerTask>(NULL);
      }
      
      // Reset chronometer
      last_task_cronometer_.Reset();
      
      // Add environment variable to identify this stream_operation_id
      worker_task_->environment().Set("system.stream_operation_id", stream_operation_id_);
      
      return worker_task_;
    }
    
    void StreamOperationRangeInfo::set_state(const std::string& state) {
      state_ = state;
    }
    
    std::string StreamOperationRangeInfo::str() const {
      std::ostringstream output;
      
      output << "StreamOperation " << stream_operation_id_;
      output << " " << stream_operation_name_;
      output << " " << range_.str();
      return output.str();
    }
    
    void StreamOperationRangeInfo::fill(samson::gpb::CollectionRecord *record, const Visualization& visualization) {
      // Common columns
      ::samson::add(record, "id", stream_operation_id_, "different");
      ::samson::add(record, "name", stream_operation_name_, "different");
      ::samson::add(record, "range", range_.str().c_str(), "different");
      
      if (visualization.get_flag("state")) {
        ::samson::add(record, "state", state_, "different");
        return;
      }
      
      if (visualization.get_flag("tasks")) {
        if (worker_task_ != NULL) {
          ::samson::add(record, "tasks", worker_task_->str_short(), "different");
        } else {
          ::samson::add(record, "tasks", "none", "different");
        }
        
        if (defrag_task_ != NULL) {
          ::samson::add(record, "tasks", defrag_task_->str_short() , "different");
        } else {
          ::samson::add(record, "tasks", "none", "different");
        }
        return;
      }
      
      // Default view
      
      ::samson::add(record, "inputs", state_input_queues_, "different");
      ::samson::add(record, "time", au::str_time(last_task_cronometer_.seconds()), "different");
      ::samson::add(record, "priority rank", priority_rank(), "f=uint64,different");
      
      if (worker_task_ != NULL) {
        ::samson::add(record, "tasks", worker_task_->str_short(), "different");
      } else {
        ::samson::add(record, "tasks", "none", "different");
      }
      
      if (defrag_task_ != NULL) {
        ::samson::add(record, "defrag", defrag_task_->str_short(), "different");
      } else {
        ::samson::add(record, "defrag", "none", "different");
      }
      
      ::samson::add(record, "state", state_, "different,left");
    }
    
    bool isStreamOperationValid(gpb::Data *data, const gpb::StreamOperation& stream_operation, au::ErrorManager *error) {
      
      std::string operation_name = stream_operation.operation();
      Operation *operation = au::Singleton<ModulesManager>::shared()->getOperation(operation_name);
      
      if (!operation) {
        error->set(au::str("Operation %s not found", operation_name.c_str()));
        return false;
      }
      
      // Check at least an input is defined
      if (stream_operation.inputs_size() == 0) {
        error->set("No inputs defined for this operation");
        return false;
      }
      
      // Check number of inputs and outputs
      if (operation->getNumInputs() != stream_operation.inputs_size()) {
        error->set(
                   au::str("Operation %s needs %d inputs and %d are provided", operation_name.c_str(),
                           operation->getNumInputs(), stream_operation.inputs_size()));
        return false;
      }
      if (operation->getNumOutputs() != stream_operation.outputs_size()) {
        error->set(
                   au::str("Operation %s needs %d output and %d are provided", operation_name.c_str(),
                           operation->getNumOutputs(), stream_operation.outputs_size()));
        return false;
      }
      
      // Check the format of all input queues
      for (int i = 0; i < operation->getNumInputs(); ++i) {
        std::string queue_name = stream_operation.inputs(i);
        gpb::Queue *queue = get_queue(data, queue_name);
        if (queue) { // If queue does not exist, there is no problem. I will be automatically created
          KVFormat format(queue->key_format(), queue->value_format());
          if (format != operation->inputFormats[i]) {
            error->set(
                       au::str("%d-th input for %s ( queue %s ) should be %s and it is %s ", i + 1, operation_name.c_str(),
                               queue_name.c_str(), operation->inputFormats[i].str().c_str(), format.str().c_str()));
            return false;
          }
        }
      }
      
      // Check the format of all output queues
      for (int i = 0; i < operation->getNumOutputs(); ++i) {
        std::string queue_name = stream_operation.outputs(i);
        gpb::Queue *queue = get_queue(data, queue_name);
        if (queue) { // If queue does not exist, there is no problem. I will be automatically created
          KVFormat format(queue->key_format(), queue->value_format());
          if (format != operation->outputFormats[i]) {
            error->set(
                       au::str("%d-th output for %s ( queue %s ) should be %s and it is %s ", i + 1,
                               operation_name.c_str(), queue_name.c_str(), operation->outputFormats[i].str().c_str(),
                               format.str().c_str()));
            return false;
          }
        }
      }
      
      // Additional checks for reduce operations....
      // TO BE COMPLETED
      
      return true;
    }
  }
} // End of namespace samson::stream
