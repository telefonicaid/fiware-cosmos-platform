
/* ****************************************************************************
 *
 * FILE            SamsonClient.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         Samson
 *
 * DATE            7/14/11
 *
 * DESCRIPTION
 *
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_Samson_SamsonClient
#define _H_Samson_SamsonClient

#include <string>       // std::string
#include <vector>       // std::vector

namespace  samson {
    
    class SamsonClient;
    
    class SamsonPushBuffer
    {
        
        SamsonClient *client;
        std::string queue;
        
        char *buffer;
        size_t max_buffer_size;
        
        size_t size;    // Current size of the buffer
        
    public:
        SamsonPushBuffer( SamsonClient *client , std::string queue );
        ~SamsonPushBuffer();
        void push( const char *data , size_t length );
    
        void flush();
    };
    
    class SamsonClient
    {
            
        size_t memory;              // Memory used internally for load / download operations
        
        size_t load_buffer_size;    // Size of the load buffer
        
        std::string error_message;  // Error message if a particular operation fails
        
        
        std::vector<size_t> delilah_ids;
        
	public:
        
        // Default constructor
        SamsonClient();

        // Set memory ( only useful before init )
        void setMemory ( size_t _memory );
        
        // Init the connection with a SAMSON cluster
        bool init( std::string controller );
        
        // Push content to a particular queue
        size_t push( std::string queue , char *data , size_t length );
        
        // Get error message
        std::string getErrorMessage();
        
        // Wait until all operations are finished
        void waitUntilFinish();
        
    };
    
}
#endif
