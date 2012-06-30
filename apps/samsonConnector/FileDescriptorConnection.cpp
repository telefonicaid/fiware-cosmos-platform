
#include "au/ThreadManager.h"

#include "FileDescriptorConnection.h"
#include "StreamConnector.h"
#include "BufferProcessor.h"


extern size_t buffer_size;
extern size_t input_buffer_size; // Size of the chunks to read

namespace samson {
    namespace connector {
        
        void* run_FileDescriptorConnection( void* p )
        {
            // Recover the correct pointer
            FileDescriptorConnection* connection = ( FileDescriptorConnection* ) p;
            
            // Main run
            connection->run();
                        
            return NULL;
            
        }
        
        FileDescriptorConnection::FileDescriptorConnection( Item  * _item , ConnectionType _type , std::string _name )
        : Connection( _item , _type , _name )
        {
            // Keep pointer to file descriptor
            file_descriptor_ = NULL;
         
            // By default, thread is not running
            thread_running_ =  false;
            
            // Init counter of connections
            num_connections_ = 0;
            
            // Default input buffer size
            input_buffer_size = 1024;
            
        }
        
        void FileDescriptorConnection::start_connection()
        {
            connect(); // Get a file descriptor and create background thread
        }

        void FileDescriptorConnection::stop_connection()
        {
            log("Message", "Connection stoped");
            
            // Stop thread in the background
            if ( file_descriptor_ )
                file_descriptor_->close();
            
            while( thread_running_ )
                usleep(100000);
        }
        
        void FileDescriptorConnection::connect()
        {
            if( thread_running_ || file_descriptor_ )
                return;
            
            // Update the counter of connections
            num_connections_++;
            cronometer_reconnection_.reset(); // Start cronometer to do not restart inmediatelly
            
            // Get file descriptor and start background thread
            file_descriptor_ = getFileDescriptor();
            if( file_descriptor_ )
            {
                // Create the thread
                thread_running_ = true;
                pthread_t t;
                au::ThreadManager::shared()->addThread("SamsonConnectorConnection",&t, NULL, run_FileDescriptorConnection, this);
            }
        }
        
        
        FileDescriptorConnection::~FileDescriptorConnection()
        {
            if( file_descriptor_ )
            {
                file_descriptor_->close();
                delete file_descriptor_;
            }
        }
        
        void FileDescriptorConnection::review_connection()
        {
            if( !file_descriptor_ )
            {
                set_as_connected(false);
                
                if( cronometer_reconnection_.diffTime() < 3 )
                    return ; // Wait a little bit
                
                // Reconnect if possible
                connect();
                
                return; 
            }
            
            // If we are disconnected, wait until thread finish and delte it to reconnect
            if( file_descriptor_->isDisconnected() )
            {
                set_as_connected(false);
                if( thread_running_ ) // Still waiting for the threads to finish
                    return;
                delete file_descriptor_;
                file_descriptor_ = NULL;
                return;
            }
            
            // Set connection based on file descriptor
            set_as_connected( !file_descriptor_->isDisconnected() );
            
        }

        
        void FileDescriptorConnection::run_as_output()
        {
            while( true )
            {
                
                if( !file_descriptor_ )
                    LM_X(1, ("Internal error"));
                
                // Container to keep a retained version of buffer
                engine::BufferContainer container;
                getNextBufferToSent(&container);
                
                engine::Buffer* buffer = container.getBuffer();
                if( buffer )
                {
                    au::Status s = file_descriptor_->partWrite(buffer->getData(), buffer->getSize(), "samsonConnectorConnection");
                    
                    if( s != au::OK )
                        return; // Just quit
                }
                else
                {
                    // Sleep a little bit before checking again
                    usleep( 100000 );
                }
            }        
        }
        
        void FileDescriptorConnection::run_as_input()
        {
            
            // Read from stdin and push blocks to the samson_connector
            while( true )
            {
                if( !file_descriptor_ )
                    LM_X(1, ("Internal error"));
                
                //Get a buffer
                engine::Buffer * buffer = engine::MemoryManager::shared()->createBuffer("stdin"
                                                                                        , "connector"
                                                                                        , input_buffer_size );
                
                
                size_t read_size = 0;
                au::Status s;
                {
                    au::Cronometer c;
                    s = file_descriptor_->partRead(buffer->getData()
                                                   , input_buffer_size
                                                   , "read connector connections"
                                                   , 300 
                                                   , &read_size );
                    if( c.diffTime() < 0.1 )
                        input_buffer_size *= 2;
                    else if( c.diffTime() > 3 )
                        input_buffer_size /= 2;
                }
                
                // If we have read something...
                if( read_size > 0 )
                {
                    // Push input buffer
                    buffer->setSize(read_size);
                    pushInputBuffer( buffer );
                }
                
                // Relase allocated buffer
                buffer->release();
                
                // If last read is not ok...
                if( s != au::OK )
                {
                    // Log activity
                    log("Message", au::str("Connection finished (%s) " , au::status(s) ));
                    
                    // Close fd
                    file_descriptor_->close();
                    
                    // Flush whatever we have..
                    flushInputBuffers();
                    
                    // Quit main threads
                    return;
                }
            }
        }
        
        void FileDescriptorConnection::run()
        {
            if( getType() == connection_input )
                run_as_input();
            else
                run_as_output();
            
            // Mark as non thread_running
            thread_running_ = false;
            
        }
        
        std::string FileDescriptorConnection::getStatus()
        {
            std::ostringstream output;
            
            if( !file_descriptor_ )
                output <<  "Not connected";
            else
                if( file_descriptor_->isDisconnected() )
                    output << "Closing connection";
                else
                    output << "Connected";
            
            output << au::str( " [fd:%d]" , file_descriptor_->getFd() );
            
            if( getType() == connection_input )
                output << au::str( " [buffer:%s]"  , au::str( input_buffer_size ).c_str() );
            
            return output.str();
        }

    }
}