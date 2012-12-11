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

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "parseArgs/paBuiltin.h"
#include "parseArgs/paConfig.h"
#include "parseArgs/paIsSet.h"
#include "parseArgs/parseArgs.h"

#include "au/Log.h"
#include "au/daemonize.h"
#include "au/file.h"
#include "au/log/LogCommon.h"
#include "au/log/LogServer.h"

#define LOC           "localhost"

#define LS_PORT       LOG_SERVER_DEFAULT_PORT
#define LS_QUERY_PORT LOG_SERVER_DEFAULT_CLIENT_PORT
#define LS_LOG_DIR    LOG_SERVER_DEFAULT_DIRECTORY

bool fg;
int query_port;
int target_port;
char log_directory[1024];

PaArgument paArgs[] =
{
  { "-fg",         &fg,                  "",                  PaBool,                    PaOpt,
    false,
    false,
    true,
    "don't start as daemon"                  },
  { "-port",       &target_port,         "",                  PaInt,                     PaOpt,
    LS_PORT,
    1,                 99999,
    "Port for default log channel"        },
  { "-query_port", &query_port,          "",                  PaInt,                     PaOpt,
    LS_QUERY_PORT,          1,                 99999,
    "Port for logClient connections"      },
  { "-dir",        log_directory,        "",                  PaString,                  PaOpt,
    _i LS_LOG_DIR,          PaNL,              PaNL,
    "Directory for default channel logs"  },
  PA_END_OF_ARGS
};

static const char *manSynopsis         = "logServer [-port X] [-query_port X] [-dir X]";
static const char *manShortDescription = "logServer is a server to collect logs from diferent nodes in a cluster\n";
static const char *manDescription      =
  "\n"
  "logServer is a server to collect logs from diferent nodes in a cluster\n"
  "\n";

static const char *manExitStatus    = "0      if OK\n 1-255  error\n";
static const char *manAuthor        = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char *manReportingBugs = "bugs to samson-dev@tid.es\n";
static const char *manCopyright     = "Copyright (C) 2011 Telefonica Digital";
static const char *manVersion       = "0.1";



int logFd = -1;


void captureSIGPIPE(int s) {
  LM_LM(("Captured SIGPIPE %d", s));
}

int main(int argC, const char *argV[]) {
  paConfig("prefix",                        (void *)"LOG_CLIENT_");
  paConfig("usage and exit on any warning", (void *)true);
  paConfig("log to screen",                 (void *)true);
  paConfig("log file line format",          (void *)"TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
  paConfig("screen line format",            (void *)"TYPE: TEXT");
  paConfig("log to file",                   (void *)false);
  paConfig("log to stderr",                 (void *)true);

  paConfig("man synopsis",                  (void *)manSynopsis);
  paConfig("man shortdescription",          (void *)manShortDescription);
  paConfig("man description",               (void *)manDescription);
  paConfig("man exitstatus",                (void *)manExitStatus);
  paConfig("man author",                    (void *)manAuthor);
  paConfig("man reportingbugs",             (void *)manReportingBugs);
  paConfig("man copyright",                 (void *)manCopyright);
  paConfig("man version",                   (void *)manVersion);

  paParse(paArgs, argC, (char **)argV, 1, true);
  // lmAux((char*) "father");
  logFd = lmFirstDiskFileDescriptor();

  // Init new log system
  au::LogCentral::InitLogSystem(argV[0]);
  au::logs.RegisterChannels();

  // Add plugins to report logs to file, server and console
  std::string log_file_name = au::path_from_directory(std::string(paLogDir), "/samsonWorker.log");
  au::log_central->AddFilePlugin("file", log_file_name);
  au::log_central->AddScreenPlugin("screen", "[type] text");  // Only to show first messages if process is not in foreground
  au::log_central->evalCommand("log_set system M");         // Set message level for the log channel system

  LOG_SM(("Please, review logs of this log server at %s", log_file_name.c_str()));

  if (fg == false) {
    LOG_SM(("logServer running in background"));
    Daemonize();
  } else {
    LOG_SM(("logServer running in foreground"));
  }

  if (signal(SIGPIPE, captureSIGPIPE) == SIG_ERR) {
    LOG_SW(("SIGPIPE cannot be handled"));
  }

  // Log server
  au::LogServer log_server;

  // Sleep forever
  while (true) {
    sleep(1);
  }

  // Cleanup log system
  au::LogCentral::StopLogSystem();

  return 0;
}

