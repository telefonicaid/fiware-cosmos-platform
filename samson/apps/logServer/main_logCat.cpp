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
#include "au/log/LogProbe.h"
#include "au/log/LogFormatter.h"
#include "au/log/LogServer.h"



char format[1024];
char filter[1024];
char target_file[1024];
bool is_reverse;
bool show_fields;
char str_time[1024];
char str_date[1024];
char file_name[1024];
bool count;
bool no_color;

PaArgument paArgs[] =
{
  { "-format", format,   "",  PaString, PaOpt, _i LOG_DEFAULT_FORMAT_LOG_CLIENT, PaNL,  PaNL, "Formats of the logs at the output" },
  { "-filter", filter,   "",  PaString, PaOpt, _i "", PaNL,  PaNL, "Filter for logs"                   },
  { "-save"  , file_name,"", PaString, PaOpt, _i "",  PaNL, PaNL,"Save received logs to file" },
  { "-count" , &count,   "", PaBool,   PaOpt,false, false, true, "Show possible fields for format argument" },
  { "-reverse", &is_reverse, "", PaBool, PaOpt,false, false, true, "Show in reverse temporal order" },
  { "-show_fields",&show_fields, "",PaBool,PaOpt,false,false, true, "Show possible fields for format argument" },
  { "-no_color" , &no_color,"", PaBool, PaOpt, false,  false, true,"No colored output" },
  { " ", target_file, "", PaString, PaReq,_i "", PaNL, PaNL, "Log file to scan"  },
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

  // Check format for filter
  au::ErrorManager error;
  au::LogFilterPointer log_filter = au::LogFilter::Create( filter , error);
  if( error.IsActivated() )
  {
    std::cerr << error.GetErrorMessage("Error in filter definition");
    exit(1);
  }

  if( show_fields )
  {
    au::SharedPointer<au::tables::Table> table = au::getTableOfFields();
    std::cout << table->str();
    return 0;
  }
  
  // Open provided log file
  std::vector<au::SharedPointer<au::Log> > logs = au::readLogFile(target_file, error);
  if (error.IsActivated()) {
    LM_X(1, ("ERROR: %s", error.GetMessage().c_str()));  // Formatter to create table
  }

  // Log Probe
  au::LogProbe log_probe;
  if(count)
    log_probe.AddPlugin("count", new au::LogProbeCounter() );
  else
    log_probe.AddPlugin("printer", new au::LogProbePriter(format , !no_color , false) );
  if( strlen( file_name) > 0 )
  {
    au::ErrorManager error;
    log_probe.AddFilePlugin("file", file_name , error );
    if( error.IsActivated())
      std::cerr << error.GetErrorMessage(au::str("Error login to file %s" , file_name)) << std::endl;
  }

  // Flush logs to probe
  size_t n_logs = logs.size();
  if ( is_reverse )
  {
    for ( size_t i = 0 ; i < n_logs ; ++i )
    {
      au::LogPointer log = logs[n_logs-i-1 ];
      if( log_filter->Accept(log ) )
        log_probe.Process( log );
    }
  }
  else
  {
    for ( size_t i = 0 ; i < n_logs ; ++i )
    {
      au::LogPointer log = logs[i];
      if( log_filter->Accept(log ) )
        log_probe.Process( log );
    }
  }

  // Clear content of the probe
  log_probe.Clear();

  
}

