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
 * FILE                     main_samsonWorker.cpp
 *
 * AUTHOR                   Ken Zangelin
 *
 * CREATION DATE            Dec 14 2010
 *
 */

#include "samson/worker/SamsonWorker.h" // Own interface


#include <arpa/inet.h>          // inet_ntoa
#include <netdb.h>              // gethostbyname
#include <netinet/in.h>         // struct sockaddr_in
#include <netinet/tcp.h>        // TCP_NODELAY
#include <signal.h>
#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un
#include <locale.h>             // setlocale

#include <string>

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"
#include "parseArgs/paBuiltin.h"  // paLsHost, paLsPort
#include "parseArgs/paConfig.h"
#include "parseArgs/paIsSet.h"
#include "parseArgs/parseArgs.h"
#include "au/ThreadManager.h"
#include "au/daemonize.h"
#include "au/log/Log.h"
#include "au/log/LogCommon.h"
#include "au/log/LogCentral.h"
#include "au/log/LogCentralPluginConsole.h"
#include "au/mutex/LockDebugger.h"            // au::LockDebugger
#include "engine/DiskManager.h"
#include "engine/Engine.h"
#include "engine/MemoryManager.h"
#include "engine/ProcessManager.h"
#include "samson/module/samson.h"
#include "samson/common/common.h"
#include "samson/common/MemoryCheck.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/samsonVars.h"
#include "samson/common/samsonVersion.h"
#include "samson/stream/ProcessItemIsolated.h"    // isolated_process_as_tread to put background process in thread mode
#include "samson/stream/SharedMemoryManager.h"
#include "samson/module/ModulesManager.h"
#include "samson/network/WorkerNetwork.h"
#include "samson/stream/BlockManager.h"
#include "samson/common/Logs.h"


/* ****************************************************************************
 *
 * Option variables
 */
SAMSON_ARG_VARS;

bool fg;
int valgrind;
int port;
int web_port;

char zoo_host[1024];

char log_command[1024];
char log_server[1024];
int log_port;
bool thread_mode;


#define LOG_PORT AU_LOG_SERVER_PORT

/* ****************************************************************************
 *
 * parse arguments
 */
PaArgument paArgs[] =
{
  SAMSON_ARGS,
  { "-zk",    zoo_host,      "",           PaString,      PaOpt,              _i "localhost:2181",
    PaNL,PaNL,"Zookeeper server"                   },
  { "-log",   log_command,                       "",                           PaString,
    PaOpt,                          _i "",     PaNL,
    PaNL,    "log server host"                          },
  { "-log_server",  log_server,                        "",                           PaString,
    PaOpt,                          _i "",     PaNL,
    PaNL,    "log server host"                          },
  { "-fg",    &fg,           "",           PaBool,        PaOpt,              false,
    false,
    true,
    "don't start as daemon"              },
  { "-port",  &port,         "",           PaInt,         PaOpt,              SAMSON_WORKER_PORT,
    1,
    9999,
    "Port to receive new connections"    },
  { "-web_port",&web_port,     "",           PaInt,         PaOpt,              SAMSON_WORKER_WEB_PORT,
    1,
    9999,
    "Port to receive web connections"    },
  { "-valgrind",&valgrind,     "",           PaInt,         PaOpt,              0,
    0,
    20,
    "help valgrind debug process"        },
  { "-thread_mode",&thread_mode,  "",           PaBool,        PaOpt,              false,
    false,
    true,     "thread_mode"                              },
  PA_END_OF_ARGS
};

/* ****************************************************************************
 *
 * global variables
 */
int logFd = -1;
samson::SamsonWorker *worker = NULL;
au::LockDebugger *lockDebugger = NULL;

/* ****************************************************************************
 *
 * man texts -
 */
static const char *manSynopsis = " [OPTION]";
static const char *manShortDescription = "samsond is the main process in a SAMSON system.\n\n";
static const char *manDescription = "\n"
  "samsond is the main process in a SAMSON system. All the nodes in the cluster has its own samsonWorker process\n"
  "All samsond processes are responsible for processing a segment of available data"
  "All clients of the platform ( delila's ) are connected to all samsonWorkers in the system"
  "See samson documentation to get more information about how to get a SAMSON system up and running"
  "\n";

static const char *manExitStatus = "0      if OK\n 1-255  error\n";
static const char *manAuthor = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char *manReportingBugs = "bugs to samson-dev@tid.es\n";
static const char *manCopyright = "Copyright (C) 2011 Telefonica Investigacion y Desarrollo";
static const char *manVersion = SAMSON_VERSION;

// Andreu: All logs in signal handlers should be local.

void captureSIGINT(int s) {
  LM_LM(("Signal SIGINT %d", s));
  _exit(1);
}

void captureSIGPIPE(int s) {
  LM_LM(("Captured SIGPIPE %d", s));
}

void captureSIGTERM(int s) {
  s = 3;
  LM_LM(("Captured SIGTERM"));

  LM_LM(("Cleaning up"));
  std::string pid_file_name = au::str("%s/samsond.pid", paLogDir);
  if (remove(pid_file_name.c_str()) != 0) {
    LM_LW(("Error deleting the pid file %s", pid_file_name.c_str()));
  }
  _exit(1);
}

static void valgrindExit(int v) {
  if (v == valgrind) {
    LM_M(("Valgrind option is %d - I exit", v));
    exit(0);
  }
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

/* ****************************************************************************
 *
 * main -
 */

int main(int argC, const char *argV[]) {
  char *oldlocale = setlocale(LC_ALL, "C");

  paConfig("builtin prefix", (void *) "SS_WORKER_");
  paConfig("usage and exit on any warning", (void *) true);

  // Searh for this flag before using pa brary
  bool flag_fg = find_flag(argC, argV, "-fg");

  if (flag_fg) {
    paConfig("log to screen", (void *) true);
  } else {
    paConfig("log to screen", (void *) "only errors");
  }
  paConfig("log file line format", (void *) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
  paConfig("screen line format", (void *) "TYPE@TIME  EXEC: TEXT");
  paConfig("default value", "-logDir", (void *) "/var/log/samson");
  paConfig("if hook active, no traces to file", (void *) false);
  paConfig("log to file", (void *) true);
  paConfig("man synopsis", (void *) manSynopsis);
  paConfig("man shortdescription", (void *) manShortDescription);
  paConfig("man description", (void *) manDescription);
  paConfig("man exitstatus", (void *) manExitStatus);
  paConfig("man author", (void *) manAuthor);
  paConfig("man reportingbugs", (void *) manReportingBugs);
  paConfig("man copyright", (void *) manCopyright);
  paConfig("man version", (void *) manVersion);
  paConfig("screen line format", (void *) "TYPE: TEXT");

  const char *extra = paIsSetSoGet(argC, (char **) argV, "-port");
  paParse(paArgs, argC, (char **) argV, 1, false, extra);

  lmAux((char *) "father");

  // log level for zo library
  zoo_set_debug_level(ZOO_LOG_LEVEL_ERROR);

  // Thread mode
  if (thread_mode) {
    LM_M(("Started in thread mode"));
    samson::ProcessItemIsolated::isolated_process_as_tread = true;
  }

  LM_V(("Started with arguments:"));
  for (int ix = 0; ix < argC; ix++) {
    LM_V(("  %02d: '%s'", ix, argV[ix]));
  }

  logFd = lmFirstDiskFileDescriptor();

  // Capturing SIGPIPE
  if (signal(SIGPIPE, captureSIGPIPE) == SIG_ERR) {
    LM_W(("SIGPIPE cannot be handled"));
  }
  if (signal(SIGINT, captureSIGINT) == SIG_ERR) {
    LM_W(("SIGINT cannot be handled"));
  }
  if (signal(SIGTERM, captureSIGTERM) == SIG_ERR) {
    LM_W(("SIGTERM cannot be handled"));  // Init basic setup stuff (necessary for memory check)
  }

  // Set directories and load setup file
  au::Singleton<samson::SamsonSetup>::shared()->SetWorkerDirectories(samsonHome, samsonWorking);

  // AU Log system
  au::log_central.Init( argV[0] );
  samson::RegisterLogChannels();   // Add all log channels for samson project ( au library included )
  
  au::log_central.AddFilePlugin("file" , std::string(paLogDir) + "/samsonWorker.log");
  au::log_central.AddFilePlugin("file2" , samson::SharedSamsonSetup()->samson_working() + "/samsonWorker.log" );
  if( strlen(log_server) > 0 )
  {
    std::string log_server_file = std::string(paLogDir) + "samsonWorker_" + log_server +  ".log";
    au::log_central.AddServerPlugin( "server" , log_server , log_server_file );
    au::log_central.evalCommand("log_set * X server");
    au::log_central.evalCommand("log_set samson::W M server");
    au::log_central.evalCommand("log_set samson::OP W server");
  }
  au::log_central.evalCommand(log_command);  // Additional command provided in command line
  
  valgrindExit(2);

  // Check to see if the current memory configuration is ok or not
  if (samson::MemoryCheck() == false) {
    LM_X(1, ("Insufficient memory configured. Check %s/samsonWorkerLog for more information.", paLogDir));
  }
  if (fg == false) {
    Daemonize();
  }
  valgrindExit(3);

  // ------------------------------------------------------
  // Write pid in /var/log/samson/samsond.pid
  // ------------------------------------------------------

  char pid_file_name[256];
  snprintf(pid_file_name, sizeof(pid_file_name), "%s/samsond.pid", paLogDir);
  FILE *file = fopen(pid_file_name, "w");
  if (!file) {
    LM_X(1, ("Error opening file '%s' to store pid", pid_file_name));
  }
  int pid = (int)getpid();
  if (fprintf(file, "%d", pid) == 0) {
    LM_X(1, ("Error writing pid %d to file %s", pid, pid_file_name));
  }
  fclose(file);

  // ------------------------------------------------------

  valgrindExit(4);

  // Recover values for the engine
  size_t memory = au::Singleton<samson::SamsonSetup>::shared()->GetUInt64("general.memory");
  int num_processors = au::Singleton<samson::SamsonSetup>::shared()->GetInt("general.num_processess");

  valgrindExit(5);
  LM_D(("engine::SharedMemoryManager::init"));
  size_t shm_size = au::Singleton<samson::SamsonSetup>::shared()->GetUInt64("general.shared_memory_size_per_buffer");
  engine::SharedMemoryManager::init(num_processors, shm_size);

  // Global init of engine
  engine::Engine::InitEngine(num_processors, memory, 1);

  valgrindExit(9);
  LM_D(("samson::ModulesManager::init"));

  valgrindExit(10);
  LM_D(("samson::stream::BlockManager::init"));
  samson::stream::BlockManager::init();

  valgrindExit(11);

  valgrindExit(12);

  // Instance of SamsonWorker object
  // -----------------------------------------------------------------------------------
  worker = new samson::SamsonWorker(zoo_host, port, web_port);

  AU_SM(("Worker Running..."));

  valgrindExit(13);

  // Change locale
  setlocale(LC_ALL, oldlocale);

  // Put in background if necessary
  if (fg == false) {
    std::cout << "OK. samsonWorker is now working in background.\n";
    Deamonize_close_all();

    while (true) {
      sleep(10);
    }
  }

  au::log_central.AddPlugin( "console" , new au::LogCentralPluginConsole(worker) );

  // Run worker console ( -fg is activated )
  worker->runConsole();

  // Stop engine to clean up
  engine::Engine::StopEngine();

  LM_T(LmtCleanup, ("Engine stopped (worker at %p)", worker));

  // Closing network connections before the wait for the threads
  LM_T(LmtCleanup, ("Stopping REST service (worker at %p)", worker));
  worker->samson_worker_rest()->StopRestService();

  LM_T(LmtCleanup, ("Stopping network listener (worker at %p)", worker));
  worker->network()->stop();
  LM_T(LmtCleanup, ("log_central marked to stop"));
  AU_SM(("log_central stopping..."));

  // Stopping the new log_central thread
  au::log_central.Stop();

  LM_T(LmtCleanup, ("Waiting for threads (worker at %p)", worker));
  au::Singleton<au::ThreadManager>::shared()->wait("samsonWorker");

  // Shutdown all GPB stuff
  google::protobuf::ShutdownProtobufLibrary();

  // Deleting worker
  LM_T(LmtCleanup, ("Shutting down worker components (worker at %p)", worker));
  if (worker != NULL) {
    LM_T(LmtCleanup, ("deleting worker"));
    delete worker;
    worker = NULL;
  }

  LM_T(LmtCleanup, ("destroying BlockManager"));
  samson::stream::BlockManager::destroy();

  LM_T(LmtCleanup, ("destroying ModulesManager"));

  LM_T(LmtCleanup, ("destroying Engine"));
  engine::Engine::DestroyEngine();

  LM_T(LmtCleanup, ("Calling paConfigCleanup"));
  paConfigCleanup();
  LM_T(LmtCleanup, ("Calling lmCleanProgName"));
  lmCleanProgName();
  LM_T(LmtCleanup, ("Cleanup DONE"));

  return 0;
}

