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

#include "samson/worker/SamsonWorker.h"  // Own interface


#include <arpa/inet.h>          // inet_ntoa
#include <locale.h>             // setlocale
#include <netdb.h>              // gethostbyname
#include <netinet/in.h>         // struct sockaddr_in
#include <netinet/tcp.h>        // TCP_NODELAY
#include <signal.h>
#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un

#include <string>

#include "au/ThreadManager.h"
#include "au/daemonize.h"
#include "au/log/Log.h"
#include "au/log/LogCentral.h"
#include "au/log/LogCentralPluginConsole.h"
#include "au/log/LogCommon.h"
#include "au/mutex/LockDebugger.h"            // au::LockDebugger
#include "engine/DiskManager.h"
#include "engine/Engine.h"
#include "engine/MemoryManager.h"
#include "engine/ProcessManager.h"
#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"
#include "parseArgs/paBuiltin.h"  // paLsHost, paLsPort
#include "parseArgs/paConfig.h"
#include "parseArgs/paIsSet.h"
#include "parseArgs/parseArgs.h"
#include "samson/common/Logs.h"
#include "samson/common/Logs.h"
#include "samson/common/MemoryCheck.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/common.h"
#include "samson/common/samsonVars.h"
#include "samson/common/samsonVersion.h"
#include "samson/module/ModulesManager.h"
#include "samson/module/samson.h"
#include "samson/network/WorkerNetwork.h"
#include "samson/stream/BlockManager.h"
#include "samson/stream/ProcessItemIsolated.h"    // isolated_process_as_tread to put background process in thread mode
#include "samson/stream/SharedMemoryManager.h"


/* ****************************************************************************
 *
 * Option variables
 */
SAMSON_ARG_VARS;

bool fg;
int port;
int web_port;
char zoo_host[1024];
char log_command[1024];
char log_server[1024];
int log_port;
bool thread_mode;
bool worker_quit = false;  // Flag used to exit background worker when sigterm is send

#define LOG_PORT LOG_SERVER_DEFAULT_PORT

/* ****************************************************************************
 *
 * parse arguments
 */
PaArgument paArgs[] =
{
  SAMSON_ARGS,
  { "-zk",                 zoo_host,      "",   PaString,
    PaOpt,
    _i "localhost:2181",
    PaNL,                  PaNL,          "Zookeeper server"                   },
  { "-log",                log_command,   "",   PaString,
    PaOpt,                 _i "",         PaNL,
    PaNL,                  "log server host"                          },
  { "-log_server",         log_server,    "",   PaString,
    PaOpt,                 _i "",         PaNL,
    PaNL,                  "log server host"                          },
  { "-fg",                 &fg,           "",   PaBool,
    PaOpt,
    false,
    false,
    true,
    "don't start as daemon"              },
  { "-port",               &port,         "",   PaInt,
    PaOpt,
    SAMSON_WORKER_PORT,
    1,
    9999,
    "Port to receive new connections"    },
  { "-web_port",           &web_port,     "",   PaInt,
    PaOpt,
    SAMSON_WORKER_WEB_PORT,
    1,
    9999,
    "Port to receive web connections"    },
  { "-thread_mode",        &thread_mode,  "",   PaBool,
    PaOpt,
    false,
    false,
    true,                  "thread_mode"                              },
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

void SamsonWorkerCleanUp() {
  LOG_M(samson::logs.cleanup, ("Cleaning up samsonWorker"));

  // Stop console just in case it is not already stopped
  LOG_M(samson::logs.cleanup, ("Stop worker console"));
  worker->StopConsole();

  LOG_M(samson::logs.cleanup, ("Stopping REST service (worker at %p)", worker));
  worker->samson_worker_rest()->StopRestService();

  LOG_M(samson::logs.cleanup, ("Stopping network listener (worker at %p)", worker));
  worker->network()->stop();

  LOG_M(samson::logs.cleanup, ("destroying BlockManager"));
  samson::stream::BlockManager::destroy();

  LOG_M(samson::logs.cleanup, ("destroying Engine"));
  engine::Engine::StopEngine();

  // Deleting worker
  LOG_M(samson::logs.cleanup, ("Removing worker instance"));
  if (worker != NULL) {
    delete worker;
    worker = NULL;
  }
  LOG_D(samson::logs.cleanup, ("Finished removing worker instance"));

  LOG_M(samson::logs.cleanup, ("Calling paConfigCleanup"));
  paConfigCleanup();

  LOG_M(samson::logs.cleanup, ("Calling lmCleanProgName"));
  lmCleanProgName();

  // Remove pid file
  LOG_M(samson::logs.cleanup, ("Removing pid file for samsonWorker"));
  std::string pid_file_name = au::str("%s/samsond.pid", paLogDir);
  if (remove(pid_file_name.c_str()) != 0) {
    LM_LW(("Error deleting the pid file %s", pid_file_name.c_str()));
  }

  // Destroy shared memory manager
  LOG_M(samson::logs.cleanup, ("Destroying shared memory manager"));
  samson::SharedMemoryManager::Destroy();

  // Stop the logging system
  LOG_M(samson::logs.cleanup, ("log_central stopping..."));
  au::LogCentral::Shared()->StopLogSystem();

  // Wait for all background threads
  LOG_M(samson::logs.cleanup, ("Waiting for threads (worker at %p)", worker));
  au::Singleton<au::ThreadManager>::shared()->wait("samsonWorker");

  // Shutdown all GPB stuff
  google::protobuf::ShutdownProtobufLibrary();

  LOG_M(samson::logs.cleanup, ("Cleanup DONE"));
}

void captureSIGINT(int s) {
  LOG_SW(("Signal SIGINT %d", s));
  if (fg) {
    worker->StopConsole();
  } else {
    worker_quit = true;
  }
}

void captureSIGPIPE(int s) {
  LOG_SV(("Captured SIGPIPE %d", s));
}

void captureSIGTERM(int s) {
  LOG_SW(("Captured SIGTERM"));
  if (fg) {
    worker->StopConsole();
  } else {
    worker_quit = true;
  }
}

/**
 * \brief Main function for samsonWorker executable
 */

int main(int argC, const char *argV[]) {
  char *oldlocale = setlocale(LC_ALL, "C");

  paConfig("builtin prefix", (void *)"SS_WORKER_");
  paConfig("usage and exit on any warning", (void *)true);
  paConfig("log to screen", (void *)false);
  paConfig("log file line format", (void *)"TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
  paConfig("screen line format", (void *)"TYPE@TIME  EXEC: TEXT");
  paConfig("default value", "-logDir", (void *)"/var/log/samson");
  paConfig("if hook active, no traces to file", (void *)false);
  paConfig("log to file", (void *)true);
  paConfig("man synopsis", (void *)manSynopsis);
  paConfig("man shortdescription", (void *)manShortDescription);
  paConfig("man description", (void *)manDescription);
  paConfig("man exitstatus", (void *)manExitStatus);
  paConfig("man reportingbugs", SAMSON_BUG_REPORTING);
  paConfig("man author", SAMSON_AUTHORS);
  paConfig("man copyright", SAMSON_COPYRIGHT);
  paConfig("man version", SAMSON_VERSION);
  paConfig("screen line format", (void *)"TYPE: TEXT");
  const char *extra = paIsSetSoGet(argC, (char **)argV, "-port");
  paParse(paArgs, argC, (char **)argV, 1, false, extra);
  lmAux((char *)"father");

  // Init log system, register all channels & add all plugins
  au::LogCentral::InitLogSystem(argV[0]);
  samson::RegisterLogChannels();

  // Log to file
  std::string log_file_name = samson::SharedSamsonSetup()->samson_working() + "/samsonWorker.log";
  au::log_central->AddFilePlugin("file", log_file_name);
  au::LogCentral::Shared()->AddFilePlugin("file2", std::string(paLogDir) + "/samsonWorker.log");

  // Log to console or screen if console is not available
  std::string log_format = "[type][node][time][channel] text";
  au::LogCentralPluginConsole *log_plugin_console_ = new au::LogCentralPluginConsole(NULL, log_format, true);
  au::LogCentral::Shared()->AddPlugin("console", log_plugin_console_);

  // Log to server is specified in command line
  if (strlen(log_server) > 0) {
    std::string log_server_file = std::string(paLogDir) + "samsonWorker_" + log_server +  ".log";
    au::log_central->AddServerPlugin("server", log_server, log_server_file);
    au::log_central->EvalCommand("log_set * X server");
    au::log_central->EvalCommand("log_set samson::W M server");
    au::log_central->EvalCommand("log_set samson::OP W server");
  }

  // Change channel levels by default
  au::log_central->EvalCommand("log_set samson::W M");  // Set message level for the log channel samson::W
  au::log_central->EvalCommand("log_set system M");     // Set message level for the log channel system
  if (lmDebug) {
    au::log_central->EvalCommand("log_set samson::W D");
    au::log_central->EvalCommand("log_set system D");
  }

  // At the moment, no worker number is assigned
  au::log_central->set_node("W?");

  // Additional log-command provided in command line
  au::log_central->EvalCommand(log_command);

  LOG_M(samson::logs.worker, ("SamsonWorker for SAMSON v. %s", SAMSON_VERSION));
  LOG_M(samson::logs.worker, ("Please, check logs for this worker at %s (using logCat tool)", log_file_name.c_str()));

  // Check to see if the current memory configuration is ok or not
  if (samson::MemoryCheck() == false) {
    LOG_W(samson::logs.worker, ("Not enough shared memory. Please check setup with samsonSetup"));
    LOG_X(1, ("Insufficient memory configured. Check %s/samsonWorkerLog for more information.", paLogDir));
  } else {
    LOG_M(samson::logs.worker, ("Memory check for samsonWorekr OK"));
  }

  // Setup ZK logs: output to /dev/null to avoid messages on screen
  LOG_M(samson::logs.worker, ("Disabled logs for Zookeeper library"));
  zoo_set_debug_level(ZOO_LOG_LEVEL_ERROR);
  FILE *zoo_log_stream = fopen("/dev/null", "w");
  zoo_set_log_stream(zoo_log_stream);

  // Thread mode: In this mode, no fork is executed to isolate background processes
  // Only used when debuggin third party software. Not used in production
  if (thread_mode) {
    LOG_M(samson::logs.worker, ("Started in thread mode"));
    samson::ProcessItemIsolated::isolated_process_as_tread = true;
  }

  // Old log system
  logFd = lmFirstDiskFileDescriptor();

  // Debug information
  LOG_D(samson::logs.worker, ("Started with arguments:"));
  for (int ix = 0; ix < argC; ++ix) {
    LOG_D(samson::logs.worker, ("  %02d: '%s'", ix, argV[ix]));
  }

  // Capturing SIGPIPE
  if (signal(SIGPIPE, captureSIGPIPE) == SIG_ERR) {
    LOG_D(samson::logs.worker, ("SIGPIPE cannot be handled"));
  }
  if (signal(SIGINT, captureSIGINT) == SIG_ERR) {
    LOG_D(samson::logs.worker, ("SIGINT cannot be handled"));
  }
  if (signal(SIGTERM, captureSIGTERM) == SIG_ERR) {
    LOG_D(samson::logs.worker, ("SIGTERM cannot be handled"));  // Init basic setup stuff (necessary for memory check)
  }

  // Set directories and load setup file
  LOG_M(samson::logs.worker, ("Setting home and working directories '%s','%s'", samsonHome, samsonWorking));
  au::Singleton<samson::SamsonSetup>::shared()->SetWorkerDirectories(samsonHome, samsonWorking);

  // Complete host of zk if no port is specified
  std::string my_zoo_host = zoo_host;
  if (my_zoo_host.find(':') == std::string::npos) {
    my_zoo_host += ":2181";
  }

  // Try connection with provided ZK to alert user if not possible
  {
    au::zoo::Connection connection(my_zoo_host, "samson", "samson");
    connection.WaitUntilConnected(2000);
    if (!connection.IsConnected()) {
      LOG_W(samson::logs.worker, ("Unable to connect to Zookeeper at %s.", zoo_host));
      if (!fg) {
        LOG_W(samson::logs.worker, ("Keep trying to connect to Zookeeper at %s in background...", zoo_host));
      }
    } else {
      LOG_M(samson::logs.worker, ("Connection with ZK at %s OK", my_zoo_host.c_str()));
    }
  }

  if (fg == false) {
    LOG_M(samson::logs.worker, ("samsonWorker will continue in background..."));
    au::log_central->Flush();  // Flush logs to make sure this message is shown on screen
    Daemonize();
  }

  // Write pid in /var/log/samson/samsond.pid
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

  // Recover values for the engine
  size_t memory = au::Singleton<samson::SamsonSetup>::shared()->GetUInt64("general.memory");
  int num_processors = au::Singleton<samson::SamsonSetup>::shared()->GetInt("general.num_processess");

  LOG_D(samson::logs.worker, ("Init shared memory manager"));
  size_t shm_size = au::Singleton<samson::SamsonSetup>::shared()->GetUInt64("general.shared_memory_size_per_buffer");
  samson::SharedMemoryManager::Init(num_processors, shm_size);

  // Global init of engine
  LOG_D(samson::logs.worker, ("Init engine system with %d cores and memory=%s", num_processors, au::str(memory).c_str()));
  engine::Engine::InitEngine(num_processors, memory, 1);

  LOG_D(samson::logs.worker, ("Init Block manager"));
  samson::stream::BlockManager::init();

  // Instance of SamsonWorker object
  LOG_D(samson::logs.worker, ("Init worker component"));
  worker = new samson::SamsonWorker(my_zoo_host, port, web_port);
  samson::stream::BlockManager::shared()->set_samson_worker(worker);  // Set worker in block manager ( to get information about how to sort blocks )

  LOG_M(samson::logs.worker, ("Worker Running..."));

  // Change locale
  setlocale(LC_ALL, oldlocale);

  // Put in background if necessary
  if (fg) {
    log_plugin_console_->SetConsole(worker);
    worker->StartConsole(true);
    log_plugin_console_->SetConsole(NULL);  // Not use console any more to print logs
    LOG_M(samson::logs.worker, ("samsonWorker is now quitting..."));
  } else {
    LOG_M(samson::logs.worker, ("samsonWorker is now working in background"));
    Deamonize_close_all();
    while (!worker_quit) {
      sleep(1);
    }
  }

  // Not use this worker any more in BlockManager
  samson::stream::BlockManager::shared()->set_samson_worker(NULL);

  // Clean up all worker setup
  SamsonWorkerCleanUp();
  return 0;
}

