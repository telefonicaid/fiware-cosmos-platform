#ifndef _H_SAMSON_CONNECTOR_CONNECTION
#define _H_SAMSON_CONNECTOR_CONNECTION

#include "au/mutex/TokenTaker.h"

#include "au/network/NetworkListener.h"
#include "au/network/SocketConnection.h"
#include "samson/client/SamsonClient.h"
#include "samson/client/SamsonPushBuffer.h"

#include "Block.h"

#include "common.h"

extern size_t buffer_size;

namespace samson{
    
    // Function to run the background thread
    void* run_SamsonConnectorConnection( void* p);
    
    class SamsonConnector;
    class BufferProcessor;
    
    class SamsonConnectorConnection : public SamsonConnectorItem
    {
        
    private:
        
        std::string name;
        au::FileDescriptor * file_descriptor;    // File descritor to read or write
        au::Cronometer cronometer;

    public:
        
        bool thread_running;                 // Flag to indicate if thread is still running in background
        
    public:
    
        SamsonConnectorConnection( SamsonConnector* _samson_connector
                                  , ConnectionType _type 
                                  , std::string _name 
                                  , au::FileDescriptor * _file_descriptor );
        
        ~SamsonConnectorConnection();
        
        // Main function of the dedicated thread
        void run();
        void run_as_input();
        void run_as_output();
        
        std::string getName()
        {
            return name;
        }
        
        std::string getStatus()
        {
            if( thread_running )
                return au::str( "connected %s" , cronometer.str().c_str() );
            else
                return "not connected";
        }
        
        // Can be removed ( no background threads and so... )
        bool canBeRemoved()
        {
            if( thread_running )
                return false;
        
            // Can be retained some time....
            return true;
        }
        
        bool isConnected()
        {
            return thread_running;
        }
        
        void review()
        {
            return; // Nothing to review here
        }
        
        
    };
    
}


#endif