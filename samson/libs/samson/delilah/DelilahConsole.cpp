/* ****************************************************************************
 *
 * FILE                     DelilahConsole.h
 *
 * DESCRIPTION			   Console terminal for delilah
 *
 */

#include <dirent.h>                              // DIR directory header
#include <iomanip>
#include <iostream>
#include <sys/stat.h>                            // stat(.)


#include "au/CommandLine.h"                      // au::CommandLine
#include "au/Descriptors.h"                      // au::Descriptors
#include "au/S.h"
#include "au/ThreadManager.h"
#include "au/containers/SharedPointer.h"
#include "au/containers/StringVector.h"
#include "au/file.h"
#include "au/string.h"                           // au::Format

#include "au/tables/Table.h"
#include "au/tables/Tree.h"                      // au::tables::TreeItem

#include "au/log/LogToServer.h"

#include "engine/DiskManager.h"
#include "engine/MemoryManager.h"                // samson::MemoryManager
#include "engine/Notification.h"                 // samson::Notification

#include "au/tables/pugi.h"                      // pugi::Pugi
#include "au/tables/pugixml.hpp"                 // pugi:...

#include "au/log/log_server_common.h"

#include "samson/common/EnvironmentOperations.h"  // Environment operations (CopyFrom)
#include "samson/common/NotificationMessages.h"

#include "samson/network/Packet.h"               // ss:Packet

#include "samson/client/SamsonClient.h"

#include "samson/module/ModulesManager.h"        // samson::ModulesManager
#include "samson/module/samsonVersion.h"         // SAMSON_VERSION

#include "samson/stream/BlockManager.h"          // samson::stream::BlockManager

#include "samson/common/SamsonDataSet.h"
#include "samson/delilah/Delilah.h"              // samson::Delailh
#include "samson/delilah/DelilahConsole.h"       // Own interface
#include "samson/delilah/WorkerCommandDelilahComponent.h"

#define DEF1 "TYPE:EXEC/FUNC: TEXT"


// Fancy function to get function to show clock evolution
std::string strClock(au::Cronometer& cronometer) {
  char c = '-';

  switch ((int)( cronometer.seconds() * 3 ) % 4) {
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
const char *general_description = \
  "SAMSON is a distributed platform for efficient processing of unbounded streams of big data";

const char *auths = "Andreu Urruela, Grant Croker, J.Gregorio Escalada & Ken Zangelin";




DelilahConsole::DelilahConsole(size_t delilah_id) : Delilah("console",
                                                            delilah_id), log_client(AU_LOG_SERVER_QUERY_PORT) {
  // Default values
  show_local_logs = false;
  show_server_logs = false;
  show_alerts = false;
  verbose = true;

  mode = mode_normal;   // Normal mode by default

  // Schedule a notification to review repeat-tasks
  engine::Engine::shared()->notify(new engine::Notification(notification_delilah_review_repeat_tasks), 1);

  // Cool stuff
  addEspaceSequence("samson");
  addEspaceSequence("q");      // ls
  addEspaceSequence("d");      // Database mode...
  addEspaceSequence("l");      // logs mode...
  addEspaceSequence("n");      // normal mode...

  // By default no save traces
  trace_file = NULL;

  simple_output = false;
  no_output = false;

  // Inform about random code for this delilah
  writeWarningOnConsole(au::str("Random delilah id generated [%s]", au::code64_str(get_delilah_id()).c_str()));
}

DelilahConsole::~DelilahConsole() {
}

std::string DelilahConsole::getPrompt() {
  if (mode == mode_database) {
    return "Database >";
  }

  if (mode == mode_logs) {
    return log_client.getPrompt();
  }

  return au::str("[%s] Delilah>", getClusterConnectionSummary().c_str());
}

void DelilahConsole::evalCommand(std::string command) {
  // Run this command
  size_t _delilah_id = runAsyncCommand(command);


  // Wait until this delilah command has finished ( give some options to the user )
  if (_delilah_id != 0) {
    au::Cronometer cronometer;
    while (true) {
      au::ConsoleEntry entry;
      std::string message;
      if (cronometer.seconds() > 1) {
        message = au::str("[ %s ] Waiting process %lu : %s ... [ b: background c: cancel ]"
                          , strClock(cronometer).c_str()
                          , _delilah_id
                          , command.c_str()
                          );
      }
      int s = waitWithMessage(message, 0.2, &entry);

      if (!isActive(_delilah_id)) {
        // Print output
        refresh();
        writeOnConsole(getOutputForComponent(_delilah_id));
        return;
      }

      if (s == 0) {
        // To something with the key

        if (entry.isChar('c')) {
          refresh();
          writeWarningOnConsole(au::str("Canceling process %lu : %s", _delilah_id, command.c_str()));
          cancelComponent(_delilah_id);
          return;
        } else if (entry.isChar('b')) {
          refresh();   // Refresh console
          return;
        }
      }
    }
  }
}

void DelilahConsole::autoCompleteOperations(au::ConsoleAutoComplete *info) {
  std::vector<std::string> operation_names = getOperationNames();

  for (size_t i = 0; i < operation_names.size(); i++) {
    info->add(operation_names[i]);
  }
}

void DelilahConsole::autoCompleteOperations(au::ConsoleAutoComplete *info, std::string type) {
  std::vector<std::string> operation_names = getOperationNames(type);

  for (size_t i = 0; i < operation_names.size(); i++) {
    info->add(operation_names[i]);
  }
}

void DelilahConsole::autoCompleteQueues(au::ConsoleAutoComplete *info) {
  au::tables::Table *table = database.getTable("queues");

  if (!table) {
    return;
  }
  for (size_t r = 0; r <  table->getNumRows(); r++) {
    info->add(table->getValue(r, "name"));
  }
  delete table;
}

void DelilahConsole::autoCompleteQueueWithFormat(
  au::ConsoleAutoComplete *info,
  std::string key_format,
  std::string value_format
  ) {
  au::tables::Table *table = database.getTable("queues");

  if (!table) {
    return;
  }

  for (size_t r = 0; r <  table->getNumRows(); r++) {
    if (table->getValue(r, "format/key_format") == key_format) {
      if (table->getValue(r, "format/value_format") == value_format) {
        info->add(table->getValue(r, "name"));
      }
    }
  }

  delete table;
}

void DelilahConsole::autoCompleteQueueForOperation(au::ConsoleAutoComplete *info, std::string operation_name,
                                                   int argument_pos) {
  // Search in the operations
  Operation *operation = ModulesManager::shared()->getOperation(operation_name);

  if (!operation) {
    return;
  }

  if (argument_pos < operation->getNumInputs()) {
    autoCompleteQueueWithFormat(
      info
      , operation->inputFormats[argument_pos].keyFormat
      , operation->inputFormats[argument_pos].valueFormat
      );
  } else {
    argument_pos -= operation->getNumInputs();
    if (argument_pos < operation->getNumOutputs()) {
      autoCompleteQueueWithFormat(
        info
        , operation->outputFormats[argument_pos].keyFormat
        , operation->outputFormats[argument_pos].valueFormat
        );
    }
  }
}

void DelilahConsole::autoComplete(au::ConsoleAutoComplete *info) {
  if (mode == mode_database) {
    if (info->completingSecondWord("set_mode")) {
      info->add("setup");
      info->add("normal");
      info->add("database");
      info->add("logs");
    }

    if (info->completingFirstWord()) {
      info->add("set_database_mode off");
    }
    autoCompleteForDatabaseCommand(info);
    return;
  }

  if (mode == mode_logs) {
    if (info->completingSecondWord("set_mode")) {
      info->add("setup");
      info->add("normal");
      info->add("database");
      info->add("logs");
    }

    log_client.autoComplete(info);
    return;
  }

  // Use catalogue for auto-completion
  delilah_command_catalogue.autoComplete(info);
  return;
}

void DelilahConsole::run() {
  // If command-file is provided
  if (commandFileName.length() > 0) {
    FILE *f = fopen(commandFileName.c_str(), "r");
    if (!f) {
      LM_E(("Error opening commands file %s", commandFileName.c_str()));
      exit(0);
    }

    char line[1024];

    while (fgets(line, sizeof(line), f)) {
      // Remove the last return of a string
      while (( strlen(line) > 0 ) && ( line[ strlen(line) - 1] == '\n') > 0) {
        line[ strlen(line) - 1] = '\0';
      }

      // LM_M(("Processing line: %s", line ));
      size_t id = runAsyncCommand(line);

      if (id != 0) {
        // LM_M(("Waiting until delilah-component %ul finish", id ));
        // Wait until this operation is finished
        while (isActive(id)) {
          sleep(1);
        }
      }
    }

    fclose(f);

    return;
  } else {
    runConsole();
  }
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
    output << list[i];;
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
  au::console::CommandInstance *command_instance = delilah_command_catalogue.parse(command, &error);

  if (error.IsActivated()) {
    write(&error);     // Write errors and messages
    return 0;
  }

  size_t delilah_id = runAsyncCommand(command_instance);
  delete command_instance;
  return delilah_id;
}

size_t DelilahConsole::runAsyncCommand(au::console::CommandInstance *command_instance) {
  std::string mainCommand = command_instance->main_command();

  // Common command in all modes
  if (mainCommand == "set_mode") {
    std::string str_mode = command_instance->get_string_argument("mode");

    if (str_mode == "normal") {
      mode = mode_normal;
      writeWarningOnConsole("Normal mode activated\n");
    } else if (str_mode == "database") {
      mode = mode_database;
      writeWarningOnConsole("Database mode activated\n");
    } else if (str_mode == "logs") {
      mode = mode_logs;
      writeWarningOnConsole("logs mode activated\n");
    } else {
      writeErrorOnConsole(command_instance->ErrorMessage("Unknown mode"));
    }

    return 0;
  }

  if (mode == mode_logs) {
    au::ErrorManager error;
    log_client.evalCommand(command_instance->command_line(), &error);
    write(&error);     // Console method to write all the answers
    return 0;
  }

  if (mode == mode_database) {
    // Run data base command
    std::string result = runDatabaseCommand(command_instance->command_line());
    writeOnConsole(au::strToConsole(result));
    return 0;
  }

  if (mainCommand == "connect") {
    // Disconnect first from whatever cluster I am connected to...
    disconnect();

    std::string host = command_instance->get_string_option("host");
    std::vector<std::string> hosts = au::split(host, ' ');

    if (hosts.size() == 0) {
      writeErrorOnConsole(command_instance->ErrorMessage(au::str("No host provided (%s)", host.c_str())));
    }
    for (int i =
           0;
         i <
         (int)
         hosts.
         size();
         i++)
    {
      writeOnConsole(au::str("Connecting to %s...\n", hosts[i].c_str()));

      au::ErrorManager error;
      if (connect(hosts[i], &error)) {
        writeOnConsole("OK\n");
        return 0;
      } else {
        writeErrorOnConsole(error.GetMessage());
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

  if (mainCommand == "reload_modules") {
    ModulesManager::shared()->reloadModules();
    writeWarningOnConsole("Modules at delilah client have been reloaded.");
  }

  if (mainCommand == "help") {
    std::string concept = command_instance->get_string_argument("concept");

    if (concept != "") {
      writeOnConsole(delilah_command_catalogue.getHelpForConcept(concept));
      return 0;
    } else {
      std::ostringstream output;
      output << "\n";
      output << au::lineInConsole('=') << "\n";
      output << " SAMSON v " << SAMSON_VERSION << "\n";
      output << au::lineInConsole('=') << "\n";
      output << "\n";
      output << au::str_indent(general_description) << "\n";
      output << "\n";
      output << au::str_indent(au::str("Authors: %s", auths)) << "\n";
      output << "\n";
      output << au::str_indent(au::str("Telefonica I+D 2010-2012")) << "\n";
      output << "\n";
      output << au::lineInConsole('-') << "\n";
      output << "\n";
      output << "\thelp all .................. get a list of all available commands\n";
      output << "\thelp categories ........... get a list of command categories\n";
      output << "\thelp <command> ............ get detailed information for a command\n";
      output << "\thelp <category> ........... get list of commands for a particular categoriy\n";
      output << "\n";
      output << au::lineInConsole('-') << "\n";
      output << "\n";


      std::string text = output.str();

      writeOnConsole(text);
      return 0;
    }
  }

  if (mainCommand == "quit") {
    Console::quitConsole();     // Quit the console
    return 0;
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
    if (name  == "") {
      au::tables::Table table(au::StringVector("Property", "Value"));
      table.setTitle("Environent variables");

      std::map<std::string, std::string>::iterator it_environment;
      for (it_environment = environment.environment.begin()
           ; it_environment != environment.environment.end()
           ; it_environment++)
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
    samson::Visualization v;   // No visualization options here
    au::tables::Table *table = WorkerCommandDelilahComponent::getStaticTable(network->getConnectionsCollection(v));
    writeOnConsole(table->str());
    delete table;
    return 0;
  }

  if (mainCommand == "local_logs") {
    std::string action = command_instance->get_string_argument("action");

    if (action == "on") {
      if (!show_local_logs) {
        // Connect plugin
        add_log_plugin(this);
      }
      show_local_logs = true;
      writeOnConsole("Local logs are now activated\n");
      return 0;
    }

    if (action == "off") {
      if (show_local_logs) {
        // Disconnec plugin
        remove_log_plugin(this);
      }
      show_local_logs = false;
      writeOnConsole("Local logs are now NOT activated\n");
      return 0;
    }

    writeErrorOnConsole("Usage: alerts on/off\n");
    return 0;
  }


  if (mainCommand == "alerts") {
    std::string action = command_instance->get_string_argument("action");

    if (action == "on") {
      show_alerts = true;
      writeOnConsole("Alerts are now activated\n");
      return 0;
    }

    if (action == "off") {
      show_alerts = false;
      writeOnConsole("Alerts are now NOT activated\n");
      return 0;
    }

    writeErrorOnConsole("Usage: alerts on/off\n");
    return 0;
  }

  if (mainCommand == "verbose") {
    std::string action = command_instance->get_string_argument("action");

    if (action == "") {
      if (verbose) {
        writeOnConsole("verbose mode is activated\n");
      } else {
        writeOnConsole("verbose mode is NOT activated\n");
      } return 0;
    }

    if (action == "on") {
      verbose = true;
      writeOnConsole("verbose mode is now activated\n");
      return 0;
    }
    if (action == "off") {
      verbose = false;
      writeOnConsole("verbose mode is now NOT activated\n");
      return 0;
    }

    writeErrorOnConsole(command_instance->ErrorMessage("Unknown action"));
    return 0;
  }

  if (mainCommand == "show_alerts") {
    std::string txt = trace_colleciton.str();
    writeOnConsole(au::strToConsole(txt));
    return 0;
  }

  if (mainCommand == "open_alerts_file") {
    std::string filename = command_instance->get_string_argument("file");

    if (trace_file) {
      writeErrorOnConsole(au::str("Please close previous alerts file (%s) with command 'close_alerts_file'\n"
                                  , trace_file_name.c_str()));
      return 0;
    }

    trace_file = fopen(filename.c_str(), "w");
    if (!trace_file) {
      writeErrorOnConsole(au::str("Error opening file '%s' to store alerts (%s)\n"
                                  , trace_file_name.c_str()
                                  , strerror(errno))
                          );
      return 0;
    }

    writeOnConsole(au::str("Saving alerts to file '%s'\n", trace_file_name.c_str()));
    return 0;
  }

  if (mainCommand == "close_alerts_file") {
    if (!trace_file) {
      writeErrorOnConsole("There is no opened alerts file. Open one with command 'open_alerts_file'\n");
      return 0;
    }

    fclose(trace_file);
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
          table.addRow(au::StringVector("Status", "Finished"));
        } else {
          table.addRow(au::StringVector("Status", "Running " + au::str_time(component->cronometer.seconds())));
        }
        if (component->error.IsActivated()) {
          table.addRow(au::StringVector("Error", component->error.GetMessage()));
        }
        table.addRow(au::StringVector("Status", component->getStatus()));

        writeOnConsole(table.str() + "\n" + component->getExtraStatus());
      }

      return 0;
    }

    std::string txt = getListOfComponents();
    writeOnConsole(au::strToConsole(txt));
    return 0;
  }

  // Push data to a queue

  if (mainCommand == "ls_local_push_operations") {
    au::tables::Table *table = push_manager->getTableOfItems();
    writeOnConsole(table->str());
    delete table;
    return 0;
  }

  if (mainCommand == "push") {
    std::string file = command_instance->get_string_argument("file");
    std::string queue = command_instance->get_string_argument("queue");


    std::vector<std::string> fileNames;
    struct stat buf;
    int rc = stat(file.c_str(), &buf);

    if (rc) {
      writeErrorOnConsole(au::str("%s is not a valid local file or dir ", file.c_str()));
      return 0;
    }

    if (S_ISREG(buf.st_mode)) {
      if (verbose) {
        std::ostringstream message;
        message << "Including regular file " << file << " with " <<  au::str((size_t)buf.st_size) << " Bytes\n";
        showMessage(message.str());
      }

      fileNames.push_back(file);
    } else if (S_ISDIR(buf.st_mode)) {
      if (verbose) {
        std::ostringstream message;
        message << "Including directory " << file << "\n";
        writeOnConsole(message.str());
      }

      {
        // first off, we need to create a pointer to a directory
        DIR *pdir = opendir(file.c_str());    // "." will refer to the current directory
        struct dirent *pent = NULL;
        if (pdir != NULL) {  // if pdir wasn't initialised correctly
          while ((pent = readdir(pdir))) {  // while there is still something in the directory to list
            if (pent != NULL) {
              std::ostringstream localFileName;
              localFileName << file << "/" << pent->d_name;

              struct stat buf2;
              stat(localFileName.str().c_str(), &buf2);

              if (S_ISREG(buf2.st_mode)) {
                fileNames.push_back(localFileName.str());
              }
            }
          }
          // finally, let's close the directory
          closedir(pdir);
        }
      }
    } else {
      writeErrorOnConsole(au::str("%s is not a valid local file or dir ", file.c_str()));
      return 0;
    }

    // Get provided queues
    std::vector<std::string> queues = au::split(queue, ' ');

    size_t id = add_push_component(fileNames, queues);
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
    std::string queue_name  = command_instance->get_string_argument("queue");
    std::string fileName    = command_instance->get_string_argument("file_name");

    bool force_flag = command_instance->get_bool_option("-force");
    bool show_flag = command_instance->get_bool_option("-show");
    size_t id = AddPopComponent(queue_name,  fileName, force_flag, show_flag);
    return id;
  }

  if (mainCommand == "rm_local") {
    std::string file = command_instance->get_string_argument("file");
    au::ErrorManager error;
    au::removeDirectory(file, error);

    if (error.IsActivated()) {
      writeErrorOnConsole(error.GetMessage());
    } else {
      writeWarningOnConsole("OK");
    } return 0;
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

      if (error.IsActivated()) {
        writeErrorOnConsole(au::str("%s", error.GetMessage().c_str()));
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

      if (error.IsActivated()) {
        writeErrorOnConsole(au::str("%s", error.GetMessage().c_str()));
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
      std::string txt =  output.str();
      writeOnConsole(output.str());
      return 0;
    } else {
      writeErrorOnConsole(au::str("%s is not a file or a directory\n", file_name));
      return 0;
    }

    // Not possible to get here...
    return 0;
  }

  /*
   *
   * if( mainCommand == "push_module" )
   * {
   *
   * if( commandLine.get_num_arguments() < 3 )
   * {
   *  writeErrorOnConsole( "Usage: push_module <file> <module_name>");
   *  return 0;
   * }
   *
   * std::string file_name = commandLine.get_argument(1);
   * std::string module_name = commandLine.get_argument(2);
   *
   * struct ::stat info;
   * if( stat(file_name.c_str(), &info) != 0 )
   * {
   *  writeErrorOnConsole( au::str("Error reading file %s (%s)" , file_name.c_str() , strerror(errno) ) );
   *  return 0;
   * }
   * // Size of the file
   * size_t file_size = info.st_size;
   * engine::BufferPointer buffer = engine::Engine::memory_manager()->createBuffer("push_module" , "delilah", file_size );
   * buffer->setSize(file_size);
   *
   * // Load the file
   * FILE* file = fopen( file_name.c_str(), "r");
   * if( fread(buffer->getData(), file_size, 1, file) != 1 )
   *  LM_W(("Errro reading file %s" , file_name.c_str() ));
   * fclose(file);
   *
   * size_t tmp_id = sendWorkerCommand( au::str("push_module %s" , module_name.c_str() ) , buffer );
   *
   * // Release the buffer we have just created
   * buffer->Release();
   *
   * return tmp_id;
   * }
   */

  // By default, it is considered a worker command
  return sendWorkerCommand(command_instance->command_line());

  return 0;
}

int DelilahConsole::_receive(const PacketPointer& packet) {
  switch (packet->msgCode) {
    case Message::Alert:
    {
      std::string _text     = packet->message->alert().text();
      std::string _type     = packet->message->alert().type();
      std::string _context  = packet->message->alert().context();

      // Add to the local collection of traces
      trace_colleciton.add(packet->from, _type, _context, _text);

      // Write to disk if required
      if (trace_file) {
        std::string trace_message =  au::str("%s %s %s %s\n"
                                             , packet->from.str().c_str()
                                             , _type.c_str()
                                             , _context.c_str()
                                             , _text.c_str()
                                             );
        fwrite(trace_message.c_str(), trace_message.length(), 1, trace_file);
      }

      if (show_alerts) {
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
      LM_W(("Unknown message %s received at delilahConsole", Message::messageCode(packet->msgCode)));
      break;
  }


  return 0;
}

void DelilahConsole::delilahComponentStartNotification(DelilahComponent *component) {
  if (component->hidden) {
    return;   // No notification for hidden processes
  }
  if (verbose) {
    std::ostringstream o;

    o << "Process started: " << au::code64_str(get_delilah_id()) << "_" <<  component->getId() << " " <<
    component->getConcept() << "\n";
    if (component->error.IsActivated()) {
      showErrorMessage(o.str());
    } else {
      showWarningMessage(o.str());
    }
  }
}

void DelilahConsole::delilahComponentFinishNotification(DelilahComponent *component) {
  if (component->hidden) {
    return;   // No notification for hidden processes
  }
  if (verbose) {
    if (!component->error.IsActivated()) {
      showWarningMessage(au::str("Process finished: %s_%lu %s\n"
                                 , au::code64_str(get_delilah_id()).c_str()
                                 , component->getId()
                                 , component->getConcept().c_str()));
    } else {
      showErrorMessage(au::str("Process finished with error: %s_%lu %s\nERROR: %s\n"
                               , au::code64_str(get_delilah_id()).c_str()
                               , component->getId()
                               , component->getConcept().c_str()
                               , component->error.GetMessage().c_str()
                               )
                       );
    }
  }
}

// Process received packets with data

void DelilahConsole::receive_buffer_from_queue(std::string queue, engine::BufferPointer buffer) {
  size_t counter = stream_out_queue_counters.appendAndGetCounterFor(queue);
  size_t packet_size = buffer->getSize();

  std::string directory_name = au::str("stream_out_%s", queue.c_str());

  if (( mkdir(directory_name.c_str(), 0755) != 0 ) && ( errno != EEXIST )) {
    showErrorMessage(au::str("It was not possible to create directory %s to store data from queue %s",
                             directory_name.c_str(), queue.c_str()));
    showErrorMessage(au::str("Rejecting a %s data from queue %s"
                             , au::str(packet_size, "B").c_str()
                             , queue.c_str()));
    return;
  }

  std::string fileName = au::str("%s/block_%l05u", directory_name.c_str(), counter);

  if (verbose) {
    // Show the first line or key-value
    SamsonClientBlock samson_client_block(buffer);      // Not remove buffer at destrutor

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
  engine::DiskOperation *o = engine::DiskOperation::newWriteOperation(buffer,  fileName, getEngineId());
  au::SharedPointer<engine::DiskOperation> operation(o);
  engine::Engine::disk_manager()->Add(operation);
}

void DelilahConsole::runAsyncCommandAndWait(std::string command) {
  LM_M(("runAsyncCommandAndWait command:%s", command.c_str()));
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
