
/*
 
 samsonPush
 
 Example app using samsonClient lib
 It listen std input and push content to the samson cluster indicated by parameters
 
 AUTHOR: Andreu Urruela
 
 */


#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // memcpy

#include "samson/client/SamsonClient.h"         // samson::SamsonClient


#define BUFFER_SIZE 64*1024*1024

int main( int argc , char *argv[] )
{
    
    if( argc < 3 )
    {
        fprintf(stderr,"Usage %s <controller_ip> queue_name\n" , argv[0] );
        exit(0);
    }
    
    
    samson::SamsonClient client;
    
    // Set 1G RAM for uploading content
    client.setMemory( 1024*1024*1024 );

    fprintf(stderr , "Connecting to %s...\n", argv[1] );
    
    // Init connection
    if( !client.init( argv[1] ) )
    {
        fprintf(stderr, "Error connecting with samson cluster: %s\n" , client.getErrorMessage().c_str() );
        exit(0);
    }

    // Read from the stdin and push to que selected queue
    fprintf(stderr , "Connected to %s\n", argv[1] );

    char line[1024];
    char * buffer = (char*) malloc(  BUFFER_SIZE );
    size_t size = 0;
    
    while( fgets(line, 1024, stdin) )
    {
        size_t line_length  = strlen(line);
        
        if( (size + line_length ) > BUFFER_SIZE )
        {
            // Process buffer
		    fprintf(stderr , "Flushing %lu bytes to queue %s\n", (unsigned long int) size , argv[2]);
            client.push(  argv[2]  , 0 , buffer, size );
            
            // Process buffer
            size = 0;
        }
    
        memcpy(buffer + size, line, line_length);
        size+= line_length;
        
    }
    
    // Process last buffer
    fprintf(stderr , "Flushing %lu bytes to queue %s\n", (unsigned long int) size , argv[2] );
    client.push(  argv[2]  , 0 , buffer, size );
    
    // Wait until all operations are complete
    client.waitUntilFinish();
    
	
}
