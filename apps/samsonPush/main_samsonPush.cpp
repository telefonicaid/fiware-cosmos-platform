
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

#include "au/CommandLine.h"      // au::CommandLine

#include "samson/client/SamsonClient.h"         // samson::SamsonClient


#define BUFFER_SIZE 64*1024*1024

int main( int argc , const char *argv[] )
{
    
    au::CommandLine cmd;
    cmd.set_flag_int( "timeOut", 0 );       // 0 value for timeOut means no timeout
    cmd.set_flag_string("controller", "localhost");
    cmd.parse( argc , argv );

    if ( cmd.get_num_arguments() < 2 )
    {
        fprintf(stderr,"Usage %s queue_name [ -controller controller_ip ] [-timeOut time_in_seconds] \n" , argv[0] );
        exit(0);
    }
    
    std::string queue_name = cmd.get_argument(1);
    std::string controller = cmd.get_flag_string("controller");
    
    int timeOut = cmd.get_flag_int("timeOut");
    
    // Instance of the client to connect to SAMSON system
    samson::SamsonClient client;
    
    // Set 1G RAM for uploading content
    client.setMemory( 1024*1024*1024 );
    
    std::cerr << "Connecting to " << controller <<  " ..." ;
    std::cerr.flush();
    
    
    // Init connection
    if( !client.init( controller ) )
    {
        fprintf(stderr, "Error connecting with samson cluster: %s\n" , client.getErrorMessage().c_str() );
        exit(0);
    }

    // Create the push buffer to send data to a queue in buffer-mode
    samson::SamsonPushBuffer *pushBuffer = new samson::SamsonPushBuffer( &client , queue_name , timeOut );
    
    std::cerr << "OK\n";
    
    char line[1024];
    
    while( fgets(line, 1024, stdin) )
    {
        size_t line_length  = strlen(line) + 1;
        //fprintf(stderr , "Read %s -> %d bytes" , line , (int) strlen( line ) );

        pushBuffer->push(line, line_length );
        
    }

    // Flush content of the buffer
    pushBuffer->flush();
    
    // Wait until all operations are complete
    std::cerr << "Waiting for all the push operations to complete...";
    std::cerr.flush();
    
    client.waitUntilFinish();
    
    std::cerr << "OK\n";

    std::cerr << "Finish correctly\n";
    
	
}
