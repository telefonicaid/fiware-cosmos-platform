

#include "parseArgs/parseArgs.h"

#include "au/log/log_server_common.h"
#include "au/log/LogServer.h"

#define DEF_FORMAT "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT"

char format[1024];
char target_file[1024];


PaArgument paArgs[] =
{
    { "-format",  format       , "", PaString, PaOpt, _i DEF_FORMAT, PaNL, PaNL, "Log file to scan" },
    { " ",        target_file  , "", PaString, PaReq, _i "",         PaNL, PaNL, "Log file to scan" },
    PA_END_OF_ARGS
};

static const char* manSynopsis         = "logCat log_file";
static const char* manShortDescription = "logCat is a command line utility to scan a log file\n";
static const char* manDescription      =
"\n"
"logCat is a command line utility to scan a log file\n"
"\n";

static const char* manExitStatus    = "0      if OK\n 1-255  error\n";
static const char* manAuthor        = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char* manReportingBugs = "bugs to samson-dev@tid.es\n";
static const char* manCopyright     = "Copyright (C) 2011 Telefonica Digital";
static const char* manVersion       = "0.1";

int logFd=-1;

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

    LM_V(("LogCat %s" , target_file));

    
    // Log formmatter
    LM_V(("Using format %s" , format));
    au::LogFormatter log_formatter(format);
    
    // Open teh log file    
    au::LogFile *logFile;
    au::Status s = au::LogFile::read( target_file , &logFile);
    
    if( s == au::OK )
    {
        size_t num_logs = logFile->logs.size();
        for( size_t i = 0 ; i <  num_logs ; i++ )
        {
            au::Log *log = logFile->logs[ num_logs - i -1 ];
            
            LM_V(("Processing log %s" , log->str().c_str() ));
            std::cout << log_formatter.get( log ) << "\n";
        }
        
    }
    else
    {
        LM_X(1, ("Unable to open file %s (%s)" , target_file , au::status(s)));
    }
    
    
    
    
}