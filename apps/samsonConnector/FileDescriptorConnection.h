#ifndef _H_SAMSON_CONNECTOR_CONNECTION_FileDescriptorConnection
#define _H_SAMSON_CONNECTOR_CONNECTION_FileDescriptorConnection

#include "au/mutex/TokenTaker.h"

#include "au/network/NetworkListener.h"
#include "au/network/SocketConnection.h"
#include "samson/client/SamsonClient.h"
#include "samson/client/SamsonPushBuffer.h"


#include "Connection.h"
#include "common.h"

extern size_t buffer_size;

namespace samson {
    namespace connector {
    
        class Item;
        
        /*
         Simple FileDescriptor connection ( used in ListenerItem or ConnectionItem )
         */
        
        class FileDescriptorConnection : public Connection
        {
            
        private:
            
            au::FileDescriptor * file_descriptor;    // File descritor to read or write
            au::Cronometer cronometer;
            
            bool thread_running;                     // Flag to indicate if thread is still running in background
            
        public:
            
            FileDescriptorConnection( Item  * _item , ConnectionType _type , std::string _name
                             , au::FileDescriptor * _file_descriptor );
            
            ~FileDescriptorConnection();
            
            // Main function of the dedicated thread
            void run();
            void run_as_input();
            void run_as_output();
            
            std::string getStatus()
            {
                if( thread_running )
                    return au::str( "Connected %s" , cronometer.str().c_str() );
                else
                    return "Not connected";
            }
            
            // Can be removed ( no background threads and so... )
            bool canBeRemoved()
            {
                return !thread_running;
            }

            // Review
            void review_connection()
            {
                // Close connection
                if( isRemoving() )
                    file_descriptor->close();
            }
            
        };
    }
}


#endif
