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

#include "au/log/log_server_common.h"
#include "au/log/LogServer.h"
#include "au/daemonize.h"

#define LOC "localhost"

#define LS_PORT       AU_LOG_SERVER_PORT
#define LS_QUERY_PORT AU_LOG_SERVER_QUERY_PORT
#define LS_LOG_DIR    AU_LOG_SERVER_DIRECTORY

bool fg;
int query_port;
int target_port;
char log_directory[1024];

PaArgument paArgs[] =
{
    { "-fg",         &fg,           "", PaBool,   PaOpt, false,      false,  true,   "don't start as daemon"             },
    { "-port",       &target_port,  "", PaInt,    PaOpt, LS_PORT,        1,   99999, "Port for default log channel"  },
    { "-query_port", &query_port,   "", PaInt,    PaOpt, LS_QUERY_PORT,  1,   99999, "Port for logClient connections"  },
    { "-dir",        log_directory, "", PaString, PaOpt, _i LS_LOG_DIR,  PaNL, PaNL, "Directory for default channel logs" },
    PA_END_OF_ARGS
};

static const char* manSynopsis         = "logServer [-port X] [-query_port X] [-dir X]";
static const char* manShortDescription = "logServer is a server to collect logs from diferent nodes in a cluster\n";
static const char* manDescription      =
"\n"
"logServer is a server to collect logs from diferent nodes in a cluster\n"
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

    
    if (fg == false)
    {
        LM_M(("logServer running in background"));
        daemonize();
    }
    else
    {
        LM_M(("logServer running in foreground"));
    }
    
    // Log server
    au::LogServer log_server;

    // Sleep forever
    while (true) 
        sleep(1);
    
   return 0;
}
