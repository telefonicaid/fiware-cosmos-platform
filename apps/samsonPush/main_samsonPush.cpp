
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

#include "samson/client/SamsonClient.h"         // samson::SamsonClient


#define BUFFER_SIZE 64*1024*1024

int main( int argc , char *argv[] )
{
    
    if( argc < 3 )
    {
        fprintf(stderr,"Usage %s <controller_ip> queue_name\n" , argv[0] );
        exit(0);
    }
    
    // Instance of the client to connect to SAMSON system
    samson::SamsonClient client;
    
    // Set 1G RAM for uploading content
    client.setMemory( 1024*1024*1024 );
    
    std::cerr << "Connecting to " << argv[1] <<  " ..." ;
    std::cerr.flush();
    
    
    // Init connection
    if( !client.init( argv[1] ) )
    {
        fprintf(stderr, "Error connecting with samson cluster: %s\n" , client.getErrorMessage().c_str() );
        exit(0);
    }
    
    std::cerr << "OK\n";
    
    char line[1024];
    char * buffer = (char*) malloc( BUFFER_SIZE );
    size_t size = 0;
    
    while( fgets(line, 1024, stdin) )
    {
        size_t line_length  = strlen(line) + 1;
        //fprintf(stderr , "Read %s -> %d bytes" , line , (int) strlen( line ) );
        
        if( (size + line_length ) > BUFFER_SIZE )
        {
            // Process buffer
            std::cerr << "Flushing " << size << " bytes to queue " << argv[2] <<  "\n";
            client.push(  argv[2] , buffer, size );
            
            // Process buffer
            size = 0;
        }
        
        memcpy( buffer + size, line , line_length );
        size+= line_length;
        
    }
    
    // Process last buffer
    if( size > 0 )
    {
        std::cerr << "Flushing " << size << " bytes to queue " << argv[2] <<  "\n";
        client.push(  argv[2] , buffer, size );
    }
    
    // Wait until all operations are complete
    std::cerr << "Waiting for all the push operations to complete...";
    std::cerr.flush();
    
    client.waitUntilFinish();
    
    std::cerr << "OK\n";

    std::cerr << "Finish correctly\n";
    
	
}
