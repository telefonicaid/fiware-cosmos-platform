#ifndef _H_SAMSON_CONNECTOR_DISK_CONNECTION
#define _H_SAMSON_CONNECTOR_DISK_CONNECTION

#include <string>

#include "au/mutex/Token.h"
#include "au/ErrorManager.h"
#include "au/mutex/TokenTaker.h"

#include "common.h" 
#include "Item.h" 
#include "Connection.h" 
#include "BufferProcessor.h"

namespace samson 
{
    namespace connector
    {
    
        class SamsonConnectorConnection;
        class SamsonConnector;
        
        
        class DiskConnection : public Connection
        {
            
            std::string file_name;               // Name of a file or directory
            std::list<std::string> files;        // Files to open ( working as input )
            au::Token token;                     // Mutex protection
            
            // File descriptor used to write or read
            au::FileDescriptor * file_descriptor;
            
            au::ErrorManager error;              // Error
            
        public:
            
            bool thread_running;
            bool stoping_threads; // Flag to stop the background thread
            
        public:
            
            // Constructor & Destructor
            DiskConnection( Item  * _item , ConnectionType _type , std::string directory );
            ~DiskConnection();
            
            // Connection methods
            virtual void start_connection();
            virtual void stop_connection();
            virtual void review_connection();
            
            // Main method to run from background thread
            void run();             


            // Status of this connection
            std::string getStatus();
            
        private:
            
            void run_as_input();
            void run_as_output();
            
        };
        
        
        class DiskItem : public Item
        {
            // Information to stablish the connection with the SAMSON system
            std::string directory;
            
        public:
            
            DiskItem( Channel * _channel , ConnectionType _type ,  std::string _directory ) : 
            Item( _channel , _type , au::str("FILE(%s)" , _directory.c_str() ) ) 
            {
                // Information for connection
                directory = _directory;

                // Add internal connection just once...
                // Add connection
                add( new DiskConnection( this , getType() , directory ) );
                
            }

            
            bool canBeRemoved()
            {
                return false; // Still not considered how to cancel this connection
            }
            
            // Get status of this element
            std::string getStatus()
            {
                return "Disk connection";
            }
            
            
            
        };
        
    }
}


#endif