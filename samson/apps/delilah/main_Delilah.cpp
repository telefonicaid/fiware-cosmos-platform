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

char log_command[1024];
char log_server[1024];
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
  { "-log",   log_command,           "",                    PaString,
    PaOpt,    _i "",                 PaNL,
    PaNL,     "log server host"                          },
  { "-log_server",log_server,            "",                    PaString,
    PaOpt,    _i "",                 PaNL,
    PaNL,     "log server host"                          },
  { "-user",  user,                  "",                    PaString,
    PaOpt,    _i "anonymous",        PaNL,
    PaNL,     "User to connect to SAMSON cluster"        },
  { "-password",password,              "",                    PaString,
    PaOpt,    _i "anonymous",        PaNL,
    PaNL,     "Password to connect to SAMSON cluster"    },
  { "-memory",&memory_gb,            "MEMORY",              PaInt,
    PaOpt,    1,                     1,
    100,      "memory in GBytes"                         },
  { "-load_buffer_size",&load_buffer_size_mb,  "LOAD_BUFFER_SIZE",    PaInt,
    PaOpt,    64,                    64,
    2048,     "load buffer size in MBytes"               },
  { "-f",     commandFileName,       "FILE_NAME",           PaString,
    PaOpt,    _i "",                 PaNL,
    PaNL,     "File with commands to run"                },
  { "-command",command,               "",                    PaString,
    PaOpt,    _i "",                 PaNL,
    PaNL,     "Single command to be executed"            },
  { "-user",  user,                  "",                    PaString,
    PaOpt,    _i "anonymous",        PaNL,
    PaNL,     "User to connect to SAMSON cluster"        },
  { "-password",password,              "",                    PaString,
    PaOpt,    _i "anonymous",        PaNL,
    PaNL,     "Password to connect to SAMSON cluster"    },
  { "-memory",&memory_gb,            "MEMORY",              PaInt,
    PaOpt,    1,                     1,
    100,      "memory in GBytes"                         },
  { "-load_buffer_size",&load_buffer_size_mb,  "LOAD_BUFFER_SIZE",    PaInt,
    PaOpt,    64,                    64,
    2048,     "load buffer size in MBytes"               },
  { "-f",     commandFileName,       "FILE_NAME",           PaString,
    PaOpt,    _i "",                 PaNL,
    PaNL,     "File with commands to run"                },
  { "-command",command,               "",                    PaString,
    PaOpt,    _i "",                 PaNL,
    PaNL,     "Single command to be executed"            },
  { "",       host,                  "",                    PaString,
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
static const char *manAuthor        = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char *manReportingBugs = "bugs to samson-dev@tid.es\n";
static const char *manCopyright     = "Copyright (C) 2011 Telefonica Investigacion y Desarrollo";
static const char *manVersion       = SAMSON_VERSION;

samson::DelilahConsole *delilahConsole = NULL;

void cleanup(void) {
  if (delilahConsole) {
    delilahConsole->StopConsole();
  }

  // Clear google protocol buffers library
  google::protobuf::ShutdownProtobufLibrary();

  LOG_M(samson::logs.cleanup, ("Shutting down delilah components (delilahConsole at %p)", delilahConsole));
  if (delilahConsole != NULL) {
    LOG_M(samson::logs.cleanup, ("deleting delilahConsole"));
    delete delilahConsole;
    delilahConsole = NULL;
  }

  LOG_M(samson::logs.cleanup, ("Calling paConfigCleanup"));
  paConfigCleanup();
  LOG_M(samson::logs.cleanup, ("Calling lmCleanProgName"));
  lmCleanProgName();
  LOG_M(samson::logs.cleanup, ("Cleanup DONE"));

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
  paConfig("man author", (void *)manAuthor);
  paConfig("man reportingbugs", (void *)manReportingBugs);
  paConfig("man copyright", (void *)manCopyright);
  paConfig("man version", (void *)manVersion);
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

  std::string str_log_file = std::string(paLogDir) + "delilah.log";
  std::string str_log_server =  log_server;
  std::string str_log_server_file = std::string(paLogDir) + "delilah_" + log_server +  ".log";

  au::LogCentral::InitLogSystem(argV[0]);
  samson::RegisterLogChannels();   // Add all log channels for samson project ( au,engine libraries included )

  au::log_central->evalCommand("log_to_file " + str_log_file);
  if (str_log_server != "") {
    au::log_central->evalCommand("log_to_server " + str_log_server + " " + str_log_server_file);
  }
  au::log_central->evalCommand(log_command);  // Command provided in command line

  LOG_M(samson::logs.delilah, ("Delilah starting..."));

  // working directories to find modules and stuff
  au::Singleton<samson::SamsonSetup>::shared()->SetWorkerDirectories(samsonHome, samsonWorking);

  // Clean up function
  atexit(cleanup);

  // Start secondary log system
  // TODO: Complete this initialization

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
  if (error.IsActivated()) {
    LOG_W(samson::logs.delilah, ("Error loading modules: %s", error.GetMessage().c_str()));
  }

  // Create a DelilahControler once network is ready
  delilahConsole = new samson::DelilahConsole(delilah_random_code);

  // Change log to console
  au::log_central->evalCommand("screen off");   // Disable log to screen since we log to console
  au::log_central->AddPlugin("console", new au::LogCentralPluginConsole(delilahConsole));

  LOG_M(samson::logs.delilah, ("Delilah running..."));

  std::vector<std::string> hosts = au::split(host, ' ');
  for (size_t i = 0; i < hosts.size(); i++) {
    au::ErrorManager error;
    if (delilahConsole->connect(hosts[i], &error)) {
      delilahConsole->writeWarningOnConsole(au::str("Connected to %s", hosts[i].c_str()));
      break;
    } else {
      delilahConsole->writeWarningOnConsole(au::str("Not possible to connect with %s: %s", hosts[i].c_str(),
                                                    error.GetMessage().c_str()));
    }
  }

  if (!delilahConsole->isConnected()) {
    LOG_SW(("Delilah client not connected to any SAMSON cluster. ( see help connect )"));
  }

  // Special mode with one command line command
  // ----------------------------------------------------------------

  if (strcmp(command, "") != 0) {
    au::log_central->AddScreenPlugin("screen", "[type][channel] text");    // Activate logs at screen
    {
      au::Cronometer cronometer;
      while (!delilahConsole->isConnected()) {
        usleep(100000);
        if (cronometer.seconds() > 1) {
          LOG_SW(("delilahConsoleConnection is not ready, waiting to connect to all workers"));
          LOG_SV(("Waiting delilah to connect to all workers"));
          cronometer.Reset();
        }
      }
    }
    // Set output to screen
    delilahConsole->setNoOutput();
    size_t id = delilahConsole->runAsyncCommand(command);

    LOG_SM(("runAsyncCommand returned for command:'%s', id:%d", command, id));
    if (id != 0) {
      // Wait until this operation is finished
      while (delilahConsole->isActive(id)) {
        // TODO(@jges): Remove log message
        LOG_SM(("Sleep after check isActive command_id:%d", id));

        usleep(100000);
      }
      LOG_SM(("Command activity is finished for command:'%s', id:%d", command, id));

      if (delilahConsole->hasError(id)) {
        LOG_E(samson::logs.delilah, ("Error running '%s'", command));
        LOG_E(samson::logs.delilah, ("Error: %s",  delilahConsole->errorMessage(id).c_str()));
      } else {
        printf("%s", delilahConsole->getOutputForComponent(id).c_str());
        fflush(stdout);
      }
    }

    // Disconnect delilah
    LOG_SM(("Calling delilahConsole->disconnect()"));
    delilahConsole->disconnect();

    exit(0);
  }

  lmFdUnregister(2);   // no more traces to stdout

  // ----------------------------------------------------------------
  // Special mode for file-based commands
  // ----------------------------------------------------------------

  if (strcmp(commandFileName, "") != 0) {
    au::log_central->AddScreenPlugin("screen", "[type][channel] text");    // Activate logs at screen

    delilahConsole->setSimpleOutput();

    {
      au::Cronometer cronometer;
      while (!delilahConsole->isConnected()) {
        usleep(100000);
        if (cronometer.seconds() > 1) {
          LOG_SM(("Waiting delilah to connect to all workers"));
          cronometer.Reset();
        }
      }
    }

    FILE *f = fopen(commandFileName, "r");
    if (!f) {
      LM_E(("Error opening commands file %s", commandFileName));
      exit(0);
    }

    int num_line = 0;
    char line[1024];

    std::cerr << au::str("Processing commands file %s\n", commandFileName);

    while (fgets(line, sizeof(line), f)) {
      // Remove the last return of a string
      while ((strlen(line) > 0) && (line[strlen(line) - 1] == '\n') > 0) {
        line[strlen(line) - 1] = '\0';
      }

      num_line++;

      if ((line[0] != '#') && (strlen(line) > 0)) {
        std::cerr << au::str("Processing: '%s'\n", line);
        size_t id = delilahConsole->runAsyncCommand(line);
        std::cerr << au::str("Delilah id generated %lu\n", id);

        if (id != 0) {
          // Wait until this operation is finished
          while (delilahConsole->isActive(id)) {
            usleep(1000);
          }

          if (delilahConsole->hasError(id)) {
            LM_E(("Error running '%s' at line %d", line, num_line));
            LM_E(("Error: %s", delilahConsole->errorMessage(id).c_str()));
          }
        }
      }
    }

    fclose(f);

    delilahConsole->disconnect();

    LOG_SM(("delilah exit correctly"));
    au::LogCentral::StopLogSystem();    // Stopping the new log_central thread

    exit(0);
  }

  // Start delilah console blockign this thread
  delilahConsole->StartConsole(true);

  // Disconnect delilah
  delilahConsole->disconnect();

  // Stopping the new log_central thread
  LOG_SM(("Calling au::log_central->Stop()"));
  au::LogCentral::StopLogSystem();

  // Flush content of console
  // delilahConsole->flush();
  LOG_SM(("delilah exit correctly"));

  // Stop log system
  au::LogCentral::StopLogSystem();

  return 0;
}

