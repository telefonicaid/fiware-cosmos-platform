

#include "parseArgs/parseArgs.h"

#include "au/console/Console.h"
#include "au/utils.h"
#include "au/console/ConsoleAutoComplete.h"
#include "au/network/SocketConnection.h"

#include "au/log/LogClient.h"
#include "au/log/log_server_common.h"


char command[1024];
char target_host[1024];
char format[1024];
int target_port;

#define LOC "localhost"
#define LS_PORT LOG_SERVER_DEFAULT_QUERY_CHANNEL_PORT


PaArgument paArgs[] =
{
    { "-command",   command,       "", PaString, PaOpt, _i "",    PaNL, PaNL,  "Command to be executed"    },
    { "-format",    format,        "", PaString, PaOpt, _i "",    PaNL, PaNL, "Log file to scan" },
    { "",           target_host,   "", PaString, PaOpt, _i LOC,   PaNL, PaNL,  "Log server hostname"       },
    { "",           &target_port,  "", PaInt,    PaOpt, LS_PORT,  1,    99999, "Log server port"           },
    
    PA_END_OF_ARGS
};

static const char* manSynopsis         = "logClient [host] [port] [-command X]";
static const char* manShortDescription =  "logClient is a command line utility to interact with a logServer\n";
static const char* manDescription      =
"\n"
"logClient is a command line utility to interact with a logServer\n"
"\n";

static const char* manExitStatus    = "0      if OK\n 1-255  error\n";
static const char* manAuthor        = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char* manReportingBugs = "bugs to samson-dev@tid.es\n";
static const char* manCopyright     = "Copyright (C) 2011 Telefonica Digital";
static const char* manVersion       = "0.1";



int logFd=-1;
au::LogClient log_client;

int main(int argC, const char *argV[])
{
    paConfig("prefix",                        (void*) "LOG_CLIENT_");
    paConfig("usage and exit on any warning", (void*) true);
    paConfig("log to screen",                 (void*) true);
    paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
    paConfig("screen line format",            (void*) "TYPE: TEXT");
    paConfig("log to file",                   (void*) false);
    paConfig("log to stderr",                 (void*) true);
    
    paConfig("man synopsis",                  (void*) manSynopsis);
    paConfig("man shortdescription",          (void*) manShortDescription);
    paConfig("man description",               (void*) manDescription);
    paConfig("man exitstatus",                (void*) manExitStatus);
    paConfig("man author",                    (void*) manAuthor);
    paConfig("man reportingbugs",             (void*) manReportingBugs);
    paConfig("man copyright",                 (void*) manCopyright);
    paConfig("man version",                   (void*) manVersion);
    
    paParse(paArgs, argC, (char**) argV, 1, true);
    //lmAux((char*) "father");
    logFd = lmFirstDiskFileDescriptor();
    
    LM_V(("Connecting to log server at %s:%d" , target_host , target_port ));
    
    // Init main log client
    au::ErrorManager error;
    log_client.connect( target_host , target_port , &error );

    // Not tolerant on errors in first connection
    if( error.isActivated() )
        LM_X(1 , ("%s" , error.getMessage().c_str() ));
    
    if( strcmp(command, "") != 0 )
    {
        // Sent a single command
        log_client.sent_command(command, &error);

        if( error.isActivated() )
            LM_X(1 , ("%s" , error.getMessage().c_str() ));

        std::string answer = log_client.getMessageFromLogServer(&error);

        if( error.isActivated() )
            LM_X(1 , ("%s" , error.getMessage().c_str() ));

        printf("%s" , answer.c_str() );
        exit(0);
    }
    
    // Run console in iterative mode
    log_client.runConsole();
    
   return 0;
}