


#include "Block.h"

#include "SamsonConnector.h"
#include "SamsonConnection.h" // Own interface


namespace samson {

    SamsonConnection::SamsonConnection( SamsonConnector * samson_connector ,  ConnectionType type , std::string _host , int _port , std::string _queue )
    : SamsonConnectorItem( samson_connector , type ) , SamsonClient( "connector" ) , token( "SamsonConnection" )
    {
        host = _host;
        port = _port;
        queue = _queue;
        
        // Init SamsonClien connection
        connected = initConnection( host , port );
        
        if( type == connection_input )
            connect_to_queue(queue, false, false); // No possible flags new of clear here
    }
    
    std::string SamsonConnection::getName()
    {
        if( type == connection_input )
            return au::str("Samson at %s:%d from queue %s" , host.c_str() , port , queue.c_str());
        else
            return au::str("Samson at %s:%d to %s" , host.c_str() , port , queue.c_str() );
    }
    
    void SamsonConnection::push( Block* block )
    {
        if( !connected )
            connected = initConnection( host , port );

        if( !connected )
            return;
        
        if( type == connection_input )
            return; // Nothing to do if we are input
        
        SamsonClient::push(queue, new BlockDataSource(block) );
        
    }

    size_t SamsonConnection::getOuputBufferSize()
    {
        if( areAllOperationsFinished() )
            return 0;
        else
            return 1;
    }

    
    void SamsonConnection::receive_buffer_from_queue(std::string queue , engine::Buffer* buffer)
    {
        // Transformation of buffer
        KVHeader *header = (KVHeader*) buffer->getData();

        if( header->isTxt() )
        {
            engine::Buffer * new_buffer = engine::MemoryManager::shared()->newBuffer("output_samson_connector", "connector",  header->info.size );
            memcpy(new_buffer->getData(), buffer->getData() + sizeof(KVHeader), header->info.size);
            new_buffer->setSize( header->info.size );
            
            // Remove previous buffer
            engine::MemoryManager::shared()->destroyBuffer( buffer );
            
            // Push the new buffer
            pushInputBuffer( new_buffer );
            
        }
        else
        {
            std::string message = au::str("Received a binary buffer %s from %s. Still not implemented how to process this" , au::str( buffer->getSize() , "B" ).c_str() , getName().c_str() );
            samson_connector->show_message( message );
            engine::MemoryManager::shared()->destroyBuffer( buffer );
            
        }
        
        
        
    }

}