/* ****************************************************************************
 *
 * FILE                     main_samsonWorker.cpp
 *
 * AUTHOR                   Ken Zangelin
 *
 * CREATION DATE            Dec 14 2010
 *
 */
#include <arpa/inet.h>          // inet_ntoa
#include <netdb.h>              // gethostbyname
#include <netinet/in.h>         // struct sockaddr_in
#include <netinet/tcp.h>        // TCP_NODELAY
#include <signal.h>
#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un

#include <locale.h>             // setlocale

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"
#include "parseArgs/paBuiltin.h"  // paLsHost, paLsPort
#include "parseArgs/paConfig.h"
#include "parseArgs/paIsSet.h"
#include "parseArgs/parseArgs.h"

#include "au/ThreadManager.h"
#include "au/daemonize.h"
#include "au/log/Log.h"
#include "au/log/LogToServer.h"
#include "au/log/log_server_common.h"
#include "au/mutex/LockDebugger.h"            // au::LockDebugger

#include "engine/DiskManager.h"
#include "engine/Engine.h"
#include "engine/MemoryManager.h"
#include "engine/ProcessManager.h"

#include "samson/common/MemoryCheck.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/samsonVars.h"
#include "samson/common/samsonVersion.h"
#include "samson/isolated/ProcessItemIsolated.h"    // isolated_process_as_tread to put background process in thread mode
#include "samson/isolated/SharedMemoryManager.h"
#include "samson/module/ModulesManager.h"
#include "samson/network/WorkerNetwork.h"
#include "samson/stream/BlockManager.h"
#include "samson/worker/SamsonWorker.h"

#include <string.h>

/* ****************************************************************************
 *
 * Option variables
 */
SAMSON_ARG_VARS;

bool fg;
bool monit;
int valgrind;
int port;
int web_port;

char zoo_host[1024];

char log_file[1024];
char log_host[1024];
bool log_classic;
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
  { "-zk",    zoo_host, "",           PaString,      PaOpt,              _i "localhost:2181",
    PaNL,
    PaNL,
    "Zookeeper server"                   },
  { "-log_classic",&log_classic, "",       PaBool,        PaOpt,              false,
    false,
    true,
    "Classical log file"                 },
  { "-log_host",log_host, "",           PaString,      PaOpt,              _i "localhost",
    PaNL,
    PaNL,     "log server host"                          },
  { "-log_port",&log_port, "",          PaInt,         PaOpt,              LOG_PORT,
    0,
    10000,    "log server port"                          },
  { "-log_file",log_file, "",           PaString,      PaOpt,              _i "",
    PaNL,
    PaNL,     "Local log file"                           },
  { "-fg",    &fg,      "",           PaBool,        PaOpt,              false,
    false,
    true,
    "don't start as daemon"              },
  { "-monit", &monit,   "",           PaBool,        PaOpt,              false,
    false,
    true,
    "to use with monit"                  },
  { "-port",  &port,    "",           PaInt,         PaOpt,              SAMSON_WORKER_PORT,
    1,
    9999,
    "Port to receive new connections"    },
  { "-web_port",&web_port, "",          PaInt,         PaOpt,              SAMSON_WORKER_WEB_PORT,
    1,
    9999,
    "Port to receive web connections"    },
  { "-valgrind",&valgrind, "",          PaInt,         PaOpt,              0,
    0,
    20,
    "help valgrind debug process"        },
  { "-thread_mode",&thread_mode, "",       PaBool,        PaOpt,              false,
    false,
    true,     "thread_mode"                              },
  PA_END_OF_ARGS
};



/* ****************************************************************************
 *
 * global variables
 */
int logFd             = -1;
samson::SamsonWorker *worker            = NULL;
au::LockDebugger *lockDebugger      = NULL;
engine::SharedMemoryManager *smManager         = NULL;



/* ****************************************************************************
 *
 * man texts -
 */
static const char *manSynopsis         = " [OPTION]";
static const char *manShortDescription = "samsond is the main process in a SAMSON system.\n\n";
static const char *manDescription      =
  "\n"
  "samsond is the main process in a SAMSON system. All the nodes in the cluster has its own samsonWorker process\n"
  "All samsond processes are responsible for processing a segment of available data"
  "All clients of the platform ( delila's ) are connected to all samsonWorkers in the system"
  "See samson documentation to get more information about how to get a SAMSON system up and running"
  "\n";

static const char *manExitStatus    = "0      if OK\n 1-255  error\n";
static const char *manAuthor        = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char *manReportingBugs = "bugs to samson-dev@tid.es\n";
static const char *manCopyright     = "Copyright (C) 2011 Telefonica Investigacion y Desarrollo";
static const char *manVersion       = SAMSON_VERSION;


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
  if (remove(pid_file_name.c_str()) != 0)
    LM_LW(("Error deleting the pid file %s", pid_file_name.c_str())); _exit(1);
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
    if (strcmp(argV[i], flag) == 0)
      return true;
  }
  return false;
}

/* ****************************************************************************
 *
 * main -
 */

int main(int argC, const char *argV[]) {
  char *oldlocale = setlocale(LC_ALL, "C");

  paConfig("builtin prefix",                    (void *)"SS_WORKER_");
  paConfig("usage and exit on any warning",     (void *)true);

  // Searh for this flag before using pa brary
  bool flag_fg = find_flag(argC, argV, "-fg");

  if (flag_fg)
    paConfig("log to screen",                 (void *)true); else
    paConfig("log to screen",
             (void *)"only errors"); paConfig("log file line format",
                                              (void *)"TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
  paConfig("screen line format",                (void *)"TYPE@TIME  EXEC: TEXT");
  paConfig("default value", "-logDir",          (void *)"/var/log/samson");

  // Searh for this flag before using pa brary
  bool flag_log_classic = find_flag(argC, argV, "-log_classic");

  if (flag_log_classic) {
    paConfig("if hook active, no traces to file", (void *)false);
    paConfig("log to file",                       (void *)true);
  } else {
    paConfig("if hook active, no traces to file", (void *)true);
  }

  paConfig("man synopsis",                      (void *)manSynopsis);
  paConfig("man shortdescription",              (void *)manShortDescription);
  paConfig("man description",                   (void *)manDescription);
  paConfig("man exitstatus",                    (void *)manExitStatus);
  paConfig("man author",                        (void *)manAuthor);
  paConfig("man reportingbugs",                 (void *)manReportingBugs);
  paConfig("man copyright",                     (void *)manCopyright);
  paConfig("man version",                       (void *)manVersion);

  const char *extra = paIsSetSoGet(argC, (char **)argV, "-port");
  paParse(paArgs, argC, (char **)argV, 1, false, extra);


  // log level for zo library
  zoo_set_debug_level(ZOO_LOG_LEVEL_ERROR);

  // Thread mode
  if (thread_mode) {
    LM_M(("Started in thread mode"));
    samson::ProcessItemIsolated::isolated_process_as_tread = true;
  }

  // New log system ( if flag log_classic is not activated )
  if (!flag_log_classic) {
    std::string local_log_file;
    if (strlen(log_file) > 0)
      local_log_file = log_file; else
      local_log_file = au::str("%s/samsonWorkerLog_%d", paLogDir, (int)getpid()); au::start_log_to_server(
      log_host, log_port,
      local_log_file);
  }

  lmAux((char *)"father");

  LM_V(("Started with arguments:"));
  for (int ix = 0; ix < argC; ix++) {
    LM_V(("  %02d: '%s'", ix, argV[ix]));
  }

  logFd = lmFirstDiskFileDescriptor();

  // Capturing SIGPIPE
  if (signal(SIGPIPE, captureSIGPIPE) == SIG_ERR)
    LM_W(("SIGPIPE cannot be handled")); if (signal(SIGINT, captureSIGINT) == SIG_ERR)
    LM_W(("SIGINT cannot be handled")); if (signal(SIGTERM, captureSIGTERM) == SIG_ERR)
    LM_W(("SIGTERM cannot be handled"));  // Init basic setup stuff (necessary for memory check)
  lockDebugger = au::LockDebugger::shared();                 // VALGRIND complains ...
  samson::SamsonSetup::init(samsonHome, samsonWorking);      // Load setup and create default directories

  valgrindExit(2);

  // Check to see if the current memory configuration is ok or not
  if (samson::MemoryCheck() == false)
    LM_X(1, ("Insufficient memory configured. Check %s/samsonWorkerLog for more information.", paLogDir));
  if ((fg == false) && (monit == false))
    daemonize(); valgrindExit(3);

  // ------------------------------------------------------
  // Write pid in /var/log/samson/samsond.pid
  // ------------------------------------------------------

  char pid_file_name[256];
  snprintf(pid_file_name, sizeof(pid_file_name), "%s/samsond.pid", paLogDir);
  FILE *file = fopen(pid_file_name, "w");
  if (!file)
    LM_X(1, ("Error opening file '%s' to store pid", pid_file_name)); int pid = (int)getpid();
  if (fprintf(file, "%d", pid) == 0)
    LM_X(1, ("Error writing pid %d to file %s", pid, pid_file_name)); fclose(file);

  // ------------------------------------------------------

  if (monit) {
    LM_M(("monit test - I stay here ..."));
    while (1) {
      sleep(1);
    }
  }

  // Make sure this singleton is created just once
  LM_D(("createWorkingDirectories"));
  samson::SamsonSetup::shared()->createWorkingDirectories();

  valgrindExit(4);
  LM_D(("engine::Engine::init"));
  engine::Engine::init();

  valgrindExit(5);
  LM_D(("engine::SharedMemoryManager::init"));
  engine::SharedMemoryManager::init(samson::SamsonSetup::shared()->getInt("general.num_processess")
                                    , samson::SamsonSetup::shared()->getUInt64("general.shared_memory_size_per_buffer")
                                    );  // VALGRIND complains ...
  smManager = engine::SharedMemoryManager::shared();

  valgrindExit(6);
  LM_D(("engine::DiskManager::init"));
  engine::DiskManager::init(1);

  valgrindExit(7);
  LM_D(("engine::ProcessManager::init"));
  engine::ProcessManager::init(samson::SamsonSetup::shared()->getInt("general.num_processess"));

  valgrindExit(8);
  LM_D(("engine::MemoryManager::init"));
  engine::MemoryManager::init(samson::SamsonSetup::shared()->getUInt64("general.memory"));

  valgrindExit(9);
  LM_D(("samson::ModulesManager::init"));
  samson::ModulesManager::init("samsonWorker");

  valgrindExit(10);
  LM_D(("samson::stream::BlockManager::init"));
  samson::stream::BlockManager::init();

  valgrindExit(11);

  valgrindExit(12);

  // Instance of SamsonWorker object
  // -----------------------------------------------------------------------------------
  worker = new samson::SamsonWorker(zoo_host, port, web_port);

  LM_M(("Worker Running..."));

  valgrindExit(13);

  // Change locale
  setlocale(LC_ALL, oldlocale);

  // Put in background if necessary
  if (fg == false) {
    std::cout << "OK. samsonWorker is now working in background.\n";
    deamonize_close_all();

    while (true) {
      sleep(10);
    }
  }

  // Run worker console ( -fg is activated )
  worker->runConsole();

  // Stop all threads to clean up
  // ---------------------------------------------------------------------------
  engine::Engine::stop();
  engine::DiskManager::stop();
  engine::ProcessManager::stop();

  // Stop the worker
  if (worker)
    worker->stop();  // Wait all threads to finish
  au::ThreadManager::shared()->wait("samsonWorker");

  // Shutdown all GPB stuff
  google::protobuf::ShutdownProtobufLibrary();

  // Deleting worker
  LM_T(LmtCleanup, ("Shutting down worker components (worker at %p)", worker));
  if (worker != NULL) {
    LM_T(LmtCleanup, ("deleting worker"));
    delete worker;
    worker = NULL;
  }

  LM_T(LmtCleanup, ("Shutting down LockDebugger"));
  au::LockDebugger::destroy();

  if (smManager != NULL) {
    LM_T(LmtCleanup, ("Shutting down Shared Memory Manager"));
    delete smManager;
    smManager = NULL;
  }

  LM_T(LmtCleanup, ("destroying BlockManager"));
  samson::stream::BlockManager::destroy();

  LM_T(LmtCleanup, ("destroying ModulesManager"));
  samson::ModulesManager::destroy("samsonWorker");

  LM_T(LmtCleanup, ("destroying ProcessManager"));
  engine::ProcessManager::destroy();

  LM_T(LmtCleanup, ("destroying DiskManager"));
  engine::DiskManager::destroy();

  LM_T(LmtCleanup, ("destroying MemoryManager"));
  engine::MemoryManager::destroy();

  LM_T(LmtCleanup, ("destroying Engine"));
  engine::Engine::destroy();

  LM_T(LmtCleanup, ("Shutting down SamsonSetup"));
  samson::SamsonSetup::destroy();

  LM_T(LmtCleanup, ("Calling paConfigCleanup"));
  paConfigCleanup();
  LM_T(LmtCleanup, ("Calling lmCleanProgName"));
  lmCleanProgName();
  LM_T(LmtCleanup, ("Cleanup DONE"));

  // Stop logging to server
  au::stop_log_to_server();

  return 0;
}

