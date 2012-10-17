#include "samson/stream/StreamOperationRangeInfo.h"      // Own interface

#include <string>

#include "engine/Engine.h"
#include "engine/ProcessManager.h"
#include "samson/common/gpb_operations.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"
#include "samson/module/ModulesManager.h"
#include "samson/stream/WorkerTask.h"
#include "samson/worker/SamsonWorker.h"
#include "samson/stream/StreamOperationGlobalInfo.h"

namespace samson {
namespace stream {
    StreamOperationRangeInfo::StreamOperationRangeInfo( StreamOperationGlobalInfo * stream_operation_global_info
                                                       , SamsonWorker *samson_worker
                                                       , size_t stream_operation_id
                                                       , const std::string& stream_operation_name
                                                       , const KVRange& range) {

  // Informaiton for this stream operation info
  stream_operation_id_ = stream_operation_id;
  stream_operation_name_ = stream_operation_name;
  range_ = range;

  // Keep a pointer to samson worker
  samson_worker_ = samson_worker;
      // Keep a reference to my parent global information
      stream_operation_global_info_ = stream_operation_global_info;

  // Default values
  priority_rank_ = 0;
      pending_size_ = 0;
  state_ = "No info";
  worker_task_ = NULL; // No task by default
}

StreamOperationRangeInfo::~StreamOperationRangeInfo() {
}

    /*
     Dynamic inputs
     ----------------------------
     map,parse,...     All inputs are dynamic (1 dynamic input)
     Batch operation:  All inputs are non dynamic ( all input required )
     Stream operation: All inputs are dynamic except last ( state ) input
     Reduce forward:   All inputs are dynamic except last ( auxiliar data-set ) input
     
     Inputs to count for pending size
     ----------------------------
     map,parse,...     Single input ( counts )
     Batch operation:  All inputs
     Stream operation: N-1 ( last one is state and it is not considered pending.size)
     */
    
    
    // Get number of inputs that can be taken in small blocks
    int GetNumberOfDynamicInputs( gpb::StreamOperation* stream_operation )
    {
      std::string operation_name = stream_operation->operation();
      Operation *operation = au::Singleton<ModulesManager>::shared()->getOperation(operation_name);
      
      if (!operation)
        LM_X(1, ("Internal error"));
      
      bool batch_operation = stream_operation->batch_operation();
      bool reduce_operation = (operation->getType() == Operation::reduce);
      
      if ( reduce_operation && batch_operation )
        return 0;
      
      if ( reduce_operation )
        return operation->getNumInputs() - 1;
      
      // No reduce... it is always 1
      return 1;
      
    }
    
    // Get the number of inputs to take into account for thrigerring tasks
    int GetNumberOfInputsForThrigering( gpb::StreamOperation* stream_operation)
    {
      std::string operation_name = stream_operation->operation();
      Operation *operation = au::Singleton<ModulesManager>::shared()->getOperation(operation_name);
      
      if (!operation)
        LM_X(1, ("Internal error"));
      
      bool batch_operation = stream_operation->batch_operation();
      bool reduce_operation = (operation->getType() == Operation::reduce);
      
      if ( reduce_operation && batch_operation )
        return operation->getNumInputs();
      
      if ( reduce_operation )
        return operation->getNumInputs() - 1;
      
      // No reduce... it is always 1
      return 1;
    }
    
    
    void StreamOperationRangeInfo::Review( gpb::Data *data )
    {
      
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
        worker_task_ = NULL; // Cancel task if any
        return;
      }

      // Check if the operation is valid
      au::ErrorManager error;
      if (!isStreamOperationValid(data, *stream_operation, &error)) {
        SetError( au::str("Error validating stream operation: %s" , error.GetMessage().c_str() ) );
        worker_task_ = NULL; // Cancel task if any
        return;
      }
      

      std::string operation_name = stream_operation->operation();
      Operation *operation = au::Singleton<ModulesManager>::shared()->getOperation(operation_name);
      if (!operation)
        LM_X(1, ("Internal error"));
      
      bool reduce_operation = (operation->getType() == Operation::reduce);
      bool batch_operation = stream_operation->batch_operation();
      
      // Check info from input queues
      
      //int num_dynamic_input = GetNumberOfDynamicInputs( stream_operation );
      //int num_thrigger_input = GetNumberOfInputsForThrigering( stream_operation );
      
      // Scan all inputs
      std::vector<size_t> input_sizes;
      for (int i = 0; i < stream_operation->inputs_size(); i++)
      {
        std::string input_queue = stream_operation->inputs(i);
        std::vector<KVRange> ranges;
        ranges.push_back(range_);
        gpb::DataInfoForRanges info = gpb::get_data_info_for_ranges(data, input_queue, ranges);
        
        state_input_queues_ += au::str("[%s %s %s]"
                                       , input_queue.c_str()
                                       , au::str(info.data_kvs_in_ranges,"kvs").c_str()
                                       , au::str(info.data_size_in_ranges,"B").c_str()
                                       );
        
        input_sizes.push_back( info.data_size_in_ranges );
      }
      
      // Reset error in timeout 60
      if ((error_.IsActivated() && (cronometer_error_.seconds() > 60))) {
        error_.Reset();
      }
      if (error_.IsActivated()) {
        // Wait until previous error is finally canceled
        state_ = au::str("Error [%s]: %s" , cronometer_error_.str().c_str() , error_.GetMessage().c_str() );
        short_state_ = "[E]";
        return;
      }
      
      // If operation is paused, do not consider then...
      if (stream_operation->paused()) {
        state_ = "Operation paused";
        short_state_ ="[P]";
        return;
      }
      
      if (worker_task_ != NULL)
      {
        ReviewCurrentTask();   // Review if this task finished...
        state_ = "Running task...";
        short_state_ ="[*]";
        return;
      }

      if (defrag_task_ != NULL)
      {
        ReviewCurrentTask();   // Review if this defrag task finished...
        state_ = "Running defrag task...";
        short_state_ ="[D]";
        return;
      }
      
      
      // Decide if the range has to be divided ( only in reduce operations )
      if( reduce_operation )
      {
        // Maximum memory per task
        size_t max_memory_per_task = GetMaxMemoryPerTask();
        
        if( batch_operation )
        {
          // sum all inputs > max per task
          size_t total = 0;
          for ( size_t i = 0 ; i < input_sizes.size() ; i++ )
            total += input_sizes[i];
          
          if( total > max_memory_per_task )
          {
            // It is not necessary to write state_ since review will be called again
            range_division_necessary_ = true;
            return;
          }
          
        }
        else
        {
          // In stream operations ( or reduce forward ), only state is responsible for breaking a range
          if( input_sizes[ input_sizes.size()-1 ] > ( max_memory_per_task/2) )
          {
            // It is not necessary to write state_ since review will be called again
            range_division_necessary_ = true;
            return;
          }
          
        }
      }

      // Study if a defrag operation is required or we should wait for others
      for (int i = 0; i < stream_operation->inputs_size(); i++ )
      {
        std::string input_queue = stream_operation->inputs(i);
        gpb::Queue* queue = gpb::get_queue(data, input_queue);
        if( !queue )
          continue; // No queue, no problem...

        bool defrag_task = false;
        for (int b = 0; b < queue->blocks_size(); b++) {
          
          const gpb::Block& block = queue->blocks(b);
          size_t block_id = block.block_id();
          KVRange range = block.range();

          if( !range.IsOverlapped(range_) )
            continue; // we are not interested in this block

          
          if ( range.hg_begin < range_.hg_begin )
          {
            state_ = au::str( "Waiting for defrag in queue %s (block %lu)" , input_queue.c_str() , block_id );
            short_state_ = "[W]";
            return;
          }
          if( range.hg_end > range_.hg_end )
          {
            defrag_task = true;
            break;
          }
          
        }
        
        if( defrag_task )
        {
          
          // Schedule a defrag task for this queue and this range
          std::vector<KVRange> defrag_ranges = stream_operation_global_info_->GetDefragKVRanges();
          defrag_task_ = new DefragTask(samson_worker_
                                        , input_queue,samson_worker_->task_manager()->getNewId()
                                        , defrag_ranges );
          
          size_t max_memory_per_task = GetMaxMemoryPerTask();
          size_t accumulated_size = 0;
          
          for (int b = 0; b < queue->blocks_size(); b++) {
            
            const gpb::Block& block = queue->blocks(b);
            size_t block_id = block.block_id();
            KVRange range = block.range();
            KVInfo info( block.size() , block.kvs() );
            
            if( !range.IsOverlapped(range_) )
              continue; // we are not interested in this block

            
            if( range.hg_end > range_.hg_end )
            {
              
              // Memory limit for this tasks
              if ( accumulated_size > 0 )
                if(  accumulated_size + block.size() > max_memory_per_task )
                  break;
              accumulated_size += block.size();
              
              BlockPointer real_block = BlockManager::shared()->GetBlock(block.block_id());
              
              if( real_block == NULL )
              {
                SetError(au::str("Block %lu not found while trying to defrag" , block_id ));
                defrag_task_ = NULL;
                return;
              }
              
              // Add this element to the defrag task
              defrag_task_->AddInput(0, real_block, range, info );

            }
            
          }
          
         
          state_ ="Running defrag";
          short_state_ = "[D]";

          // Schedule this tasks
          samson_worker_->task_manager()->Add(defrag_task_.dynamic_pointer_cast<WorkerTaskBase> ());
          
          return;
          
        }
        
      }
      
      
      // Compute pending size based on input size
      if( reduce_operation && !batch_operation )
      {
        // We do not consider pending size the state or the joint-set
        for (int i = 0; i < ( stream_operation->inputs_size() -1 ); i++ )
          pending_size_ += input_sizes[i];
      }
      else
      {
        // all inputs are pending size
        for (int i = 0; i < stream_operation->inputs_size(); i++ )
          pending_size_ += input_sizes[i];
      }
      
      // Eval pending size to scehdule new tasks
      
      if ( pending_size_ > 0 )
      {
        // Compute priotiy based on pending size and time
        size_t time = 1 + last_task_cronometer_.seconds();
        priority_rank_ = pending_size_ * time;
        
        state_ = "Ready to schedule a new task";
        short_state_ = "[R]";
      }
      else
      {
        state_ = "No data to be processed";
        short_state_ = "[N]";
        last_task_cronometer_.Reset();
        // Reset the cronometer if not data sice otherwise it will count a lot of time when some data appears
      }
      
      
    }
    
    void StreamOperationRangeInfo::ReviewCurrentTask() {
      
      LM_W(("We do not check if all generated blocks are distributed"));

  if (worker_task_ != NULL) {

    // Process possibly generated output buffers
    worker_task_->processOutputBuffers();

    // If we are running a task, let see if it is finished
        if (worker_task_->IsWorkerTaskFinished()) {

      // Process outputs generated by this task
      worker_task_->processOutputBuffers();

          if (worker_task_->error().IsActivated())
          {
            // Tansfer the error message ( this will block this for 60 seconds )
            SetError( worker_task_->error().GetMessage() );
            worker_task_ = NULL;
        return;
      } else {

        // Commit changes and release task
        std::string commit_command = worker_task_->commit_command();
        std::string caller = au::str("task %lu // %s", worker_task_->worker_task_id(), str().c_str());
        au::ErrorManager error;
        samson_worker_->data_model()->Commit(caller, commit_command, &error);
            if (error.IsActivated())
              SetError( error.GetMessage() );
            
            worker_task_ = NULL;        // Release our copy of this task
            return;
          }
        }
        }

      if (defrag_task_ != NULL) {

        // If we are running a task, let see if it is finished
        if (defrag_task_->IsWorkerTaskFinished() ) {

          if (defrag_task_->error().IsActivated())
          {
            // Tansfer the error message ( this will block this for 60 seconds )
            SetError( defrag_task_->error().GetMessage() );
            defrag_task_ = NULL;
            return;
          } else {
            
            
            // Commit changes and release task
            std::string commit_command = defrag_task_->commit_command();
            std::string caller = au::str("defrag task %lu // %s", defrag_task_->worker_task_id(), str().c_str());
            au::ErrorManager error;
            samson_worker_->data_model()->Commit(caller, commit_command, &error);
            if (error.IsActivated())
              SetError( error.GetMessage() );
            defrag_task_ = NULL;        // Release our copy of this task
            return;
      }
    }
  }
}

size_t StreamOperationRangeInfo::priority_rank() {
  return priority_rank_;
}

au::SharedPointer<WorkerTask> StreamOperationRangeInfo::schedule_new_task(size_t task_id, gpb::Data *data) {

      // Memory limits
      size_t max_memory_per_task = GetMaxMemoryPerTask();

  gpb::StreamOperation *stream_operation = gpb::getStreamOperation(data, stream_operation_id_);

  std::string operation_name = stream_operation->operation();
  Operation *operation = au::Singleton<ModulesManager>::shared()->getOperation(operation_name);

      // Everything is checked by Rreview command, so no error is accepted
      if( priority_rank_ == 0 )
        LM_X(1, ("Internal error"));
      if (!stream_operation)
        LM_X(1, ("Internal error"));
      if (error_.IsActivated() )
        LM_X(1, ("Internal error"));
      au::ErrorManager error;
      if (!isStreamOperationValid(data, *stream_operation, &error))
        LM_X(1, ("Internal error"));
      if (stream_operation->paused())
        LM_X(1, ("Internal error"));
      if (!operation)
        LM_X(1, ("Internal error"));

      // Get the relevant number of input for all aspects
      int num_dynamic_input = GetNumberOfDynamicInputs( stream_operation );
      //int num_thrigger_input = GetNumberOfInputsForThrigering( stream_operation );

  // Create candidate task ( if id is provided )
    worker_task_ = new WorkerTask(samson_worker_, task_id, *stream_operation, operation, range_);

      // Accumulated memory used for this task ( we will limit taken blocks with this element )
  size_t accumulated_size = 0; // Accumulated size required to be in memory for this operation
      au::Uint64Set block_ids;     // Set of Block ids required so far

      // Scan all inputs ( in reverse order to include first state.... )
      for (int i = stream_operation->inputs_size()-1; i >=0 ; i--) {
    std::string input_queue = stream_operation->inputs(i);
    gpb::Queue *queue = ::samson::gpb::get_queue(data, input_queue);

    if (!queue) {
      continue; // No data at this input
    }

    for (int b = 0; b < queue->blocks_size(); b++) {
      const gpb::Block& block = queue->blocks(b);
      size_t block_id = block.block_id();
          KVRange range = block.range();
          KVInfo info( block.size() , block.kvs());
          // If this block is not in our range of interest, just skip it
          if( !range.IsOverlapped(range_))
            continue;

          if( !range_.includes( range ) )
            LM_X(1, ("Internal error. Block %lu (%s) in queue %s shoudl be contained in range %s"
                     , block_id
                     , range.str().c_str()
                     , input_queue.c_str()
                     , range_.str().c_str() ));

          BlockPointer real_block = BlockManager::shared()->GetBlock(block.block_id());
          if (real_block == NULL) {
            worker_task_ = NULL;
            SetError( au::str("Block %lu not found", block.block_id()));
            return au::SharedPointer<WorkerTask>(NULL);
          }


          // Accumulate size if the block was not considered before
          if (!block_ids.contains(block_id)) {
            accumulated_size += real_block->getSize(); // Accumulate size of the block
            block_ids.insert(block_id);
          }
          // Add input to the task
          worker_task_->AddInput(i, real_block, range_ , info );
      }

      // Stop if we need to much memory for this task
        if (i < num_dynamic_input )
          if (accumulated_size > max_memory_per_task ) {
          break; // No more data from this input
        }
  }

      if (accumulated_size > 2*max_memory_per_task )
      {
        worker_task_ = NULL;
        SetError( au::str("Error: Memory footprint of %s for previous operation." , au::str(accumulated_size).c_str()));
    return au::SharedPointer<WorkerTask>(NULL);
  }

      // Reset cronometer
      last_task_cronometer_.Reset();

      // Add environment variable to identify this stream_operation_id
      worker_task_->environment().Set("system.stream_operation_id", stream_operation_id_);


  return worker_task_;
}

std::string StreamOperationRangeInfo::state() {
  return state_;
}

void StreamOperationRangeInfo::set_state(const std::string& state) {
  state_ = state;
}


std::string StreamOperationRangeInfo::str() {
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
      ::samson::add(record, "tasks",
                    au::str("%lu: %s", worker_task_->worker_task_id(), worker_task_->task_state().c_str()), "different");
    } else {
      ::samson::add(record, "tasks", "none", "different");
    }
    return;
  }

  // Default view

      ::samson::add(record, "inputs", state_input_queues_ , "different");
  ::samson::add(record, "time", au::str_time(last_task_cronometer_.seconds()), "different");
  ::samson::add(record, "priority rank", priority_rank(), "f=uint64,different");

  if (worker_task_ != NULL) {
    ::samson::add(record, "tasks", worker_task_->worker_task_id(), "different");
  } else {
    ::samson::add(record, "tasks", "none", "different");
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
  for (int i = 0; i < operation->getNumInputs(); i++) {
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
  for (int i = 0; i < operation->getNumOutputs(); i++) {
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

au::SharedPointer<WorkerTask> StreamOperationRangeInfo::worker_task() {
  return worker_task_;
}

}
} // End of namespace samson::stream
