#ifndef _H_SAMSON_CONNECTOR_CONNECTION_FileDescriptorConnection
#define _H_SAMSON_CONNECTOR_CONNECTION_FileDescriptorConnection

#include "au/mutex/TokenTaker.h"
#include "au/Cronometer.h"

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
         Simple FileDescriptor connection ( used in ListenerAdaptor or ConnectionItem )
         */
        
        class FileDescriptorConnection : public Connection
        {
            
            au::FileDescriptor * file_descriptor_;    // Current file descritor to read or write
            bool thread_running_;                     // Flag to indicate if thread is still running in background
            

            int num_connections_;                     // Number of connections
            au::Cronometer cronometer_reconnection_;  // Cronometer since last reconnection

            size_t input_buffer_size;                 // Variable length input buffer
            
        public:
            
            FileDescriptorConnection( Item  * _item , ConnectionType _type , std::string _name );
            ~FileDescriptorConnection();
            
            // Get the File descriptor
            virtual au::FileDescriptor * getFileDescriptor()=0;
            
            // Main function of the dedicated thread
            void run();
            void run_as_input();
            void run_as_output();
            
            // Connection virtual methods
            virtual void start_connection();
            virtual void review_connection();
            virtual void stop_connection();
            virtual std::string getStatus();
            
        private:
            
            void connect();
            
        };
        
        
        class SimpleFileDescriptorConnection : public FileDescriptorConnection
        { 
            au::FileDescriptor * file_descriptor_;
            
        public:
            
            SimpleFileDescriptorConnection( Item  * _item , ConnectionType _type , std::string _name , au::FileDescriptor * file_descriptor ) :
            FileDescriptorConnection( _item , _type , _name )
            {
                // Keep the file descriptor
                file_descriptor_ = file_descriptor;
            }
            
            virtual au::FileDescriptor * getFileDescriptor()
            {

                // Mark component as finished
                if( file_descriptor_ == NULL )
                {
                    set_as_finished(); // It is over
                    return NULL;
                }
                
                // Return the provided file descriptor just once.
                au::FileDescriptor * return_file_descriptor = file_descriptor_;
                file_descriptor_ = NULL;
                return return_file_descriptor;
            }
            
            
        };
        
    }
}


#endif
