

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
char file_name[1024];
char host[1024];
char filter[1024];
bool count;

#define LOC     "localhost"
#define LS_PORT AU_LOG_SERVER_QUERY_PORT

PaArgument paArgs[] =
{
  { "-host"  , host,     "", PaString, PaOpt, _i "localhost",  PaNL, PaNL,"Log server hostname" },
  { "-format", format,   "", PaString, PaOpt, _i AU_LOG_DEFAULT_FORMAT_LOG_CLIENT, PaNL, PaNL, "Formats of the logs at the output" },
  { "-filter", filter,   "", PaString, PaOpt, _i "", PaNL, PaNL, "Command to execute" },
  { "-save"  , file_name,"", PaString, PaOpt, _i "",  PaNL, PaNL,"Save received logs to file" },
  { "-count" , &count,   "", PaBool,   PaOpt,false, false, true, "Show possible fields for format argument" },
  PA_END_OF_ARGS
};

// logQuery localhost today count

static const char *manSynopsis         = "logQuery [command] [-format X] [-host host[:port]]";
static const char *manShortDescription = "logQuery scans logs from a log server based on a query command\n";
static const char *manDescription      = manShortDescription;

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
    logFd = lmFirstDiskFileDescriptor();
    
    // Check format for filter
    au::ErrorManager error;
    au::LogFilter::Create( filter , error);
    if( error.IsActivated() )
    {
      std::cerr << error.GetErrorMessage("Error in filter definition");
      exit(1);
    }
    
    // Log Probe to get logs and print on screen...
    au::LogProbe log_probe;
    if(count)
      log_probe.AddPlugin("count", new au::LogProbeCounter() );
    else
      log_probe.AddPlugin("printer", new au::LogProbePriter(format) );
    if( strlen( file_name) > 0 )
    {
      au::ErrorManager error;
      log_probe.AddFilePlugin("file", file_name , error );
      if( error.IsActivated())
        std::cerr << error.GetErrorMessage(au::str("Error login to file %s" , file_name)) << std::endl;
    }
    
    // Connect with LogServer
    log_probe.ConnectAsQuery( host, filter, count, error);
    if( error.IsActivated() )
    {
      std::cerr << "Error connecting " << host << " : " << error.GetMessage() << std::endl;
      exit(1);
    }
    
    // Sleep forever
    while (true) {
      sleep(1);
      if( !log_probe.IsConnected() )
      {
        log_probe.Clear();
        exit(1);
      }
    }
    
    return 0;
  }
  
