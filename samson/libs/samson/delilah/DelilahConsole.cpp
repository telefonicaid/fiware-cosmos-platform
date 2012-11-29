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
  simple_output_ = false;
  no_output_ = false;

  // Inform about random code for this delilah
  writeWarningOnConsole(au::str("Random delilah id generated [%s]", au::code64_str(get_delilah_id()).c_str()));
}

DelilahConsole::~DelilahConsole() {
}

std::string DelilahConsole::getPrompt() {
  return au::str("[%s] Delilah>", getClusterConnectionSummary().c_str());
}

void DelilahConsole::evalCommand(const std::string& command) {
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

      if (!isActive(_delilah_id)) {
        // Print output
        Refresh();
        writeOnConsole(getOutputForComponent(_delilah_id));
        return;
      }

      if (s == 0) {
        // To something with the key

        if (entry.isChar('c')) {
          Refresh();
          writeWarningOnConsole(au::str("Canceling process %lu : %s", _delilah_id, command.c_str()));
          cancelComponent(_delilah_id);
          return;
        } else if (entry.isChar('b')) {
          Refresh();   // Refresh console
          return;
        }
      }
    }
  }
}

void DelilahConsole::autoCompleteOperations(au::console::ConsoleAutoComplete *info) {
  std::vector<std::string> operation_names = getOperationNames();

  for (size_t i = 0; i < operation_names.size(); i++) {
    info->add(operation_names[i]);
  }
}

void DelilahConsole::autoCompleteOperations(au::console::ConsoleAutoComplete *info, std::string type) {
  std::vector<std::string> operation_names = getOperationNames(type);

  for (size_t i = 0; i < operation_names.size(); i++) {
    info->add(operation_names[i]);
  }
}

void DelilahConsole::autoCompleteQueues(au::console::ConsoleAutoComplete *info) {
  au::tables::Table *table = database.getTable("queues");

  if (!table) {
    return;
  }
  for (size_t r = 0; r < table->getNumRows(); r++) {
    info->add(table->getValue(r, "name"));
  }
  delete table;
}

void DelilahConsole::autoCompleteQueueWithFormat(au::console::ConsoleAutoComplete *info, std::string key_format,
                                                 std::string value_format) {
  au::tables::Table *table = database.getTable("queues");

  if (!table) {
    return;
  }

  for (size_t r = 0; r < table->getNumRows(); r++) {
    if (table->getValue(r, "format/key_format") == key_format) {
      if (table->getValue(r, "format/value_format") == value_format) {
        info->add(table->getValue(r, "name"));
      }
    }
  }

  delete table;
}

void DelilahConsole::autoCompleteQueueForOperation(au::console::ConsoleAutoComplete *info, std::string operation_name,
                                                   int argument_pos) {
  // Search in the operations
  Operation *operation = au::Singleton<ModulesManager>::shared()->GetOperation(operation_name);

  if (!operation) {
    return;
  }

  if (argument_pos < operation->getNumInputs()) {
    autoCompleteQueueWithFormat(info, operation->inputFormats[argument_pos].keyFormat,
                                operation->inputFormats[argument_pos].valueFormat);
  } else {
    argument_pos -= operation->getNumInputs();
    if (argument_pos < operation->getNumOutputs()) {
      autoCompleteQueueWithFormat(info, operation->outputFormats[argument_pos].keyFormat,
                                  operation->outputFormats[argument_pos].valueFormat);
    }
  }
}

void DelilahConsole::autoComplete(au::console::ConsoleAutoComplete *info) {
  // Use catalogue for auto-completion
  delilah_command_catalogue_.autoComplete(info);
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
  au::console::CommandInstance *command_instance = delilah_command_catalogue_.parse(command, error);

  if (error.HasErrors()) {
    LM_E(("Error parsing command:'%s', error: '%s'", command.c_str(), error.GetLastError().c_str()));
    write(&error);   // Write errors and messages
    return 0;
  }

  size_t delilah_id = runAsyncCommand(command_instance);
  delete command_instance;
  return delilah_id;
}

size_t DelilahConsole::runAsyncCommand(au::console::CommandInstance *command_instance) {
  std::string mainCommand = command_instance->main_command();

  /*
   * if (mode == mode_database) {
   * // Run data base command
   * std::string result = runDatabaseCommand(command_instance->command_line());
   * writeOnConsole(au::StringInConsole(result));
   * return 0;
   * }
   */

  if (mainCommand == "wlog_show") {
    if (log_probe != NULL) {
      log_probe = NULL;
    }

    std::string host = command_instance->get_string_argument("host");
    std::string format = command_instance->get_string_option("format");
    std::string filter = command_instance->get_string_option("filter");

    log_probe = new au::LogProbe();
    log_probe->AddPlugin("console", new au::LogProbeConsole(this, format));

    au::ErrorManager error;
    log_probe->ConnectAsProbe(host, filter, error);

    if (error.HasErrors()) {
      write(&error);
      log_probe = NULL;
    } else {
      writeWarningOnConsole(au::str("Connected to %s to get logs (Filter %s)", host.c_str(), filter.c_str()));
    }

    return 0;
  }

  if (mainCommand == "wlog_hide") {
    if (log_probe != NULL) {
      writeWarningOnConsole("Logs deactivated");
      log_probe = NULL;
    } else {
      writeWarningOnConsole("Logs were not activated");
    }

    return 0;
  }

  if (au::CheckIfStringsBeginWith(mainCommand, "log_")) {
    au::ErrorManager error;
    au::log_central->evalCommand(command_instance->command_line(), error);
    write(&error);
    return 0;
  }

  if (mainCommand == "connect") {
    // Disconnect first from whatever cluster I am connected to...
    disconnect();

    std::string host = command_instance->get_string_argument("host");
    std::vector<std::string> hosts = au::split(host, ' ');

    if (hosts.size() == 0) {
      writeErrorOnConsole(command_instance->ErrorMessage(au::str("No host provided (%s)", host.c_str())));
    }
    for (int i = 0; i < static_cast<int>(hosts.size()); i++) {
      writeOnConsole(au::str("Connecting to %s...\n", hosts[i].c_str()));

      au::ErrorManager error;
      if (connect(hosts[i], &error)) {
        writeOnConsole("OK\n");
        return 0;
      } else {
        writeErrorOnConsole(error.GetLastError());
      }
    }

    if (hosts.size() > 2) {
      writeErrorOnConsole("Not possible to connect to any hosts\n");
    }
    return 0;
  }

  if (mainCommand == "disconnect") {
    disconnect();
    return 0;
  }

  if (mainCommand == "show_cluster_setup") {
    writeOnConsole(network->getClusterSetupStr() + "\n");
    return 0;
  }

  if (mainCommand == "show_cluster_assignation") {
    writeOnConsole(network->getClusterAssignationStr() + "\n");
    return 0;
  }

  if (mainCommand == "history") {
    int limit = command_instance->get_int_option("limit");
    showMessage(str_history(limit));
    return 0;
  }

  if (mainCommand == "reload_modules_local") {
    au::Singleton<ModulesManager>::shared()->ClearModulesManager();

    au::ErrorManager error;
    au::Singleton<ModulesManager>::shared()->AddModulesFromDefaultDirectory(error);
    if (error.HasErrors()) {
      Write(error);
    } else {
      writeWarningOnConsole("Modules at delilah client have been reloaded.");
    }
  }

  if (mainCommand == "help") {
    std::string concept = command_instance->get_string_argument("concept");

    if (concept != "") {
      writeOnConsole(delilah_command_catalogue_.getHelpForConcept(concept));
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

      writeOnConsole(text);
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
    writeOnConsole(message.str());
    return 0;
  }

  if (mainCommand == "set") {
    std::string name = command_instance->get_string_argument("name");
    std::string value = command_instance->get_string_argument("value");

    // Only set, we show all the defined parameters
    if (name == "") {
      au::tables::Table table(au::StringVector("Property", "Value"));
      table.setTitle("Environent variables");

      std::map<std::string, std::string>::iterator it_environment;
      for (it_environment = environment.environment.begin()
           ; it_environment != environment.environment.end(); it_environment++)
      {
        table.addRow(au::StringVector(it_environment->first, it_environment->second));
      }

      writeOnConsole(table.str());
      return 0;
    }

    environment.set(name, value);
    writeWarningOnConsole(au::str("Environment variable '%s' set to '%s'", name.c_str(), value.c_str()));
    return 0;
  }

  if (mainCommand == "unset") {
    std::string name = command_instance->get_string_argument("name");

    if (!environment.isSet(name)) {
      writeErrorOnConsole(au::str("Variable %s is not set", name.c_str()));
      return 0;
    }

    environment.unset(name);
    writeWarningOnConsole(au::str("Environment variable '%s' is removed ", name.c_str()));
    return 0;
  }

  if (mainCommand == "ls_local_connections") {
    writeOnConsole(GetTableFromCollection(network->GetConnectionsCollection(samson::Visualization()))->str());
    return 0;
  }

  if (mainCommand == "alerts") {
    std::string action = command_instance->get_string_argument("action");

    if (action == "on") {
      show_alerts_ = true;
      writeOnConsole("Alerts are now activated\n");
      return 0;
    }

    if (action == "off") {
      show_alerts_ = false;
      writeOnConsole("Alerts are now NOT activated\n");
      return 0;
    }

    writeErrorOnConsole("Usage: alerts on/off\n");
    return 0;
  }

  if (mainCommand == "verbose") {
    std::string action = command_instance->get_string_argument("action");

    if (action == "") {
      if (verbose_) {
        writeOnConsole("verbose mode is activated\n");
      } else {
        writeOnConsole("verbose mode is NOT activated\n");
      }
      return 0;
    }

    if (action == "on") {
      verbose_ = true;
      writeOnConsole("verbose mode is now activated\n");
      return 0;
    }
    if (action == "off") {
      verbose_ = false;
      writeOnConsole("verbose mode is now NOT activated\n");
      return 0;
    }

    writeErrorOnConsole(command_instance->ErrorMessage("Unknown action"));
    return 0;
  }

  if (mainCommand == "show_alerts") {
    std::string txt = trace_colleciton_.str();
    writeOnConsole(au::StringInConsole(txt));
    return 0;
  }

  if (mainCommand == "open_alerts_file") {
    std::string filename = command_instance->get_string_argument("file");

    if (trace_file_) {
      writeErrorOnConsole(
        au::str("Please close previous alerts file (%s) with command 'close_alerts_file'\n",
                trace_file_name_.c_str()));
      return 0;
    }

    trace_file_ = fopen(filename.c_str(), "w");
    if (!trace_file_) {
      writeErrorOnConsole(
        au::str("Error opening file '%s' to store alerts (%s)\n", trace_file_name_.c_str(),
                strerror(errno)));
      return 0;
    }

    writeOnConsole(au::str("Saving alerts to file '%s'\n", trace_file_name_.c_str()));
    return 0;
  }

  if (mainCommand == "close_alerts_file") {
    if (!trace_file_) {
      writeErrorOnConsole("There is no opened alerts file. Open one with command 'open_alerts_file'\n");
      return 0;
    }

    fclose(trace_file_);
    writeOnConsole(
      "Stop saving alerts to file '%s'.\nRemeber you can open a new alerts file with command 'open_alerts_file\n'");
    return 0;
  }

  if (mainCommand == "ps") {
    if (command_instance->get_bool_option("-clear")) {
      clearComponents();
    }
    std::string str_id = command_instance->get_string_argument("id");

    if (str_id != "") {
      size_t id = atoll(str_id.c_str());

      DelilahComponent *component = components_.findInMap(id);

      if (!component) {
        writeErrorOnConsole(au::str("Unknown process with id %d", id));
      } else {
        au::tables::Table table("Concept|Value,left");

        table.setTitle(au::str("Job %lu description ", id));

        table.addRow(au::StringVector("Delilah", au::code64_str(get_delilah_id())));
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

        writeOnConsole(table.str() + "\n" + component->getExtraStatus());
      }

      return 0;
    }

    std::string txt = getListOfComponents();
    writeOnConsole(au::StringInConsole(txt));
    return 0;
  }

  // Push data to a queue

  if (mainCommand == "ls_local_push_operations") {
    au::tables::Table *table = push_manager->getTableOfItems();
    writeOnConsole(table->str());
    delete table;
    return 0;
  }

  if (mainCommand == "push_module") {
    std::string file_name = command_instance->get_string_argument("file");

    au::ErrorManager error;
    std::vector<std::string> file_names = au::GetListOfFiles(file_name, error);
    if (error.HasErrors()) {
      write(&error);
      return -1;
    }

    return add_push_module_component(file_names);
  }

  if (mainCommand == "push") {
    std::string file_name = command_instance->get_string_argument("file");
    std::string queue = command_instance->get_string_argument("queue");

    au::ErrorManager error;
    std::vector<std::string> file_names = au::GetListOfFiles(file_name, error);
    if (error.HasErrors()) {
      write(&error);
      return -1;
    }

    // Get provided queues
    std::vector<std::string> queues = au::split(queue, ' ');

    // Check queue names
    for (size_t i = 0; i < queues.size(); i++) {
      if (queues[i].size() == 0) {
        writeErrorOnConsole("Invalid queue");
        return -1;
      }

      if (queues[i][0] == '.') {
        writeErrorOnConsole("Invalid queue. Name can not start with a dot (.)");
        return -1;
      }
    }

    size_t id = add_push_component(file_names, queues);
    return id;
  }

  // Connect and disconnect to a queue

  if (mainCommand == "connect_to_queue") {
    std::string queue = command_instance->get_string_argument("queue");
    size_t id = connect_to_queue(queue);
    writeWarningOnConsole(au::str("Connected to queue %s using pop component %lu", queue.c_str(), id));
    return 0;
  }

  // Push data to a queue

  if (mainCommand == "pop") {
    std::string queue_name = command_instance->get_string_argument("queue");
    std::string fileName = command_instance->get_string_argument("file_name");

    bool force_flag = command_instance->get_bool_option("-force");
    bool show_flag = command_instance->get_bool_option("-show");
    size_t id = AddPopComponent(queue_name, fileName, force_flag, show_flag);
    return id;
  }

  if (mainCommand == "rm_local") {
    std::string file = command_instance->get_string_argument("file");
    au::ErrorManager error;
    au::RemoveDirectory(file, error);

    if (error.HasErrors()) {
      writeErrorOnConsole(error.GetLastError());
    } else {
      writeWarningOnConsole("OK");
    }
    return 0;
  }

  if (mainCommand == "ls_local_modules") {
    writeOnConsole(au::Singleton<ModulesManager>::shared()->GetTableOfModules() + "\n");
    return 0;
  }

  if (mainCommand == "ls_local") {
    std::string file = command_instance->get_string_argument("file");

    writeOnConsole(getLsLocal(file, false));
    return 0;
  }

  if (mainCommand == "ls_local_queues") {
    std::string file = command_instance->get_string_argument("file");

    writeOnConsole(getLsLocal(file, true));
    return 0;
  }

  if (mainCommand == "show_local_queue") {
    std::string queue = command_instance->get_string_argument("queue");

    const char *file_name = queue.c_str();
    size_t limit = command_instance->get_uint64_option("-limit");
    bool header = command_instance->get_bool_option("-header");
    bool show_hg = command_instance->get_bool_option("-show_hg");

    struct stat filestatus;
    stat(file_name, &filestatus);

    if (S_ISREG(filestatus.st_mode)) {
      // Open a single file
      au::ErrorManager error;
      au::SharedPointer<samson::SamsonFile> samson_file = samson::SamsonFile::create(file_name, error);

      if (error.HasErrors()) {
        writeErrorOnConsole(au::str("%s", error.GetLastError().c_str()));
        return 0;
      }

      if (header) {
        std::ostringstream output;
        samson_file->printHeader(output);
        writeOnConsole(output.str());
        return 0;
      }

      std::ostringstream output;
      samson_file->printContent(limit, show_hg, output);
      writeOnConsole(output.str());
      return 0;
    } else if (S_ISDIR(filestatus.st_mode)) {
      au::ErrorManager error;
      au::SharedPointer<samson::SamsonDataSet> samson_data_set = samson::SamsonDataSet::create(file_name, error);

      if (error.HasErrors()) {
        writeErrorOnConsole(au::str("%s", error.GetLastError().c_str()));
        return 0;
      }

      if (header) {
        std::ostringstream output;
        samson_data_set->printHeaders(output);
        writeOnConsole(output.str());
        return 0;
      }

      std::ostringstream output;
      samson_data_set->printContent(limit, show_hg, output);
      std::string txt = output.str();
      writeOnConsole(output.str());
      return 0;
    } else {
      writeErrorOnConsole(au::str("%s is not a file or a directory\n", file_name));
      return 0;
    }

    // Not possible to get here...
    return 0;
  }

  // By default, it is considered a worker command
  return sendWorkerCommand(command_instance->command_line());

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
          writeErrorOnConsole(trace_message);
        } else if (_type == "warning") {
          writeWarningOnConsole(trace_message);
        } else {
          writeOnConsole(trace_message);
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

void DelilahConsole::delilahComponentStartNotification(DelilahComponent *component) {
  if (component->hidden) {
    return;   // No notification for hidden processes
  }
  if (verbose_) {
    std::ostringstream o;

    o << "[ " << au::code64_str(get_delilah_id()) << "_" << component->getId() << " ] Process started: ";
    o << component->getConcept();
    if (component->error.HasErrors()) {
      showErrorMessage(o.str());
    } else {
      showMessage(au::str(au::Yellow, "%s", o.str().c_str()));
    }
  }
}

void DelilahConsole::delilahComponentFinishNotification(DelilahComponent *component) {
  if (component->hidden) {
    return;   // No notification for hidden processes
  }
  if (verbose_) {
    std::string component_id = au::str("[ %s_%lu ]", au::code64_str(get_delilah_id()).c_str(), component->getId());

    // Write all messages, warnings and errors for this worker task
    Show(component->error, component_id);

    if (!component->error.HasErrors()) {
      showMessage(au::str(au::Yellow, "[ %s_%lu ] Process finished: %s", au::code64_str(get_delilah_id()).c_str(),
                          component->getId(), component->getConcept().c_str()));
    } else {
      showErrorMessage(
        au::str("[ %s_%lu ] Process finished with error: %s\n",
                au::code64_str(get_delilah_id()).c_str(), component->getId(), component->getConcept().c_str()));
    }
  }
}

// Process received packets with data

void DelilahConsole::receive_buffer_from_queue(std::string queue, engine::BufferPointer buffer) {
  size_t counter = stream_out_queue_counters_.appendAndGetCounterFor(queue);
  size_t packet_size = buffer->size();

  std::string directory_name = au::str("stream_out_%s", queue.c_str());

  if ((mkdir(directory_name.c_str(), 0755) != 0) && (errno != EEXIST)) {
    showErrorMessage(
      au::str("It was not possible to create directory %s to store data from queue %s",
              directory_name.c_str(), queue.c_str()));
    showErrorMessage(au::str("Rejecting a %s data from queue %s", au::str(packet_size, "B").c_str(), queue.c_str()));
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

    showMessage(output.str());
  }

  // Disk operation....
  engine::DiskOperation *o = engine::DiskOperation::newWriteOperation(buffer, fileName, engine_id());
  au::SharedPointer<engine::DiskOperation> operation(o);
  engine::Engine::disk_manager()->Add(operation);
}

void DelilahConsole::runAsyncCommandAndWait(std::string command) {
  LOG_SM(("runAsyncCommandAndWait command:%s", command.c_str()));
  size_t tmp_id = runAsyncCommand(command);

  if (tmp_id == 0) {
    return;   // Sync command
  }
  while (true) {
    if (!isActive(tmp_id)) {
      return;
    }
    usleep(10000);
  }
}
}
