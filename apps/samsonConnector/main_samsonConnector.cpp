

#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // memcpy
#include <iostream>     // std::cout

#include "parseArgs/parseArgs.h"
#include "parseArgs/paConfig.h"

#include "logMsg/logMsg.h"

#include "au/string.h"              // au::str()

#include "samson/client/SamsonClient.h"         // samson::SamsonClient
#include "samson/client/SamsonPushBuffer.h"

#include "samson/common/coding.h"               // KVHeader

#include "samson/network/NetworkListener.h"
#include "samson/network/SocketConnection.h"

#include "SamsonConnector.h"

size_t buffer_size;
char input[1024];
char output[1024];
char splitter[1024];
bool interactive;

static const char* manShortDescription = 
"samsonConnector is a easy-to-use network tool used for small processing and data transportation in a SAMSON system.\n";

static const char* manSynopsis = "[-help] [-input \"[stdin] [port:X] [connection:server:port] [samson:X:queue]\"] [-output \"[stdout] [port:X] [connection:server:port] [samson:X:queue]\"]  [-buffer_size int_size] [-splitter XX]\n";

int default_buffer_size = 64*1024*1024 - sizeof(samson::KVHeader);

PaArgument paArgs[] =
{   
	{ "-input",            input,             "",  PaString,  PaOpt, _i "stdin"  , PaNL, PaNL,        "Input sources "          },
	{ "-output",           output,            "",  PaString,  PaOpt, _i "stdout"  , PaNL, PaNL,       "Output sources "         },
	{ "-buffer_size",      &buffer_size,      "",  PaInt,     PaOpt,       default_buffer_size, 1,   default_buffer_size,  "Buffer size in bytes"    },
	{ "-splitter",         splitter,          "",  PaString,  PaOpt, _i "system.line",   PaNL, PaNL,  "Splitter"  },
	{ "-i",                &interactive,      "",  PaBool,    PaOpt,    false,  false,   true,        "Interactive console"          },
	PA_END_OF_ARGS
};

// Log fg for traces
int logFd = -1;


// Network connections ( input and output )
samson::SamsonConnector samson_connector;



// Instance of the client to connect to SAMSON system
samson::SamsonClient *samson_client;


void* review_samson_connector(void*p)
{
    // Endless loop waiting for data....
    while( true )
    {
        samson_connector.review();
        samson_connector.exit_if_necessary();
        sleep(5);
    }
   
}

int main( int argC , const char *argV[] )
{
    paConfig("usage and exit on any warning", (void*) true);
    
    paConfig("log to screen",                 (void*) true);
    paConfig("log to file",                   (void*) false);
    paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
    paConfig("man shortdescription",          (void*) manShortDescription);
    paConfig("man synopsis",          (void*) manSynopsis);
    paConfig("log to stderr",         (void*) true);
    
    // Parse input arguments    
    paParse(paArgs, argC, (char**) argV, 1, false);
    logFd = lmFirstDiskFileDescriptor();
    
    // Random initialization
    srand( time(NULL) );
    
    if( buffer_size == 0)
        LM_X(1,("Wrong buffer size %lu", buffer_size ));
    
    //Init engine
    engine::Engine::init();
    
    LM_D(("engine::MemoryManager::init"));
    engine::MemoryManager::init( 1000000 );

    // -------------------------------------------------------
    // Parsing outputs
    // -------------------------------------------------------
    {
        au::CommandLine cmdLine;
        cmdLine.parse(output);
        
        
        if( cmdLine.get_num_arguments() == 0 )
            LM_X(1, ("No output specified"));
        
        for ( int i = 0 ; i < cmdLine.get_num_arguments() ; i++ )
        {
            std::string input_string = cmdLine.get_argument(i);
            
            std::vector<std::string> components = au::split(input_string, ':');
            
            if( components[0] == "stdout" )
            {
                LM_V(("Adding stdout as output channel"));
                samson_connector.add_stdout();
            }
            else if( components[0] == "stderr" )
            {
                LM_V(("Adding stderr as output channel"));
                samson_connector.add_stderr();
            }
            
            else if( components[0] == "port" )
            {
                if( components.size() < 2 )
                    LM_X(1, ("Output port without number. Please specifiy port to open (ex port:10000)"));
                
                int port = atoi( components[1].c_str() );
                if( port == 0 )
                    LM_X(1, ("Wrong input port"));
                
                LM_V(("Adding port %d as output port" , port));
                samson_connector.add_output_port( port );
            }
            else if( components[0] == "connection" )
            {
                if( components.size() < 3 )
                    LM_X(1, ("Output connection without host and port. Please specifiy as connection:host:port)"));
                
                int port = atoi( components[2].c_str() );
                if( port == 0 )
                    LM_X(1, ("Wrong connection port for %s" , components[1].c_str() ));
                
                samson_connector.add_output_connection(components[1], port );
                
            }
            else if( components[0] == "samson" )
            {
                int port = SAMSON_WORKER_PORT;
                samson_connector.add_samson_output_connection(components[1], port, components[2] );
            }
        }
    }
    
    // Review to create dedicated connections
    samson_connector.review();
    
    // -------------------------------------------------------
    // Parsing inputs
    // -------------------------------------------------------
    {
        au::CommandLine cmdLine;
        cmdLine.parse(input);
        
        
        if( cmdLine.get_num_arguments() == 0 )
            LM_X(1, ("No input specified"));
        
        for ( int i = 0 ; i < cmdLine.get_num_arguments() ; i++ )
        {
            std::string input_string = cmdLine.get_argument(i);
            
            std::vector<std::string> components = au::split(input_string, ':');
            
            if( components[0] == "stdin" )
            {
                LM_V(("Adding stdin as input channel"));
                samson_connector.add_stdin();
            }
            else if( components[0] == "port" )
            {
                if( components.size() < 2 )
                    LM_X(1, ("Input port without number. Please specifiy port to open (ex port:10000)"));
                
                int port = atoi( components[1].c_str() );
                if( port == 0 )
                    LM_X(1, ("Wrong input port"));
                
                LM_V(("Adding port %d as input port" , port));
                samson_connector.add_input_port( port );
            }
            else if( components[0] == "connection" )
            {
                if( components.size() < 3 )
                    LM_X(1, ("Input connection without host and port. Please specifiy as connection:host:port)"));
                
                int port = atoi( components[2].c_str() );
                if( port == 0 )
                    LM_X(1, ("Wrong connection port for %s" , components[1].c_str() ));
                
                samson_connector.add_input_connection(components[1], port );
                
            }
            else if( components[0] == "samson" )
            {
                int port = SAMSON_WORKER_PORT;
                samson_connector.add_samson_input_connection(components[1], port, components[2] );
            }
        }
    }
    
    
    // Background thread to review connections in samson connector
    pthread_t t;
    pthread_create(&t, NULL, review_samson_connector, NULL);
    
    // Run console if interactive mode is activated
    if( interactive )
        samson_connector.runConsole();
    else
        while( true )
            sleep(10000);
    
    
    
	return 0;
}
