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
 * FILE            main_Delilah.cpp
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * Main file for the "delilah" console app
 *
 */

#include "parseArgs/paBuiltin.h"        // paLsHost, paLsPort
#include "parseArgs/paConfig.h"         // paConfig()
#include "parseArgs/paIsSet.h"
#include "parseArgs/parseArgs.h"

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"


#include "au/ThreadManager.h"
#include "au/log/Log.h"
#include "au/log/LogCentral.h"
#include "au/log/LogCentralPluginConsole.h"
#include "au/log/LogCommon.h"
#include "au/mutex/LockDebugger.h"            // au::LockDebugger
#include "au/string/StringUtilities.h"

#include "engine/DiskManager.h"
#include "engine/Engine.h"
#include "engine/MemoryManager.h"
#include "engine/ProcessManager.h"

#include "samson/common/Logs.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/ports.h"
#include "samson/common/samsonDirectories.h"
#include "samson/common/samsonVars.h"
#include "samson/common/samsonVersion.h"
#include "samson/common/status.h"
#include "samson/module/ModulesManager.h"       // samson::ModulesManager

#include "samson/network/DelilahNetwork.h"
#include "samson/network/Packet.h"

#include "samson/delilah/DelilahConsole.h"

#include "samson/common/status.h"

/* ****************************************************************************
 *
 * Option variables
 */
SAMSON_ARG_VARS;

char user[1024];
char password[1024];

int memory_gb;
int load_buffer_size_mb;
char commandFileName[1024];
bool monitorization;
char command[1024];
bool stdin_commands;
char log_command[1024];
char log_server[1024];
bool interactive_mode;
unsigned short log_port;

char host[1024];

#define LOC      "localhost"
#define LOG_PORT LOG_SERVER_DEFAULT_PORT
/* ****************************************************************************
 *
 * parse arguments
 */
PaArgument paArgs[] =
{
  SAMSON_ARGS,
  { "-log",   log_command,           "",                 PaString,
    PaOpt,    _i "",                 PaNL,
    PaNL,     "log server host"                          },
  { "-log_server",log_server,            "",                 PaString,
    PaOpt,    _i "",                 PaNL,
    PaNL,     "log server host"                          },
  { "-user",  user,                  "",                 PaString,
    PaOpt,    _i "anonymous",        PaNL,
    PaNL,     "User to connect to SAMSON cluster"        },
  { "-password",password,              "",                 PaString,
    PaOpt,    _i "anonymous",        PaNL,
    PaNL,     "Password to connect to SAMSON cluster"    },
  { "-memory",&memory_gb,            "MEMORY",           PaInt,
    PaOpt,    1,                     1,
    100,      "memory in GBytes"                         },
  { "-load_buffer_size",&load_buffer_size_mb,  "LOAD_BUFFER_SIZE", PaInt,
    PaOpt,    64,                    64,
    2048,     "load buffer size in MBytes"               },
  { "-f",     commandFileName,       "FILE_NAME",        PaString,
    PaOpt,    _i "",                 PaNL,
    PaNL,     "File with commands to run"                },
  { "-command",command,               "",                 PaString,
    PaOpt,    _i "",                 PaNL,
    PaNL,     "Single command to be executed"            },
  { "-user",  user,                  "",                 PaString,
    PaOpt,    _i "anonymous",        PaNL,
    PaNL,     "User to connect to SAMSON cluster"        },
  { "-password",password,              "",                 PaString,
    PaOpt,    _i "anonymous",        PaNL,
    PaNL,     "Password to connect to SAMSON cluster"    },
  { "-memory",&memory_gb,            "MEMORY",           PaInt,
    PaOpt,    1,                     1,
    100,      "memory in GBytes"                         },
  { "-load_buffer_size",&load_buffer_size_mb,  "LOAD_BUFFER_SIZE", PaInt,
    PaOpt,    64,                    64,
    2048,     "load buffer size in MBytes"               },
  { "-f",     commandFileName,       "FILE_NAME",        PaString,
    PaOpt,    _i "",                 PaNL,
    PaNL,     "File with commands to run"                },
  { "-command",command,               "",                 PaString,
    PaOpt,    _i "",                 PaNL,
    PaNL,     "Single command to be executed"            },
  { "-",      &stdin_commands,       "",                 PaBool,  PaOpt,  false, false, true },
  { "-i",     &interactive_mode,     "",                 PaBool,  PaOpt,  false, false, true },
  { "",       host,                  "",                 PaString,
    PaOpt,    _i "localhost",        PaNL,
    PaNL,     "host to be connected"                     },

  PA_END_OF_ARGS
};

/* ****************************************************************************
 *
 * logFd - file descriptor for log file used in all libraries
 */
int logFd = -1;

/* ****************************************************************************
 *
 * man texts -
 */
static const char *manSynopsis         = " [OPTION]";
static const char *manShortDescription =  "delilah is the command-line client for SAMSON system\n";
static const char *manDescription      =
  "\n"
  "delilah is the command-line client to upload & download data, run processing commands and monitor a SAMSON system.\n"
  "See pdf document about samson system to get more information about how to use delilah client"
  "\n";

static const char *manExitStatus    = "0      if OK\n 1-255  error\n";

samson::DelilahConsole *delilahConsole = NULL;

void cleanup(void) {
  if (delilahConsole) {
    delilahConsole->StopConsole();
  }

  // Clear google protocol buffers library
  google::protobuf::ShutdownProtobufLibrary();

  LOG_V(samson::logs.cleanup, ("Shutting down delilah components (delilahConsole at %p)", delilahConsole));
  if (delilahConsole != NULL) {
    LOG_V(samson::logs.cleanup, ("deleting delilahConsole"));
    delete delilahConsole;
    delilahConsole = NULL;
  }

  LOG_V(samson::logs.cleanup, ("Calling paConfigCleanup"));
  paConfigCleanup();
  LOG_V(samson::logs.cleanup, ("Calling lmCleanProgName"));
  lmCleanProgName();
  LOG_V(samson::logs.cleanup, ("Cleanup DONE"));

  engine::Engine::StopEngine();

  // Stopping the new log_central thread
  LOG_SM(("Calling au::log_central->Stop()"));
  au::LogCentral::StopLogSystem();
}

// Handy function to find a flag in command line without starting paParse
bool find_flag(int argc, const char *argV[], const char *flag) {
  for (int i = 0; i < argc; i++) {
    if (strcmp(argV[i], flag) == 0) {
      return true;
    }
  }
  return false;
}

// Custom name for the log file
extern char *paProgName;
size_t delilah_random_code;

/* ****************************************************************************
 *
 * main -
 */

int main(int argC, const char *argV[]) {
  // This option makes delilah not to use SAMSON_HOME and SAMSON_WORKING as environment variables,
  // but DELILAH_SAMSON_HOME...
  // We could always define them, but usually we don't use the prefix option.
  // Perhaps the right solution would be to have a configurable "prefixable" option in the arguments

  // paConfig("prefix",                        (void *)"DELILAH_");
  paConfig("builtin prefix", (void *)"SS_DELILAH_");
  paConfig("usage and exit on any warning", (void *)true);
  paConfig("log to screen", (void *)true);
  paConfig("log file line format", (void *)"TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
  paConfig("screen line format", (void *)"TYPE: TEXT");
  paConfig("log to stderr", (void *)true);

  paConfig("man synopsis", (void *)manSynopsis);
  paConfig("man shortdescription", (void *)manShortDescription);
  paConfig("man description", (void *)manDescription);
  paConfig("man exitstatus", (void *)manExitStatus);
  paConfig("man reportingbugs", SAMSON_BUG_REPORTING);
  paConfig("man author", SAMSON_AUTHORS);
  paConfig("man copyright", SAMSON_COPYRIGHT);
  paConfig("man version", SAMSON_VERSION);
  paConfig("default value", "-logDir", (void *)"/var/log/samson");
  paConfig("if hook active, no traces to file", (void *)false);
  paConfig("log to file", (void *)true);

  // Random initialization
  struct timeval tp;
  gettimeofday(&tp, NULL);
  int rand_seq = tp.tv_sec * 1000000 + tp.tv_usec;
  srand(rand_seq);

  // Random code for delilah
  delilah_random_code = au::code64_rand();
  paUsageProgName = strdup("delilah");
  paProgName = strdup(au::str("delilah_%s", au::code64_str(delilah_random_code).c_str()).c_str());

  paParse(paArgs, argC, (char **)argV, 1, true);

  // New log system
  au::LogCentral::InitLogSystem(argV[0]);
  samson::RegisterLogChannels();   // Add all log channels for samson project ( au,engine libraries included )

  // Log to file
  std::string str_log_file = std::string(paLogDir) + "delilah.log";
  au::LogCentral::Shared()->EvalCommand("log_to_file " + str_log_file);

  // Log to server
  std::string str_log_server =  log_server;
  std::string str_log_server_file = std::string(paLogDir) + "delilah_" + log_server +  ".log";
  if (str_log_server != "") {
    au::LogCentral::Shared()->EvalCommand("log_to_server " + str_log_server + " " + str_log_server_file);
  }

  // Log to console or screen
  au::LogCentralPluginConsole *console_log_plugin =
    new au::LogCentralPluginConsole(delilahConsole, "[type][channel] text",
                                    true);
  au::log_central->AddPlugin("console", console_log_plugin);

  // Command line provided interface for logging
  au::LogCentral::Shared()->EvalCommand(log_command);

  // Verbose mode if required
  if (lmVerbose) {
    au::log_central->EvalCommand("log_set system V");
    au::log_central->EvalCommand("log_set delilah::G V");  // Set level for delilah::G channel to "M" ( messages )
  }

  LOG_M(samson::logs.delilah, ("Delilah starting..."));

  // working directories to find modules and stuff
  au::Singleton<samson::SamsonSetup>::shared()->SetWorkerDirectories(samsonHome, samsonWorking);

  // Clean up function
  atexit(cleanup);

  lmAux((char *)"father");
  logFd = lmFirstDiskFileDescriptor();

  // Setup parameters from command line
  size_t _memory = (size_t)memory_gb * (size_t)(1024 * 1024 * 1024);
  size_t _load_buffer_size = (size_t)load_buffer_size_mb * (size_t)(1024 * 1024);

  au::Singleton<samson::SamsonSetup>::shared()->Set("general.memory", au::str("%lu", _memory));
  au::Singleton<samson::SamsonSetup>::shared()->Set("load.buffer_size", au::str("%lu", _load_buffer_size));

  // Engine and its associated elements
  int num_cores = au::Singleton<samson::SamsonSetup>::shared()->GetInt("general.num_processess");
  engine::Engine::InitEngine(num_cores, _memory, 1);

  au::ErrorManager error;
  au::Singleton<samson::ModulesManager>::shared()->AddModulesFromDefaultDirectory(error);
  if (error.HasErrors()) {
    LOG_W(samson::logs.delilah, ("Error loading modules: %s", error.GetLastError().c_str()));
  }

  // List of commands to be executed ( if -command XX or -f XXX or - is specififed )
  std::vector<std::string> commands;
  bool commands_mode = false;

  // Special mode with -c parameter
  if (strcmp(command, "") != 0) {
    commands_mode = true;
    commands.push_back(command);
  }

  // Special mode with -f parameter
  if (strcmp(commandFileName, "") != 0) {
    commands_mode = true;

    FILE *file = fopen(commandFileName, "r");
    if (file == NULL) {
      LOG_X(1, ("Error opening commands file '%s': %s", commandFileName, strerror(errno)));
    }

    int num_line = 0;
    char buffer_line[1024];
    LOG_SV(("Reading commands from file '%s'", commandFileName));

    while (fgets(buffer_line, sizeof(buffer_line), file) != NULL) {
      std::string line = au::StripString(buffer_line);
      ++num_line;
      if ((line.length() > 0) && (line[0] != '#')) {
        commands.push_back(line);
        LOG_SV(("File '%s' [line %d] Command '%s'", commandFileName, num_line, line.c_str()));
      }
    }

    fclose(file);
  }

  if (stdin_commands) {
    commands_mode = true;
    char buffer_line[1024];
    int num_line = 0;
    while (fgets(buffer_line, sizeof(buffer_line), stdin) != NULL) {
      std::string line = au::StripString(buffer_line);
      ++num_line;
      if ((line.length() > 0) && (line[0] != '#')) {
        commands.push_back(line);
        LOG_SV(("Read command '%s' from stdin at line %d\n", line.c_str(), num_line));
      }
    }
  }

  // Create a DelilahConsole
  delilahConsole = new samson::DelilahConsole(delilah_random_code);

  if (commands_mode) {
    // Special setup to execute commands directly
    delilahConsole->set_colors(false);
    delilahConsole->set_verbose(false);
  } else {
    // Normal setup for delilah logging
    au::log_central->EvalCommand("log_set delilah::G M");    // Set level for delilah::G channel to "M" ( messages )
    LOG_M(samson::logs.delilah, ("Delilah running with delilah generated id %s"
                                 , au::code64_str(delilah_random_code).c_str()));
  }

  // Try connect to each host in a given list, in turn, until a successful connection is made
  std::vector<std::string> hosts = au::split(host, ' ');
  for (size_t i = 0; i < hosts.size(); ++i) {
    au::ErrorManager error;
    if (delilahConsole->connect(hosts[i], &error)) {
      LOG_M(samson::logs.delilah, ("Connected to %s", hosts[i].c_str()));
      break;
    } else {
      LOG_W(::samson::logs.delilah, ("Unable to connect to %s: %s",
                                     hosts[i].c_str(),
                                     error.GetLastError().c_str()));
    }
  }

  if (commands_mode) {
    // Execute a set of commands sequentially and exit

    if (!delilahConsole->isConnected()) {
      // If not connected, exit here
      LOG_X(1, ("Delilah client not connected to any SAMSON cluster. Exiting..."));
    }

    if (commands.size() == 0) {
      LOG_X(1, ("No command provided. Nothing to do..."));
    }

    for (size_t i = 0; i < commands.size(); i++) {
      LOG_SV(("Processing: '%s'", commands[i].c_str()));

      size_t id = delilahConsole->runAsyncCommand(commands[i]);
      LOG_SV(("Delilah id generated %lu", id));

      if (id != 0) {
        // Wait until this operation is finished
        au::Cronometer total_cronometer;
        au::Cronometer cronometer;
        while (delilahConsole->isActive(id)) {
          usleep(1000);
          if (cronometer.seconds() > 1) {
            LOG_M(samson::logs.delilah,
                  ("[%s] Waiting for command: %s", total_cronometer.str().c_str(), commands[i].c_str()));
            cronometer.Reset();
          }
          if (total_cronometer.seconds() > 100) {
            LOG_X(1, ("Excessive time for command: %s", commands[i].c_str()));
          }
        }

        if (delilahConsole->hasError(id)) {
          LOG_SE(("Error running '%s' (line %d)", commands[i].c_str(), i + 1));
          LOG_SE(("Error: %s", delilahConsole->errorMessage(id).c_str()));
        }

        // manual output
        std::string output = delilahConsole->getOutputForComponent(id);
        delilahConsole->WriteOnDelilah(output);
      }
    }

    // Flush all messages
    delilahConsole->FlushBackgroundMessages();

    if (!interactive_mode) {
      delilahConsole->disconnect();
      LOG_SV(("delilah exits correctly"));
      au::LogCentral::StopLogSystem();
      exit(0);
    } else {
      // Change to interactive setup
      delilahConsole->set_colors(true);
      delilahConsole->set_verbose(true);
      au::log_central->RemovePlugin("screen");
      au::log_central->AddPlugin("console", new au::LogCentralPluginConsole(delilahConsole, "[type][channel] text"));
      au::log_central->EvalCommand("log_set delilah::G M");
      LOG_M(samson::logs.delilah, ("Delilah running now in interactive mode with delilah generated id %s",
                                   au::code64_str(delilah_random_code).c_str()));
    }
  }

  // Show a warning is console is still un connected
  if (!delilahConsole->isConnected()) {
    LOG_SW(("Delilah client not connected to any SAMSON cluster. ( see help connect )"));
  }

  lmFdUnregister(2);   // no more traces to stdout

  // Use console to display logs
  console_log_plugin->SetConsole(delilahConsole);

  // Start delilah console, blocking this thread
  delilahConsole->StartConsole(true);

  // Stop using console to display logs (use screen instead)
  console_log_plugin->SetConsole(NULL);

  // Disconnect delilah
  delilahConsole->disconnect();

  // Stopping the new log_central thread
  LOG_SM(("Calling au::log_central->Stop()"));
  au::LogCentral::StopLogSystem();

  // Flush content of console
  // delilahConsole->flush();
  LOG_SM(("delilah exits correctly"));

  // Stop log system
  au::LogCentral::StopLogSystem();

  return 0;
}

