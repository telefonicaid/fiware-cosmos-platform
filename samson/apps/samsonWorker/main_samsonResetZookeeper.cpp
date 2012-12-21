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
 * FILE                     main_samsonResetZookeeper.cpp
 *
 * AUTHOR                   Andreu Urruela
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
  { "",                 zoo_host,"", PaString,
    PaOpt,
    _i "localhost:2181",
    PaNL,               PaNL,  "Zookeeper server"                   },
  PA_END_OF_ARGS
};

static const char *manSynopsis = " [OPTION]";
static const char *manShortDescription = "samsonResetZookeeper is an admin tool to reset an old instance of zookeeper";
static const char *manDescription = "\n"
                                    "\n";
static const char *manExitStatus = "0      if OK\n 1-255  error\n";


int main(int argC, const char *argV[]) {
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

  // Init log system, register all channels & add all plugins
  au::LogCentral::InitLogSystem(argV[0]);
  samson::RegisterLogChannels();
  au::LogCentral::Shared()->AddScreenPlugin("screen", "[type] text");

  // Setup ZK logs: output to /dev/null to avoid messages on screen
  LOG_M(samson::logs.worker, ("Disabled logs for Zookeeper library"));
  zoo_set_debug_level(ZOO_LOG_LEVEL_ERROR);
  FILE *zoo_log_stream = fopen("/dev/null", "w");
  zoo_set_log_stream(zoo_log_stream);

  // Complete host of zk if no port is specified
  std::string my_zoo_host = zoo_host;
  if (my_zoo_host.find(':') == std::string::npos) {
    my_zoo_host += ":2181";
  }

  // connect with ZK
  au::zoo::Connection connection(my_zoo_host, "samson", "samson");
  connection.WaitUntilConnected(2000);
  if (!connection.IsConnected()) {
    LOG_X(1, ("Unable to connect to Zookeeper at %s.", zoo_host));
  } else {
    LOG_M(samson::logs.worker, ("Connection with ZK at %s OK", my_zoo_host.c_str()));
    connection.RecursiveRemove("/samson");
  }

  au::LogCentral::Shared()->Flush();
  return 0;
}

