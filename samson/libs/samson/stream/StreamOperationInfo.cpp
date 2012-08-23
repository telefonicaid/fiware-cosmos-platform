

#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/gpb_operations.h"
#include "samson/module/ModulesManager.h"
#include "samson/worker/SamsonWorker.h"

#include "WorkerTask.h"

#include "samson/stream/StreamOperationInfo.h"      // Own interface

namespace samson {
namespace stream {
StreamOperationInfo::StreamOperationInfo(SamsonWorker *samson_worker
                                         , size_t stream_operation_id
                                         , const KVRange& range
                                         , const gpb::StreamOperation& stream_operation) {
  // Keep identifiers & operation range
  stream_operation_id_ = stream_operation_id;
  range_ = range;

  // Keep a pointer to samson worker
  samson_worker_ = samson_worker;

  // Duplicate stream operation information
  stream_operation_ = new gpb::StreamOperation();
  stream_operation_->CopyFrom(stream_operation);

  // Default values
  priority_rank_ = 0;
  state_ = "No info";
  data_ = NULL;

  worker_task_ = NULL;                     // No task by default
}

StreamOperationInfo::~StreamOperationInfo() {
}

void StreamOperationInfo::review(gpb::Data *data) {
  // Init pending size and priority to be recomputed
  pending_size_ = 0;
  priority_rank_ = 0;

  // Keep data model
  data_ = data;

  // If we are running a task, let see if it is finished
  if (worker_task_ != NULL) {
    if (worker_task_->is_finished()) {
      // If there is an error, reset
      const au::ErrorManager& worker_task_error = worker_task_->error();
      if (worker_task_error.IsActivated()) {
        // Set the error
        std::string error_message = worker_task_error.GetMessage();
        ResetWithError(error_message);
      } else {
        // Commit changes and release task
        std::string commit_command = worker_task_->commit_command();

        LM_W(("Commit command for task %lu: %s", worker_task_->get_id(), commit_command.c_str()));

        au::ErrorManager error;
        std::string caller = au::str("task %lu // %s"
                                     , worker_task_->get_id()
                                     , str().c_str());

        samson_worker_->data_model()->Commit(caller, commit_command, &error);
        if (error.IsActivated()) {
          ResetWithError(au::str("Error commiting tasks: %s", error.GetMessage().c_str()));
          return;
        }

        // Release our copy of this task
        worker_task_ = NULL;
      }

      // We have to update data and review in the next iteration
      return;
    }                     // Compute aproximate pending size ( always independently of the state to display information in the list )
  }
  std::string input_queue = stream_operation_->inputs(0);
  gpb::Queue *queue = ::samson::gpb::get_queue(data, input_queue);
  if (!queue) {
    state_ = au::str("Queue %s not present in data model", input_queue.c_str());
    return;
  }

  pending_size_ = 0;
  for (int i = 0; i < queue->blocks_size(); i++) {
    const gpb::Block& block = queue->blocks(i);
    KVRanges ranges = block.ranges();                     // Implicit conversion

    double overlap_factor = ranges.GetOverlapFactor(range_);
    if (overlap_factor > 1) {
      LM_X(1, ("Internal error"));
    }
    pending_size_ += ( overlap_factor * block.size());
  }

  // Check if are already running a tasks ( no more than one task per range )
  if (worker_task_ != NULL) {
    state_ = au::str("Worker task %lu is currently scheduled (state = %s) "
                     , worker_task_->get_id()
                     , worker_task_->task_state().c_str());
    priority_rank_ = 0;
    return;
  }

  // Check if the operatio is valid
  au::ErrorManager error;
  if (!isValid(&error)) {
    state_ = au::str("E[%s]: ", error.GetMessage().c_str());
    priority_rank_ = 0;
    return;
  }

  // If operation is paused, do not consider then...
  if (stream_operation_->paused()) {
    state_ = "Operation paused";
    priority_rank_ = 0;
    return;
  }

  if (pending_size_ == 0) {
    state_ = au::str("Queue %s has no data to be processed in the range %s"
                     , input_queue.c_str()
                     , range_.str().c_str());
    priority_rank_ = 0;
    return;
  }

  state_ = "ready to schedule a new task";

  // Compute priority rank based on time and size
  priority_rank_ = pending_size_ * last_task_cronometer_.seconds();
}

size_t StreamOperationInfo::priority_rank() {
  return priority_rank_;
}

au::SharedPointer<WorkerTask> StreamOperationInfo::schedule_new_task(size_t task_id) {
  if (worker_task_ != NULL) {
    LM_X(1, ("Internal error"));
  }
  Operation *operation = ModulesManager::shared()->getOperation(
    stream_operation_->operation());

  if (!operation) {
    LM_X(1, ("Internal error: Operation not found"));                     // Create a new task
  }
  worker_task_ = new WorkerTask(samson_worker_, task_id, *stream_operation_, operation, range_);

  // Recover main input queue
  std::string input_queue = stream_operation_->inputs(0);
  gpb::Queue *queue = ::samson::gpb::get_queue(data_, input_queue);

  if (!queue) {
    LM_X(1, ("Internal error: Queue not found"));                     // This cannot happen since I would never be called
  }
  // Compute the limit block to start packaging blocks in the task
  size_t accumulated_size = 0;
  au::Uint64Set block_ids;

  for (int i = 0; i < queue->blocks_size(); i++) {
    const gpb::Block& block = queue->blocks(i);

    size_t block_id = block.block_id();

    const gpb::KVRanges ranges = block.ranges();
    for (int b = 0; b < ranges.range_size(); b++) {
      KVRange range = ranges.range(b);                     // Implicit conversion
      KVRange intersection = range.Intersection(range_);

      if (intersection.size() > 0) {
        // This block should be considered
        BlockPointer real_block = BlockManager::shared()->getBlock(block.block_id());
        if (real_block == NULL) {
          LM_X(1,
               ("Internal error: Block %lu not found.",
                block.block_id()));
        }
        worker_task_->add_input(0, real_block, intersection, KVInfo(0,
                                                                    0));

        // Accumulate size if the block was not considered before
        if (!block_ids.contains(block_id)) {
          accumulated_size += real_block->getKVInfo().size;
          block_ids.insert(block_id);
        }
      }
    }

    // Stop if we need to much memory for this task
    if (accumulated_size > 120000000) {
      break;                     // No more accumulation for this operation
    }
  }

  // Reset cronometer
  last_task_cronometer_.Reset();

  // Add environment varialble to identify this stream_operation_id
  worker_task_->environment().Set("system.stream_operation_id", stream_operation_id_);

  LM_W(("Scheduling a new task %lu for range %s", task_id, range_.str().c_str()));

  // Return the newly generated worker task to be really shcedulled in the worker task manager
  return worker_task_;
}

std::string StreamOperationInfo::state() {
  return state_;
}

// Reset all pending worker_tasks
void StreamOperationInfo::Reset() {
  // Release current working task ( if any )
  worker_task_ = NULL;
}

void StreamOperationInfo::ResetWithError(const std::string& error_message) {
  error_.set(error_message);
  cronometer_error_.Reset();
  Reset();
}

std::string StreamOperationInfo::str() {
  std::ostringstream output;

  output << "StreamOperation " << stream_operation_id_;
  output << " " << stream_operation_->name();
  output << " " << range_.str();
  return output.str();
}

void StreamOperationInfo::fill(samson::gpb::CollectionRecord *record,
                               const Visualization& visualization) {
  // Common columns
  ::samson::add(record, "name", stream_operation_->name(), "different");
  ::samson::add(record, "id", stream_operation_id_, "different");
  ::samson::add(record, "range", range_.str().c_str(), "different");

  if (visualization.get_flag("state")) {
    ::samson::add(record, "state", state_, "different");
    return;
  }

  if (visualization.get_flag("tasks")) {
    if (worker_task_ != NULL) {
      ::samson::add(record, "tasks", au::str("%lu: %s", worker_task_->get_id(),
                                             worker_task_->task_state().c_str()), "different");
    } else {
      ::samson::add(record, "tasks", "none", "different");
    } return;
  }

  // Default view

  ::samson::add(record, "pending_size", pending_size_, "sum,f=uint64");
  ::samson::add(record, "priority rank", priority_rank(), "different");

  if (worker_task_ != NULL) {
    ::samson::add(record, "tasks", worker_task_->get_id(), "different");
  } else {
    ::samson::add(record, "tasks", "none", "different");
  } ::samson::add(record, "state", state_,
                  "different");
}

bool StreamOperationInfo::isValid(au::ErrorManager *error) {
  if (error_.IsActivated()) {
    if (cronometer_error_.seconds() > 60) {
      error_.Reset();                     // Remove errors
    } else {
      // Transfer the error
      error->set(au::str("Error ( time %s ) %s", au::S(cronometer_error_).str().c_str(),
                         error_.GetMessage().c_str()));
      return false;
    }
  }

  std::string operation_name = stream_operation_->operation();
  Operation *operation = ModulesManager::shared()->getOperation(operation_name);

  if (!operation) {
    error->set(au::str("Operation %s not found", operation_name.c_str()));
    return false;
  }

  // Check at least an input is defined
  if (stream_operation_->inputs_size() == 0) {
    error->set("No inputs defined for this operation");
    return false;
  }

  // Check number of inputs and outputs
  if (operation->getNumInputs() != stream_operation_->inputs_size()) {
    error->set(au::str("Operation %s needs %d inputs and %d are provided"
                       , operation_name.c_str()
                       , operation->getNumInputs()
                       , stream_operation_->inputs_size()));
    return false;
  }
  if (operation->getNumOutputs() != stream_operation_->outputs_size()) {
    error->set(au::str("Operation %s needs %d output and %d are provided"
                       , operation_name.c_str()
                       , operation->getNumOutputs()
                       , stream_operation_->outputs_size()));
    return false;
  }

  // Check the format of all input queues
  for (int i = 0; i < operation->getNumInputs(); i++) {
    std::string queue_name = stream_operation_->inputs(i);
    gpb::Queue *queue = get_queue(data_, queue_name);
    if (queue) {                     // If queue does not exist, there is no problem. I will be automatically created
      KVFormat format(queue->key_format(), queue->value_format());
      if (format != operation->inputFormats[i]) {
        error->set(au::str("%d-th input for %s ( queue %s ) should be %s and it is %s "
                           , i + 1
                           , operation_name.c_str()
                           , queue_name.c_str()
                           , operation->inputFormats[i].str().c_str()
                           , format.str().c_str()));
        return false;
      }
    }
  }

  // Check the format of all output queues
  for (int i = 0; i < operation->getNumOutputs(); i++) {
    std::string queue_name = stream_operation_->outputs(i);
    gpb::Queue *queue = get_queue(data_, queue_name);
    if (queue) {                     // If queue does not exist, there is no problem. I will be automatically created
      KVFormat format(queue->key_format(), queue->value_format());
      if (format != operation->outputFormats[i]) {
        error->set(au::str("%d-th output for %s ( queue %s ) should be %s and it is %s "
                           , i + 1
                           , operation_name.c_str()
                           , queue_name.c_str()
                           , operation->outputFormats[i].str().c_str()
                           , format.str().c_str()));
        return false;
      }
    }
  }


  // Additional checks for reduce operations....
  // TO BE COMPLETED


  return true;
}

/*
 * size_t next_pow_2( size_t value )
 * {
 * if( value < 2)
 * return 1;
 *
 * int p = 1;
 * while ( true )
 * {
 * if( value == pow(2.0 , p))
 * return pow( 2.0 , p);
 *
 * if( value < pow( 2.0 , p ) )
 * return pow( 2.0 , p );
 * p++;
 * }
 *
 * LM_X(1,("Internal error"));
 * return 1;
 *
 * }
 *
 * #pragma StreamOperation
 *
 * StreamOperation::StreamOperation(  ) : BlockListContainer("StreamOperation")
 * {
 * streamManager = NULL;
 *
 * // Default environment parameters
 * environment.set("max_latency" , "60" );
 * environment.set("priority", "1" );
 *
 * // Init all counters
 * in_num_operations = 0;
 * out_num_operations = 0;
 * out_core_seconds = 0;
 *
 * latency_time = 0;
 *
 * }
 *
 * void StreamOperation::add_latency_time( double t )
 * {
 *
 * if( t < 0 )
 * return;
 *
 * if( latency_time == 0 )
 * latency_time = t;
 * else
 * latency_time = 0.9*latency_time + 0.1*t;
 * }
 *
 * void StreamOperation::add_block_to_cronometers( Block* block )
 * {
 *
 * if( block_cronometers.findInMap(block) )
 * return; // Blcok already included.... ( common in reduce operations )
 *
 * block_cronometers.insertInMap(block, new au::Cronometer() );
 * }
 *
 * void StreamOperation::remove_block_to_cronometers( Block* block )
 * {
 *
 * if( isBlockIncluded(block) )
 * return; // It is still included as an input somewhere...
 *
 * au::Cronometer* cronometer = block_cronometers.extractFromMap(block);
 * if( !cronometer )
 * return; // We do not have a temporal reference for this block
 *
 * add_latency_time( cronometer->seconds() );
 *
 * delete cronometer;
 * }
 *
 * void StreamOperation::add_block_to_cronometers( BlockList* block_list )
 * {
 * au::list< Block >::iterator it_blocks;
 * for( it_blocks = block_list->blocks.begin() ; it_blocks != block_list->blocks.end() ; it_blocks++ )
 * add_block_to_cronometers(*it_blocks);
 * }
 *
 * void StreamOperation::remove_block_to_cronometers( BlockList* block_list )
 * {
 * au::list< Block >::iterator it_blocks;
 * for( it_blocks = block_list->blocks.begin() ; it_blocks != block_list->blocks.end() ; it_blocks++ )
 * remove_block_to_cronometers(*it_blocks);
 * }
 *
 * void StreamOperation::add_block_to_cronometers( BlockListContainer * block_list )
 * {
 * std::vector<std::string> block_lists = block_list->get_block_list_names();
 * for( size_t i = 0 ; i < block_lists.size() ; i++)
 * add_block_to_cronometers( block_list->getBlockList(block_lists[i]) );
 * }
 *
 * void StreamOperation::remove_block_to_cronometers( BlockListContainer* block_list )
 * {
 * std::vector<std::string> block_lists = block_list->get_block_list_names();
 * for( size_t i = 0 ; i < block_lists.size() ; i++)
 * remove_block_to_cronometers( block_list->getBlockList(block_lists[i]) );
 * }
 *
 * StreamOperation* StreamOperation::newStreamOperation( StreamManager *streamManager
 * ,  std::string command
 * , au::ErrorManager& error )
 * {
 * // Expected format add_stream_operation name operation input_queues... output_queues ... parameters
 *
 * au::CommandLine cmd;
 *
 * // Forward flag to indicate that this is a reduce forward operation ( no update if state )
 * cmd.SetFlagBoolean("forward");
 *
 * // Number of divisions in state operations
 * cmd.SetFlagInt("divisions", au::Singleton<SamsonSetup>::shared()->getInt("general.num_processess") );
 *
 * // Use third party software only for state with new input
 * cmd.SetFlagBoolean("update_only");
 *
 * // Prefix used to change names of queues and operations
 * cmd.SetFlagString("prefix", "");
 * cmd.Parse( command );
 *
 * std::string prefix = cmd.GetFlagString("prefix");
 *
 * if( cmd.get_num_arguments() < 3 )
 * {
 * error.set("Usage: add_stream_operation name operation input1 input2 ... outputs1 outputs2 output3");
 * return NULL;
 * }
 *
 * std::string name            = prefix + cmd.get_argument( 1 );
 * std::string operation       = cmd.get_argument( 2 );
 *
 * // If the operation exist, it will be replaced by this one, so no check if the stream operation is here
 *
 * // Check operation
 * Operation* op = ModulesManager::shared()->getOperation(operation);
 *
 * if( !op )
 * {
 * error.set( "Unsupported operation " + operation );
 * return NULL;
 * }
 *
 * // Check the number of input / outputs
 * if( cmd.get_num_arguments() < ( 3 + op->getNumInputs() +  op->getNumOutputs() ) )
 * {
 * error.set( au::str("Not enough parameters for operation %s. It has %d inputs and %d outputs" , operation.c_str() , (int) op->getNumInputs() ,  (int) op->getNumOutputs() ) );
 * return NULL;
 *
 * }
 *
 * au::Environment operation_environment;
 * for ( size_t i = ( 3 + ( op->getNumInputs() + op->getNumOutputs() )) ; i < (size_t)cmd.get_num_arguments() ; i++ )
 * {
 * std::string full_name = cmd.get_argument(i);
 * std::string prefix = "env:";
 * if( full_name.substr(0,prefix.size()) == prefix  )
 * {
 * std::string name = full_name.substr( prefix.size() );
 * if( i <= (size_t) (cmd.get_num_arguments()-2 ) )
 * {
 * // Set property...
 * std::string value = cmd.get_argument(i+1);
 * operation_environment.set(name , value );
 * }
 * }
 * }
 *
 *
 * StreamOperation *stream_operation = NULL;
 *
 * switch ( op->getType() ) {
 *
 * case Operation::map:
 * case Operation::parser:
 * case Operation::parserOut:
 *
 * stream_operation = new StreamOperationForward();
 * break;
 *
 * case Operation::reduce:
 * {
 *
 * if( op->getNumInputs() != 2 )
 * {
 * error.set( au::str("Only reduce operations with 2 inputs are supported at the moment. ( In the future, reducers with 3 or more inputs will be supported.") );
 * return NULL;
 * }
 *
 * if( cmd.GetFlagBool("forward") )
 * stream_operation = new StreamOperationForwardReduce();
 * else
 * stream_operation = new StreamOperationUpdateState( cmd.GetFlagInt("divisions")
 * , cmd.GetFlagBool("update_only")  );
 *
 * }
 * break;
 *
 * case Operation::script:
 * {
 * error.set( "Script operations cannot be used to process stream queues. Only parsers, maps and special reducers" );
 * return NULL;
 * }
 * break;
 *
 * default:
 * {
 * error.set( "Operation type is currently not supported... coming soon!" );
 * return NULL;
 *
 * }
 * break;
 * }
 *
 * // Copy environment properties
 * stream_operation->environment.copyFrom( &operation_environment );
 *
 * // Common things for all StreamOperations
 * stream_operation->streamManager = streamManager;
 * stream_operation->command =  command;
 * stream_operation->name = name;
 * stream_operation->operation = operation;
 *
 * // Get the inputs and output queues
 * int num_inputs  = op->getNumInputs();
 * int num_outputs = op->getNumOutputs();
 *
 * for (int i = 0 ; i < num_inputs ; i++ )
 * {
 * std::string queue_name = prefix + cmd.get_argument( 3 + i );
 * stream_operation->input_queues.push_back( queue_name );
 * }
 *
 * for (int i = 0 ; i < num_outputs ; i++ )
 * {
 * std::string original_queue_name = cmd.get_argument( 3 + num_inputs + i );
 * std::string queue_name = prefix + original_queue_name;
 *
 * if( original_queue_name == "null" )
 * queue_name = "null";
 *
 * stream_operation->output_queues.push_back( queue_name );
 * }
 *
 * if( stream_operation->isValid() )
 * return stream_operation;
 *
 * delete stream_operation;
 *
 * error.set("Not valid operation");
 * return NULL;
 * }
 *
 * void StreamOperation::setPaused( bool _paused )
 * {
 * if( _paused )
 * environment.set("paused", "yes" );
 * else
 * environment.set("paused", "no" );
 * }
 *
 * bool StreamOperation::isPaused()
 * {
 * return  (environment.get("paused","no") == "yes" );
 * }
 *
 * int StreamOperation::getPriority()
 * {
 * int p = environment.getInt( "priority" , -1 );
 *
 * if( p <= 0)
 * return 1;
 *
 * return p;
 * }
 *
 * void StreamOperation::getInfo( std::ostringstream &output )
 * {
 *
 * au::xml_open(output, "stream_operation");
 *
 * au::xml_simple(output, "name", name);
 * au::xml_simple(output, "operation", operation);
 *
 * au::xml_open(output, "inputs");
 * for ( size_t i = 0 ; i < input_queues.size() ; i++)
 * output << input_queues[i] << " ";
 * au::xml_close(output, "inputs");
 *
 * au::xml_open(output, "outputs");
 * for ( size_t i = 0 ; i < output_queues.size() ; i++)
 * output << output_queues[i] << " ";
 * au::xml_close(output, "outputs");
 *
 *
 * au::xml_simple(output, "properties", environment.getEnvironmentDescription() );
 *
 *
 * au::xml_simple(output,"last_review" , last_review + " " + getStatus() );
 *
 * // Information about input
 * BlockInfo input_block_info = getUniqueBlockInfo();
 * au::xml_single_element(output, "input", &input_block_info);
 *
 * // Current running operations
 * au::xml_simple(output, "running_tasks", running_tasks.size() );
 *
 * // Information about history
 * au::xml_simple(output, "history_block_info", history_block_info.str() );
 *
 * au::xml_simple( output , "input_str" , input_block_info.strShort() );
 *
 * au::xml_close(output, "stream_operation");
 * }
 *
 *
 *
 * void StreamOperation::add( WorkerTask* task )
 * {
 * // Take some statistics about this operation
 * input_rate.Push( task->getBlockList("input_0")->getBlockInfo().info );
 * in_num_operations++;
 *
 * // Set the environment property to make sure, it is removed when finished
 * task->environment.set("system.stream_operation" , name );
 *
 * // Unique name for all of them to be able to cancel
 * task->environment.set("worker_command_id" , au::str("stream_%s" , name.c_str() ) );
 *
 * // Add the task to the list of running task
 * running_tasks.insert( task );
 *
 * std::string message = au::str("[ %s ] [ task %lu ] Input data %s" ,
 * name.c_str() ,
 * task->getId(),
 * task->getBlockListContainerDataDescription().c_str()
 * );
 * streamManager->worker->sendTrace( "message" , "stream" , message );
 *
 * // Schedule tmp task into WorkerTaskManager
 * streamManager->WorkerTaskManager.add( task );
 *
 * }
 *
 * void StreamOperation::remove( WorkerTask* task )
 * {
 * // Stop all the cronometers for included blocks
 * remove_block_to_cronometers(task);
 *
 * // Remove this task from the list of running tasks
 * running_tasks.erase( task );
 *
 * int core_seconds = std::max( 1 , task->cronometer.getSeconds() );
 * environment.appendInt("system.core_seconds" , core_seconds );
 *
 * // History counter about operations and core-seconds
 * out_num_operations++;
 * out_core_seconds += core_seconds;
 *
 * // Count output data ( still not here becase data is districted on the go... )
 * LM_TODO(("Take statistics about output data when finish stream operation"));
 *
 *
 *
 * // Inform about this operation
 * {
 * std::string  message = au::str("[ %s ] [ task %lu ] Finishing after %s ( Defined %s ago )" ,
 * name.c_str() ,
 * task->getId() ,
 * au::str_time( task->cronometer.getSeconds() ).c_str(),
 * au::str_time( task->creation_cronometer.seconds() ).c_str()
 * );
 * streamManager->worker->sendTrace( "message" , "stream" , message );
 * }
 *
 * {
 * std::string message;
 * if( task->error.IsActivated() )
 * message = au::str("[ %s ] [ task %lu ] Finished with error %s" ,
 * name.c_str() ,
 * task->getId() ,
 * task->error.GetMessage().c_str()
 * );
 * else
 * message = au::str("[ %s ] [ task %lu ] Finished correctly" ,
 * name.c_str() ,
 * task->getId()
 * );
 * streamManager->worker->sendTrace( "message" , "stream" , message );
 * }
 * }
 *
 * Operation* StreamOperation::getOperation()
 * {
 * return samson::ModulesManager::shared()->getOperation( operation );
 * }
 *
 * #pragma mark StreamOperationForward
 *
 * bool StreamOperationForward::isValid()
 * {
 * Operation* op = getOperation();
 *
 * switch (op->getType())
 * {
 * case Operation::map:
 * case Operation::parser:
 * case Operation::parserOut:
 * // Valid cases...
 * break;
 *
 * default:
 * // Not valid cases
 * return false;
 * break;
 * }
 *
 * if( !op )
 * return false;
 *
 * if( op->getNumInputs() != (int)input_queues.size() )
 * {
 * LM_E(("op->getNumInputs(%d) != input_queues.size(%d)", op->getNumInputs() , (int)input_queues.size()));
 * return false;
 * }
 * if( op->getNumOutputs() != (int)output_queues.size() )
 * {
 * LM_E(("op->getNumOutputs(%d) != output_queues.size(%d)", op->getNumOutputs() , (int)output_queues.size()));
 * return false;
 * }
 *
 * return true;
 * }
 *
 * void StreamOperationForward::push( BlockList *list )
 * {
 * add_block_to_cronometers(list);
 *
 * // Update history information since we will absorb all data included in this list
 * list->update( history_block_info );
 * getBlockList("input")->copyFrom( list , 0 );
 * }
 *
 * size_t StreamOperationForward::getNextWorkerTaskPriorityParameter( )
 * {
 * // Get the input BLockList
 * BlockList *input = getBlockList("input");
 *
 * if( input->isEmpty() )
 * {
 * last_review = au::str("No more tasks ( No data in queue %s )" , input_queues[0].c_str() );
 * return 0; // No task to be implemented
 * }
 *
 * int max_operations = getMaxTasks();
 * if( max_operations > 0 )
 * if( running_tasks.size() >= (size_t) max_operations )
 * return 0;
 *
 * // Rigth now using size * time * priority
 * BlockInfo input_block_info = input->getBlockInfo();
 * return input_block_info.size * input_block_info.max_time_diff() * getPriority();
 * }
 *
 * void StreamOperationForward::scheduleNextWorkerTasks( )
 * {
 * if( !isValid() )
 * LM_X(1,("scheduleNextWorkerTasks for a non-valid operaiton"));
 *
 * // Properties for this stream operation
 * size_t max_size         = au::Singleton<SamsonSetup>::shared()->getUInt64("stream.max_operation_input_size");
 *
 * // Remove the last_review label
 * last_review = "";
 *
 * // Get the input BLockList
 * BlockList *input = getBlockList("input");
 *
 * if( input->isEmpty() )
 * LM_X(1,("Internal error. Empty input in stream operation...."));
 *
 * // Get detailed information about data to be processed
 * BlockInfo operation_block_info = input->getBlockInfo();
 *
 * // Get a new id for the next operation
 * size_t id = streamManager->getNewId();
 *
 * // Get the operation itself
 * Operation *op = getOperation( );
 *
 * WorkerTask *tmp = NULL;
 * switch ( op->getType() ) {
 * case Operation::parser:
 * {
 * tmp = new ParserWorkerTask( id , this );
 * }
 * break;
 *
 * case Operation::map:
 * {
 * tmp = new MapWorkerTask( id , this , KVRange(0,KVFILE_NUM_HASHGROUPS) );
 * }
 * break;
 *
 * case Operation::parserOut:
 * {
 * tmp = new ParserOutWorkerTask( id , this , KVRange(0,KVFILE_NUM_HASHGROUPS) );
 * }
 * break;
 *
 * default:
 * LM_X(1,("Internal error"));
 * break;
 * }
 *
 * // Set the outputs
 * tmp->addOutputsForOperation(op);
 *
 * // Copy input data
 * tmp->getBlockList("input_0")->extractFrom( input , max_size );
 *
 * // Set working size for correct monitoring of data
 * tmp->setWorkingSize();
 *
 * // Schedule this task into task manager
 * add( tmp );
 *
 * }
 *
 * void StreamOperation::fill( samson::gpb::CollectionRecord* record , VisualitzationOptions options )
 * {
 *
 *
 *
 * if( options == stream_block )
 * {
 * ::samson::add( record , "type" , "stream_op" , "left,different" );
 * ::samson::add( record , "name" , name , "left,different" );
 * ::samson::add( record , "In: #kvs" , input_rate.get_total_kvs() , "f=uint64,sum" );
 * ::samson::add( record , "In: size" , input_rate.get_total_size() , "f=uint64,sum" );
 * ::samson::add( record , "State: #kvs" , "" , "f=uint64,sum" );
 * ::samson::add( record , "State: size" , "" , "f=uint64,sum" );
 * ::samson::add( record , "Out: #kvs" , output_rate.get_total_kvs() , "f=uint64,sum" );
 * ::samson::add( record , "Out: size" , output_rate.get_total_size() , "f=uint64,sum" );
 * return;
 * }
 *
 * if( options == stream_block_rates )
 * {
 * ::samson::add( record , "type" , "stream_op" , "left,different" );
 * ::samson::add( record , "name" , name , "left,different" );
 * ::samson::add( record , "In: #kvs/s" , input_rate.get_rate_kvs() , "f=uint64,sum" );
 * ::samson::add( record , "In: size/s" , input_rate.get_rate_size() , "f=uint64,sum" );
 * ::samson::add( record , "State: #kvs/s" , "" , "f=uint64,sum" );
 * ::samson::add( record , "State: size/s" , "" , "f=uint64,sum" );
 * ::samson::add( record , "Out: #kvs/s" , output_rate.get_rate_kvs() , "f=uint64,sum" );
 * ::samson::add( record , "Out: size/s" , output_rate.get_rate_size() , "f=uint64,sum" );
 *
 * return;
 * }
 *
 * // Common fields
 * ::samson::add( record , "name"      , name      , "left,different" );
 * ::samson::add( record , "operation" , operation , "different" );
 *
 *
 * if( ( options == properties ) || (options == all ) )
 * {
 * ::samson::add( record , "properties"    , environment.getEnvironmentDescription() , "different" );
 * }
 *
 *
 * if( ( options == normal ) || (options == all ) )
 * {
 * std::ostringstream inputs,outputs;
 * for ( size_t i = 0 ; i < input_queues.size() ; i++)
 * inputs << input_queues[i] << " ";
 * for ( size_t i = 0 ; i < output_queues.size() ; i++)
 * outputs << output_queues[i] << " ";
 *
 * ::samson::add( record , "inputs"    , inputs.str()    , "different" );
 * ::samson::add( record , "outputs"   , outputs.str()   , "different" );
 * }
 *
 * if( ( options == running ) || (options == all ) )
 * {
 * BlockInfo input_block_info = getUniqueBlockInfo();
 * //::samson::add( record , "inputs"        , input_block_info.strShort() , "different" );
 * ::samson::add( record , "Input Size"    , input_block_info.info.size , "f=uint64,sum" );
 * ::samson::add( record , "Input #Kvs"    , input_block_info.info.kvs , "f=uint64,sum" );
 *
 * ::samson::add( record , "Latency (ms)"    , 1000*latency_time , "f=uint64,different" );
 *
 *
 * ::samson::add( record , "running_tasks" , running_tasks.size()        , "f=uint64,sum" );
 *
 * ::samson::add( record , "last_review"   , last_review + " " + getStatus() , "left,different" );
 * }
 *
 * if( ( options == in ) || (options == all ) )
 * {
 * ::samson::add( record , "#Ops", in_num_operations , "f=uint64,sum" );
 *
 * ::samson::add( record , "In: size"  , input_rate.get_total_size() , "f=uint64,sum" );
 * ::samson::add( record , "In: #kvs"  , input_rate.get_total_kvs() , "f=uint64,sum" );
 * ::samson::add( record , "In: B/s"    , input_rate.get_rate_size() , "f=uint64,sum" );
 * ::samson::add( record , "In: #kvs/s" , input_rate.get_rate_kvs() , "f=uint64,sum" );
 * }
 *
 * if( ( options == out ) || (options == all ) )
 * {
 * ::samson::add( record , "#Ops", out_num_operations , "f=uint64,sum" );
 * ::samson::add( record , "CoreSeconds", out_core_seconds , "f=uint64,sum" );
 *
 * ::samson::add( record , "Out: size"  , output_rate.get_total_size() , "f=uint64,sum" );
 * ::samson::add( record , "Out: #kvs"  , output_rate.get_total_kvs() , "f=uint64,sum" );
 * ::samson::add( record , "Out: B/s"    , output_rate.get_rate_size() , "f=uint64,sum" );
 * ::samson::add( record , "Out: #kvs/s" , output_rate.get_rate_kvs() , "f=uint64,sum" );
 * }
 *
 * }
 *
 *
 * #pragma mark StreamOperationUpdateState::
 *
 * StreamOperationUpdateState::StreamOperationUpdateState( int _num_divisions , bool _update_only )
 * {
 * // Number of divisions defined by user at creation time
 * num_divisions = _num_divisions;
 *
 * // Flag to only process states with new inputs...
 * update_only = _update_only;
 *
 * if( update_only )
 * environment.set("update_only", "yes");
 * else
 * environment.set("update_only", "no");
 *
 * // Alloc vector of booleans to flag what divisions we are updating...
 * updating_division = new bool[num_divisions];
 *
 * for (int i = 0 ; i < num_divisions ; i++ )
 * updating_division[i] =  false;  // Flag as non updating
 *
 * }
 *
 * StreamOperationUpdateState::~StreamOperationUpdateState()
 * {
 * delete[] updating_division;
 * }
 *
 * bool StreamOperationUpdateState::isValid()
 * {
 * Operation* op = getOperation();
 *
 * if( op->getType() != Operation::reduce )
 * return false;
 *
 * if( op->getNumInputs() != 2 )
 * return false;
 *
 * // Check input output format for state and same name for the queue
 * // Check state format is coherent
 * KVFormat a = op->getInputFormats()[op->getNumInputs() - 1];
 * KVFormat b = op->getOutputFormats()[ op->getNumOutputs() - 1 ];
 *
 * if( !a.isEqual(b)  )
 * {
 * //error.set("Last input and output should be the same data type to qualify as stream-reduce");
 * return false;
 * }
 *
 * // Check that the last input and the last output are indeed the same queue
 * std::string last_input = input_queues[ input_queues.size()-1 ];
 * std::string last_output = output_queues[ output_queues.size()-1];
 *
 * if(  last_input !=  last_output )
 * {
 * //error.set( au::str("Last input and last output should be the same state. ( %s != %s)" , last_input.c_str() , last_output.c_str() ) );
 * return false;
 * }
 *
 * if( !op )
 * return false;
 *
 * if( op->getNumInputs() != (int)input_queues.size() )
 * return false;
 * if( op->getNumOutputs() != (int)output_queues.size() )
 * return false;
 *
 * return true;
 * }
 *
 * void StreamOperationUpdateState::push( BlockList *list )
 * {
 * add_block_to_cronometers(list);
 *
 * // Extract data from input queue to the "input" blocklist ( no size limit... all blocks )
 * BlockList tmp;
 * tmp.copyFrom( list , 0 );
 *
 * // Update history information since we will absorb all data included in this list
 * tmp.update( history_block_info );
 *
 * // Put content in all the input blocklist for all the divisions
 * std::list<Block*>::iterator b;
 * for ( b = tmp.blocks.begin() ; b != tmp.blocks.end() ; b++ )
 * {
 * Block* block = *b;
 *
 * for ( int d = 0 ; d < num_divisions ; d++ )
 * if( block->getKVRange().overlap( rangeForDivision(d, num_divisions ) ) )
 * getBlockList( au::str("input_%d", d ) )->add( block );
 * }
 *
 * }
 * size_t StreamOperationUpdateState::getNextWorkerTaskPriorityParameter( )
 * {
 * int division = next_division_to_be_updated();
 *
 * if( division == -1 )
 * return 0; // No division to be updated
 *
 * // Get the input BLockList
 * BlockList *input = getBlockList( au::str("input_%d",division) );
 *
 * if( input->isEmpty() )
 * return 0; // No task to be implemented
 *
 * // Rigth now using size * time * priority
 * BlockInfo input_block_info = input->getBlockInfo();
 * return input_block_info.size * input_block_info.max_time_diff() * getPriority();
 * }
 *
 *
 * void StreamOperationUpdateState::scheduleNextWorkerTasks( )
 * {
 * if( !isValid() )
 * LM_X(1,("scheduleNextWorkerTasks for a non-valid operaiton"));
 *
 * // State queue
 * std::string queue_state_name = input_queues[1];
 * Queue *state = streamManager->getQueue( queue_state_name );
 *
 * // Get the operation itself
 * Operation *op = getOperation( );
 *
 * // Properties for this stream operation
 * size_t max_size = au::Singleton<SamsonSetup>::shared()->getUInt64("general.memory") / 8;       // Max memory to get
 *
 * last_review = "";
 *
 * int division = next_division_to_be_updated();
 *
 * if( division == -1 )
 * LM_X(1,("Internal error"));
 *
 * if( updating_division[division] )
 * LM_X(1,("Internal error"));
 *
 * // Updating division "division"
 * // --------------------------------------------------------------------------------
 *
 * updating_division[division] = true;
 *
 * // Get the input and state data
 * BlockList inputBlockList("inputBlockList");
 * BlockList stateBlockList("stateBlockList");
 *
 * BlockList *input = getBlockList( au::str("input_%d",division) );
 * inputBlockList.extractFrom( input, max_size );
 *
 * if( !state->getAndLockBlocksForKVRange( rangeForDivision(division , num_divisions) , & stateBlockList ) )
 * LM_X(1,("Internal error"));
 *
 * if( inputBlockList.isEmpty() )
 * LM_X(1,("Interal error"));
 *
 * // Get detailed information about data to be processed
 * BlockInfo operation_block_info = input->getBlockInfo();
 *
 * // Get a new id for the next operation
 * size_t id = streamManager->getNewId();
 *
 * ReduceWorkerTask * task = new ReduceWorkerTask( id , this , rangeForDivision( division, num_divisions ) );
 * if( update_only )
 * task->set_update_only();
 *
 * task->addOutputsForOperation(op);
 *
 * // Special flag to indicate update_state mode ( process different output buffers )
 * task->setUpdateStateDivision( division );
 *
 * // Add inputs
 * task->getBlockList("input_0")->copyFrom( &inputBlockList );
 * task->getBlockList("input_1")->copyFrom( &stateBlockList );
 *
 * // Set the working size to get statistics at ProcessManager
 * task->setWorkingSize();
 *
 * // Add this task...
 * add( task );
 *
 *
 * }
 *
 * int StreamOperationUpdateState::numUpdatingDivisions()
 * {
 * int total = 0 ;
 * for ( int i = 0 ; i < num_divisions ; i++ )
 * if( updating_division[i] )
 * total++;
 * return total;
 * }
 *
 * size_t StreamOperationUpdateState::getMaxStateSize()
 * {
 * size_t max_size = 0 ;
 * Queue *state = streamManager->getQueue( input_queues[1] );
 *
 * for (int i = 0 ; i < num_divisions ; i++ )
 * {
 * BlockList block_list;
 * state->getBlocksForKVRange( rangeForDivision(i,num_divisions) , &block_list );
 * size_t tmp = block_list.getBlockInfo().size;
 * if( tmp > max_size )
 * max_size = tmp;
 * }
 *
 * return max_size;
 * }
 *
 *
 * int StreamOperationUpdateState::next_division_to_be_updated()
 * {
 * // State queue to check if the state blocks can be locked
 * std::string queue_state_name = input_queues[1];
 * Queue *state = streamManager->getQueue( queue_state_name );
 *
 * size_t max_size = 0;
 * int division = -1;
 *
 * for (int i = 0 ; i < num_divisions ; i++ )
 * {
 * if( !updating_division[i] )
 * {
 * if( state->canLockBlocksForKVRange( rangeForDivision(i, num_divisions) ) )
 * {
 *
 * size_t size = getBlockList( au::str("input_%d", i ) )->getBlockInfo().size;
 *
 * if( size > 0 )
 * if( ( division == -1 ) || (size > max_size ) )
 * {
 * max_size = size;
 * division = i;
 * }
 * }
 * }
 * }
 * return division;
 * }
 *
 *
 *
 *
 * void StreamOperationUpdateState::getAllInputBlocks( BlockList *blockList )
 * {
 * std::set<Block*> blocks;
 *
 * for (int i = 0 ; i < num_divisions ; i++ )
 * {
 * BlockList *inputBlockList = getBlockList( au::str("input_%d", i ) );
 *
 * au::list< Block >::iterator b;
 * for (b = inputBlockList->blocks.begin() ;  b != inputBlockList->blocks.end() ; b++)
 * {
 * Block *block = *b;
 * blocks.insert( block );
 * }
 * }
 *
 * // Add all the blocks
 * for ( std::set<Block*>::iterator it = blocks.begin() ; it != blocks.end() ; it++ )
 * blockList->add( *it );
 *
 * }
 *
 *
 * #pragma mark StreamOperationForwardReduce
 *
 *
 * bool StreamOperationForwardReduce::isValid()
 * {
 * Operation* op = getOperation();
 *
 * switch (op->getType())
 * {
 * case Operation::reduce:
 * break;
 *
 * default:
 * // Not valid cases
 * return false;
 * break;
 * }
 *
 * if( !op )
 * return false;
 *
 * if( op->getNumInputs() != (int)input_queues.size() )
 * return false;
 * if( op->getNumOutputs() != (int)output_queues.size() )
 * return false;
 *
 * // Only two inputs allowed ( main input / aux queue )
 * if( op->getNumInputs() != 2 )
 * return false;
 *
 * return true;
 * }
 *
 * void StreamOperationForwardReduce::push( BlockList *list )
 * {
 * add_block_to_cronometers(list);
 *
 * // Update history information since we will absorb all data included in this list
 * list->update( history_block_info );
 * getBlockList("input")->copyFrom( list , 0 );
 * }
 *
 * size_t StreamOperationForwardReduce::getNextWorkerTaskPriorityParameter( )
 * {
 * // Get the input BLockList
 * BlockList *input = getBlockList("input");
 *
 * if( input->isEmpty() )
 * {
 * last_review = au::str("No more tasks ( No data in queue %s )" , input_queues[0].c_str() );
 * return 0; // No task to be implemented
 * }
 *
 * // Rigth now using size * time * priority
 * BlockInfo input_block_info = input->getBlockInfo();
 * return input_block_info.size * input_block_info.max_time_diff() * getPriority();
 * }
 *
 * void StreamOperationForwardReduce::scheduleNextWorkerTasks( )
 * {
 * if( !isValid() )
 * LM_X(1,("scheduleNextWorkerTasks for a non-valid operaiton"));
 *
 * // Get the operation itself
 * Operation *op = getOperation( );
 *
 * // Properties for this stream operation
 * size_t max_size         = au::Singleton<SamsonSetup>::shared()->getUInt64("stream.max_operation_input_size");
 *
 * last_review = "";
 *
 * // Get the input BLockList
 * BlockList *input = getBlockList("input");
 *
 * if( input->isEmpty() )
 * LM_X(1,("Internal erro"));
 *
 * // Get detailed information about data to be processed
 * BlockInfo operation_block_info = input->getBlockInfo();
 *
 * // Get a new id for the next operation
 * size_t id = streamManager->getNewId();
 *
 * // Create a new task
 * WorkerTask *tmp = new ReduceWorkerTask( id , this , KVRange(0,KVFILE_NUM_HASHGROUPS) );
 *
 * // Set the outputs
 * tmp->addOutputsForOperation(op);
 *
 * // Copy input data
 * tmp->getBlockList("input_0")->extractFrom( input , max_size );
 *
 * Queue *aux = streamManager->getQueue( input_queues[1] );
 * tmp->getBlockList("input_1")->copyFrom( aux->list );
 *
 * // Set working size for correct monitoring of data
 * tmp->setWorkingSize();
 *
 * // Add this task
 * add( tmp );
 *
 * }
 *
 */
}
}                       // End of namespace samson::stream
