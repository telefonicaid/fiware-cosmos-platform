
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
#include <pthread.h>        /* pthread*/

#include "au/Cronometer.h"          // au::Cro
#include "au/Token.h"               // au::Token
#include "au/TokenTaker.h"          // au::TokenTaker

#include "engine/Object.h"          // engine::Object

namespace  samson {
    
    class SamsonClient;
    
    /*
     Class used to push data to a txt queue in buffer mode
     */
     
    class SamsonPushBuffer : public engine::Object
    {
        
        SamsonClient *client;
        std::string queue;
        
        char *buffer;
        size_t max_buffer_size;
        
        size_t size;    // Current size of the buffer
        int timeOut;    // Timeout in seconds to flush content to the queue

        au::Cronometer cronometer; // Time since the last flush
        
        au::Token token;

    
    public:
        SamsonPushBuffer( SamsonClient *client , std::string queue , int timeOut );
        ~SamsonPushBuffer();

        void push( const char *data , size_t length );
        void flush();

        // Recevie notifications
        void notify( engine::Notification* notification );
        
    private:
        
        void _flush();      // No token protected flush operation

        
    };
    
    /*
        Main class to connect to a samson cluster
     */
    
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
