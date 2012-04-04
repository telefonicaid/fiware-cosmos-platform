#ifndef _H_SAMSON_CONNECTOR_DISK_CONNECTION
#define _H_SAMSON_CONNECTOR_DISK_CONNECTION

#include <string>

#include "au/Token.h"
#include "au/ErrorManager.h"
#include "au/TokenTaker.h"

#include "common.h" 
#include "BufferProcessor.h"

namespace samson 
{
    
    class SamsonConnectorConnection;
    class SamsonConnector;
    
    
    class DiskConnection : public SamsonConnectorItem
    {
        
        std::string file_name;               // Name of a file or directory
        std::list<std::string> files;        // Files to open ( working as input )
        au::Token token;                     // Mutex protection

        // File descriptor used to write or read
        au::FileDescriptor * file_descriptor;

        au::ErrorManager error;              // Error
        
    public:
        
        bool thread_running;
        
    public:
        
        // Constructor & Destructor
        DiskConnection( SamsonConnector* _samson_connector , ConnectionType _type , std::string _file_name );
        ~DiskConnection();

        // Can be removed
        bool canBeRemoved();
        
        
        // Method called every 5 seconds to re-connect or whatever is necessary here...
        void review()
        {
            // Nothing to review here
        }
        
        // Get informatio
        std::string getName();

        // Check if this disk connection is over
        bool isConnected();
        
        // Main method to run from background thread
        void run();
        
        // Status of this connection
        std::string getStatus();
        
    private:
        
        void run_as_input();
        void run_as_output();

        
    };
    
    
}


#endif