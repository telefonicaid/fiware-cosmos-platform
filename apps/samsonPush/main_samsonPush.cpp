
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
#include "parseArgs/paConfig.h"

#include "logMsg/logMsg.h"

#include "au/string.h"              // au::str()

#include "samson/client/SamsonClient.h"         // samson::SamsonClient


size_t buffer_size;
size_t timeOut;
char breaker_sequence[1024];
char controller[1024];
char queue_name[1024];
bool lines;                         // Flag to indicate that input is read line by line

static const char* manShortDescription = 
"samsonPush is a easy-to-use client to send data to a particular queue in a SAMSON system. Just push data into the standard input\n";

static const char* manSynopsis =
"[-help] [-controller str_controller] [-timeout int_t] [-buffer_size int_size] [-breaker_sequence str_pattern] [-lines bool] queue\n";

PaArgument paArgs[] =
{
	{ "-controller",            controller,           "CONTROLLER",            PaString, PaOpt, _i "localhost"  , PaNL, PaNL,       "controller IP:port"         },
    { "-timeout",               &timeOut,             "TIMEOUT",               PaInt,    PaOpt,      0   ,    0,  10000,              "Timeout to deliver a block to the platform"           },
    { "-buffer_size",           &buffer_size,         "BUFFER_SIZE",           PaInt,    PaOpt,      10000000   ,    1,  64000000,  "Buffer size in bytes"           },
	{ "-breaker_sequence",      breaker_sequence,     "BREAKER_SEQUENCE",      PaString, PaOpt, _i "\n"         , PaNL, PaNL,       "Breaker sequence ( by default \\n )"       },
	{ "-lines",                 &lines,               "LINES",                 PaBool,    PaOpt,  false, false,  true,  "Read std-in line by line"   },
    { " ",                      queue_name,           "QUEUE",                 PaString,  PaReq,  (long) "null",   PaNL,   PaNL,  "name of the queue to push data"         },
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
    paConfig("man shortdescription",          (void*) manShortDescription);
    paConfig("man synopsis",          (void*) manSynopsis);
    
    // Parse input arguments    
    paParse(paArgs, argC, (char**) argV, 1, false);
    logFd = lmFirstDiskFileDescriptor();


	// Check queue is specified
	if( strcmp( queue_name , "null") == 0 )
	   LM_X(1,("Please, specify a queue to push data to"));
    
    // Instance of the client to connect to SAMSON system
    samson::SamsonClient client;
    
    // Set 1G RAM for uploading content
    client.setMemory( 1024*1024*1024 );
    
    if( paVerbose )
        LM_M(("Connecting to %s ..." , controller));
    
    // Init connection
    if( !client.init( controller ) )
    {
        fprintf(stderr, "Error connecting with samson cluster: %s\n" , client.getErrorMessage().c_str() );
        exit(0);
    }

    // Create the push buffer to send data to a queue in buffer-mode
    samson::SamsonPushBuffer *pushBuffer = new samson::SamsonPushBuffer( &client , queue_name , timeOut );

    if( paVerbose )
        LM_M(("Conection to %s OK" , controller));

    
    // Read data in blocks, lock the separator backward
    // --------------------------------------------------------------------------------
    
    if( buffer_size == 0)
        LM_X(1,("Wrong buffer size %lu", buffer_size ));
    
    char *data = (char*) malloc ( buffer_size );
    if( !data )
        LM_X(1,("Error allocating %lu bytes" , buffer_size ));
    

    size_t size = 0;                // Bytes currently contained in the buffer
    size_t total_process = 0;    
    

    std::string tmp_separator = breaker_sequence;
    literal_string( tmp_separator );

    LM_V(("Setup buffer_size %s / timeout %s / break_sequence '%s' ( length %d ) " , au::str(buffer_size).c_str() , au::time_string( timeOut ).c_str() , tmp_separator.c_str() , strlen(breaker_sequence) ));

    
    while( true )
    {
        
        size_t read_bytes = 0;
        if( lines ) 
        {
            char * string = fgets(data , buffer_size, stdin );

            if( string ) 
                read_bytes = strlen( string );
            else
                read_bytes = 0;
            
        }
        else
        {
            read_bytes = full_read( 0 , data + size , buffer_size - size );
        }

        
        //LM_M(("Read command for %lu bytes. Read %lu" , buffer_size - size , read_bytes ));
        
        if( read_bytes == 0 )
            break;

        // Increase the size of data contained in the buffer
        size += read_bytes;

        
        // Processing data contained in "data" with size "size"
        // -----------------------------------------------------------------
        
        // Find backward a particular string...
        const char * last_pos = au::laststrstr( data , size , breaker_sequence );
        
        if( !last_pos )
            LM_X(1,("Not found breaker sequecny '%s' in a buffer of %lu bytes" , breaker_sequence , size ));
        
        size_t output_size = last_pos - data + strlen( breaker_sequence );
        
        //LM_M(("Processing buffer with %s --> %s block push to queue" , au::str(size).c_str() , au::str(output_size).c_str() ));
        
        // Emit this block of data
        pushBuffer->push(data, output_size );

        // -----------------------------------------------------------------

        
        // Total accumulation of data
        total_process +=  output_size;
        

        // Move rest of data to the begining of the buffer
        if( output_size < size )
        {
            memmove( data , data + output_size , size - output_size );
            size = size-output_size;
        }
        else
            size = 0;
        
    }
    
    // Last push
    pushBuffer->flush();

    if( paVerbose ) 
        LM_M(("Total process %s" , au::str(total_process,"B").c_str() ));
    
    
    // --------------------------------------------------------------------------------
    
    
    /*
    char line[1024];
    while( fgets(line, 1024, stdin) )
    {
        size_t line_length  = strlen(line) + 1;
        pushBuffer->push(line, line_length );
    }

    // Flush content of the buffer
    pushBuffer->flush();
    */
    
    
    
    // Wait until all operations are complete
    if( paVerbose ) 
        LM_M(("Waiting for all the push operations to complete..."));
    
    client.waitUntilFinish();
    
	
}
