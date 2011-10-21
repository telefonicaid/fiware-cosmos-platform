
/*
 
 samsonPush
 
 Example app using samsonClient lib
 It listen std input and push content to the samson cluster indicated by parameters
 
 AUTHOR: Andreu Urruela
 
 */


#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // memcpy
#include <iostream>     // std::cout

#include "parseArgs/parseArgs.h"
#include "logMsg/logMsg.h"

#include "au/string.h"              // au::str()

#include "samson/client/SamsonClient.h"         // samson::SamsonClient


size_t buffer_size;
size_t timeOut;

char controller[1024];
char queue_name[1024];

bool show_header;

PaArgument paArgs[] =
{
	{ "-controller",            controller,           "CONTROLLER",            PaString, PaOpt, _i "localhost"  , PaNL, PaNL,       "controller IP:port"         },
	{ "-header",                &show_header,         "SHOW_HEADER",      PaBool,    PaOpt,  false, false,  true,  "Show only header of blocks"   },
	{ " ",                      queue_name,           "QUEUE",                 PaString,  PaOpt,  (long) "no_queue",   PaNL,   PaNL,  "name of the queue to push data"         },
    PA_END_OF_ARGS
};

int logFd = -1;

void find_and_replace( std::string &source, const std::string find, std::string replace ) {
   size_t j;
   for ( ; (j = source.find( find )) != std::string::npos ; ) {
	  source.replace( j, find.length(), replace );
   }
}


void literal_string( std::string& txt )
{
   std::string slash = "\n"; 
   std::string replace_slash = "\\n";
   find_and_replace( txt , slash , replace_slash );   
}


size_t full_read( int fd , char* data , size_t size)
{
    size_t read_size = 0;
    
    while( read_size < size )
    {
        ssize_t t = read( fd , data+read_size , size - read_size );
        
        if( t==-1)
            LM_X(1,("Error reading input data"));
        
        if( t == 0)
            break;
        else
            read_size+=t;
    }
    
    return read_size;    
}

int main( int argC , const char *argV[] )
{
    paConfig("usage and exit on any warning", (void*) true);
    
    paConfig("log to screen",                 (void*) true);
    paConfig("log to file",                   (void*) false);
    paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
    
    // Parse input arguments    
    paParse(paArgs, argC, (char**) argV, 1, false);
    logFd = lmFirstDiskFileDescriptor();
    
    // Instance of the client to connect to SAMSON system
    samson::SamsonClient client;
    
    // Set 1G RAM for uploading content
    client.setMemory( 1024*1024*1024 );
    
    LM_M(("Connecting to %s ..." , controller));
    
    // Init connection
    if( !client.init( controller ) )
    {
        fprintf(stderr, "Error connecting with samson cluster: %s\n" , client.getErrorMessage().c_str() );
        exit(0);
    }
    LM_M(("Conection to %s OK" , controller));

    
    // Connect to a particular queue
    LM_M(("Connecting to queue %s" , queue_name ));
    client.connect_to_queue( queue_name );

    while( true )
    {
        
        samson::SamsonClientBlockInterface *block = client.getNextBlock( queue_name );
        
        if( block )
        {
            if( show_header )
                block->print_header();
            else
                block->print_content();
            
            delete block;
        }
        else
            usleep(100000);
        
    }
    
    
    // Wait until all operations are complete
    LM_M(("Waiting for all the push operations to complete..."));
    
    client.waitUntilFinish();
    
    LM_M(("Finish correctly\n"));
    
	
}
