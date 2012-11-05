#include "samson/worker/WorkerCommand.h"     // Own interface

#include <algorithm>

#include "au/ErrorManager.h"
#include "au/string/StringUtilities.h"
#include "au/utils.h"

#include "engine/DiskManager.h"
#include "engine/Notification.h"
#include "engine/ProcessManager.h"

#include "samson/common/EnvironmentOperations.h"        // copyEnvironment
#include "samson/common/SamsonSetup.h"                  // SamsonSetup
#include "samson/delilah/DelilahCommandCatalogue.h"
#include "samson/network/Packet.h"                      // samson::Packet
#include "samson/stream/Block.h"
#include "samson/stream/BlockList.h"
#include "samson/stream/BlockManager.h"
#include "samson/worker/SamsonWorker.h"

namespace samson {

bool ignoreCommand(std::string command) {
  if (command.length() == 0) {
    return true;
  }

  for (size_t i = 0; i < command.length(); i++) {
    if (command[i] != ' ') {
      if (command[i] == '#') {
        return true;
      } else {
        return false;
      }
    }
  }

  return true;
}

class AliasManager {
    au::simple_map<std::string, std::string> aliases;

  public:
    void add(std::string reference, std::string name) {
      aliases.insertInMap(reference, name);
    }

    std::string transform(std::string command) {
      au::simple_map<std::string, std::string>::iterator it_aliases;

      for (it_aliases = aliases.begin(); it_aliases != aliases.end(); it_aliases++) {
        au::FindAndReplaceInString(command, it_aliases->first, it_aliases->second);
      }

      return command;
    }
};

WorkerCommand::WorkerCommand(SamsonWorker* samson_worker, std::string worker_command_id, size_t delilah_id,
                             size_t delilah_component_id, const gpb::WorkerCommand& command) {
  
  AU_D(logs.worker_command, ("New WorkerCommand %s ( Delilah %s : %lu )"
                             , worker_command_id.c_str()
                             , au::code64_str(delilah_id).c_str()
                             , delilah_component_id_));
  
  // Keep pointer to samson worker
  samson_worker_ = samson_worker;

  // Unique identifier of the worker
  worker_command_id_ = worker_command_id;

  // Identifiers to notify when finished
  delilah_id_ = delilah_id;
  delilah_component_id_ = delilah_component_id;

  notify_finish_ = (delilah_id_ != 0);   // 0 used as a non notify delilah

  // Copy the original message
  originalWorkerCommand_.Reset(new gpb::WorkerCommand());
  originalWorkerCommand_->CopyFrom(command);

  // Extract environment properties
  copyEnviroment(originalWorkerCommand_->environment(), &enviroment_);

  // Extract command for simplicity
  command_ = originalWorkerCommand_->command();

  // Original value for the flags
  pending_to_be_executed_ = true;
  finished_ = false;

  // No pending process at the moment
  num_pending_processes_ = 0;
  num_pending_disk_operations_ = 0;
}

WorkerCommand::~WorkerCommand() {
  AU_D(logs.worker_command, ("Destructor WorkerCommand %s", worker_command_id_.c_str() ));
}

bool WorkerCommand::finished() {
  return finished_;
}

void WorkerCommand::RunCommand(std::string command, au::ErrorManager& error) {

  if (ignoreCommand(command)) {
    return;
  }

  AU_M(logs.worker_command, ("[%s] Running command %s", worker_command_id_.c_str() , command.c_str() ));
  
  // Parse command
  au::CommandLine cmd;
  cmd.SetFlagBoolean("clear_inputs");
  cmd.SetFlagBoolean("f");

  cmd.SetFlagBoolean("v");
  cmd.SetFlagBoolean("vv");
  cmd.SetFlagBoolean("vvv");

  cmd.SetFlagBoolean("new");
  cmd.SetFlagBoolean("remove");
  cmd.SetFlagString("prefix", "");
  cmd.Parse(command);

  std::string prefix = cmd.GetFlagString("prefix");

  if (cmd.get_num_arguments() == 0) {
    error.set("No command provided");
    return;
  }

  // Set the main command
  std::string main_command = cmd.get_argument(0);

  if (main_command == "init_stream") {
    if (cmd.get_num_arguments() < 2) {
      error.set(
                 au::str("Not enough parameters for command 'init_stream' ( only %d argument provided )",
                         cmd.get_num_arguments()));
      return;
    }

    std::string operation_name;

    if (cmd.get_num_arguments() == 3) {
      prefix.append(cmd.get_argument(1));
      prefix.append(".");

      operation_name = cmd.get_argument(2);
    } else if (cmd.get_num_arguments() == 2) {
      operation_name = cmd.get_argument(1);
    }

    Operation *op = au::Singleton<ModulesManager>::shared()->getOperation(operation_name);
    if (!op) {
      error.set(au::str("Unknown operation:'%s' in command arguments to init_stream", operation_name.c_str()));
      return;
    }

    if (op->getType() != Operation::script) {
      error.set(
                 au::str("Non valid operation %d. Only script operations supported for init_stream command",
                         operation_name.c_str()));
      return;
    }

    // Structure to set names alias
    AliasManager alias_manager;

    // Read code, execute it recursively
    au::ErrorManager sub_error;
    for (size_t i = 0; i < op->code.size(); i++) {
      std::string sub_command = op->code[i];

      au::CommandLine intern_cmdLine;
      intern_cmdLine.Parse(sub_command);

      if (intern_cmdLine.get_argument(0) == "alias") {
        if (intern_cmdLine.get_num_arguments() < 3) {
          error.set(au::str("Not enough parameters for command %s", main_command.c_str()));
          return;
        }

        std::string reference = intern_cmdLine.get_argument(1);
        std::string value = intern_cmdLine.get_argument(2);

        alias_manager.add(reference, value);
      } else {
        std::string full_command = alias_manager.transform(sub_command);

        if (prefix.length() > 0) {
          full_command.append(au::str(" -prefix %s", prefix.c_str()));
        }
        RunCommand(full_command, sub_error);
      }

      if (sub_error.IsActivated()) {
        error.set(au::str("[%s:%d]%s", operation_name.c_str(), i, sub_error.GetMessage().c_str()));
        return;
      }
    }

    return;
  }

  // If operation can be process by DataMode, go ahead
  if (samson_worker_->data_model()->isValidCommand(main_command)) {
    std::string caller = au::str("Command %s from delilah %s",main_command.c_str(),au::code64_str(delilah_id_).c_str());
    samson_worker_->data_model()->Commit(caller, command, error);
    return;
  }

  // Unknown command error message
  AU_E( logs.worker_command , ("Unknown command %s", main_command.c_str()));
  error.set(au::str("Unknown command %s", main_command.c_str()));
}

bool compare_blocks_defrag(stream::Block *b, stream::Block *b2) {
  KVHeader h1 = b->getHeader();
  KVHeader h2 = b2->getHeader();

  if (h1.range.hg_begin_ < h2.range.hg_begin_) {
    return true;
  }
  if (h1.range.hg_begin_ > h2.range.hg_begin_) {
    return false;
  }

  // If the same
  if (h1.range.hg_end_ > h2.range.hg_end_) {
    return true;
  } else {
    return false;
  }
}

typedef struct LogLineInfo {
    char type;
    std::string date;
    int ms;
    std::string progName;
    std::string fileName;
    int lineNo;
    int pid;
    int tid;
    std::string funcName;
    std::string message;
} LogLineInfo;

void WorkerCommand::Run() {
  // Nothing to to if this is waiting for another thing
  if (!pending_to_be_executed_) {
    return;
  }

  AU_M(logs.worker_command, ("[%s] Run", worker_command_id_.c_str() ));

  pending_to_be_executed_ = false;   // Not pending any more, except if something happen...

  // Parse a delilah command
  DelilahCommandCatalogue delilah_command_catalogue;
  au::console::CommandInstance *command_instance = delilah_command_catalogue.parse(command_, error_);
  if (error_.IsActivated()) {
    return;   // Finish with this error
  }
  // General visualization options
  Visualization visualization;

  // Add all bool flags like -v -state automatically to visualization
  const std::vector<au::console::CommandItem *> options = command_instance->command()->options();
  for (size_t i = 0; i < options.size(); i++) {
    if (options[i]->type() == au::console::options::option_bool) {
      std::string name = options[i]->name();
      visualization.set_flag(name, command_instance->get_bool_option(name));
    }
  }

  // Add argument pattern if exist
  if (command_instance->has_string_argument("pattern")) {
    visualization.set_pattern(command_instance->get_string_argument("pattern"));   // Get main command
  }
  std::string main_command = command_instance->main_command();

  // TODO(@jges): Remove log message
  AU_D( logs.worker_command, ("Processing '%s' command", main_command.c_str()));

  if (main_command == "ls") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->data_model()->GetCollectionForQueues(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }
  
  if( main_command == "data_model_status")
  {
    au::SharedPointer<gpb::Collection> c = samson_worker_->GetCollectionForDataModelStatus(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if( main_command == "data_model_commits")
  {
    au::SharedPointer<gpb::Collection> c = samson_worker_->GetCollectionForDataModelCommits(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }
  
  if (main_command == "ls_queue_ranges") {
    std::string queue_name = command_instance->get_string_argument("name");
    au::SharedPointer<gpb::Collection> c = samson_worker_->data_model()->GetCollectionForQueueRanges(visualization,
                                                                                                     queue_name);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_queue_blocks") {
    au::SharedPointer<gpb::Collection> c =
        samson_worker_->data_model()->GetCollectionForQueuesWithBlocks(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_last_data_commits") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->data_model()->GetLastCommitsCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_last_tasks") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->task_manager()->GetLastTasksCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_block_requests") {
    au::SharedPointer<gpb::Collection> c =
        samson_worker_->worker_block_manager()->GetCollectionForBlockRequests(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_blocks") {
    au::SharedPointer<gpb::Collection> c = stream::BlockManager::shared()->GetCollectionOfBlocks(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_buffers") {
    au::SharedPointer<gpb::Collection> c = GetCollectionOfBuffers(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_pop_connections") {
    /*
     * gpb::Collection * c = streamManager->GetCollectionForPopConnections(&visualitzation);
     * c->set_title( command  );
     * collections_.push_back( c );
     */
    error_.set("Unimplemented");
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_stream_operations") {
    au::SharedPointer<gpb::Collection> c =
        samson_worker_->data_model()->GetCollectionForStreamOperations(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_batch_operations") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->data_model()->GetCollectionForBatchOperations(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ps_stream_operations_ranges") {
    au::SharedPointer<gpb::Collection> c =
    samson_worker_->task_manager()->GetCollectionForStreamOperationsRanges(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ps_stream_operations") {
    au::SharedPointer<gpb::Collection> c =
    samson_worker_->task_manager()->GetCollectionForStreamOperations(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_queue_connections") {
    au::SharedPointer<gpb::Collection> c =
        samson_worker_->data_model()->GetCollectionForQueueConnections(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ps_tasks") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->task_manager()->GetCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_workers") {
    // Add the ps stream
    au::SharedPointer<gpb::Collection> c = samson_worker_->GetWorkerCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }
  
  if( main_command == "wlog_status") {
    // Add the ps stream
    au::SharedPointer<gpb::Collection> c = samson_worker_->GetWorkerLogStatus(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if( main_command == "wlog_set") {
    
    std::string channel_pattern_string = command_instance->get_string_argument("channel_pattern");
    std::string str_log_level = command_instance->get_string_argument("log_level");

    au::log_central.evalCommand("log_set "+channel_pattern_string+" "+str_log_level+" server");
    FinishWorkerTask();
    return;
  }

  if( main_command == "wlog_set_log_server") {
    std::string host = command_instance->get_string_argument("host");

    au::log_central.RemovePlugin( "server" );
    au::log_central.AddServerPlugin( "server" , host , host + "_local_log.log" );
    au::log_central.evalCommand("log_set * X server");
    au::log_central.evalCommand("log_set samson::W M server");
    au::log_central.evalCommand("log_set samson::OP W server");
    
    
    FinishWorkerTask();
    return;
  }
  
  
  if (main_command == "ls_modules") {
    au::SharedPointer<gpb::Collection> c = au::Singleton<ModulesManager>::shared()->GetModulesCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }
  if (main_command == "ls_operations") {
    au::SharedPointer<gpb::Collection> c =
        au::Singleton<ModulesManager>::shared()->GetOperationsCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }
  if (main_command == "ls_datas") {
    au::SharedPointer<gpb::Collection> c = au::Singleton<ModulesManager>::shared()->GetDatasCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_connections") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->network()->GetConnectionsCollection(visualization);
    if (c != NULL) {
      c->set_title(command_);
      collections_.push_back(c);
    }
    FinishWorkerTask();
    return;
  }

  if (main_command == "ps_workers") {
    au::SharedPointer<gpb::Collection> c =
        samson_worker_->workerCommandManager()->GetCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "wait") {
    // Recovering old wait command
    if (samson_worker_->data_model()->CheckForAllOperationsFinished() == false) {
      pending_to_be_executed_ = true;
      return;
    }

    // Nothing else to be waited
    FinishWorkerTask();
    return;
  }

  if (main_command == "send_alert") {
    std::string message = command_instance->get_string_argument("message");
    bool error = command_instance->get_bool_option("-error");
    bool warning = command_instance->get_bool_option("-error");

    // Full message
    std::string full_message = au::str("[Alert from Delilah_%s] %s", au::code64_str(delilah_id_).c_str(),
                                       message.c_str());

    // Send a trace to all delilahs
    if (error) {
      samson_worker_->network()->SendAlertToAllDelilahs("error", "delilah", full_message);
    } else if (warning) {
      samson_worker_->network()->SendAlertToAllDelilahs("warning", "delilah", full_message);
    } else {
      samson_worker_->network()->SendAlertToAllDelilahs("message", "delilah", full_message);
    }
    FinishWorkerTask();
    return;
  }

  if (main_command == "cancel_stream_operation") {
    error_.set("Unimplemented");
    FinishWorkerTask();
    return;

    /*
     * if( cmd.get_num_arguments() < 2 )
     * {
     * FinishWorkerTaskWithError( au::str("Not enough parameters for command %s\nUsage: cancel_stream_operation operation_id" , main_command.c_str() ) );
     * return;
     * }
     *
     * std::string canceled_worker_command_id = cmd.get_argument(1);
     *
     * // Emit a cancellation notification to cancel as much cas possible
     * engine::Notification * notification = new engine::Notification("cancel");
     * notification->environment.set("id", canceled_worker_command_id );
     *
     * // This is a stream operation ( not return error ;) )
     * std::string prefix = "stream_";
     * if( canceled_worker_command_id.substr( 0 , prefix.length() ) == prefix )
     * FinishWorkerTask();
     *
     * // Remove in the worker command manager
     * if( samson_worker_->workerCommandManager->cancel( canceled_worker_command_id ) )
     * FinishWorkerTask();
     * else
     * FinishWorkerTaskWithError(au::str("Worker command %s not found" , canceled_worker_command_id.c_str() ));
     */
    return;
  }

  if (main_command == "run") {
    std::string operation = command_instance->get_string_argument("operation");
    std::string inputs = command_instance->get_string_option("input");
    std::string outputs = command_instance->get_string_option("output");

    std::string command = au::str("batch %s -input \"%s\" -output \"%s\" -delilah_id %lu -delilah_component_id %lu ",
                                  operation.c_str(), inputs.c_str(), outputs.c_str(), delilah_id_,
                                  delilah_component_id_);

    au::ErrorManager error;
    std::string caller = au::str("run_deliah_%s_%lu", au::code64_str(delilah_id_).c_str(), delilah_component_id_);
    samson_worker_->data_model()->Commit(caller, command, error);

    if (error.IsActivated()) {
      AU_E( logs.worker_command,("Error in Commit for command:'%s', error:'%s'", command.c_str(), error.GetMessage().c_str()));
      FinishWorkerTaskWithError(error.GetMessage());
    } else {
      FinishWorkerTask();
    }
    return;
  }

  // Simple commands
  au::ErrorManager error;
  RunCommand(command_, error_);
  if (error.IsActivated()) {
    AU_E( logs.worker_command,("Error in Commit for command:'%s', error:'%s'", command_.c_str(), error.GetMessage().c_str()));
    FinishWorkerTaskWithError(error.GetMessage());
  } else {
    FinishWorkerTask();
  }
}

void WorkerCommand::FinishWorkerTaskWithError(std::string error_message) {

  AU_D(logs.worker_command, ("[%s] Finished with error %s ", worker_command_id_.c_str() , error_message.c_str() ));
  
  error_.set(error_message);
  FinishWorkerTask();

  // Notify everything so it is automatically canceled
  engine::Notification *notification = new engine::Notification("cancel");
  notification->environment().Set("id", worker_command_id_);
  engine::Engine::shared()->notify(notification);
}

void WorkerCommand::FinishWorkerTask() {
  if (finished_) {
    // If this worker command is finished, not do anything else again.
    return;
  }

  AU_D(logs.worker_command, ("[%s] Finished OK", worker_command_id_.c_str() ));
  
  if (notify_finish_) {
    AU_D( logs.worker_command, ("notify_finish for command:'%s', delilah_id:%lu", command_.c_str(), delilah_id_));
    PacketPointer p(new Packet(Message::WorkerCommandResponse));
    gpb::WorkerCommandResponse *c = p->message->mutable_worker_command_response();
    c->mutable_worker_command()->CopyFrom(*originalWorkerCommand_);

    // Put the error if any
    if (error_.IsActivated()) {
      c->mutable_error()->set_message(error_.GetMessage());
    }
    // Set delilah id
    p->message->set_delilah_component_id(delilah_component_id_);

    // Direction of this packets
    p->to.node_type = DelilahNode;
    p->to.id = delilah_id_;

    // Add collections as answers...
    for (size_t i = 0; i < collections_.size(); i++) {
      p->message->add_collection()->CopyFrom(*collections_[i]);
    }

    // Send the packet
    samson_worker_->network()->Send(p);
  }

  // Set the finished flag
  finished_ = true;
}

void WorkerCommand::notify(engine::Notification *notification) {
  if (notification->isName(notification_process_request_response)) {
    
    AU_D( logs.worker_command , ("Notification about finished process"));
    --num_pending_processes_;
    if (notification->environment().IsSet("error")) {
      error_.set(notification->environment().Get("error", "no_error"));
    }
    CheckFinish();
    return;
  }

  AU_W( logs.worker_command , ("Unexpected notification at WorkerCommand"));
}

void WorkerCommand::CheckFinish() {
  if (error_.IsActivated()) {
    FinishWorkerTask();
  } else if ((num_pending_processes_ <= 0) && (num_pending_disk_operations_ <= 0)) {
    FinishWorkerTask();
  }
}

void WorkerCommand::fill(samson::gpb::CollectionRecord *record, const Visualization& visualization) {
  std::string name = NodeIdentifier(DelilahNode, delilah_id_).getCodeName();

  add(record, "id", worker_command_id_, "left,different");

  if (finished_) {
    add(record, "status", "finished", "left,different");
  } else {
    add(record, "status", "running", "left,different");
  }
  add(record, "command", command_, "left,different");
  add(record, "#operations", num_pending_processes_, "left,uint64,sum");
  add(record, "#disk_operations", num_pending_disk_operations_, "left,uint64,sum");
  add(record, "error", error_.GetMessage(), "left,different");
}

au::SharedPointer<gpb::Collection> WorkerCommand::GetCollectionOfBuffers(const Visualization& visualization) {
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name("buffers");

  au::tables::Table table = engine::Engine::memory_manager()->getTableOfBuffers();

  // Debug
  // printf("%s\n" , table.str().c_str() );

  for (size_t r = 0; r < table.getNumRows(); r++) {
    gpb::CollectionRecord *record = collection->add_record();

    for (size_t c = 0; c < table.getNumColumns(); c++) {
      std::string concept = table.getColumn(c);
      std::string value = table.getValue(r, c);
      ::samson::add(record, concept, value, "left");
    }
  }

  return collection;
}
  
  std::string WorkerCommand::str()
  {
    std::ostringstream output;
    output << worker_command_id_ << " " << command_;
    return output.str();
  }

}
