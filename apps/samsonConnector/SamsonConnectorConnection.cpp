
#include "au/ThreadManager.h"

#include "SamsonConnector.h"
#include "BufferProcessor.h"
#include "SamsonConnectorConnection.h" // Own interface


extern size_t buffer_size;

namespace samson {
    
    void* run_SamsonConnectorConnection( void* p )
    {
        SamsonConnectorConnection* connection = ( SamsonConnectorConnection* ) p;
        
        // Keep samson connector pointer
        SamsonConnector * samson_connector = NULL;
        samson_connector = connection->getSamsonConnector();

        // Main run
        connection->run();
        LM_V(("Connection finish: %s" , connection->str().c_str() ));
        connection->thread_running = false;
        
        // Review to exit if it is necessary
        samson_connector->review();
        
        return NULL;

    }

    
    SamsonConnectorConnection::SamsonConnectorConnection( SamsonConnector* samson_connector
                                                         , ConnectionType type 
                                                         , std::string _name 
                                                         , au::FileDescriptor * _file_descriptor )
    : SamsonConnectorItem( samson_connector , type )
    {
        name = _name;
        file_descriptor = _file_descriptor;
        
        // Create the thread
        thread_running = true;
        pthread_t t;
        au::ThreadManager::shared()->addThread("SamsonConnectorConnection",&t, NULL, run_SamsonConnectorConnection, this);
    }
    
    SamsonConnectorConnection::~SamsonConnectorConnection()
    {
        if( file_descriptor )
            delete file_descriptor;
    }

    void SamsonConnectorConnection::run_as_output()
    {
        while( true )
        {
            Block* current_block = getNextOutputBlock();
            if( current_block )
            {
                engine::Buffer* buffer = current_block->buffer;
                au::Status s = file_descriptor->partWrite(buffer->getData(), buffer->getSize(), "samsonConnectorConnection");
                
                
                if( s != au::OK )
                    return; // Just quit
                else
                    popOutputBlock(); // Pop the block we have just sent

            }
            else
            {
                // Review samson connector to exit if necessary
                samson_connector->review();
                usleep( 100000 );
            }
        }        
    }

    void SamsonConnectorConnection::run_as_input()
    {
        // Read from stdin and push blocks to the samson_connector
        while( true )
        {
            //Get a buffer
            engine::Buffer * buffer = engine::MemoryManager::shared()->newBuffer("stdin", "connector", buffer_size );
            
            //LM_V(("%s: Reading buffer up to %s" , file_descriptor->getName().c_str() , au::str(buffer_size).c_str() ));

            // Read the entire buffer
            size_t read_size = 0;
            au::Status s = file_descriptor->partRead(buffer->getData()
                                                 , buffer_size
                                                 , "read connector connections"
                                                 , 300 
                                                 , &read_size );
            
            // If we have read something...
            if( read_size > 0 )
            {
                // Push input buffer
                buffer->setSize(read_size);
                pushInputBuffer( buffer );
                buffer = NULL; // Avoid releasing latter
            }
            
            // If last read is not ok...
            if( s != au::OK )
            {
                // Deallocated the buffer
                if( buffer )
                    engine::MemoryManager::shared()->destroyBuffer(buffer);
                
                // Flush whatever we have..
                flush();

                // Quit main threads
                return;
            }
        }
    }
    
    void SamsonConnectorConnection::run()
    {
        if( type == connection_input )
            run_as_input();
        else
            run_as_output();
        
    }
}