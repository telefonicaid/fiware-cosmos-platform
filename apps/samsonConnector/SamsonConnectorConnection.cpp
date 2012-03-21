
#include "SamsonConnector.h"
#include "SamsonConnectorConnection.h" // Own interface


extern size_t buffer_size;

namespace samson {
    
    void* run_SamsonConnectorConnection( void* p)
    {
        SamsonConnectorConnection* connection = ( SamsonConnectorConnection* ) p;
        connection->run();
        return NULL;
    }

    
    SamsonConnectorConnection::SamsonConnectorConnection( SamsonConnector* _samson_connector
                                                         , FileDescriptor * _file_descriptor 
                                                         ,ConnectionType _type ) : token("ConnectorConnection")
    {
        samson_connector = _samson_connector;
        file_descriptor = _file_descriptor;
        type = _type;
        
        if( type == connection_input )
            block_processor = new BufferProcessor( samson_connector );
        else
            block_processor = NULL;
        
        // No current block when  starting
        current_block = NULL;
        
        // Create the thread
        thread_running = true;
        pthread_t t;
        pthread_create(&t, NULL, run_SamsonConnectorConnection, this);
    }
    
    SamsonConnectorConnection::~SamsonConnectorConnection()
    {
        if( block_processor )
            delete block_processor;
        
        if( file_descriptor )
            delete file_descriptor;
    }

    void SamsonConnectorConnection::run_as_output()
    {
        while( true )
        {
            
            // Get the next block to be sent
            {
                au::TokenTaker tt(&token);
                current_block = pending_blocks.extractFront();
            }
            
            if( current_block )
            {
                engine::Buffer* buffer = current_block->buffer;
                
                Status s = file_descriptor->partWrite(buffer->getData(), buffer->getSize(), "samsonConnectorConnection");
                
                if( s != OK )
                {
                    thread_running = false;
                    samson_connector->review();
                    samson_connector->exit_if_necessary();
                    return;
                }

                // Statistics
                output_rate.push( current_block->buffer->getSize() );
                
                // Release the block
                {
                    au::TokenTaker tt(&token);
                    //LM_V(("Releasing block after sending to stdout "));
                    current_block->release();
                    current_block = NULL;
                }
                
                // Quit if necessary here
                samson_connector->exit_if_necessary();
                
            }
            else
                usleep( 100000 );
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
            size_t read_size;
            Status s = file_descriptor->partRead(buffer->getData()
                                                 , buffer_size
                                                 , "read connector connections"
                                                 , 300 
                                                 , &read_size );

            if( read_size > 0 )
                input_rate.push( read_size );
            
/*
 LM_V(("%s: Readed buffer %s (max %s)" , file_descriptor->getName().c_str() 
                  , au::str(read_size).c_str()
                  , au::str(buffer_size).c_str() ));
*/
            
            // If we have read something...
            if( read_size > 0 )
            {
                // Set the buffer size
                buffer->setSize(read_size);

                // Push the buffer to samson_connector
                block_processor->push(buffer);
                buffer = NULL; // Avoid releasing latter
            }
            
            // If last read is not ok...
            if( s != OK )
            {
                // Deallocated the buffer
                if( buffer )
                    engine::MemoryManager::shared()->destroyBuffer(buffer);
                
                // Flush whatever we have..
                block_processor->flush();

                // Set thead running flag to false
                thread_running = false;
                
                // Review just in case we have to exit
                samson_connector->review();
                samson_connector->exit_if_necessary();
                return;
            }
            
        }

    }
    
    size_t SamsonConnectorConnection::getOutputBufferSize()
    {
        au::TokenTaker tt(&token);
        
        if( type == connection_input )
            return 0;
        
        size_t total = 0;
        
        if( current_block )
            total += current_block->buffer->getSize();

        au::list<Block>::iterator it_pending_blocks;
        for( it_pending_blocks = pending_blocks.begin() ; it_pending_blocks != pending_blocks.end() ; it_pending_blocks++ )
            total += (*it_pending_blocks)->buffer->getSize();
        
        return total;
        
        
    }

    void SamsonConnectorConnection::push( Block* block )
    {
        au::TokenTaker tt(&token);

        if( type == connection_input )
        {
            //LM_V(("Not pushing since we are an input connection"));
            return; // Just ignoring
        }
         
        //LM_V(("Block scheduled in %s" , file_descriptor->getName().c_str() ));
        
        // Retain the block
        block->retain();
        
        pending_blocks.push_back( block );
        
        
    }
    
    void SamsonConnectorConnection::run()
    {
        if( type == connection_input )
            run_as_input();
        else
            run_as_output();
        
    }
}