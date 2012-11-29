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


#include "parseArgs/parseArgs.h"

#include "au/console/Console.h"
#include "au/console/ConsoleAutoComplete.h"
#include "au/network/SocketConnection.h"
#include "au/utils.h"

#include "au/log/LogCommon.h"
#include "au/log/LogFilter.h"
#include "au/log/LogProbe.h"
#include "au/network/ConsoleService.h"

char format[1024];
char host[1024];
char filter[1024];
char file_name[1024];
bool no_color;

#define LOC     "localhost"
#define LS_PORT LOG_SERVER_DEFAULT_CLIENT_PORT


PaArgument paArgs[] =
{
  { "-host",     host,          "",          PaString,      PaOpt,   _i "localhost",                     PaNL,
    PaNL,  "Log server hostname"               },
  { "-format",   format,        "",      PaString, PaOpt,   _i LOG_DEFAULT_FORMAT_LOG_CLIENT, PaNL,
    PaNL,
    "Formats of the logs at the output" },
  { "-filter",   filter,    "", PaString, PaOpt, _i "",                            PaNL,  PaNL, "Filter for logs"                   },
  { "-save",     file_name, "", PaString, PaOpt, _i "",                            PaNL,  PaNL,
    "Save received logs to file"        },
  { "-no_color", &no_color, "", PaBool,   PaOpt, false,                            false, true, "No colored output"                 },
  PA_END_OF_ARGS
};

static const char *manSynopsis         = "logProbe [host:port] [-format X]";
static const char *manShortDescription =  "logProbe scans logs from a log server getting real-time log information\n";
static const char *manDescription      = manShortDescription;

static const char *manExitStatus    = "0      if OK\n 1-255  error\n";
static const char *manAuthor        = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char *manReportingBugs = "bugs to samson-dev@tid.es\n";
static const char *manCopyright     = "Copyright (C) 2011 Telefonica Digital";
static const char *manVersion       = "0.1";


int logFd = -1;




int main(int argC, const char *argV[]) {
  paConfig("prefix", (void *)"LOG_CLIENT_");
  paConfig("usage and exit on any warning", (void *)true);
  paConfig("log to screen", (void *)true);
  paConfig("log file line format", (void *)"TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
  paConfig("screen line format", (void *)"TYPE: TEXT");
  paConfig("log to file", (void *)false);
  paConfig("log to stderr", (void *)true);

  paConfig("man synopsis", (void *)manSynopsis);
  paConfig("man shortdescription", (void *)manShortDescription);
  paConfig("man description", (void *)manDescription);
  paConfig("man exitstatus", (void *)manExitStatus);
  paConfig("man author", (void *)manAuthor);
  paConfig("man reportingbugs", (void *)manReportingBugs);
  paConfig("man copyright", (void *)manCopyright);
  paConfig("man version", (void *)manVersion);

  paParse(paArgs, argC, (char **)argV, 1, true);
  logFd = lmFirstDiskFileDescriptor();

  // Check format for filter
  au::ErrorManager error;
  au::LogFilter::Create(filter, error);
  if (error.HasErrors()) {
    std::cerr << "Error in filter definition: " << error.GetLastError();
    exit(1);
  }

  // Log Probe to get logs and print on screen...
  au::LogProbe log_probe;
  log_probe.AddPlugin("printer", new au::LogProbePriter(format, !no_color, false));
  if (strlen(file_name) > 0) {
    au::ErrorManager error;
    log_probe.AddFilePlugin("file", file_name, error);
    if (error.HasErrors()) {
      std::cerr << au::str("Error login to file %s: ", file_name) << error.GetLastError() << std::endl;
    }
  }

  // Connect with LogServer
  log_probe.ConnectAsProbe(host, filter, error);

  if (error.HasErrors()) {
    std::cerr << "Error connecting " << host << " : " << error.GetLastError() << std::endl;
    exit(1);
  }

  // Sleep forever
  while (true) {
    sleep(1);
    if (!log_probe.IsConnected()) {
      // Show only if verbose is activated
      // std::cerr << "Error receiving logs from " << host << " : " << log_probe_printer.error().GetMessage() << std::endl;
      exit(1);
    }
  }

  return 0;
}

