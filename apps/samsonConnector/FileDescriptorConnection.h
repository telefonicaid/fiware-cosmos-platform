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
                if( file_descriptor->isDisconnected() )
                    return "Closed fd";
                else
                    return au::str( "Connected (fd %d)" , file_descriptor->getFd() );
            }

            virtual void start_connection();
            
            virtual void stop_connection()
            {
                
                log("Message", "Connection stoped");
                
                // Stop thread in the background
                file_descriptor->close();
                while( thread_running )
                    usleep(100000);
            }
            
            // Review
            virtual void review_connection()
            {
                if( file_descriptor->isDisconnected() )
                    set_as_finished();
                
                set_as_connected( !file_descriptor->isDisconnected() );

            }
            
        };
    }
}


#endif
