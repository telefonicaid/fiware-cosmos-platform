
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
#include "au/daemonize.h"

#include "samson/client/SamsonClient.h"         // samson::SamsonClient
#include "samson/client/SamsonPushBuffer.h"

#include "samson/common/coding.h"               // KVHeader

#include "au/network/NetworkListener.h"
#include "au/network/SocketConnection.h"

#include "common.h"
#include "SamsonConnector.h"

size_t input_buffer_size;
size_t buffer_size;
char input[1024];
char output[1024];
char input_splitter_name[1024];
char file_name[1024];

bool interactive;
bool run_as_daemon;

static const char* manShortDescription = 
"samsonConnector is a easy-to-use network tool used for small processing and data transportation in a SAMSON system.\n"
"Several inputs and outputs can be especified as follows:\n"
"\n"
"------------------------------------------------------------------------------------\n"
" Inputs\n"
"------------------------------------------------------------------------------------\n"
"      stdin                        Data is read from stdin\n"
"      port:X                       Opens a port waiting for incomming connections that push data to us\n"
"      connection[:host]:port       We connect to this host and port and read data\n" 
"      samson[:host][:port]:queue   We connect to this SAMSON cluster and pop data from queue\n" 
"\n"
"------------------------------------------------------------------------------------\n"
" Outputs\n"
"------------------------------------------------------------------------------------\n"
"      stdout                       Data is write to stdout\n"
"      port:X                       Opens a port waiting for output connections. Data is pushed to all connections\n"
"      connection:host:port         We connect to this host and port and write data\n" 
"      samson[:host][:port]:queue   We connect to this SAMSON cluster and push data from queue\n" 
"\n"
"------------------------------------------------------------------------------------\n"
" Examples:\n"
"------------------------------------------------------------------------------------\n"
"\n"
" samsonConnector -input stdin -output samson:samson01:input\n"
"\n"
"        Data is read from stdin and pushed to queue 'input' in a SAMSON cluster deployed in server 'samson01'\n"
"\n"
" samsonConnector -input port:10000 -output \"port:20000 samson:samson01:input\"\n"
"\n"
"        Data is read from incomming connections to port 10000.\n"
"        Data is then pushed to queue 'input' in a SAMSON cluster deployed in server 'samson01' and to any connection to port 20000\n"

;

static const char* manSynopsis = "[-input input_description]  [-output output_description]  [-buffer_size size] [-splitter splitter_name]\n";


int sc_console_port;
int sc_web_port;
int default_buffer_size = 64*1024*1024 - sizeof(samson::KVHeader);
int default_input_buffer_size = 100000; // 100Kb


PaArgument paArgs[] =
{   
	{ "-input",            input,               "",  PaString,  PaOpt, _i "stdin"  , PaNL, PaNL,        "Input sources "          },
	{ "-output",           output,              "",  PaString,  PaOpt, _i "stdout"  , PaNL, PaNL,       "Output sources "         },
	{ "-buffer_size",      &buffer_size,        "",  PaInt,     PaOpt, default_buffer_size,       1, default_buffer_size,  "Buffer size in bytes"    },
	{ "-inputbuffer_size", &input_buffer_size,  "",  PaInt,     PaOpt, default_input_buffer_size, 1, 10000000,               "Read inputs in chunks of this size" },
	{ "-splitter",         input_splitter_name, "",  PaString,  PaOpt, _i "",   PaNL, PaNL,  "Splitter to be used ( only valid for the default channel )"  },
	{ "-i",                &interactive,        "",  PaBool,    PaOpt,    false,  false,   true,        "Interactive console"          },
	{ "-daemon",           &run_as_daemon,      "",  PaBool,    PaOpt,    false,  false,   true,        "Run in background. Remove connection & REST interface activated"  },
    { "-console_port",     &sc_console_port,    "",  PaInt,     PaOpt,    SC_CONSOLE_PORT,     1,      9999,  "Port to receive new console connections"   },
    { "-web_port",         &sc_web_port,        "",  PaInt,     PaOpt,    SC_WEB_PORT,         1,      9999,  "Port to receive REST connections"   },
	{ "-f",                file_name,           "",  PaString,  PaOpt, _i "",   PaNL, PaNL,  "Input file with commands to setup channels and adapters"  },
	PA_END_OF_ARGS
};

// Log fg for traces
int logFd = -1;


// Network connections ( input and output )
samson::connector::SamsonConnector* samson_connector;


// Instance of the client to connect to SAMSON system
samson::SamsonClient *samson_client;



void captureSIGPIPE(int s )
{
    // Nothing
}

int main( int argC , const char *argV[] )
{
    paConfig("usage and exit on any warning", (void*) true);
    
    paConfig("log to screen",                 (void*) true);
    paConfig("log to file",                   (void*) true);  // In production it will be false?
    paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
    paConfig("man shortdescription",          (void*) manShortDescription);
    paConfig("man synopsis",                  (void*) manSynopsis);
    paConfig("log to stderr",                 (void*) true);
    
    // Parse input arguments    
    paParse(paArgs, argC, (char**) argV, 1, false);
    logFd = lmFirstDiskFileDescriptor();
    
    // Random initialization
    srand( time(NULL) );
    
    // Capturing SIGPIPE
    if (signal(SIGPIPE, captureSIGPIPE) == SIG_ERR)
        LM_W(("SIGPIPE cannot be handled"));
    
    if( buffer_size == 0)
        LM_X(1,("Wrong buffer size %lu", buffer_size ));

    // Run in background if required
    if( run_as_daemon )
    {
        daemonize();
        deamonize_close_all();
    }
    
    // Init samson setup with default values
	samson::SamsonSetup::init("","");			
    
    // Engine and its associated elements
	engine::Engine::init();
	engine::MemoryManager::init( 1000000000 );
	//engine::DiskManager::init(1);
	//engine::ProcessManager::init(samson::SamsonSetup::shared()->getInt("general.num_processess"));
    
	samson::ModulesManager::init("samsonConnector");         // Init the modules manager

    // Ignore verbose mode if interactive is activated
    if( interactive )
        lmVerbose = false;
    
    // Init samsonConnector
    
    samson_connector = new samson::connector::SamsonConnector();


    if( strcmp(file_name, "") != 0 )
    {

        // Common error messages

        // Read commands from file
        FILE *f = fopen( file_name , "r" );
        if( !f )
        {
            LM_E(("Error opening commands file %s", file_name));
            exit(0);
        }
        
        int num_line = 0;
        char line[1024];
        
        LM_M(("Processing commands file %s", file_name ));
        
        while( fgets(line, sizeof(line), f) )
        {
            // Remove the last return of a string
            while( ( strlen( line ) > 0 ) && ( line[ strlen(line)-1] == '\n') > 0 )
                line[ strlen(line)-1]= '\0';
            
            //LM_M(("Processing line: %s", line ));
            num_line++;
            
            if( ( line[0] != '#' ) && ( strlen(line) > 0) )
            {
                au::ErrorManager error;
                error.add_message(au::str("Processing: '%s'" , line ));
                samson_connector->process_command( line  , &error );
                std::cerr << error.str();
            }
        }
        
        // Print the error on screen
        
        
        fclose(f);
        
    }
    else
    {
        // Create default channel
        au::ErrorManager error;
        samson_connector->process_command( au::str("add_channel default %s" , input_splitter_name )  , &error );
        
        // Add outputs
        samson_connector->process_command( au::str("add_output_to_channel default %s" , output )  , &error );
        if( error.isActivated() )
            samson_connector->writeError( error.getMessage().c_str()  );
        
        // Add inputs
        samson_connector->process_command( au::str("add_input_to_channel default %s" , input )  , &error );
        if( error.isActivated() )
            samson_connector->writeError( error.getMessage().c_str()  );

        // Print all setup on screen
        std::cerr << error.str();
    }
        
    // Run console if interactive mode is activated
    if( run_as_daemon )
    {
        // Add service to accept monitor connections from samsonConnectorClient
        samson_connector->add_service();               
        // Add REST service to accept REST-full connections
        samson_connector->initRESTInterface();         
        // Add service to accept inter-channel connections
        samson_connector->initInterChannelInteface();
        
        while( true )
            sleep(1000);
        
    }
    else if( interactive )
    {
        // Add service to accept monitor connections from samsonConnectorClient
        samson_connector->add_service();               
        // Add REST service to accept REST-full connections
        samson_connector->initRESTInterface();         
        // Add service to accept inter-channel connections
        samson_connector->initInterChannelInteface();
        
        samson_connector->runConsole();
    }
    else
        while( true )
        {
            // Verify if can exit....
            if( samson_connector->getNumInputItems() == 0 )
                if( samson_connector->getOutputConnectionsSize() == 0 )                 // Check no pending data to be send....
                    LM_X(0, ("Finish correctly. No more inputs data"));
            
            usleep(100000);
        }
    
    
    
	return 0;
}
