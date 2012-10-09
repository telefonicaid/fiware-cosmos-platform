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
#include "au/log/LogPluginConsole.h"
#include "au/log/LogCommon.h"
#include "au/mutex/LockDebugger.h"            // au::LockDebugger
#include "au/string.h"

#include "engine/DiskManager.h"
#include "engine/Engine.h"
#include "engine/MemoryManager.h"
#include "engine/ProcessManager.h"

#include "samson/module/ModulesManager.h"       // samson::ModulesManager
#include "samson/common/samsonDirectories.h"
#include "samson/common/samsonVars.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/ports.h"
#include "samson/common/samsonVersion.h"
#include "samson/common/status.h"

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
#define LOG_PORT AU_LOG_SERVER_PORT
/* ****************************************************************************
 *
 * parse arguments
 */
PaArgument paArgs[] =
{
  SAMSON_ARGS,
  { "-log",         log_command,                       "",                           PaString,
    PaOpt,                          _i "",     PaNL,
    PaNL,    "log server host"                          },
  { "-log_server",  log_server,                        "",                           PaString,
    PaOpt,                          _i "",     PaNL,
    PaNL,    "log server host"                          },
  { "-user",             user,                           "",                           PaString,
    PaOpt,                          _i "anonymous",     PaNL,
    PaNL,    "User to connect to SAMSON cluster"        },
  { "-password",         password,                       "",                           PaString,
    PaOpt,                          _i "anonymous",     PaNL,
    PaNL,    "Password to connect to SAMSON cluster"    },
  { "-memory",           &memory_gb,                     "MEMORY",                     PaInt,
    PaOpt,                          1,                  1,
    100,     "memory in GBytes"                         },
  { "-load_buffer_size", &load_buffer_size_mb,           "LOAD_BUFFER_SIZE",           PaInt,
    PaOpt,                          64,                 64,
    2048,    "load buffer size in MBytes"               },
  { "-f",                commandFileName,                "FILE_NAME",                  PaString,
    PaOpt,                          _i "",              PaNL,
    PaNL,    "File with commands to run"                },
  { "-command",          command,                        "",                           PaString,
    PaOpt,                          _i "",              PaNL,
    PaNL,    "Single command to be executed"            },
  { "-user",             user,                           "",                           PaString,
    PaOpt,                          _i "anonymous",     PaNL,
    PaNL,    "User to connect to SAMSON cluster"        },
  { "-password",         password,                       "",                           PaString,
    PaOpt,                          _i "anonymous",     PaNL,
    PaNL,    "Password to connect to SAMSON cluster"    },
  { "-memory",           &memory_gb,                     "MEMORY",                     PaInt,
    PaOpt,                          1,                  1,
    100,     "memory in GBytes"                         },
  { "-load_buffer_size", &load_buffer_size_mb,           "LOAD_BUFFER_SIZE",           PaInt,
    PaOpt,                          64,                 64,
    2048,    "load buffer size in MBytes"               },
  { "-f",                commandFileName,                "FILE_NAME",                  PaString,
    PaOpt,                          _i "",              PaNL,
    PaNL,    "File with commands to run"                },
  { "-command",          command,                        "",                           PaString,
    PaOpt,                          _i "",              PaNL,
    PaNL,    "Single command to be executed"            },
  { "",                  host,                           "",                           PaString,
    PaOpt,                          _i "localhost",     PaNL,
    PaNL,    "host to be connected"                     },

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
  // Stop engine
  engine::Engine::StopEngine();

   if( delilahConsole ) {
     delilahConsole->stop();
   }

  // Wait all threads to finish
  au::Singleton<au::ThreadManager>::shared()->wait("Delilah");

  // Clear google protocol buffers library
  google::protobuf::ShutdownProtobufLibrary();

  LM_T(LmtCleanup, ("Shutting down delilah components (delilahConsole at %p)", delilahConsole));
  if (delilahConsole != NULL) {
    LM_T(LmtCleanup, ("deleting delilahConsole"));
    delete delilahConsole;
    delilahConsole = NULL;
  }

  LM_T(LmtCleanup, ("Calling paConfigCleanup"));
  paConfigCleanup();
  LM_T(LmtCleanup, ("Calling lmCleanProgName"));
  lmCleanProgName();
  LM_T(LmtCleanup, ("Cleanup DONE"));

  // Remove engine
  engine::Engine::DestroyEngine();

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
  paConfig("prefix", (void *) "DELILAH_");
  paConfig("builtin prefix", (void *) "SS_DELILAH_");
  paConfig("usage and exit on any warning", (void *) true);
  paConfig("log to screen", (void *) true);
  paConfig("log file line format", (void *) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
  paConfig("screen line format", (void *) "TYPE: TEXT");
  paConfig("log to stderr", (void *) true);

  paConfig("man synopsis", (void *) manSynopsis);
  paConfig("man shortdescription", (void *) manShortDescription);
  paConfig("man description", (void *) manDescription);
  paConfig("man exitstatus", (void *) manExitStatus);
  paConfig("man author", (void *) manAuthor);
  paConfig("man reportingbugs", (void *) manReportingBugs);
  paConfig("man copyright", (void *) manCopyright);
  paConfig("man version", (void *) manVersion);
  paConfig("default value", "-logDir", (void *) "/var/log/samson");
  paConfig("if hook active, no traces to file", (void *) false);
  paConfig("log to file", (void *) true);

  // Random initialization
  struct timeval tp;
  gettimeofday(&tp, NULL);
  int rand_seq = tp.tv_sec * 1000000 + tp.tv_usec;
  srand(rand_seq);

  // Random code for delilah
  delilah_random_code = au::code64_rand();
  paUsageProgName = strdup("delilah");
  paProgName = strdup(au::str("delilah_%s", au::code64_str(delilah_random_code).c_str()).c_str());

  paParse(paArgs, argC, (char **) argV, 1, true);

  // New log system
  
  std::string str_log_file = std::string(paLogDir) + "delilah.log";
  std::string str_log_server =  log_server;
  std::string str_log_server_file = std::string(paLogDir) + "delilah_" + log_server +  ".log";
  
  au::log_central.Init( argV[0] );
  au::log_central.evalCommand("log_to_file " + str_log_file);
  if( str_log_server != "" )
    au::log_central.evalCommand("log_to_server " + str_log_server + " " + str_log_server_file );
  au::log_central.evalCommand(log_command);  // Command provided in command line

  AU_LM_M(("Delilah starting..."));

  // working directories to find modules and stuff
  au::Singleton<samson::SamsonSetup>::shared()->SetWorkerDirectories(samsonHome, samsonWorking);

  // Clean up function
  atexit(cleanup);

  // Start secondary log system
  // TODO: Complete this initialization

  lmAux((char *) "father");
  logFd = lmFirstDiskFileDescriptor();

  // Setup parameters from command line
  size_t _memory = (size_t) memory_gb * (size_t) (1024 * 1024 * 1024);
  size_t _load_buffer_size = (size_t) load_buffer_size_mb * (size_t) (1024 * 1024);

  au::Singleton<samson::SamsonSetup>::shared()->setValueForParameter("general.memory", au::str("%lu", _memory));
  au::Singleton<samson::SamsonSetup>::shared()->setValueForParameter("load.buffer_size",
                                                                     au::str("%lu", _load_buffer_size));
  // Engine and its associated elements
  int num_cores = au::Singleton<samson::SamsonSetup>::shared()->getInt("general.num_processess");
  engine::Engine::InitEngine(num_cores, _memory, 1);

  // Load modules
  au::Singleton<samson::ModulesManager>::shared()->addModulesFromDefaultDirectory();

  // Create a DelilahControler once network is ready
  delilahConsole = new samson::DelilahConsole(delilah_random_code);

  // Change log to console
  au::log_central.evalCommand("screen off");   // Disable log to screen since we log to console
  au::log_central.AddPlugin("console", new au::LogPluginConsole(delilahConsole));

  AU_LM_M(("Delilah running..."));

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
    delilahConsole->writeWarningOnConsole("Delilah client not connected to any SAMSON cluster. ( see help connect )");  // ----------------------------------------------------------------
  }
  // Special mode with one command line command
  // ----------------------------------------------------------------

  if (strcmp(command, "") != 0) {
    {
      au::Cronometer cronometer;
      while (!delilahConsole->isConnected()) {
        usleep(100000);
        if (cronometer.seconds() > 1) {
          LM_W(("delilahConsoleConnection is not ready, waiting to connect to all workers"));
          LM_V(("Waiting delilah to connect to all workers"));
          cronometer.Reset();
        }
      }
    }
    // Set output to screen
    delilahConsole->setNoOutput();
    size_t id = delilahConsole->runAsyncCommand(command);

    if (id != 0) {
      // Wait until this operation is finished
      while (delilahConsole->isActive(id)) {
        // Wait until command is finished
        usleep(1000);
      }

      if (delilahConsole->hasError(id)) {
        LM_E(("Error running '%s' \n", command ));
        LM_E(("Error: %s",  delilahConsole->errorMessage(id).c_str()));
      } else {
        printf("%s", delilahConsole->getOutputForComponent(id).c_str());
        fflush(stdout);
      }
    }

    // Disconnect delilah
    LM_M(("Calling delilahConsole->disconnect()"));
    delilahConsole->disconnect();

    // Stopping network connections
    delilahConsole->stop();
    
    // Stopping the new log_central thread
    au::log_central.Stop();
    
    exit(0);
  }

  // LM_M(("Delilah random code %s" , au::code64_str( delilah_random_code ).c_str() ));
  // LM_M(("Running delilah console..."));
  lmFdUnregister(2);   // no more traces to stdout

  // ----------------------------------------------------------------
  // Special mode for file-based commands
  // ----------------------------------------------------------------

  if (strcmp(commandFileName, "") != 0) {
    // Set simple output
    delilahConsole->setSimpleOutput();

    {
      au::Cronometer cronometer;
      while (!delilahConsole->isConnected()) {
        usleep(100000);
        if (cronometer.seconds() > 1) {
          LM_M(("Waiting delilah to connect to all workers"));
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

    // Disconnect delilah
    delilahConsole->disconnect();

    // Flush content of console
    // delilahConsole->flush();
    LM_M(("delilah exit correctly"));
    exit(0);
  }

  // Run console
  delilahConsole->run();

  
  return 0;
}

