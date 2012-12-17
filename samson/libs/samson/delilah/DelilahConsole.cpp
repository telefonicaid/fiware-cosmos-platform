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
/* ****************************************************************************
 *
 * FILE                     DelilahConsole.h
 *
 * DESCRIPTION			   Console terminal for delilah
 *
 */
#include "samson/delilah/DelilahConsole.h"       // Own interface

#include <dirent.h>                              // DIR directory header
#include <sys/stat.h>                            // stat(.)

#include <iomanip>
#include <iostream>

#include "au/CommandLine.h"                      // au::CommandLine
#include "au/ThreadManager.h"
#include "au/containers/SharedPointer.h"
#include "au/containers/StringVector.h"
#include "au/file.h"
#include "au/log/LogCommon.h"
#include "au/log/LogMain.h"
#include "au/string/Descriptors.h"               // au::Descriptors
#include "au/string/S.h"
#include "au/string/StringUtilities.h"           // au::Format
#include "au/tables/Table.h"
#include "au/tables/Tree.h"                      // au::tables::TreeItem
#include "au/tables/pugi.h"                      // pugi::Pugi
#include "au/tables/pugixml.hpp"                 // pugi:...

#include "engine/DiskManager.h"
#include "engine/MemoryManager.h"                // samson::MemoryManager
#include "engine/Notification.h"                 // samson::Notification

#include "samson/client/SamsonClient.h"
#include "samson/common/EnvironmentOperations.h"  // Environment operations (CopyFrom)
#include "samson/common/NotificationMessages.h"
#include "samson/common/SamsonDataSet.h"
#include "samson/delilah/Delilah.h"              // samson::Delailh
#include "samson/delilah/WorkerCommandDelilahComponent.h"
#include "samson/module/ModulesManager.h"        // samson::ModulesManager
#include "samson/module/samsonVersion.h"         // SAMSON_VERSION
#include "samson/network/Packet.h"               // ss:Packet

#define DEF1 "TYPE:EXEC/FUNC: TEXT"

// Fancy function to get function to show clock evolution
std::string strClock(au::Cronometer& cronometer) {
  char c = '-';

  switch (static_cast<int>((cronometer.seconds()) * 3) % 4) {
    case 0:
      c = '-';
      break;
    case 1:
      c = '\\';
      break;
    case 2:
      c = '|';
      break;
    default:
      c = '/';
      break;
  }
  return au::S() << c << " " << cronometer;
}

namespace samson {
const char *general_description =
  "SAMSON is a distributed platform for efficient processing of unbounded streams of big data";

const char *auths = "Andreu Urruela, Grant Croker, J.Gregorio Escalada & Ken Zangelin";

DelilahConsole::DelilahConsole(size_t delilah_id) : Delilah("console", delilah_id) {
  // Default values
  show_server_logs_ = false;
  show_alerts_ = false;
  verbose_ = true;

  // Schedule a notification to review repeat-tasks
  engine::Engine::shared()->notify(new engine::Notification(notification_delilah_review_repeat_tasks), 1);

  // Cool stuff
  AddEspaceSequence("samson");

  // By default no save traces
  trace_file_ = NULL;
}

DelilahConsole::~DelilahConsole() {
}

std::string DelilahConsole::GetPrompt() {
  return au::str("[%s] Delilah>", GetClusterConnectionSummary().c_str());
}

void DelilahConsole::EvalCommand(const std::string& command) {
  // Run this command
  size_t _delilah_id = runAsyncCommand(command);

  // Wait until this delilah command has finished ( give some options to the user )
  if (_delilah_id != 0) {
    au::Cronometer cronometer;
    while (true) {
      au::console::ConsoleEntry entry;
      std::string message;
      if (cronometer.seconds() > 1) {
        message = au::str("[ %s ] Waiting process %lu : %s ... [ b: background c: cancel ]",
                          strClock(cronometer).c_str(), _delilah_id, command.c_str());
      }
      int s = WaitWithMessage(message, 0.2, &entry);

      if (!DelilahComponentIsActive(_delilah_id)) {
        // Print output in this thread to control pages using au::Console
        Refresh();
        Write(GetOutputForComponent(_delilah_id));
        return;
      }

      if (s == 0) {
        if (entry.isChar('c')) {
          Refresh();
          WriteWarningOnConsole(au::str("Canceling process %lu : %s", _delilah_id, command.c_str()));
          au::ErrorManager error;
          CancelDelilahComponent(_delilah_id, error);
          Write(error);
          return;
        } else if (entry.isChar('b')) {
          Refresh();   // Refresh console
          return;
        }
      }
    }
  }
}

void DelilahConsole::AutoComplete(au::console::ConsoleAutoComplete *info) {
  delilah_command_catalogue_.AutoComplete(info);  // Use catalogue for auto-completion
  return;
}

/*
 * // Generic list of information from the xml document
 * std::string generic_node_to_string_function( const pugi::xml_node& node )
 * {
 * std::ostringstream output;
 * pugi::str( node , 0 ,  output , 1000 );
 * return output.str();
 * }
 */
std::string string_for_list(const char *list[]) {
  std::ostringstream output;
  int i = 0;

  while (list[i] != NULL) {
    output << list[i];
    if (list[i + 1] != NULL) {
      if (((i + 1) % 4) == 0) {
        output << "\n";
      } else {
        output << ", ";
      }
    }
    i++;
  }
  output << "\n";

  return output.str();
}

void add(const char *list[], std::vector<std::string> &commands) {
  int i = 0;

  while (list[i] != NULL) {
    commands.push_back(list[i]);
    i++;
  }
}

size_t DelilahConsole::runAsyncCommand(std::string command) {
  // Parse command acording to the commands catalogue
  au::ErrorManager error;

  au::SharedPointer<au::console::CommandInstance> command_instance = delilah_command_catalogue_.Parse(command, error);

  if (error.HasErrors()) {
    Write(error);   // Write errors and messages
    return 0;
  }

  size_t delilah_id = runAsyncCommand(command_instance.shared_object());
  return delilah_id;
}

size_t DelilahConsole::runAsyncCommand(au::console::CommandInstance *command_instance) {
  std::string mainCommand = command_instance->main_command();

  /*
   * if (mode == mode_database) {
   * // Run data base command
   * std::string result = runDatabaseCommand(command_instance->command_line());
   * Write(au::StringInConsole(result));
   * return 0;
   * }
   */

  if (mainCommand == "wlog_show") {
    if (log_probe != NULL) {
      log_probe.Reset();
    }

    std::string host = command_instance->GetStringArgument("host");
    std::string format = command_instance->GetStringOption("format");
    std::string filter = command_instance->GetStringOption("filter");

    log_probe.Reset(new au::LogProbe());
    log_probe->AddPlugin("console", new au::LogProbeConsole(this, format));

    au::ErrorManager error;
    log_probe->ConnectAsProbe(host, filter, error);

    if (error.HasErrors()) {
      Write(error);
      log_probe.Reset();
    } else {
      WriteWarningOnConsole(au::str("Connected to %s to get logs (Filter %s)", host.c_str(), filter.c_str()));
    }

    return 0;
  }

  if (mainCommand == "wlog_hide") {
    if (log_probe != NULL) {
      log_probe.Reset();
    }
    WriteWarningOnConsole("Logs deactivated");
    return 0;
  }

  if (au::CheckIfStringsBeginWith(mainCommand, "log_")) {
    au::ErrorManager error;
    au::log_central->EvalCommand(command_instance->command_line(), error);
    Write(error);
    return 0;
  }

  if (mainCommand == "connect") {
    // Disconnect first from whatever cluster I am connected to...
    Disconnect();

    std::string host = command_instance->GetStringArgument("host");
    std::vector<std::string> hosts = au::split(host, ' ');

    if (hosts.size() == 0) {
      WriteErrorOnConsole(command_instance->GetErrorMessage("No host provided"));
    }
    for (int i = 0; i < static_cast<int>(hosts.size()); i++) {
      Write(au::str("Connecting to %s...\n", hosts[i].c_str()));

      au::ErrorManager error;
      if (Connect(hosts[i], &error)) {
        Write("OK\n");
        return 0;
      } else {
        WriteErrorOnConsole(error.GetLastError());
      }
    }

    if (hosts.size() > 2) {
      WriteErrorOnConsole("Unable to connect to any host");
    }
    return 0;
  }

  if (mainCommand == "disconnect") {
    Disconnect();
    return 0;
  }

  if (mainCommand == "cluster_show_setup") {
    Write(network_->getClusterSetupStr() + "\n");
    return 0;
  }

  if (mainCommand == "cluster_show_assignation") {
    Write(network_->getClusterAssignationStr() + "\n");
    return 0;
  }

  if (mainCommand == "history") {
    int limit = command_instance->GetIntOption("limit");
    Write(str_history(limit));
    return 0;
  }

  if (mainCommand == "reload_modules_local") {
    au::Singleton<ModulesManager>::shared()->ClearModulesManager();

    au::ErrorManager error;
    au::Singleton<ModulesManager>::shared()->AddModulesFromDefaultDirectory(error);
    if (error.HasErrors()) {
      Write(error);
    } else {
      WriteWarningOnConsole("Reloaded delilah client modules");
    }
  }

  if (mainCommand == "help") {
    std::string concept = command_instance->GetStringArgument("concept");

    if (concept != "") {
      Write(delilah_command_catalogue_.GetHelpForConcept(concept));
      return 0;
    } else {
      std::ostringstream output;
      output << "\n";
      output << au::StringRepeatingCharInConsole('=') << "\n";
      output << " SAMSON v " << SAMSON_VERSION << "\n";
      output << au::StringRepeatingCharInConsole('=') << "\n";
      output << "\n";
      output << au::str_indent(general_description) << "\n";
      output << "\n";
      output << au::str_indent(au::str("Authors: %s", auths)) << "\n";
      output << "\n";
      output << au::str_indent(au::str("Telefonica I+D 2010-2012")) << "\n";
      output << "\n";
      output << au::StringRepeatingCharInConsole('-') << "\n";
      output << "\n";
      output << "\thelp all .................. get a list of all available commands\n";
      output << "\thelp categories ........... get a list of command categories\n";
      output << "\thelp <command> ............ get detailed information for a command\n";
      output << "\thelp <category> ........... get list of commands for a particular categoriy\n";
      output << "\n";
      output << au::StringRepeatingCharInConsole('-') << "\n";
      output << "\n";

      std::string text = output.str();

      Write(text);
      return 0;
    }
  }

  if (mainCommand == "quit") {
    exit(0);
    // Console::StopConsole();   // Quit the console
    // return 0;
  }

  if (mainCommand == "threads") {
    std::ostringstream message;
    message << au::Singleton<au::ThreadManager>::shared()->str();
    Write(message.str());
    return 0;
  }

  if (mainCommand == "set") {
    std::string name = command_instance->GetStringArgument("name");
    std::string value = command_instance->GetStringArgument("value");

    // Only set, we show all the defined parameters
    if (name == "") {
      au::tables::Table table(au::StringVector("Property", "Value"));
      table.setTitle("Environent variables");

      std::map<std::string, std::string>::iterator it_environment;
      for (it_environment = environment_.environment.begin()
           ; it_environment != environment_.environment.end(); it_environment++)
      {
        table.addRow(au::StringVector(it_environment->first, it_environment->second));
      }

      Write(table.str());
      return 0;
    }

    environment_.set(name, value);
    WriteWarningOnConsole(au::str("Environment variable '%s' set to '%s'", name.c_str(), value.c_str()));
    return 0;
  }

  if (mainCommand == "unset") {
    std::string name = command_instance->GetStringArgument("name");

    if (!environment_.isSet(name)) {
      WriteErrorOnConsole(au::str("Variable %s is not set", name.c_str()));
      return 0;
    }

    environment_.unset(name);
    WriteWarningOnConsole(au::str("Environment variable '%s' is removed ", name.c_str()));
    return 0;
  }

  if (mainCommand == "ls_local_connections") {
    Write(GetTableFromCollection(network_->GetConnectionsCollection(samson::Visualization()))->str());
    return 0;
  }

  if (mainCommand == "alerts") {
    std::string action = command_instance->GetStringArgument("action");

    if (action == "on") {
      show_alerts_ = true;
      Write("Alerts are activated\n");
      return 0;
    }

    if (action == "off") {
      show_alerts_ = false;
      Write("Alerts are deactivated\n");
      return 0;
    }

    WriteErrorOnConsole("Usage: alerts on/off\n");
    return 0;
  }

  if (mainCommand == "verbose") {
    std::string action = command_instance->GetStringArgument("action");

    if (action == "") {
      if (verbose_) {
        Write("verbose mode is activated\n");
      } else {
        Write("verbose mode is deactivated\n");
      }
      return 0;
    }

    if (action == "on") {
      verbose_ = true;
      Write("verbose mode is activated\n");
      return 0;
    }
    if (action == "off") {
      verbose_ = false;
      Write("verbose mode is deactivated\n");
      return 0;
    }

    WriteErrorOnConsole(command_instance->GetErrorMessage("Unknown action"));
    return 0;
  }

  if (mainCommand == "show_alerts") {
    std::string txt = trace_colleciton_.str();
    Write(au::StringInConsole(txt));
    return 0;
  }

  if (mainCommand == "open_alerts_file") {
    std::string filename = command_instance->GetStringArgument("file");

    if (trace_file_) {
      WriteErrorOnConsole(
        au::str("Please close previous alerts file (%s) with command 'close_alerts_file'\n",
                trace_file_name_.c_str()));
      return 0;
    }

    trace_file_name_ = filename;
    trace_file_ = fopen(trace_file_name_.c_str(), "w");
    if (!trace_file_) {
      WriteErrorOnConsole(
        au::str("Error opening file '%s' to store alerts (%s)\n", trace_file_name_.c_str(), strerror(errno)));
      return 0;
    }

    Write(au::str("Saving alerts to file '%s'\n", trace_file_name_.c_str()));
    return 0;
  }

  if (mainCommand == "close_alerts_file") {
    if (!trace_file_) {
      WriteErrorOnConsole("No active alerts file is open. Open one with command 'open_alerts_file'");
      return 0;
    }

    fclose(trace_file_);
    Write(au::str("Stop saving alerts to file '%s'.\nOpen a new alerts file with command 'open_alerts_file\n'",
                  trace_file_name_.c_str()));
    return 0;
  }

  if (mainCommand == "ps") {
    if (command_instance->GetBoolOption("-clear")) {
      ClearFinishedComponents();
    }
    std::string str_id = command_instance->GetStringArgument("id");

    if (str_id != "") {
      size_t id = atoll(str_id.c_str());

      DelilahComponent *component = components_.findInMap(id);

      if (!component) {
        WriteErrorOnConsole(au::str("Unknown process with id %d", id));
      } else {
        au::tables::Table table("Concept|Value,left");

        table.setTitle(au::str("Job %lu description ", id));

        table.addRow(au::StringVector("Delilah", au::code64_str(delilah_id())));
        table.addRow(au::StringVector("Job id", au::str("%lu", id)));

        if (component->isComponentFinished()) {
          table.addRow(au::StringVector("Finished", "Yes"));
        } else {
          table.addRow(au::StringVector("Finished", "No. Running " + au::str_time(component->cronometer.seconds())));
        }
        if (component->error.HasErrors()) {
          table.addRow(au::StringVector("Error", component->error.GetLastError()));
        }
        table.addRow(au::StringVector("Status", component->getStatus()));

        Write(table.str() + "\n" + component->getExtraStatus());
      }

      return 0;
    }

    std::string txt = GetListOfComponents();
    Write(au::StringInConsole(txt));
    return 0;
  }

  // Push data to a queue

  if (mainCommand == "ls_local_push_operations") {
    au::tables::Table *table = push_manager_->getTableOfItems();
    Write(table->str());
    delete table;
    return 0;
  }

  if (mainCommand == "push_module") {
    std::string file_name = command_instance->GetStringArgument("file");

    au::ErrorManager error;
    std::vector<std::string> file_names = au::GetListOfFiles(file_name, error);
    if (error.HasErrors()) {
      Write(error);
      return -1;
    }

    au::ErrorManager error_push;
    size_t id = AddPushModuleComponent(file_names, error);
    Write(error_push);  // Write any error message on screen
    return id;
  }

  if (mainCommand == "push") {
    std::string file_name = command_instance->GetStringArgument("file");
    std::string queue = command_instance->GetStringArgument("queue");

    au::ErrorManager error;
    std::vector<std::string> file_names = au::GetListOfFiles(file_name, error);
    if (error.HasErrors()) {
      Write(error);
      return -1;
    }

    // Get provided queues
    std::vector<std::string> queues = au::split(queue, ' ');

    // Check queue names
    for (size_t i = 0; i < queues.size(); i++) {
      if (queues[i].size() == 0) {
        WriteErrorOnConsole("Invalid queue");
        return -1;
      }

      if (queues[i][0] == '.') {
        WriteErrorOnConsole("Invalid queue specified. Name can not start with a dot (.)");
        return -1;
      }
    }

    au::ErrorManager error_push;
    size_t id = AddPushComponent(file_names, queues, error_push);
    Write(error_push);
    return id;
  }

  // Connect and disconnect to a queue

  if (mainCommand == "connect_to_queue") {
    std::string queue = command_instance->GetStringArgument("queue");
    size_t id = ConnectToQueue(queue);
    WriteWarningOnConsole(au::str("Connected to queue %s using pop component %lu", queue.c_str(), id));
    return 0;
  }

  // Push data to a queue

  if (mainCommand == "pop") {
    std::string queue_name = command_instance->GetStringArgument("queue");
    std::string fileName = command_instance->GetStringArgument("file_name");

    bool force_flag = command_instance->GetBoolOption("-force");
    bool show_flag = command_instance->GetBoolOption("-show");
    size_t id = AddPopComponent(queue_name, fileName, force_flag, show_flag);
    return id;
  }

  if (mainCommand == "rm_local") {
    std::string file = command_instance->GetStringArgument("file");
    au::ErrorManager error;
    au::RemoveDirectory(file, error);

    if (error.HasErrors()) {
      WriteErrorOnConsole(error.GetLastError());
    } else {
      WriteWarningOnConsole("OK");
    }
    return 0;
  }

  if (mainCommand == "ls_local_modules") {
    Write(au::Singleton<ModulesManager>::shared()->GetTableOfModules() + "\n");
    return 0;
  }

  if (mainCommand == "ls_local") {
    std::string file = command_instance->GetStringArgument("file");

    Write(GetLsLocal(file, false));
    return 0;
  }

  if (mainCommand == "ls_local_queues") {
    std::string file = command_instance->GetStringArgument("file");

    Write(GetLsLocal(file, true));
    return 0;
  }

  if (mainCommand == "show_local_queue") {
    std::string queue = command_instance->GetStringArgument("queue");

    const char *file_name = queue.c_str();
    size_t limit = command_instance->GetUint64Option("-limit");
    bool header = command_instance->GetBoolOption("-header");
    bool show_hg = command_instance->GetBoolOption("-show_hg");

    struct stat filestatus;
    stat(file_name, &filestatus);

    if (S_ISREG(filestatus.st_mode)) {
      // Open a single file
      au::ErrorManager error;
      au::SharedPointer<samson::SamsonFile> samson_file = samson::SamsonFile::create(file_name, error);

      if (error.HasErrors()) {
        WriteErrorOnConsole(au::str("%s", error.GetLastError().c_str()));
        return 0;
      }

      if (header) {
        std::ostringstream output;
        samson_file->printHeader(output);
        Write(output.str());
        return 0;
      }

      std::ostringstream output;
      samson_file->printContent(limit, show_hg, output);
      Write(output.str());
      return 0;
    } else if (S_ISDIR(filestatus.st_mode)) {
      au::ErrorManager error;
      au::SharedPointer<samson::SamsonDataSet> samson_data_set = samson::SamsonDataSet::create(file_name, error);

      if (error.HasErrors()) {
        WriteErrorOnConsole(au::str("%s", error.GetLastError().c_str()));
        return 0;
      }

      if (header) {
        std::ostringstream output;
        samson_data_set->printHeaders(output);
        Write(output.str());
        return 0;
      }

      std::ostringstream output;
      samson_data_set->printContent(limit, show_hg, output);
      std::string txt = output.str();
      Write(output.str());
      return 0;
    } else {
      WriteErrorOnConsole(au::str("%s is not a file or a directory\n", file_name));
      return 0;
    }

    // Not possible to get here...
    return 0;
  }

  // By default, it is considered a worker command
  return SendWorkerCommand(command_instance->command_line());

  return 0;
}

int DelilahConsole::_receive(const PacketPointer& packet) {
  switch (packet->msgCode) {
    case Message::Alert: {
      std::string _text = packet->message->alert().text();
      std::string _type = packet->message->alert().type();
      std::string _context = packet->message->alert().context();

      // Add to the local collection of traces
      trace_colleciton_.add(packet->from, _type, _context, _text);

      // Write to disk if required
      if (trace_file_) {
        std::string trace_message = au::str("%s %s %s %s\n", packet->from.str().c_str(), _type.c_str(),
                                            _context.c_str(), _text.c_str());
        fwrite(trace_message.c_str(), trace_message.length(), 1, trace_file_);
      }

      if (show_alerts_ || _context == "system") {
        au::tables::Table table("Concept|Value,left");
        table.setTitle("ALERT");

        table.addRow(au::StringVector("From", packet->from.str()));
        table.addRow(au::StringVector("Type", _type));
        table.addRow(au::StringVector("Context", _context));
        table.addRow(au::StringVector("Message", _text));

        std::string trace_message = table.str();

        if (_type == "error") {
          WriteErrorOnConsole(trace_message);
        } else if (_type == "warning") {
          WriteWarningOnConsole(trace_message);
        } else {
          Write(trace_message);
        }
      }
    }
    break;

    default:
      LOG_SW(("Unknown message %s received at delilahConsole", Message::messageCode(packet->msgCode)));
      break;
  }

  return 0;
}

void DelilahConsole::DelilahComponentStartNotification(DelilahComponent *component) {
  if (component->hidden) {
    return;   // No notification for hidden processes
  }
  if (verbose_) {
    std::string component_id = au::str("[ %s_%lu ]", au::code64_str(delilah_id()).c_str(), component->getId());
    std::string message = component_id + " Process started: " + component->getConcept();
    Write(component->error, component_id);
    WriteBoldOnConsole(au::str("%s", message.c_str()));
  }
}

void DelilahConsole::DelilahComponentFinishNotification(DelilahComponent *component) {
  if (component->hidden) {
    return;   // No notification for hidden processes
  }
  if (verbose_) {
    std::string component_id = au::str("[ %s_%lu ]", au::code64_str(delilah_id()).c_str(), component->getId());
    std::string message = component_id + " Process finished: " + component->getConcept();
    Write(component->error, component_id);
    if (component->error.HasErrors()) {
      WriteErrorOnConsole(au::str("%s", message.c_str()));
    } else {
      WriteBoldOnConsole(au::str("%s", message.c_str()));
    }
  }
}

// Process received packets with data

void DelilahConsole::receive_buffer_from_queue(std::string queue, engine::BufferPointer buffer) {
  size_t counter = stream_out_queue_counters_.appendAndGetCounterFor(queue);
  size_t packet_size = buffer->size();

  std::string directory_name = au::str("stream_out_%s", queue.c_str());

  if ((mkdir(directory_name.c_str(), 0755) != 0) && (errno != EEXIST)) {
    WriteErrorOnConsole(au::str("Error creating the directory '%s' to store data from queue '%s' (%s)",
                                directory_name.c_str(), queue.c_str(), strerror(errno)));
    WriteErrorOnConsole(au::str("Rejecting '%s' data from queue '%s'", au::str(packet_size, "B").c_str(), queue.c_str()));
    return;
  }

  std::string fileName = au::str("%s/block_%l05u", directory_name.c_str(), counter);

  if (verbose_) {
    // Show the first line or key-value
    SamsonClientBlock samson_client_block(buffer);   // Not remove buffer at destrutor

    std::ostringstream output;
    output << "====================================================================\n";
    output << au::str("Received stream data for queue %s\n", queue.c_str());
    output << au::str("Stored at file %s\n", fileName.c_str());
    output << samson_client_block.get_header_content();
    output << "====================================================================\n";
    output << samson_client_block.get_content(5);
    output << "====================================================================\n";

    Write(output.str());
  }

  // Disk operation....
  au::SharedPointer<engine::DiskOperation> operation;
  operation = engine::DiskOperation::newWriteOperation(buffer, fileName, engine_id());
  engine::Engine::disk_manager()->Add(operation);
}

void DelilahConsole::runAsyncCommandAndWait(std::string command) {
  LOG_SM(("runAsyncCommandAndWait command:%s", command.c_str()));
  size_t tmp_id = runAsyncCommand(command);

  if (tmp_id == 0) {
    return;   // Sync command
  }
  while (true) {
    if (!DelilahComponentIsActive(tmp_id)) {
      return;
    }
    usleep(10000);
  }
}
}
