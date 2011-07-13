
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

namespace  samson {
    
    class SamsonClient
    {
            
        size_t memory;              // Memory used internally for load / download operations
        
        size_t load_buffer_size;    // Size of the load buffer
        
        std::string error_message;  // Error message if a particular operation fails
        
	public:
        
        // Default constructor
        SamsonClient();

        // Set memory ( only useful before init )
        void setMemory ( size_t _memory );
        
        // Init the connection with a SAMSON cluster
        bool init( std::string controller );
        
    };
    
}
#endif
