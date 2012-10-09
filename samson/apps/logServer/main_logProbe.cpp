

#include "parseArgs/parseArgs.h"

#include "au/console/Console.h"
#include "au/console/ConsoleAutoComplete.h"
#include "au/network/SocketConnection.h"
#include "au/utils.h"

#include "au/log/LogCommon.h"
#include "au/network/ConsoleService.h"

char format[1024];
char host[1024];
char filter[1024];

#define LOC     "localhost"
#define LS_PORT AU_LOG_SERVER_QUERY_PORT


PaArgument paArgs[] =
{
  { "",        host,  "", PaString, PaOpt, _i "localhost",  PaNL, PaNL, "Log server hostname" },
  { "-format", format, "", PaString, PaOpt, _i AU_LOG_DEFAULT_FORMAT_LOG_CLIENT, PaNL, PaNL, "Formats of the logs at the output"},
  { "-filter", filter, "", PaString, PaOpt, _i "", PaNL, PaNL, "Filter for logs"},
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



au::Color GetColorForLog( au::LogPointer log )
{
  if( log->Get("channel_alias") == "E" )
    return  au::red;
  if( log->Get("channel_alias") == "W" )
    return  au::purple;
  
  return au::normal;
  
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

  LM_V(("Connecting to log server at %s", host ));

  // Add default port to host if necessary
  std::string str_host = host;
  if ( str_host.find_last_of(":") == std::string::npos )
    str_host += au::str(":%d" , AU_LOG_SERVER_PORT );
  
  // Create a socket connection with the provided host
  au::ErrorManager error;
  au::SharedPointer<au::SocketConnection> socket_connection = au::SocketConnection::Create( str_host  , error );

  if( error.IsActivated() )
  {
    fprintf(stderr, "Error connecting with %s (%s)" , host , error.GetMessage().c_str() );
    exit(0);
  }

  // Write a Hello message
  au::gpb::LogConnectionHello hello;
  hello.set_type( au::gpb::LogConnectionHello_LogConnectionType_LogProbe );
  hello.set_filter( filter );
  
  au::Status s = au::writeGPB( socket_connection->fd() , &hello );
  if( s != au::OK )
  {
    fprintf(stderr, "Not possible to sent hello message to log server");
    exit(0);
  }

  // Log formatter to get the string to show
  au::LogFormatter log_formatter( format );
  
  // Infinite loop to receive logs
  while( true )
  {
    au::LogPointer log( new au::Log() );
    bool real_log = log->Read( socket_connection.shared_object() );
    
    if( real_log )
    {
      au::Color color = GetColorForLog( log );
      std::cerr << au::str( color , "%s", log_formatter.get(log).c_str() ) << "\n";
    }
    else{
      fprintf(stderr, "Error reading log\n");
      exit(0);
    }
    
    if( socket_connection->IsClosed() )
    {
      fprintf(stderr, "Socket connection closed by server");
      exit(0);
    }
    
  }
  
  return 0;
}

