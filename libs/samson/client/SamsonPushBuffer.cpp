

#include "au/mutex/TokenTaker.h"
#include "samson/common/coding.h"

#include "samson/client/SamsonClient.h"

#include "SamsonPushBuffer.h" // Own interface


namespace samson {

    
    SamsonPushBuffer::SamsonPushBuffer( SamsonClient *_client , std::string _queue  ) : token("SamsonPushBuffer")
    {
        // Client and queue name to push data to
        client = _client;
        queue = _queue;
        
        
        // Init the simple buffer
        max_buffer_size = 64*1024*1024 - sizeof(KVHeader) ; // Perfect size for this ;)
        buffer = (char*) malloc( max_buffer_size );
        size = 0;
        
        
    }
    
    SamsonPushBuffer::~SamsonPushBuffer()
    {
        if( buffer )
            free(buffer);
    }
    
    void SamsonPushBuffer::push( const char *data , size_t length , bool flushing )
    {
        // Mutex protection
        au::TokenTaker tt(&token);
        
        // Statistics
        rate.push( length );
        
        if( (size + length ) > max_buffer_size )
        {
            // Push to the client
            client->push(  queue , buffer, size );
            
            // Come back to "0"
            size = 0;
        }
        
        // Copy new content to the upload buffer
        {
            // Acumulate contents
            memcpy(buffer+size, data, length);
            size += length;
            
        }
        
        if( flushing )
            _flush();
    }
    
    void SamsonPushBuffer::flush()
    {
        au::TokenTaker tt(&token);
        _flush();
    }
    
    void SamsonPushBuffer::_flush()
    {
        if ( size > 0 )
        {
            // Process buffer
            LM_V(("SamsonPushBuffer: Pushing %s to queue %s\n" , au::str(size,"B").c_str() , queue.c_str()));
            client->push(  queue , buffer, size );
            
            // Come back to "0"
            size = 0;
            
        }
    }
    

}