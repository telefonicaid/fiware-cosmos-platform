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
#include  <string>

#include "parseArgs/parseArgs.h"

#include "au/log/LogCommon.h"
#include "au/log/LogFile.h"
#include "au/log/LogFormatter.h"
#include "au/log/LogServer.h"
#include "au/log/TableLogFormatter.h"


char format[1024];
char pattern[1024];
char type[1024];
char target_file[1024];
int limit;
bool is_table;
bool is_reverse;
bool is_multi_session;
char str_time[1024];
char str_date[1024];

PaArgument paArgs[] =
{
  { "-format",        format,                   "",                     PaString,                     PaOpt,
    _i AU_LOG_DEFAULT_FORMAT,    PaNL,
    PaNL,
    "Log file to scan"                         },
  { "-pattern",       pattern,                  "",                     PaString,                     PaOpt,
    _i "",     PaNL,            PaNL,
    "Pattern to be found in logs"         },
  { "-limit",         &limit,                   "",                     PaInt,                        PaOpt,
    10000,     0,               100000000,
    "Max number of logs to be displayed"  },
  { "-table",         &is_table,                "",                     PaBool,                       PaOpt,
    false,     false,           true,                "Show in table format"                      },
  { "-reverse",       &is_reverse,              "",                     PaBool,                       PaOpt,
    false,     false,           true,
    "Show in reverse temporal order"      },
  { "-multi_session", &is_reverse,              "",                     PaBool,                       PaOpt,
    false,     false,           true,                "Skip new_session marks"                    },
  { "-time",          &str_time,                "",                     PaString,                     PaOpt,
    _i "",     PaNL,            PaNL,
    "Show only logs older that this time" },
  { "-date",          &str_date,                "",                     PaString,                     PaOpt,
    _i "",     PaNL,            PaNL,
    "Show only logs older that this date" },
  { "-type",          type,                     "",                     PaString,                     PaOpt,
    _i "",     PaNL,            PaNL,
    "Filter a particular type of logs"    },
  { " ",              target_file,              "",                     PaString,                     PaReq,
    _i "",     PaNL,            PaNL,                "Log file to scan"                          },
  PA_END_OF_ARGS
};

static const char *manSynopsis         = "logCat log_file";
static const char *manShortDescription = "logCat is a command line utility to scan a log file\n";
static const char *manDescription      =
  "\n"
  "logCat is a command line utility to scan a log file\n"
  "\n";

static const char *manExitStatus    = "0      if OK\n 1-255  error\n";
static const char *manAuthor        = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char *manReportingBugs = "bugs to samson-dev@tid.es\n";
static const char *manCopyright     = "Copyright (C) 2011 Telefonica Digital";
static const char *manVersion       = "0.1";

int logFd = -1;

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

  LM_V(("LogCat %s", target_file));

  // Log formmatter
  LM_V(("Using format %s", format));

  // Open teh log file
  au::ErrorManager error;
  std::vector<au::SharedPointer<au::Log> > logs = au::readLogFile(target_file, error);

  if (error.IsActivated()) {
    LM_X(1, ("ERROR: %s", error.GetMessage().c_str()));  // Formatter to create table
  }
  au::TableLogFormatter table_log_formater(format);

  // Setup of the table log formatter
  table_log_formater.set_pattern(pattern);
  table_log_formater.set_time(str_time);
  table_log_formater.set_date(str_date);
  table_log_formater.set_as_table(is_table);
  table_log_formater.set_reverse(is_reverse);
  table_log_formater.set_as_multi_session(is_multi_session);
  table_log_formater.set_limit(limit);

  table_log_formater.init(error);

  if (error.IsActivated()) {
    LM_X(1, ("Error: %s", error.GetMessage().c_str()));
  }
  size_t num_logs = logs.size();
  for (size_t i = 0; i <  num_logs; i++) {
    // Add log to the table formatter
    table_log_formater.add(logs[ num_logs - i - 1 ]);

    // Check if we have enougth
    if (table_log_formater.enougthRecords()) {
      break;
    }
  }

  // Emit output to the stdout
  std::cout << table_log_formater.str();
}

