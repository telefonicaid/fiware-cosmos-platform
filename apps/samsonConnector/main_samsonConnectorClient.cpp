#include <signal.h>
#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // memcpy
#include <iostream>     // std::cout

#include "parseArgs/parseArgs.h"
#include "parseArgs/paConfig.h"

#include "logMsg/logMsg.h"

#include "au/string.h"              // au::str()
#include "au/ThreadManager.h"
#include "au/network/Service.h"

#include "samson/client/SamsonClient.h"         // samson::SamsonClient
#include "samson/client/SamsonPushBuffer.h"

#include "samson/common/coding.h"               // KVHeader

#include "au/network/ConsoleService.h"

#include "SamsonConnector.h"

char host[1024];

static const char* manShortDescription = "samsonConnectorClient is the client for samsonConnector";
static const char* manSynopsis = "";

int default_buffer_size = 64*1024*1024 - sizeof(samson::KVHeader);
int default_input_buffer_size = 10000; // 10Kb

PaArgument paArgs[] =
{   
	{ "",      host,     "",  PaString,  PaOpt, _i ""  , PaNL, PaNL,        "Host to connect"     },
	PA_END_OF_ARGS
};

// Log fg for traces
int logFd = -1;


int main( int argC , const char *argV[] )
{
    paConfig("usage and exit on any warning", (void*) true);
    
    paConfig("log to screen",                 (void*) true);
    paConfig("log to file",                   (void*) true);  // In production it will be false
    paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
    paConfig("man shortdescription",          (void*) manShortDescription);
    paConfig("man synopsis",                  (void*) manSynopsis);
    paConfig("log to stderr",                 (void*) true);
    
    // Parse input arguments    
    paParse(paArgs, argC, (char**) argV, 1, false);
    logFd = lmFirstDiskFileDescriptor();
    

    
    au::network::ConsoleServiceClient console( 1234 );  // Default port for this client
    if( strcmp( host , "" ) != 0 ) 
        console.connect( host );    // Connect to the given host
    
    // Run console
    console.runConsole();
    
}