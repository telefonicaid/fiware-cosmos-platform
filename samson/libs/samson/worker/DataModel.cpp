#include "samson/worker/DataModel.h"  // Own interface

#include <vector>

#include "au/log/LogMain.h"
#include "samson/common/gpb_operations.h"
#include "samson/worker/SamsonWorkerController.h"
#include "samson/common/NotificationMessages.h"
#include "samson/stream/Block.h"
#include "samson/network/Packet.h"
#include "samson/common/Logs.h"

namespace samson {
  
  // Default path in ZK for this information
  const std::string DataModel::kDefaultSamsonDataPath("/samson/data");
  
  // Constant strings for valid commands
  const std::string DataModel::kAdd("add");
  const std::string DataModel::kAddQueueConnection("add_queue_connection");
  const std::string DataModel::kAddStreamOperation("add_stream_operation");
  const std::string DataModel::kBatch("batch");
  const std::string DataModel::kBlock("block");
  const std::string DataModel::kClearBatchOPerations("clear_batch_operations");
  const std::string DataModel::kClearModules("clear_modules");
  const std::string DataModel::kPushQueue("push_queue");
  const std::string DataModel::kRemoveAll("remove_all");
  const std::string DataModel::kRemoveAllData("remove_all_data");
  const std::string DataModel::kRemoveAllStreamOperations("remove_all_stream_operations");
  const std::string DataModel::kRemoveStreamOperation("remove_stream_operation");
  const std::string DataModel::kRm("rm");
  const std::string DataModel::kRmQueueConnection("rm_queue_connection");
  const std::string DataModel::kSetQueueProperty("set_queue_property");
  const std::string DataModel::kSetStreamOperationProperty("set_stream_operation_property");
  const std::string DataModel::kUnsetStreamOperationProperty("unset_stream_operation_property");
  const std::string DataModel::kFreezeDataModel("data_model_freeze");
  const std::string DataModel::kCancelFreezeDataModel("data_model_cancel_freeze");
  const std::string DataModel::kRecoverDataModel("data_model_recover");
  const std::string DataModel::kConsolidateDataModel("consolidate_data_model");
  
  const std::string DataModel::commands[] = {DataModel::kAdd , DataModel::kAddQueueConnection, kAddStreamOperation,kBatch,kBlock,kClearBatchOPerations,kClearModules, kPushQueue, kRemoveAll,kRemoveAllData,kRemoveAllStreamOperations,kRemoveStreamOperation,kRm,kRmQueueConnection,kSetQueueProperty,
    kSetStreamOperationProperty, kUnsetStreamOperationProperty , kFreezeDataModel ,kCancelFreezeDataModel,  kRecoverDataModel, kConsolidateDataModel};

  const std::string DataModel::recovery_commands[] = {DataModel::kAdd , DataModel::kAddQueueConnection, kAddStreamOperation, kBatch, kClearBatchOPerations, kClearModules, kPushQueue, kRemoveAll,kRemoveAllData,kRemoveAllStreamOperations,kRemoveStreamOperation,kRm,kRmQueueConnection,kSetQueueProperty,kSetStreamOperationProperty, kUnsetStreamOperationProperty };
  
  // Constant strings for item commands
  const std::string DataModel::kAddItem("add");
  const std::string DataModel::kRmItem("rm");
  
  au::SharedPointer<au::CommandLine> DataModel::GetCommandLine() {
    
    au::SharedPointer<au::CommandLine> cmd( new au::CommandLine() );
    // Input output definition of queues
    cmd->SetFlagString("input", "");
    cmd->SetFlagString("output", "");
    // Forward flag to indicate that this is a reduce forward operation ( no update if state )
    cmd->SetFlagBoolean("forward");
    //Paused
    cmd->SetFlagBoolean("paused");
    // -a flag
    cmd->SetFlagBoolean("a");
    // Number of divisions in state operations
    cmd->SetFlagInt("divisions", au::Singleton<SamsonSetup>::shared()->GetInt("general.num_processess"));
    // Flag to indicate that states with no inputs should be copied to output automatically
    cmd->SetFlagBoolean("update_only");
    // Flag to indicate that this is a batch operation
    cmd->SetFlagBoolean("batch_operation");
    // Prefix used to change names of queues and operations
    cmd->SetFlagString("prefix", "");
    cmd->SetFlagUint64("delilah_id", (size_t) -1);
    cmd->SetFlagUint64("delilah_component_id", (size_t) -1);
    
    return cmd;
  }
  
  void DataModel::NotificationNewModel(int previous_version
                                    , au::SharedPointer<gpb::DataModel> previous_data
                                    , int version
                                    , au::SharedPointer<gpb::DataModel> new_data ) {
    LOG_M(logs.data_model, ("Update DataModel from ZK from version %d to version %d" , previous_version , version));
  }

  
  void DataModel::PerformCommit( au::SharedPointer<gpb::DataModel> data
                                , std::string command
                                , int version
                                , au::ErrorManager& error ) {
    
    
    // Analyse input commands
    au::SharedPointer<au::CommandLine> cmd = GetCommandLine();
    cmd->Parse(command);
    
    // No command error
    if (cmd->get_num_arguments() == 0) {
      error.set("No command specified");
      return;
    }
    
    // Spetial high level commands ( restart_data_model )
    std::string main_command = cmd->get_argument(0);
    if ( main_command == kFreezeDataModel ){
      ProcessFreezeDataModel(data, error);
      return;
    }
    if ( main_command == kCancelFreezeDataModel ){
      ProcessCancelFreezeDataModel(data, error);
      return;
    }
    if ( main_command == kRecoverDataModel ){
      ProcessRecoverDataModel(data, error);
      return;
    }
    if( main_command == kConsolidateDataModel){
      ProcessConsolidateDataModel(data,error);
      return;
    }

    // Process whatever opertion on current data model
    ProcessCommand( data->mutable_current_data() , cmd , error );
    
    if( error.IsActivated() )
      return;

    // update the commit_id
    size_t commit_id = data->mutable_current_data()->commit_id();
    LOG_M(logs.data_model, ("Trying to perform commit over Data model [candidate for %lu] %s (data version  %d)" , commit_id , command.c_str() , version ));
    data->mutable_current_data()->set_commit_id( commit_id+1 );
    
    // Add the commit to the global data model and increase the counter
    gpb::DataCommit* data_commit =  data->add_commit();
    data_commit->set_id( commit_id );
    data_commit->set_message(command);
    
  }
  
  void DataModel::ProcessCommand(gpb::Data* data , au::SharedPointer<au::CommandLine> cmd , au::ErrorManager& error)
  {
    
    // Get main command
    std::string main_command = cmd->get_argument(0);
    LOG_M(logs.data_model, ("ProcessCommand %s" , main_command.c_str() ));

    if (main_command == kAdd) {
      ProcessAddCommand(data, cmd, error);
    } else if (main_command == kAddQueueConnection) {
      ProcessAddQueueConnectionCommand(data, cmd, error);
    } else if (main_command == kAddStreamOperation) {
      ProcessAddStreamOperationCommand(data, cmd, error);
    } else if (main_command == kBatch) {
      ProcessBatchCommand(data, cmd, error);
    } else if (main_command == kBlock) {
      ProcessBlockCommand(data, cmd, error);
    } else if (main_command == kClearBatchOPerations) {
      ProcessClearBatchOPerationsCommand(data, cmd, error);
    } else if (main_command == kClearModules) {
      ProcessClearModulesCommand(data, cmd, error);
    } else if (main_command == kPushQueue) {
      ProcessPushQueueCommand(data, cmd, error);
    } else if (main_command == kRemoveAll) {
      ProcessRemoveAllCommand(data, cmd, error);
    } else if (main_command == kRemoveAllData) {
      ProcessRemoveAllDataCommand(data, cmd, error);
    } else if (main_command == kRemoveAllStreamOperations) {
      ProcessRemoveAllStreamOperationsCommand(data, cmd, error);
    } else if (main_command == kRemoveStreamOperation) {
      ProcessRemoveStreamOperationCommand(data, cmd, error);
    } else if (main_command == kRm) {
      ProcessRmCommand(data, cmd, error);
    } else if (main_command == kRmQueueConnection) {
      ProcessRmQueueConnectionCommand(data, cmd, error);
    } else if (main_command == kSetQueueProperty) {
      ProcessSetQueuePropertyCommand(data, cmd, error);
    } else if (main_command == kSetStreamOperationProperty) {
      ProcessSetStreamOperationPropertyCommand(data, cmd, error);
    } else if (main_command == kUnsetStreamOperationProperty) {
      ProcessUnsetStreamOperationPropertyCommand(data, cmd, error);
    } else {
      LOG_W(logs.data_model,("Unknown command (%s) in the commit to data model", main_command.c_str()));
      error.set(au::str("Unknown command (%s) in the commit to data model", main_command.c_str()));
    }
    return;
  }
  
  
  void DataModel::ProcessAddCommand(gpb::Data* data , au::SharedPointer<au::CommandLine> cmd , au::ErrorManager& error) {
    if (cmd->get_num_arguments() < 3) {
      error.set(au::str("Usage: '%s' queue_name key_format value_format", cmd->get_argument(0).c_str()));
      return;
    }
    if ((cmd->get_num_arguments() == 3) && (cmd->get_argument(2) != "txt")) {
      error.set(au::str("Usage: '%s' queue_name key_format value_format ( '%s' queue_name txt )",
                        cmd->get_argument(0).c_str(), cmd->get_argument(0).c_str()));
      return;
    }
    
    std::string name = cmd->get_argument(1);
    std::string key_format = cmd->get_argument(2);
    std::string value_format;
    if (cmd->get_num_arguments() == 3) {
      value_format = key_format;
    } else {
      value_format = cmd->get_argument(3);   // Get or create this queue
    }
    gpb::get_or_create_queue(data, name, KVFormat(key_format, value_format), error);
    if (!error.IsActivated()) {
      error.AddMessage(au::str("Queue %s added correctly", name.c_str()));
    }
    return;
  }
  
  // All
  
  void DataModel::ProcessAddQueueConnectionCommand(gpb::Data* data , au::SharedPointer<au::CommandLine> cmd , au::ErrorManager& error) {
    if (cmd->get_num_arguments() < 3) {
      error.set(au::str("Usage: '%s' source_queue target_queue", cmd->get_argument(0).c_str()));
      return;
    }
    
    std::string queue_source = cmd->get_argument(1);
    std::vector<std::string> target_queues = au::split(cmd->get_argument(2), ' ');
    for (size_t i = 0; i < target_queues.size(); ++i) {
      // Check if the connection exist
      if (!data_exist_queue_connection(data, queue_source, target_queues[i])) {
        gpb::data_create_queue_connection(data, queue_source, target_queues[i]);
      }
    }
    return;
  }
  
  // All
  
  
  void DataModel::ProcessAddStreamOperationCommand(gpb::Data* data , au::SharedPointer<au::CommandLine> cmd , au::ErrorManager& error) {
    if (cmd->get_num_arguments() < 3) {
      LOG_W(logs.data_model,("Error in add_stream_operation, num_arguments < 3"));
      LOG_W(logs.data_model,("Offending command: '%s'", cmd->command().c_str()));
      error.set( au::str("Usage: '%s' name operation -input \"input1 input2\" -output \"outputs1 outputs2 output3\"",
                         cmd->get_argument(0).c_str()));
      return;
    }
    // Recover prefix
    std::string prefix = cmd->GetFlagString("prefix");
    
    std::string name = prefix + cmd->get_argument(1);
    std::string operation = cmd->get_argument(2);
    std::string inputs = cmd->GetFlagString("input");
    std::string outputs = cmd->GetFlagString("output");
    
    // Get a new stream operation and increase the global counter
    size_t stream_operation_id = data->next_stream_operation_id();
    data->set_next_stream_operation_id(stream_operation_id + 1);
    
    // Create the new stream operation
    gpb::StreamOperation *stream_operation = gpb::getStreamOperation(data, name);
    
    if (stream_operation != NULL) {
      LOG_W(logs.data_model,("Stream operation %s already exist", name.c_str()));
      LOG_W(logs.data_model,("Offending command: '%s'", cmd->command().c_str()));
      error.set(au::str("Stream operation %s already exist", name.c_str()));
      return;
    }
    
    // Create a new stream operation
    gpb::StreamOperation *so = data->add_operations();
    so->set_stream_operation_id(stream_operation_id);
    so->set_name(name);
    so->set_operation(operation);
    so->set_paused(cmd->GetFlagBool("paused"));
    // Add input queues
    au::CommandLine cmd_inputs(inputs);
    for (int i = 0; i < cmd_inputs.get_num_arguments(); ++i) {
      so->add_inputs(cmd_inputs.get_argument(i));
    }
    // Add output queues
    au::CommandLine cmd_outputs(outputs);
    for (int i = 0; i < cmd_outputs.get_num_arguments(); ++i) {
      so->add_outputs(cmd_outputs.get_argument(i));
    }
    // Optional flags for the new operation
    so->set_reduce_forward(cmd->GetFlagBool("forward"));
    so->set_reduce_update_only(cmd->GetFlagBool("update_only"));
    so->set_batch_operation(cmd->GetFlagBool("batch_operation"));
    
    error.AddMessage(au::str("StreamOperation %s added correctly", name.c_str()));
    return;
  }
  
  void DataModel::ProcessBatchCommand(gpb::Data* data , au::SharedPointer<au::CommandLine> cmd , au::ErrorManager& error) {
    if (cmd->get_num_arguments() < 2) {
      error.set( au::str("Usage: '%s' -input \"input1 input2\" -output \"outputs1 outputs2 output3\"",
                         cmd->get_argument(0).c_str()));
      return;
    }
    
    std::string operation = cmd->get_argument(1);
    std::string inputs = cmd->GetFlagString("input");
    std::string outputs = cmd->GetFlagString("output");
    size_t delilah_id = cmd->GetFlagUint64("delilah_id");
    size_t delilah_component_id = cmd->GetFlagUint64("delilah_component_id");
    
    std::vector<std::string> input_queues = au::split(inputs, ' ');
    std::vector<std::string> output_queues = au::split(outputs, ' ');
    std::string prefix = au::str(".%s_%lu_", au::code64_str(delilah_id).c_str(), delilah_component_id);
    
    // Push content to parallel queues
    for (size_t i = 0; i < input_queues.size(); ++i) {
      std::string new_command = au::str("push_queue %s %s%s", input_queues[i].c_str(), prefix.c_str(),
                                        input_queues[i].c_str());
      ProcessCommand(data, new_command, error);
      if (error.IsActivated()) {
        return;
      }
    }
    
    // Schedule stream operations
    std::string operation_name = prefix + "stream_operation";
    std::ostringstream new_command;
    
    new_command << "add_stream_operation " << operation_name << " " << operation << " ";
    new_command << "-input \"";
    for (size_t i = 0; i < input_queues.size(); ++i) {
      if (i != 0) {
        new_command << " ";
      }
      new_command << prefix << input_queues[i];
    }
    new_command << "\"";
    new_command << "-output \"";
    for (size_t i = 0; i < output_queues.size(); ++i) {
      if (i != 0) {
        new_command << " ";
      }
      new_command << prefix << output_queues[i];
    }
    new_command << "\"";
    new_command << " -batch_operation ";   // Add this flag to identify the stream
    
    ProcessCommand(data, new_command.str() , error);
    if (error.IsActivated()) {
      return;
    }
    
    // Add the operation itself in the list
    gpb::BatchOperation *batch_operation = data->add_batch_operations();
    batch_operation->set_delilah_id(delilah_id);
    batch_operation->set_delilah_component_id(delilah_component_id);
    batch_operation->set_finished(false);
    batch_operation->set_operation(operation);
    
    for (size_t i = 0; i < input_queues.size(); ++i) {
      batch_operation->add_inputs(input_queues[i]);
      gpb::KVInfo *info = batch_operation->add_input_info();
      gpb::Queue *queue = get_queue(data, input_queues[i]);
      if (queue) {
        size_t num_blocks;
        size_t kvs;
        size_t size;
        getQueueInfo(*queue, &num_blocks, &kvs, &size);
        info->set_kvs(kvs);
        info->set_size(size);
      } else {
        info->set_kvs(0);
        info->set_size(0);
      }
    }
    
    for (size_t i = 0; i < output_queues.size(); ++i) {
      batch_operation->add_outputs(output_queues[i]);
    }
    return;
  }
  
  void DataModel::ProcessBlockCommand( gpb::Data* data, au::SharedPointer<au::CommandLine> cmd, au::ErrorManager& error) {
    // Main command to add or remove blocks to queues
    CommitCommand commit_command;
    commit_command.ParseCommitCommand(cmd->command(), error);
    if (error.IsActivated()) {
      return;
    }
    
    int version = data->commit_id();
    
    // Perform all changes
    const au::vector<CommitCommandItem>& items = commit_command.items();
    for (size_t i = 0; i < items.size(); ++i) {
      CommitCommandItem *item = items[i];
      
      if (item->command() == kAddItem) {
        add_block(data, item->queue(), item->block_id(), item->block_size(), item->format(),
                  item->range(), item->info(), version, error);
        if (error.IsActivated()) {
          LOG_W(logs.data_model,("Error in '%s' operation, error:'%s'", cmd->get_argument(0).c_str(), error.GetMessage().c_str()));
          return;
        }
        // add also to the connected queues
        au::StringVector connected_queues = data_get_queues_connected(data, item->queue());
        for (size_t i = 0; i < connected_queues.size(); ++i) {
          add_block(data, connected_queues[i], item->block_id(), item->block_size(), item->format(),
                    item->range(), item->info(), version, error);
        }
      } else if (item->command() == kRmItem) {
        rm_block(data, item->queue(), item->block_id(), item->format(), item->range(), item->info(),
                 version, error);
        if (error.IsActivated()) {
          LOG_W(logs.data_model,("Error in '%s' operation, error:'%s'", cmd->get_argument(0).c_str(), error.GetMessage().c_str()));
          return;
        }
      }
    }
    
    // Blocks have been added or removed... review batch operation
    ReviewBatchOperations(data, error);
    if (error.IsActivated()) {
      LOG_W(logs.data_model,("Error in '%s' operation, error:'%s'", cmd->get_argument(0).c_str(), error.GetMessage().c_str()));
    }
    return;
  }
  
  // All
  
  void DataModel::ProcessClearBatchOPerationsCommand(gpb::Data* data, au::SharedPointer<au::CommandLine> cmd,au::ErrorManager & error) {
    bool all_flag = cmd->GetFlagBool("a");
    remove_finished_operation(data, all_flag);
    return;
  }
  
  // All
  
  void DataModel::ProcessClearModulesCommand(gpb::Data* data, au::SharedPointer<au::CommandLine> cmd /* cmd */, au::ErrorManager &/* error */) {
    // Remove queue .modules
    au::ErrorManager error2;   // we are not interested in this error
    ProcessCommand(data, "rm .modules", error2);
    return;
  }
  
  void DataModel::ProcessPushQueueCommand(gpb::Data* data, au::SharedPointer<au::CommandLine> cmd,au::ErrorManager &error) {
    if (cmd->get_num_arguments() < 3) {
      error.set(au::str("Usage: push_queue source_queue target_queues", cmd->get_argument(0).c_str()));
      return;
    }
    
    samson::gpb::Queue *queue = get_queue(data, cmd->get_argument(1));
    if (!queue) {
      LOG_W(logs.data_model,("queue '%s' not found", cmd->get_argument(1).c_str()));
      return;   // nothing to do
    }
    queue->set_version( data->commit_id() );   // Update version where this queue was updated
    KVFormat format(queue->key_format(), queue->value_format());
    samson::gpb::Queue *target_queue = get_or_create_queue(data, cmd->get_argument(2), format, error);
    
    if (error.IsActivated()) {
      LOG_W(logs.data_model,("Error for queue:'%s' in get_or_create_queue, error:%s",
            cmd->get_argument(2).c_str(), error.GetMessage().c_str()));
      return;
    }
    // Copy all the content
    for (int i = 0; i < queue->blocks_size(); ++i) {
      target_queue->add_blocks()->CopyFrom(queue->blocks(i));
    }
    return;
  }
  
  // All
  
  
  void DataModel::ProcessRemoveAllCommand(gpb::Data* data, au::SharedPointer<au::CommandLine> cmd , au::ErrorManager&/* error */) {
    reset_stream_operations(data);
    reset_data(data);
    return;
  }
  
  void DataModel::ProcessRemoveAllDataCommand(gpb::Data* data, au::SharedPointer<au::CommandLine> cmd /* cmd */,
                                               au::ErrorManager&/* error */) {
    reset_data(data);
    return;
  }
  
  void DataModel::ProcessRemoveAllStreamOperationsCommand(gpb::Data* data,
                                                          au::SharedPointer<au::CommandLine> cmd /* cmd */, 
                                                          au::ErrorManager&/* error */) {
    reset_stream_operations(data);
    return;
  }
  
  void DataModel::ProcessRemoveStreamOperationCommand(gpb::Data* data, au::SharedPointer<au::CommandLine> cmd,
                                                       au::ErrorManager&error) {
    // Recover prefix
    std::string prefix = cmd->GetFlagString("prefix");
    std::string name = prefix + cmd->get_argument(1);
    
    gpb::StreamOperation *stream_operation = gpb::getStreamOperation(data, name);
    if (!stream_operation) {
      error.set(au::str("Stream operation %s not found", name.c_str()));
      return;
    }
    gpb::removeStreamOperation(data, name);
    error.AddMessage(au::str("Stream operation %s removed correctly", name.c_str()));
    return;
  }
  
  void DataModel::ProcessRmCommand(gpb::Data* data, au::SharedPointer<au::CommandLine> cmd, 
                                   au::ErrorManager&error) {
    if (cmd->get_num_arguments() < 2) {
      error.set(au::str("Usage: rm queue_name queue_name2 ....", cmd->get_argument(0).c_str()));
      return;
    }
    
    for (int i = 1; i < cmd->get_num_arguments(); ++i) {
      const std::string name = cmd->get_argument(i);
      
      samson::gpb::Queue *queue = get_queue(data, name);
      if (!queue) {
        error.AddError(au::str("Queue %s does not exist already exists", name.c_str()));
      } else {
        removeQueue(data, name);
        error.AddMessage(au::str("Queue %s has been removed correctly", name.c_str()));
      }
    }
    return;
  }
  
  void DataModel::ProcessRmQueueConnectionCommand(gpb::Data* data, au::SharedPointer<au::CommandLine> cmd,
                                                   au::ErrorManager&error) {
    if (cmd->get_num_arguments() < 3) {
      error.set(au::str("Usage: rm_queue_connections source_queue target_queue", cmd->get_argument(0).c_str()));
      return;
    }
    
    std::vector<std::string> source_queues = au::split(cmd->get_argument(1), ' ');
    std::vector<std::string> target_queues = au::split(cmd->get_argument(2), ' ');
    for (size_t i = 0; i < source_queues.size(); i++) {
      for (size_t j = 0; j < target_queues.size(); j++) {
        data_remove_queue_connection(data, source_queues[i], target_queues[j]);
      }
    }
    return;
  }
  
  void DataModel::ProcessSetQueuePropertyCommand(gpb::Data* /* data */, au::SharedPointer<au::CommandLine> cmd,
                                                  au::ErrorManager&error) {
    error.set(au::str("Command:'%s', still not implemented", cmd->get_argument(0).c_str()));
    return;
  }
  
  void DataModel::ProcessSetStreamOperationPropertyCommand(gpb::Data* data, au::SharedPointer<au::CommandLine> cmd,
                                                            au::ErrorManager&error) {
    if (cmd->get_num_arguments() < 4) {
      error.set(au::str("Usage: set_stream_operation_property name property value", cmd->get_argument(0).c_str()));
      return;
    }
    // Recover prefix
    std::string prefix = cmd->GetFlagString("prefix");
    
    std::string name = prefix + cmd->get_argument(1);
    std::string property = cmd->get_argument(2);
    std::string value = cmd->get_argument(3);
    gpb::StreamOperation *stream_operation = gpb::getStreamOperation(data, name);
    if (!stream_operation) {
      error.set(au::str("Stream operation %s not found", name.c_str()));
      return;
    }
    
    if (property == "paused") {
      if (value == "yes" || value == "y")
        stream_operation->set_paused(true);
      else if (value == "no" || value == "n")
        stream_operation->set_paused(false);
      else
        error.set(au::str("Unknown value %d for paused. Say yes or no", value.c_str()));
      
      return;
    }
    
    setProperty(stream_operation->mutable_environment(), property, value);
    error.AddMessage(au::str("Stream operation %s has been updated correctly", name.c_str()));
    return;
  }
  
  void DataModel::ProcessUnsetStreamOperationPropertyCommand(gpb::Data* data,
                                                             au::SharedPointer<au::CommandLine> cmd, 
                                                             au::ErrorManager&error) {
    if (cmd->get_num_arguments() < 3) {
      error.set(au::str("Usage: unset_stream_operation_property name property", cmd->get_argument(0).c_str()));
      return;
    }
    // Recover prefix
    std::string prefix = cmd->GetFlagString("prefix");
    
    std::string name = prefix + cmd->get_argument(1);
    std::string property = cmd->get_argument(2);
    
    gpb::StreamOperation *stream_operation = gpb::getStreamOperation(data, name);
    if (!stream_operation) {
      error.set(au::str("Stream operation %s not found", name.c_str()));
      return;
    }
    unsetProperty(stream_operation->mutable_environment(), property);
    error.AddMessage(au::str("Stream operation %s has been updated correctly", name.c_str()));
    return;
  }
  
  // All
  void DataModel::ProcessFreezeDataModel(au::SharedPointer<gpb::DataModel> data_model ,au::ErrorManager&error)
  {
    if( data_model->has_candidate_data() )
    {
      error.set("Still frozing a previous data model");
      return;
    }

    // If data model has not changed, no update
    if( data_model->current_data().commit_id() == data_model->previous_data().commit_id() )
    {
      error.set("Nothing to be updated");
      return;
    }
    
    // Copy current in candidate
    data_model->mutable_candidate_data()->CopyFrom(  data_model->current_data() );
    error.AddMessage("Ok. Frozen data model state scheduled correcly");
    
  }
  
  void DataModel::ProcessCancelFreezeDataModel(au::SharedPointer<gpb::DataModel> data_model ,au::ErrorManager&error)
  {
    if( !data_model->has_candidate_data() )
    {
      error.set("No candidate data-model to cancel");
      return;
    }

    // Clear candidate data
    data_model->clear_candidate_data();
  }
  
  bool DataModel::IsRecoveryCommand( const std::string& command )
  {
    au::SharedPointer<au::CommandLine> cmd = GetCommandLine();
    cmd->Parse(command);
    std::string main_command = cmd->get_argument(0);

    for ( size_t i = 0 ; i < sizeof(recovery_commands)/sizeof(std::string) ; i++ )
      if( main_command == recovery_commands[i])
        return true;
    return false;
  }
  
  void DataModel::ProcessRecoverDataModel( au::SharedPointer<gpb::DataModel> data_model ,au::ErrorManager&error)
  {
    // Recover previous version as current
    data_model->clear_candidate_data();
    
    // Current data based on previous model
    data_model->clear_current_data();
    data_model->mutable_current_data()->CopyFrom( data_model->previous_data() );

    // Apply some selected data commits based on history
    std::vector< std::pair<size_t, std::string> > commits;
    for ( int i = 0 ; i < data_model->commit_size() ; i++ )
    {
      if( IsRecoveryCommand( data_model->commit(i).message() ) )
        commits.push_back(std::pair<size_t,std::string>(data_model->commit(i).id() ,data_model->commit(i).message()));
    }
    data_model->clear_commit();
    for ( size_t i = 0 ; i < commits.size() ; i++ )
    {
      gpb::DataCommit* new_commit = data_model->add_commit();
      new_commit->set_id( commits[i].first );
      new_commit->set_message( commits[i].second );
      
      // Apply to current data model
      au::ErrorManager error2; // Ignore this erors
      ProcessCommand( data_model->mutable_current_data() , commits[i].second , error2);
      data_model->mutable_current_data()->set_commit_id( commits[i].first ); // Update to this commit_id
    }
    
  }

  
  void DataModel::ProcessConsolidateDataModel( au::SharedPointer<gpb::DataModel> data_model ,au::ErrorManager&error)
  {
    if( !data_model->has_candidate_data() )
    {
      error.set("No candidate state model to be consolidated");
      return;
    }
    
    data_model->mutable_previous_data()->CopyFrom( data_model->candidate_data() );
    data_model->clear_candidate_data(); // Remove candidate

    // Remove preivous commits
    size_t commit_id = data_model->previous_data().commit_id();
    std::vector< std::pair<size_t, std::string> > commits;
    for ( int i = 0 ; i < data_model->commit_size() ; i++ )
      if( data_model->commit(i).id() > commit_id ) // Keep this commit since it is posterior
        commits.push_back(std::pair<size_t,std::string>(data_model->commit(i).id() ,data_model->commit(i).message()));
    data_model->clear_commit();
    for ( size_t i = 0 ; i < commits.size() ; i++ )
    {
      gpb::DataCommit* new_commit = data_model->add_commit();
      new_commit->set_id( commits[i].first );
      new_commit->set_message( commits[i].second );
    }
  }

  
  void DataModel::FreezeCandidateDataModel( )
  {
    au::ErrorManager error;
    Commit( "SAMSON system", "consolidate_data_model", error );
    if( error.IsActivated() ) {
      LOG_W( logs.data_model ,("Error in data-model commit to consolidate data model:  %s " , error.GetMessage().c_str() ));
    }
  }
  
  bool DataModel::isValidCommand(const std::string& main_command) {
    
    for ( size_t i = 0 ; i < sizeof(commands)/sizeof(std::string) ; i++ )
      if( main_command == commands[i])
        return true;
    return false;
  }

  
  // Get collections for all the operations
  au::SharedPointer<gpb::Collection> DataModel::GetCollectionForStreamOperations(const Visualization& visualization) {

    // Get a copy of the current version
    gpb::Data* data = getCurrentModel()->mutable_current_data();
    
    bool all_flag = visualization.get_flag("a");
    
    au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
    collection->set_name("stream_operations");
    
    for (size_t i = 0; i < (size_t) data->operations_size(); ++i) {
      const gpb::StreamOperation & stream_operation = data->operations(i);
      
      const std::string name = stream_operation.name();
      if ((name.length() == 0) || (!all_flag && (name[0] == '.'))) {
        continue;
      }
      gpb::CollectionRecord *record = collection->add_record();
      ::samson::add(record, "id", stream_operation.stream_operation_id(), "different");
      ::samson::add(record, "name", stream_operation.name(), "different,left");
      ::samson::add(record, "operation", stream_operation.operation(), "different");
      
      std::ostringstream flags;
      if (stream_operation.batch_operation()) {
        flags << "batch_operation ";
      }
      if (stream_operation.reduce_forward()) {
        flags << "reduce_forward ";
      }
      if (stream_operation.reduce_update_only()) {
        flags << "update_only ";
      }
      ::samson::add(record, "Flags", flags.str(), "different");
      
      std::ostringstream inputs;
      for (int j = 0; j < stream_operation.inputs_size(); ++j) {
        inputs << stream_operation.inputs(j) << " ";
      }
      
      std::ostringstream outputs;
      for (int j = 0; j < stream_operation.outputs_size(); ++j) {
        outputs << stream_operation.outputs(j) << " ";
      }
      
      ::samson::add(record, "inputs", inputs.str(), "different");
      ::samson::add(record, "outputs", outputs.str(), "different");
      ::samson::add(record, "environment", str(stream_operation.environment()), "different");
    }
    return collection;
  }
  
  // Get collections for all the operations
  au::SharedPointer<gpb::Collection> DataModel::GetCollectionForBatchOperations(const Visualization& visualization) {
    // Get optional flags
    bool input_flag = visualization.get_flag("-input");
    bool output_flag = visualization.get_flag("-output");
    // Get a copy of the current version
    gpb::Data* data = getCurrentModel()->mutable_current_data();
    au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
    collection->set_name("batch_operations");
    
    for (size_t i = 0; i < (size_t) data->batch_operations_size(); ++i) {
      const gpb::BatchOperation & batch_operation = data->batch_operations(i);
      gpb::CollectionRecord *record = collection->add_record();
      size_t delilah_id = batch_operation.delilah_id();
      size_t delilah_component_id = batch_operation.delilah_component_id();
      std::string name = au::str("%s %lu", au::code64_str(delilah_id).c_str(), delilah_component_id);
      ::samson::add(record, "delilah", name, "different");
      ::samson::add(record, "operation", batch_operation.operation(), "different");
      if (batch_operation.finished()) {
        ::samson::add(record, "finish", "YES", "different");
      } else {
        ::samson::add(record, "finish", "NO", "different");
      }
      
      // Prefix for all queues
      std::string prefix = au::str(".%s_%lu_", au::code64_str(delilah_id).c_str(), delilah_component_id);
      // Inputs
      std::ostringstream inputs;
      std::ostringstream original_inputs_data;
      std::ostringstream inputs_data;
      // Original Inputs
      for (int j = 0; j < batch_operation.inputs_size(); ++j) {
        inputs << batch_operation.inputs(j) << " ";
        // Get original size and current size
        KVInfo original_info = KVInfo(batch_operation.input_info(j).size(), batch_operation.input_info(j).kvs());
        KVInfo current_info;
        std::string queue_name = prefix + batch_operation.inputs(j);
        gpb::Queue *queue = get_queue(data, queue_name);
        if (queue) {
          size_t num_blocks;
          size_t kvs;
          size_t size;
          getQueueInfo(*queue, &num_blocks, &kvs, &size);
          current_info = KVInfo(size, kvs);
        }
        inputs_data << current_info.str() << " ";
        original_inputs_data << original_info.str();
        original_inputs_data << "[ " << au::str_percentage(original_info.size - current_info.size, original_info.size)
        << " ]" << " ";
      }
      
      // Outputs
      std::ostringstream outputs;
      std::ostringstream outputs_data;
      for (int j = 0; j < batch_operation.outputs_size(); ++j) {
        outputs << batch_operation.outputs(j) << " ";
        std::string queue_name = prefix + batch_operation.outputs(j);
        gpb::Queue *queue = get_queue(data, queue_name);
        if (queue) {
          size_t num_blocks;
          size_t kvs;
          size_t size;
          getQueueInfo(*queue, &num_blocks, &kvs, &size);
          outputs_data << KVInfo(size, kvs).str() << " ";
        }
      }
      
      ::samson::add(record, "inputs", inputs.str(), "different");
      ::samson::add(record, "outputs", outputs.str(), "different");
      // Data information
      if (input_flag) {
        ::samson::add(record, "Input data", inputs_data.str(), "different");
      } else if (output_flag) {
        ::samson::add(record, "Output data", outputs_data.str(), "different");
      } else {
        ::samson::add(record, "Input data", original_inputs_data.str(), "different");
      }
    }
    return collection;
  }
  
  // All
  
  au::SharedPointer<gpb::Collection> DataModel::GetCollectionForQueuesWithBlocks(const Visualization& visualization) {
    // Get a copy of the current node
    gpb::Data* data = getCurrentModel()->mutable_current_data();
    
    bool all_flag = visualization.get_flag("a");
    
    au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
    collection->set_name("block_queues");
    
    for (int q = 0; q < data->queue_size(); ++q) {
      const gpb::Queue& queue = data->queue(q);
      const std::string queue_name = queue.name();
      if ((queue_name.length() == 0) || (!all_flag && (queue_name[0] == '.'))) {
        continue;
      }
      
      int blocks_size = queue.blocks_size();
      for (int b = 0; b < blocks_size; b++) {
        const gpb::Block& block = queue.blocks(b);
        gpb::CollectionRecord *record = collection->add_record();
        
        ::samson::add(record, "queue", queue_name, "different");
        ::samson::add(record, "block", str_block_id(block.block_id()), "different");
        ::samson::add(record, "block_size", block.block_size(), "f=uint64,different");
        KVRange range = block.range();   // Implicit conversion
        ::samson::add(record, "ranges", range.str(), "different");
        KVInfo info( block.size() , block.kvs() );
        ::samson::add(record, "info", info.str(), "different");
        ::samson::add(record, "commit", block.commit_id(), "different");
        ::samson::add(record, "time", block.time(), "f=timestamp");
      }
    }
    return collection;
  }
  
  // All
  au::SharedPointer<gpb::Collection> DataModel::GetCollectionForQueueConnections(const Visualization& visualization) {
    // Get a copy of the current node
    gpb::Data* data = getCurrentModel()->mutable_current_data();
    
    bool all_flag = visualization.get_flag("a");
    // Get all source queues
    std::set<std::string> source_queues;
    for (size_t i = 0; i < (size_t) data->queue_connections_size(); ++i) {
      const std::string queue_name = data->queue_connections(i).queue_source();
      if ((queue_name.length() == 0) || (!all_flag && (queue_name[0] == '.'))) {
        continue;
      }
      source_queues.insert(queue_name);
    }
    
    au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
    collection->set_name("queues_connections");
    std::set<std::string>::iterator it;
    for (it = source_queues.begin(); it != source_queues.end(); ++it) {
      // Target queues for this source queue
      au::StringVector target_queues = data_get_queues_connected(data, *it);
      
      gpb::CollectionRecord *record = collection->add_record();
      ::samson::add(record, "source", *it, "different");
      ::samson::add(record, "target", target_queues.str(), "different");
    }
    return collection;
  }
  
  // All
  
  au::SharedPointer<gpb::Collection> DataModel::GetCollectionForQueues(const Visualization& visualization) {
    // Get a copy of the current node
    gpb::Data* data = getCurrentModel()->mutable_current_data();
    
    bool all_flag = visualization.get_flag("a");
    
    au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
    collection->set_name("queues");
    
    for (size_t q = 0; q < (size_t) data->queue_size(); ++q) {
      const gpb::Queue& queue = data->queue(q);
      const std::string queue_name = queue.name();
      if ((queue_name.length() == 0) || (!all_flag && (queue_name[0] == '.'))) {
        continue;
      }
      if (!visualization.match(queue_name)) {
        continue;
      }
      size_t kvs;
      size_t size;
      size_t num_blocks;
      getQueueInfo(queue, &num_blocks, &kvs, &size);
      
      gpb::CollectionRecord *record = collection->add_record();
      ::samson::add(record, "name", queue.name(), "different");
      std::string format = au::str("%s-%s", queue.key_format().c_str(), queue.value_format().c_str());
      ::samson::add(record, "format", format, "different");
      ::samson::add(record, "#blocks", num_blocks, "different,f=uint64");
      ::samson::add(record, "#kvs", kvs, "different,f=uint64");
      ::samson::add(record, "size", size, "different,f=uint64");
    }
    return collection;
  }
  
  au::SharedPointer<gpb::Collection> DataModel::GetCollectionForQueueRanges(const Visualization& visualization,
                                                                            const std::string& queue_name) {
    // Create collection
    au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
    collection->set_name("queue_ranges");
    // Get a copy of the current node
    gpb::Data* data = getCurrentModel()->mutable_current_data();
    // Get queue of interest
    gpb::Queue* queue = gpb::get_queue(data, queue_name);
    if (!queue) {
      return collection;
    }
    
    // For all ranges
    int num_ranges = 32;
    std::vector<KVRange> ranges;
    std::vector<size_t> sizes;
    std::vector<size_t> kvs;
    for (int i = 0; i < num_ranges; ++i) {
      ranges.push_back(GetKVRangeForDivision(i, num_ranges));
      sizes.push_back(0);
      kvs.push_back(0);
    }
    
    // Compute size in each range
    for (int i = 0; i < queue->blocks_size(); ++i) {
      KVRange range = queue->blocks(i).range();   // Implicit conversion
      
      for (int r = 0; r < num_ranges; ++r) {
        double overlap = range.GetOverlapFactor(ranges[r]);
        sizes[r] += overlap * queue->blocks(i).size();
        kvs[r] += overlap * queue->blocks(i).kvs();
      }
    }
    
    // Compute max size
    size_t max_size = sizes[0];
    for (int r = 1; r < num_ranges; ++r) {
      if (sizes[r] > max_size) {
        max_size = sizes[r];
      }
    }
    
    // Generate all records
    for (int r = 0; r < num_ranges; ++r) {
      gpb::CollectionRecord *record = collection->add_record();
      ::samson::add(record, "range", ranges[r].str(), "different");
      ::samson::add(record, "#kvs", kvs[r], "f=uint64");
      ::samson::add(record, "size", sizes[r], "f=uint64");
      ::samson::add(record, "comparison", au::str_progress_bar(static_cast<double>(sizes[r]) / static_cast<double>(max_size), 50), "different");
    }
    return collection;
  }
  
  std::set<size_t> DataModel::GetAllBlockIds() {
    
    // Get a copy of the data
    au::SharedPointer<gpb::DataModel> data_model = getCurrentModel();

    // Prepare list of ids to be returned
    std::set<size_t> block_ids;
    
    AddBlockIds( data_model->mutable_current_data() , block_ids );
    if( data_model->has_candidate_data() )
      AddBlockIds( data_model->mutable_candidate_data() , block_ids );
    AddBlockIds( data_model->mutable_previous_data() , block_ids );
    
    return block_ids;
  }
  
  size_t DataModel::GetLastCommitIdForPreviousDataModel()
  {
    au::SharedPointer<gpb::DataModel> data_model = getCurrentModel();
    return data_model->previous_data().commit_id();
  }
  
  std::set<size_t> DataModel::GetMyBlockIdsForPreviousAndCandidateDataModel(const std::vector<KVRange>& ranges )
  {
    std::set<size_t> block_ids;  // Prepare list of ids to be returned
    
    au::SharedPointer<gpb::DataModel> data_model = getCurrentModel();
    gpb::AddBlockIds( data_model->mutable_previous_data() , ranges, block_ids);
    if( data_model->has_candidate_data() )
      gpb::AddBlockIds( getCurrentModel()->mutable_candidate_data(), ranges  , block_ids);
    return block_ids;
  }
  
  std::set<size_t> DataModel::GetMyBlockIdsForPreviousDataModel(const std::vector<KVRange>& ranges )
  {
    std::set<size_t> block_ids;  // Prepare list of ids to be returned
    
    au::SharedPointer<gpb::DataModel> data_model = getCurrentModel();
    gpb::AddBlockIds( data_model->mutable_previous_data() , ranges, block_ids);
    return block_ids;
  }

  size_t DataModel::GetLastCommitIdForCandidateDataModel()
  {
    au::SharedPointer<gpb::DataModel> data_model = getCurrentModel();
    
    if( !data_model->has_candidate_data() )
      return (size_t)-1;
    return data_model->candidate_data().commit_id();
  }

  std::set<size_t> DataModel::GetMyBlockIdsForCandidateDataModel(const std::vector<KVRange>& ranges )
  {
    std::set<size_t> block_ids;  // Prepare list of ids to be returned
    au::SharedPointer<gpb::DataModel> data_model = getCurrentModel();
    if( data_model->has_candidate_data() )
      gpb::AddBlockIds( getCurrentModel()->mutable_candidate_data(), ranges  , block_ids);
    return block_ids;
  }
  
  // method trying to discover if all operations have finished
  // (if no operations has pending data in its first input queue)
  bool DataModel::CheckForAllOperationsFinished() {
    
    // Get a copy of the current version
    gpb::Data* data = getCurrentModel()->mutable_current_data();
    
    int operations_size = data->operations_size();
    
    // TODO(@jges): Remove log message
    LOG_D(logs.data_model, ("operations.size(%d)", operations_size));
    
    for (int i = 0; i < operations_size; ++i) {
      const gpb::StreamOperation & stream_operation = data->operations(i);
      std::string name = stream_operation.name();
      LOG_D(logs.data_model, ("CheckForAllOperationsFinished stream_operation:'%s' with %d inputs",
                                 name.c_str(), operations_size));
      
      // To check for finish, we should take into account just the first input,
      // as the other would be the state or a permanent queue
      std::string queue_name = stream_operation.inputs(0);
      
      gpb::Queue *queue = get_queue(data, queue_name);
      if (queue) {
        size_t num_blocks;
        size_t kvs;
        size_t size;
        getQueueInfo(*queue, &num_blocks, &kvs, &size);
        if (size > 0) {
          LOG_D(logs.data_model, ("Active operation: '%s', queue:'%s' with size:%lu",
                                     name.c_str(), queue_name.c_str(), size));
          return false;
        }
      }
    }
    
    operations_size = data->batch_operations_size();
    LOG_D(logs.data_model, ("batch operations.size(%d)", operations_size));
    for (int i = 0; i < operations_size; ++i) {
      gpb::BatchOperation *batch_operation = data->mutable_batch_operations(i);
      if ( gpb::batch_operation_is_finished(data, *batch_operation) == false) {
        return false;
      }
    }
    return true;
  }
  
  au::SharedPointer<gpb::Collection> DataModel::GetLastCommitsCollection(const Visualization& visualization) {
    au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
    collection->set_name("last_commits");
    
    std::list<au::CommitRecord>::iterator it;
    for (it = last_commits_.begin(); it != last_commits_.end(); it++) {
      {
        gpb::CollectionRecord *record = collection->add_record();
        ::samson::add(record, "#commit", it->id, "different");
        ::samson::add(record, "Concept", "Caller", "different");
        ::samson::add(record, "Value", it->caller, "different");
      }
      
      std::vector<std::string> components = au::split(it->commit_command, ' ');
      for (size_t i = 0; i < components.size(); i++) {
        gpb::CollectionRecord *record = collection->add_record();
        ::samson::add(record, "#commit", it->id, "different");
        ::samson::add(record, "Concept", "Command", "different");
        ::samson::add(record, "Value", components[i], "different");
      }
      
      {
        gpb::CollectionRecord *record = collection->add_record();
        ::samson::add(record, "#commit", it->id, "different");
        ::samson::add(record, "Concept", "Result", "different");
        ::samson::add(record, "Value", it->result, "different");
      }
      
      {
        gpb::CollectionRecord *record = collection->add_record();
        ::samson::add(record, "#commit", it->id, "different");
        ::samson::add(record, "Concept", "Time", "different");
        ::samson::add(record, "Value", au::str(it->commit_time), "different");
      }
      
    }
    
    return collection;
  }
  
  void DataModel::ReviewBatchOperations(gpb::Data* data, au::ErrorManager&error) {
    
    int operations_size = data->batch_operations_size();
    for (int i = 0; i < operations_size; ++i) {
      gpb::BatchOperation *batch_operation = data->mutable_batch_operations(i);
      
      if (batch_operation->finished()) {
        continue;
      }
      
      bool finished = gpb::batch_operation_is_finished(data, *batch_operation);
      
      if (finished) {
        // Set finished and move data
        batch_operation->set_finished(true);
        
        // Send a message to original delilah
        engine::Notification* notification = new engine::Notification(notification_samson_worker_send_message);
        std::string message = au::str("Batch operation %s_%lu has finished correctly",
                                      au::code64_str(batch_operation->delilah_id()).c_str(),
                                      batch_operation->delilah_component_id());
        notification->environment().Set("message", message);
        notification->environment().Set("context", "system");
        notification->environment().Set("type", "warning");
        notification->environment().Set("delilah_id", batch_operation->delilah_id());
        
        engine::Engine::shared()->notify(notification);
        
        size_t delilah_id = batch_operation->delilah_id();
        size_t delilah_component_id = batch_operation->delilah_component_id();
        std::string prefix = au::str(".%s_%lu_", au::code64_str(delilah_id).c_str(), delilah_component_id);
        
        // Perform all push for output queues
        for (int j = 0; j < batch_operation->outputs_size(); ++j) {
          std::string queue_name = batch_operation->outputs(j);
          std::string final_queue_name = prefix + queue_name;
          std::string command = au::str("push_queue %s %s", final_queue_name.c_str(), queue_name.c_str());
          
          ProcessCommand(data, command , error);
          if (error.IsActivated()) {
            LOG_W(logs.data_model,("Error performing commit command:'%s'  error:'%s'", command.c_str(), error.GetMessage().c_str()));
            return;
          }
        }
      }
    }
  }
}   // namespace samson
