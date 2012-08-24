
#include <algorithm>

#include "au/ErrorManager.h"
#include "au/log/LogToServer.h"
#include "au/string.h"
#include "au/utils.h"

#include "engine/DiskManager.h"
#include "engine/Notification.h"
#include "engine/ProcessManager.h"


#include "samson/common/EnvironmentOperations.h"        // copyEnvironment
#include "samson/common/SamsonSetup.h"                  // SamsonSetup

#include "samson/network/Packet.h"                      // samson::Packet

#include "samson/worker/PushManager.h"
#include "samson/worker/SamsonWorker.h"

#include "samson/stream/BlockList.h"
#include "samson/stream/BlockManager.h"
#include "samson/stream/StreamOperationInfo.h"

#include "samson/stream/Block.h"
#include "samson/stream/BlockList.h"

#include "samson/delilah/DelilahCommandCatalogue.h"

#include "WorkerCommand.h"     // Own interface

namespace samson {
class KVRangeAndSize {
public:

  KVRange range;
  size_t size;

  KVRangeAndSize(KVRange _range, size_t _size) {
    range = _range;
    size = _size;
  }
};

class KVRangeAndSizeManager {
public:

  std::vector<KVRangeAndSize> items;    // Input information
  std::vector<KVRange> ranges;          // Output ranges

  au::ErrorManager error;               // Error management

  void compute_ranges(size_t max_size) {
    int num_cores = au::Singleton<SamsonSetup>::shared()->getInt("general.num_processess");

    int hg_begin = 0;
    int hg_end = 0;

    while (true) {
      if (hg_begin >= KVFILE_NUM_HASHGROUPS) {
        break;   // No more ranges
      }
      while (
        (hg_end < KVFILE_NUM_HASHGROUPS)
        &&
        (compute_size(KVRange(hg_begin, hg_end + 1)) < max_size )
        && ((hg_end - hg_begin) < (KVFILE_NUM_HASHGROUPS / num_cores))       // Distribute small reducers...
        )
      {
        hg_end++;
      }

      if (hg_begin == hg_end) {
        // Error sice it is not possible to include this hash-group alone
        size_t size = compute_size(KVRange(hg_begin, hg_begin + 1));
        error.set(au::str("Not possible to process hash-group %d ( %s > %s ). Please degrag input queues"
                          , hg_begin
                          , au::str(size).c_str()
                          , au::str(max_size).c_str()));
        return;
      }

      // Create a new set
      KVRange r(hg_begin, hg_end);
      ranges.push_back(r);

      // Next hash-groups
      hg_begin = hg_end;
      hg_end = hg_begin;
    }
  }

  size_t compute_size(KVRange range) {
    size_t total = 0;

    for (size_t i = 0; i < items.size(); i++) {
      if (items[i].range.IsOverlapped(range)) {
        total += items[i].size;
      }
    }
    return total;
  }
};

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
      au::find_and_replace(command, it_aliases->first, it_aliases->second);
    }

    return command;
  }
};

WorkerCommand::WorkerCommand(std::string _worker_command_id
                             , size_t _delilah_id
                             , size_t _delilah_component_id
                             ,  const gpb::WorkerCommand& _command) {
  // Unique identifier of the worker
  worker_command_id = _worker_command_id;

  samsonWorker = NULL;

  // Identifiers to notify when finished
  delilah_id = _delilah_id;
  delilah_component_id = _delilah_component_id;

  notify_finish = ( _delilah_id != 0 );   // 0 used as a non notify delilah

  // Copy the original message
  originalWorkerCommand = new gpb::WorkerCommand();
  originalWorkerCommand->CopyFrom(_command);

  // Extract environment properties
  copyEnviroment(originalWorkerCommand->environment(), &enviroment);

  // Extract command for simplicity
  command = originalWorkerCommand->command();

  // Original value for the flags
  pending_to_be_executed =  true;
  finished = false;

  // No pending process at the moment
  num_pending_processes = 0;
  num_pending_disk_operations = 0;
}

WorkerCommand::~WorkerCommand() {
  if (originalWorkerCommand) {
    delete originalWorkerCommand;  // Remove collections created for this command
  }
  collections.clearVector();
}

void WorkerCommand::setSamsonWorker(SamsonWorker *_samsonWorker) {
  samsonWorker = _samsonWorker;
}

bool WorkerCommand::isFinished() {
  return finished;
}

void WorkerCommand::runCommand(std::string command, au::ErrorManager *error) {
  // LM_M(("WC Running command '%s'" , command.c_str() ));

  if (ignoreCommand(command)) {
    return;
  }

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
    error->set("No command provided");
    return;
  }

  // Set the main command
  std::string main_command = cmd.get_argument(0);

  if (main_command == "init_stream") {
    if (cmd.get_num_arguments() < 2) {
      error->set(au::str("Not enough parameters for command 'init_stream' ( only %d argument provided )",
                         cmd.get_num_arguments()));
      return;
    }

    std::string operation_name;

    if (cmd.get_num_arguments() == 3) {
      prefix.append(cmd.get_argument(1));
      prefix.append(".");

      operation_name  = cmd.get_argument(2);
    } else if (cmd.get_num_arguments() == 2) {
      operation_name  = cmd.get_argument(1);
    }

    Operation *op = ModulesManager::shared()->getOperation(operation_name);
    if (!op) {
      error->set(au::str("Unknown operation:'%s' in command arguments to init_stream", operation_name.c_str()));
      return;
    }

    if (op->getType() != Operation::script) {
      error->set(
        au::str("Non valid operation %d. Only script operations supported for init_stream command"
                , operation_name.c_str()));
      return;
    }


    // Structure to set names alias
    AliasManager alias_manager;

    // Read code, execute it recursivelly
    au::ErrorManager sub_error;
    for (size_t i = 0; i < op->code.size(); i++) {
      std::string sub_command = op->code[i];


      au::CommandLine intern_cmdLine;
      intern_cmdLine.Parse(sub_command);

      if (intern_cmdLine.get_argument(0) == "alias") {
        if (intern_cmdLine.get_num_arguments() < 3) {
          error->set(au::str("Not enough parameters for command %s", main_command.c_str()));
          return;
        }

        std::string reference = intern_cmdLine.get_argument(1);
        std::string value = intern_cmdLine.get_argument(2);

        // LM_M(("Alias %s=%s", reference.c_str() , value.c_str() ));

        alias_manager.add(reference, value);
      } else {
        std::string full_command = alias_manager.transform(sub_command);

        if (prefix.length() > 0) {
          full_command.append(au::str(" -prefix %s", prefix.c_str()));  // LM_M(("Full Command %s (original %s)" , full_command.c_str(),  sub_command.c_str() ));
        }
        runCommand(full_command, &sub_error);
      }

      if (sub_error.IsActivated()) {
        error->set(au::str("[%s:%d]%s", operation_name.c_str(), i, sub_error.GetMessage().c_str()));
        return;
      }
    }

    return;
  }

  if (main_command == "set_log_server") {
    if (cmd.get_num_arguments() < 2) {
      error->set("Usage: set_log_server host [port]");
      return;
    }

    if (cmd.get_num_arguments() == 2) {
      std::string host = cmd.get_argument(1);
      au::set_log_server(host);
      return;
    }

    if (cmd.get_num_arguments() > 2) {
      std::string host = cmd.get_argument(1);
      int port = atoi(cmd.get_argument(2).c_str());
      au::set_log_server(host, port);
      return;
    }
  }

  if (main_command == "reload_modules") {
    // Spetial operation to reload modules
    ModulesManager::shared()->reloadModules();
    return;
  }

  // Command over data model
  // ls
  // push_queue
  // set_queue_property
  // unset_queue_property
  // add_stream_operation
  // rm_stream_operation
  // set_stream_operation_property
  // unset_stream_operation_property
  // add_queue_connection ( pending )
  // rm_queue_connection ( pending )

  if (samsonWorker->data_model()->isValidCommand(main_command)) {
    std::string caller = au::str("Command %s from delilah %s"
                                 , main_command.c_str()
                                 , au::code64_str(delilah_id).c_str());

    samsonWorker->data_model()->Commit(caller, command, error);
    return;
  }

  // Unknown command error message
  error->set(au::str("Unknown command %s", main_command.c_str()));
}

bool compare_blocks_defrag(stream::Block *b, stream::Block *b2) {
  KVHeader h1 = b->getHeader();
  KVHeader h2 = b2->getHeader();

  if (h1.range.hg_begin < h2.range.hg_begin) {
    return true;
  }
  if (h1.range.hg_begin > h2.range.hg_begin) {
    return false;
  }

  // If the same
  if (h1.range.hg_end > h2.range.hg_end) {
    return true;
  } else {
    return false;
  }
}

static bool logFilter
(
  const char *filter,
  const char *all,
  char type,
  const char *fileName
) {
  std::string item;
  char *value;
  char *eq;
  char filterCopy[512];

  strncpy(filterCopy, filter, sizeof(filterCopy));

  eq = strchr((char *)filterCopy, '=');
  if (eq != NULL) {
    *eq = 0;
    ++eq;
    item  = std::string(filterCopy);
    value = eq;
  } else {
    item  = std::string("XXXXX_NO_ITEM");
    value = (char *)filterCopy;
  }

  // No item - grep in all
  if (item == "XXXXX_NO_ITEM") {
    if (strstr(all, value) != NULL) {
      return true;
    }
    return false;
  }

  if (item == "Type") {
    if (strlen(value) != 1) {
      return false;
    }

    if (value[0] != type) {
      return false;
    }

    return true;
  }

  if (item == "FileName") {
    if (strncmp(value, fileName, strlen(value)) == 0) {
      return true;
    }

    return false;
  }

  return false;
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

void WorkerCommand::run() {
  // Nothing to to if this is waiting for another thing
  if (!pending_to_be_executed) {
    return;
  }

  pending_to_be_executed = false;   // Not pending any more, except if something happen...

  // Parse a delilah command
  DelilahCommandCatalogue delilah_command_catalogue;
  au::console::CommandInstance *command_instance = delilah_command_catalogue.parse(command, &error);
  if (error.IsActivated()) {
    return;   // Finish with this error
  }
  // General visualization options
  Visualization visualization;

  // Add all bool falgs like -v -state automatically
  const std::vector<au::console::CommandItem *> options = command_instance->command()->options();
  for (size_t i = 0; i < options.size(); i++) {
    if (options[i]->type() == au::console::options::option_bool) {
      std::string name = options[i]->name();
      visualization.set_flag(name, command_instance->get_bool_option(name));
    }
  }

  // Add argument pattern if exist
  if (command_instance->has_string_argument("pattern")) {
    visualization.set_pattern(command_instance->get_string_argument("pattern"));  // Get main command
  }
  std::string main_command = command_instance->main_command();

  /*
   *
   * if ( main_command == "push_module" )
   * {
   * engine::BufferPointer buffer = buffer_container.buffer();
   * if ( !buffer )
   * {
   * finishWorkerTaskWithError("No data provided for this module");
   * return;
   * }
   *
   * // Destination file
   * std::string file_name = au::str("%s/lib%s.so"
   * , au::Singleton<SamsonSetup>::shared()->modulesDirectory().c_str()
   * , cmd.get_argument(1).c_str()
   * );
   *
   * engine::DiskOperation *operation = engine::DiskOperation::newWriteOperation(buffer, file_name, engine_id() );
   * operation->environment.set("push_module", "yes");
   * engine::Engine::disk_manager()->add( operation );
   * operation->Release(); // It is now retained by disk manager
   *
   * num_pending_disk_operations++;
   *
   * // Buffer will be destroyed by the disk operation
   * buffer = NULL;
   *
   * return;
   * }
   */

  // Query commands
  if (main_command == "ls") {
    gpb::Collection *c = samsonWorker->data_model()->getCollectionForQueues(visualization);
    c->set_title(command);
    collections.push_back(c);
    finishWorkerTask();
    return;
  }

  if (main_command == "ls_queue_blocks") {
    gpb::Collection *c = samsonWorker->data_model()->getCollectionForQueuesWithBlocks(visualization);
    c->set_title(command);
    collections.push_back(c);
    finishWorkerTask();
    return;
  }

  if (main_command == "ls_data_commits") {
    gpb::Collection *c =  samsonWorker->data_model()->getLastCommitsCollection(visualization);
    c->set_title(command);
    collections.push_back(c);
    finishWorkerTask();
    return;
  }

  if (main_command == "ls_last_tasks") {
    gpb::Collection *c =  samsonWorker->task_manager()->getLastTasksCollection(visualization);
    c->set_title(command);
    collections.push_back(c);
    finishWorkerTask();
    return;
  }


  if (main_command == "ls_push_operations") {
    gpb::Collection *c = samsonWorker->push_manager()->getCollectionForPushOperations(visualization);
    c->set_title(command);
    collections.push_back(c);
    finishWorkerTask();
    return;
  }

  if (main_command == "ls_block_distribution") {
    gpb::Collection *c = samsonWorker->distribution_blocks_manager()->GetCollectionForDistributionBlocks(visualization);
    c->set_title(command);
    collections.push_back(c);
    finishWorkerTask();
    return;
  }

  if (main_command == "ls_block_requests") {
    gpb::Collection *c = samsonWorker->distribution_blocks_manager()->GetCollectionForBlockRequests(visualization);
    c->set_title(command);
    collections.push_back(c);
    finishWorkerTask();
    return;
  }


  if (main_command == "ls_blocks") {
    gpb::Collection *c = stream::BlockManager::shared()->getCollectionOfBlocks(visualization);
    c->set_title(command);
    collections.push_back(c);
    finishWorkerTask();
    return;
  }

  if (main_command == "ls_buffers") {
    gpb::Collection *c = getCollectionOfBuffers(visualization);
    c->set_title(command);
    collections.push_back(c);
    finishWorkerTask();
    return;
  }

  if (main_command == "ls_pop_connections") {
    /*
     * gpb::Collection * c = streamManager->getCollectionForPopConnections(&visualitzation);
     * c->set_title( command  );
     * collections.push_back( c );
     */
    error.set("Unimplemented");
    finishWorkerTask();
    return;
  }

  if (main_command == "ls_stream_operations") {
    gpb::Collection *c;
    c = samsonWorker->data_model()->getCollectionForStreamOperations(visualization);
    c->set_title(command);
    collections.push_back(c);
    finishWorkerTask();
    return;
  }

  if (main_command == "ls_batch_operations") {
    gpb::Collection *c;
    c = samsonWorker->data_model()->getCollectionForBatchOperations(visualization);
    c->set_title(command);
    collections.push_back(c);
    finishWorkerTask();
    return;
  }


  if (main_command == "ps_stream_operations") {
    gpb::Collection *c;
    c = samsonWorker->task_manager()->getCollectionForStreamOperationsInfo(visualization);
    c->set_title(command);
    collections.push_back(c);
    finishWorkerTask();
    return;
  }


  if (main_command == "ls_queue_connections") {
    gpb::Collection *c = samsonWorker->data_model()->getCollectionForQueueConnections(visualization);
    c->set_title(command);
    collections.push_back(c);

    finishWorkerTask();
    return;
  }

  if (main_command == "ps_tasks") {
    gpb::Collection *c = samsonWorker->task_manager()->getCollection(visualization);
    c->set_title(command);
    collections.push_back(c);
    finishWorkerTask();
    return;
  }

  if (main_command == "ls_workers") {
    // Add the ps stream
    gpb::Collection *c = samsonWorker->getWorkerCollection(visualization);
    c->set_title(command);
    collections.push_back(c);
    finishWorkerTask();
    return;
  }

  if (main_command == "ls_modules") {
    gpb::Collection *c = ModulesManager::shared()->getModulesCollection(visualization);
    c->set_title(command);
    collections.push_back(c);
    finishWorkerTask();
    return;
  }
  if (main_command == "ls_operations") {
    gpb::Collection *c = ModulesManager::shared()->getOperationsCollection(visualization);
    c->set_title(command);
    collections.push_back(c);
    finishWorkerTask();
    return;
  }
  if (main_command == "ls_datas") {
    gpb::Collection *c = ModulesManager::shared()->getDatasCollection(visualization);
    c->set_title(command);
    collections.push_back(c);
    finishWorkerTask();
    return;
  }

  if (main_command == "ls_connections") {
    gpb::Collection *c = samsonWorker->network()->getConnectionsCollection(visualization);
    if (c) {
      c->set_title(command);
      collections.push_back(c);
    }
    finishWorkerTask();
    return;
  }

  if (main_command == "ps_workers") {
    gpb::Collection *c = samsonWorker->workerCommandManager()->getCollectionOfWorkerCommands(visualization);
    c->set_title(command);
    collections.push_back(c);
    finishWorkerTask();
    return;
  }


  if (main_command == "wait") {
    error.set("Unimplemented");

    /*
     *
     * // Special operation to wait until no activity is present in stream manager
     * if( streamManager->WorkerTaskManager.isActive() )
     * {
     * pending_to_be_executed = true;
     * return;
     * }
     *
     * // No pending data to be processed
     * if( streamManager->isSomethingPending() )
     * {
     * pending_to_be_executed = true;
     * return;
     * }
     *
     */

    // Nothing else to be waited
    finishWorkerTask();
    return;
  }

  if (main_command == "log") {
    char type;
    char date[64];
    int ms;
    char progName[64];
    char fileName[64];
    int lineNo;
    int pid;
    int tid;
    char funcName[64];
    char message[256];
    int hits   = 0;
    std::list<LogLineInfo>        lines;
    LogLineInfo logLine;
    std::string filter     = command_instance->get_string_argument("pattern");
    int maxLines   = command_instance->get_int_option("lines");
    long lmPos      = 0;
    char *all        = NULL;

    while (1) {  // Until lmLogLineGet returns EOF (-2)
      if (all != NULL) {  // strdup and free - a little slow ... I might replace this mechanism with a char[] ...
        free(all);
      }
      lmPos = lmLogLineGet(&type, date, &ms, progName, fileName, &lineNo, &pid, &tid, funcName, message, lmPos,
                           &all);
      if (lmPos < 0) {
        if (all != NULL) {
          free(all);
        }
        break;
      }

      if (filter != "no-argument") {
        if (logFilter(filter.c_str(), all, type, fileName) == false) {
          continue;
        }
      }
      logLine.type          = type;
      logLine.date          = date;
      logLine.ms            = ms;
      logLine.progName      = progName;
      logLine.fileName      = fileName;
      logLine.lineNo        = lineNo;
      logLine.pid           = pid;
      logLine.tid           = tid;
      logLine.funcName      = funcName;
      logLine.message       = message;

      lines.push_front(logLine);

      ++hits;
      if (maxLines != 0) {
        if ((int)lines.size() > maxLines) {
          lines.pop_back();
        }
      }
    }

    // Close fP for log file peeking ...
    lmPos = lmLogLineGet(NULL, (char *)date, &ms, progName, fileName, &lineNo, &pid, &tid, funcName, message, lmPos,
                         NULL);

    samson::gpb::Collection *collection = new samson::gpb::Collection();
    collection->set_name("Log File Lines");
    collection->set_title("Log File Lines");

    while (lines.size() > 0) {
      samson::gpb::CollectionRecord *record = collection->add_record();

      logLine = lines.back();
      lines.pop_back();

      ::samson::add(record, "Type",         logLine.type,     "left,different");
      ::samson::add(record, "Date",         logLine.date,     "left,different");
      ::samson::add(record, "Milliseconds", logLine.ms,       "left,different");
      ::samson::add(record, "ProgramName",  logLine.progName, "left,different");
      ::samson::add(record, "FileName",     logLine.fileName, "left,different");
      ::samson::add(record, "LineNo",       logLine.lineNo,   "left,different");
      ::samson::add(record, "PID",          logLine.pid,      "left,different");
      ::samson::add(record, "TID",          logLine.tid,      "left,different");
      ::samson::add(record, "Function",     logLine.funcName, "left,different");
      ::samson::add(record, "Message",      logLine.message,  "left,different");
    }

    collections.push_back(collection);

    finishWorkerTask();
    return;
  }

  if (main_command == "wlog") {
    int vLevel;
    if (lmVerbose5 == true) {
      vLevel = 5;
    } else if (lmVerbose4 == true) {
      vLevel = 4;
    } else if (lmVerbose3 == true) {
      vLevel =
        3;
    } else if (lmVerbose2 == true) {
      vLevel = 2;
    } else if (lmVerbose  == true) {
      vLevel = 1;
    } else {  vLevel = 0; }
    samson::gpb::Collection *collection = new  samson::gpb::Collection();
    collection->set_title("Logs");
    collection->set_name("logs");
    samson::gpb::CollectionRecord *record = collection->add_record();
    ::samson::add(record, "Verbose Level", vLevel, "left,different");
    collections.push_back(collection);

    if (lmDebug) {
      ::samson::add(record, "Debug", "on", "left,different");
    } else {
      ::samson::add(record, "Debug", "off", "left,different");
    } if (lmReads) {
      ::samson::add(record, "Reads", "on", "left,different");
    } else {
      ::samson::add(record, "Reads", "off", "left,different");
    } if (lmWrites) {
      ::samson::add(record, "Writes", "on", "left,different");
    } else {
      ::samson::add(record, "Writes", "off", "left,different");
    } char levels[1024];
    lmTraceGet(levels);

    ::samson::add(record, "Trace levels", levels, "left,different");

    finishWorkerTask();
  }


  if (main_command == "wverbose") {
    std::string subcommand;

    subcommand = command_instance->get_string_argument("action");

    LM_M(("Got a wverbose command: '%s'", subcommand.c_str()));

    if ((subcommand == "off") || (subcommand == "0")) {
      lmVerbose  = false;
      lmVerbose2 = false;
      lmVerbose3 = false;
      lmVerbose4 = false;
      lmVerbose5 = false;
    } else if ((subcommand == "1") || (subcommand == "2") || (subcommand == "3") || (subcommand == "4") ||
               (subcommand == "5"))
    {
      int level = subcommand.c_str()[0] - '0';

      lmVerbose  = false;
      lmVerbose2 = false;
      lmVerbose3 = false;
      lmVerbose4 = false;
      lmVerbose5 = false;

      switch (level) {
        case 5: lmVerbose5 = true;
        case 4: lmVerbose4 = true;
        case 3: lmVerbose3 = true;
        case 2: lmVerbose2 = true;
        case 1: lmVerbose  = true;
      }

      LM_V(("Verbose level 1 is ON"));
      LM_V2(("Verbose level 2 is ON"));
      LM_V3(("Verbose level 3 is ON"));
      LM_V4(("Verbose level 4 is ON"));
      LM_V5(("Verbose level 5 is ON"));
    } else if (subcommand == "get") {
      int vLevel;

      if (lmVerbose5 == true) {
        vLevel = 5;
      } else if (lmVerbose4 == true) {
        vLevel = 4;
      } else if (lmVerbose3 ==
                 true)
      {
        vLevel = 3;
      } else if (lmVerbose2 == true) {
        vLevel = 2;
      } else if (lmVerbose  == true) {
        vLevel = 1;
      } else { vLevel = 0; }
      samson::gpb::Collection *collection = new  samson::gpb::Collection();
      collection->set_title("Verbose");
      collection->set_name("verbose");
      samson::gpb::CollectionRecord *record = collection->add_record();
      ::samson::add(record, "Verbose Level", vLevel, "left,different");
      collections.push_back(collection);
    } else {
      finishWorkerTaskWithError("Usage: verbose [ off ] [ 0 - 5 ]");
      return;
    }

    finishWorkerTask();
    return;
  }

  if (main_command == "wdebug") {
    std::string subcommand = command_instance->get_string_argument("action");

    if (subcommand == "off") {
      lmDebug = false;
      LM_F(("DEBUG is turned OFF"));
      LM_D(("DEBUG is STILL ON - this is a bug"));
    } else if (subcommand == "on") {
      lmDebug = true;
      LM_F(("DEBUG is turned ON"));
      LM_D(("DEBUG is ON"));
    } else if (subcommand == "get") {
      samson::gpb::Collection *collection = new  samson::gpb::Collection();
      collection->set_title("Debug");
      collection->set_name("debug_levels");
      samson::gpb::CollectionRecord *record = collection->add_record();

      if (lmDebug) {
        ::samson::add(record, "Debug", "on", "left,different");
      } else {
        ::samson::add(record, "Debug", "off", "left,different");
      } collections.push_back(collection);
    } else {
      finishWorkerTaskWithError("Usage: wdebug [ off | on ]");
      return;
    }

    finishWorkerTask();
    return;
  }

  if (main_command == "wreads") {
    std::string subcommand = command_instance->get_string_argument("action");

    if (subcommand == "off") {
      lmReads = false;
      LM_F(("READS OFF"));
    } else if (subcommand == "on") {
      lmReads = true;
      LM_F(("READS ON"));
    } else if (subcommand == "get") {
      samson::gpb::Collection *collection = new  samson::gpb::Collection();
      collection->set_title("Reads");
      collection->set_name("reads_levels");
      samson::gpb::CollectionRecord *record = collection->add_record();

      if (lmReads) {
        ::samson::add(record, "Reads", "on", "left,different");
      } else {
        ::samson::add(record, "Reads", "off", "left,different");
      } collections.push_back(collection);
    } else {
      finishWorkerTaskWithError("Usage: wreads [ off | on ]");
      return;
    }

    finishWorkerTask();
    return;
  }

  if (main_command == "wwrites") {
    std::string subcommand = command_instance->get_string_argument("action");

    if (subcommand == "off") {
      lmWrites = false;
      LM_F(("WRITES OFF"));
    } else if (subcommand == "on") {
      lmWrites = true;
      LM_F(("WRITES ON"));
    } else if (subcommand == "get") {
      samson::gpb::Collection *collection = new  samson::gpb::Collection();
      collection->set_title("Writes");
      collection->set_name("writes_levels");
      samson::gpb::CollectionRecord *record = collection->add_record();

      if (lmWrites) {
        ::samson::add(record, "Writes", "on", "left,different");
      } else {
        ::samson::add(record, "Writes", "off", "left,different");
      } collections.push_back(collection);
    } else {
      finishWorkerTaskWithError("Usage: wwrites [ off | on ]");
      return;
    }

    finishWorkerTask();
    return;
  }

  if (main_command == "wtrace") {
    //
    // 01: off
    // 02: get
    // 03: set [09-,]*f
    // 04: add [09-,]*
    // 05: del [09-,]*
    //

    std::string subcommand = command_instance->get_string_argument("action");
    std::string levels = command_instance->get_string_argument("levels");

    if (subcommand == "off") {
      lmTraceSet(NULL);
      LM_F(("All trace levels removed"));
    } else if (subcommand == "get") {
      char levels[1024];
      lmTraceGet(levels);

      samson::gpb::Collection *collection = new  samson::gpb::Collection();
      collection->set_title("Trace information");
      collection->set_name("trace_levels");
      samson::gpb::CollectionRecord *record = collection->add_record();
      ::samson::add(record, "Trace levels", levels, "left,different");
      collections.push_back(collection);

      finishWorkerTask();
      return;
    } else if ((subcommand == "set") || (subcommand == "add") || (subcommand == "del")) {
      if (levels == "") {
        finishWorkerTaskWithError("Usage: wtrace " + subcommand + " (range-list of trace levels)");
        return;
      }

      if (subcommand == "set") {
        lmTraceSet((char *)levels.c_str());
        LM_F(("Set trace levels '%s'", levels.c_str()));
      } else if (subcommand == "add") {
        lmTraceAdd((char *)levels.c_str());
        LM_F(("Added trace levels '%s'", levels.c_str()));
      } else if (subcommand == "del") {
        lmTraceSub((char *)levels.c_str());
        LM_F(("Removed trace levels '%s'", levels.c_str()));
      }
    } else {
      finishWorkerTaskWithError(au::str("Usage: bad subcommand for 'wtrace': %s", subcommand.c_str()));
      return;
    }

    finishWorkerTask();
    return;
  }

  if (main_command == "send_alert") {
    std::string message = command_instance->get_string_argument("message");
    bool error = command_instance->get_bool_option("-error");
    bool warning = command_instance->get_bool_option("-error");

    // Full message
    std::string full_message = au::str("[Alert from Delilah_%s] %s"
                                       , au::code64_str(delilah_id).c_str()
                                       , message.c_str()
                                       );


    // Send a trace to all delilahs
    if (error) {
      samsonWorker->network()->SendAlertToAllDelilahs("error", "delilah", full_message);
    } else if (warning) {
      samsonWorker->network()->SendAlertToAllDelilahs("warning", "delilah", full_message);
    } else {
      samsonWorker->network()->SendAlertToAllDelilahs("message", "delilah", full_message);
    } finishWorkerTask();
    return;
  }

  if (main_command == "defrag") {
    error.set("Unimplemented");
    finishWorkerTask();

    /*
     * if( cmd.get_num_arguments() < 3 )
     * {
     * finishWorkerTaskWithError( "Usage: defrag queue_source queue_destination" );
     * return;
     * }
     *
     * std::string queue_from = cmd.get_argument(1);
     * std::string queue_to   = cmd.get_argument(2);
     *
     * // Pointer to the queue we want to defrag
     * stream::Queue*queue = streamManager->getQueue( queue_from );
     *
     * // Create information about how data is distributed in files to do the best defrag
     * stream::BlockList init_list("defrag_block_list");
     * init_list.copyFrom( queue->list );
     *
     * size_t input_operation_size = au::Singleton<SamsonSetup>::shared()->getUInt64("general.memory") / 3;
     *
     * size_t num_defrag_blocks = init_list.getBlockInfo().size / input_operation_size;
     * if( num_defrag_blocks == 0)
     * num_defrag_blocks = 1;
     * size_t output_operation_size = input_operation_size / num_defrag_blocks;
     *
     * if( output_operation_size > (64*1024*1024) )
     * output_operation_size = (64*1024*1024);
     *
     * while( true )
     * {
     * {
     * stream::BlockList tmp_list("defrag_block_list");
     * tmp_list.extractFromForDefrag( &init_list , input_operation_size );
     *
     * if( tmp_list.isEmpty() )
     * break;
     *
     *
     * // Create an operation to process this set of blocks
     * size_t new_id = streamManager->WorkerTaskManager.getNewId();
     * stream::BlockBreakWorkerTask *tmp = new stream::BlockBreakWorkerTask( new_id , queue_to , output_operation_size );
     *
     * // Fill necessary blocks
     * tmp->getBlockList( au::str("input" ) )->copyFrom( &tmp_list );
     *
     * // Set the working size to get statistics at ProcessManager
     * tmp->setWorkingSize();
     *
     * // Add me as listener and increase the number of operations to run
     * tmp->addListenerId( engine_id() );
     * num_pending_processes++;
     *
     * tmp->environment.set("worker_command_id", worker_command_id);
     *
     * // Schedule tmp task into WorkerTaskManager
     * streamManager->WorkerTaskManager.add( tmp );
     * }
     * }
     *
     */
    return;
  }

  if (main_command == "cancel_stream_operation") {
    error.set("Unimplemented");
    finishWorkerTask();
    return;

    /*
     * if( cmd.get_num_arguments() < 2 )
     * {
     * finishWorkerTaskWithError( au::str("Not enough parameters for command %s\nUsage: cancel_stream_operation operation_id" , main_command.c_str() ) );
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
     * finishWorkerTask();
     *
     * // Remove in the worker command manager
     * if( samsonWorker->workerCommandManager->cancel( canceled_worker_command_id ) )
     * finishWorkerTask();
     * else
     * finishWorkerTaskWithError(au::str("Worker command %s not found" , canceled_worker_command_id.c_str() ));
     */
    return;
  }

  if (main_command == "run") {
    std::string operation = command_instance->get_string_argument("operation");
    std::string inputs = command_instance->get_string_option("input");
    std::string outputs = command_instance->get_string_option("output");

    std::string command = au::str("batch %s -input %s -output %s -delilah_id %lu -delilah_component_id %lu "
                                  , operation.c_str()
                                  , inputs.c_str()
                                  , outputs.c_str()
                                  , delilah_id
                                  , delilah_component_id);

    au::ErrorManager error;
    std::string caller = au::str("run_deliah_%s_%lu", au::code64_str(delilah_id).c_str(), delilah_component_id);
    samsonWorker->data_model()->Commit(caller, command, &error);

    if (error.IsActivated()) {
      finishWorkerTaskWithError(error.GetMessage());
    } else {
      finishWorkerTask();
    } return;
  }

  // Simple commands
  au::ErrorManager error;
  runCommand(command, &error);
  if (error.IsActivated()) {
    finishWorkerTaskWithError(error.GetMessage());
  } else {
    finishWorkerTask();
  }
}

void WorkerCommand::finishWorkerTaskWithError(std::string error_message) {
  // LM_M(("Setting error message %s" , error_message.c_str() ));
  error.set(error_message);
  finishWorkerTask();

  // Notify everything so it is automatically canceled
  engine::Notification *notification = new engine::Notification("cancel");
  notification->environment().Set("id", worker_command_id);
  engine::Engine::shared()->notify(notification);
}

void WorkerCommand::finishWorkerTask() {
  if (finished) {
    // If this worker command is finished, not do anything else again.
    return;
  }

  if (notify_finish) {
    PacketPointer p(new Packet(Message::WorkerCommandResponse));
    gpb::WorkerCommandResponse *c = p->message->mutable_worker_command_response();
    c->mutable_worker_command()->CopyFrom(*originalWorkerCommand);

    // Put the error if any
    if (error.IsActivated()) {
      // LM_M(("Sending error message %s" , error.GetMessage().c_str() ));
      c->mutable_error()->set_message(error.GetMessage());  // Set delilah id
    }
    p->message->set_delilah_component_id(delilah_component_id);

    // Direction of this packets
    p->to.node_type = DelilahNode;
    p->to.id = delilah_id;


    // Add collections as answers...
    for (size_t i = 0; i < collections.size(); i++) {
      p->message->add_collection()->CopyFrom(*collections[i]);
    }

    // Send the packet
    samsonWorker->network()->Send(p);
  }

  // Set the finished flag
  finished = true;
}

stream::StreamOperationBase *WorkerCommand::getStreamOperation(Operation *op) {
  // Parsing the global command
  au::CommandLine cmd;

  cmd.Parse(command);

  int pos_argument = 1;     // We skip the "run" parameter

  std::string operation_name = cmd.get_argument(pos_argument++);

  // Distribution information for this stream operation
  /*
   * DistributionInformation distribution_information;
   *
   * distribution_information.workers = samsonWorker->network->getWorkerIds();
   * if (distribution_information.workers.size() == 0)
   * {
   * LM_W(("No workers connected"));
   * }
   *
   * distribution_information.network = samsonWorker->network;
   *
   * stream::StreamOperationBase *operation = new stream::StreamOperationBase( operation_name , distribution_information );
   *
   * for (int i = 0 ; i < op->getNumInputs() ; i++)
   * operation->input_queues.push_back( cmd.get_argument( pos_argument++ ) );
   * for (int i = 0 ; i < op->getNumOutputs() ; i++)
   * operation->output_queues.push_back( cmd.get_argument( pos_argument++ ) );
   *
   * // Reassign the name for better description
   * operation->name = au::str("run_%s_delilah_%s_%lu" , op->getName().c_str() , au::code64_str(delilah_id).c_str() , delilah_component_id );
   *
   *
   * return operation;
   */


  LM_X(1, ("Not implemented"));
  return NULL;
}

void WorkerCommand::notify(engine::Notification *notification) {
  if (notification->isName(notification_process_request_response)) {
    num_pending_processes--;

    if (notification->environment().IsSet("error")) {
      error.set(notification->environment().Get("error", "no_error"));
    }
    checkFinish();
    return;
  }

  if (notification->isName(notification_disk_operation_request_response)) {
    num_pending_disk_operations--;
    if (notification->environment().IsSet("error")) {
      error.set(notification->environment().Get("error", "no_error"));  // In case of push module, just reload modules
    }
    if (notification->environment().Get("push_module", "no") == "yes") {
      ModulesManager::shared()->reloadModules();
    }
    checkFinish();
    return;
  }

  LM_W(("Unexpected notification at WorkerCommand"));
}

void WorkerCommand::checkFinish() {
  if (error.IsActivated()) {
    finishWorkerTask();
  } else if (( num_pending_processes <= 0 ) && (num_pending_disk_operations <= 0 )) {
    finishWorkerTask();
  }
}

void WorkerCommand::getInfo(std::ostringstream& output) {
  au::xml_open(output,  "worker_command");

  au::xml_simple(output, "command", command);

  au::xml_close(output,  "worker_command");
}

void WorkerCommand::fill(samson::gpb::CollectionRecord *record, const Visualization& visualization) {
  std::string name = NodeIdentifier(DelilahNode, delilah_id).getCodeName();

  add(record, "id",  worker_command_id, "left,different");

  if (finished) {
    add(record, "status", "finished", "left,different");
  } else {
    add(record, "status", "running", "left,different");
  } add(record, "command", command, "left,different");
  add(record, "#operations", num_pending_processes, "left,uint64,sum");
  add(record, "#disk_operations", num_pending_disk_operations, "left,uint64,sum");
  add(record, "error", error.GetMessage(), "left,different");
}

gpb::Collection *WorkerCommand::getCollectionOfBuffers(const Visualization& visualization) {
  gpb::Collection *collection = new gpb::Collection();

  collection->set_name("buffers");

  au::tables::Table table = engine::Engine::memory_manager()->getTableOfBuffers();

  // Debug
  // printf("%s\n" , table.str().c_str() );

  for (size_t r = 0; r < table.getNumRows(); r++) {
    gpb::CollectionRecord *record = collection->add_record();

    for (size_t c = 0; c < table.getNumColumns(); c++) {
      std::string concept = table.getColumn(c);
      std::string format = table.getFormatForColumn(c);

      std::string value = table.getValue(r, c);

      ::samson::add(record, concept, value, format);
    }
  }

  return collection;
}
}
