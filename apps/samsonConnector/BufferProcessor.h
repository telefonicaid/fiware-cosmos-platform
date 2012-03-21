#ifndef _H_SAMSON_CONNECTOR_BLOCK_PROCESSOR
#define _H_SAMSON_CONNECTOR_BLOCK_PROCESSOR

#include "engine/Buffer.h"
#include "samson/module/Operation.h"

namespace samson 
{

    class SamsonConnector;
    
    
    class BufferProcessor
    {
        
        SamsonConnector * stream_connector;
        Splitter * splitter;
        
        char *buffer;
        size_t max_size;
        size_t size;
        
    public:
        
        BufferProcessor( SamsonConnector * _stream_connector );
        ~BufferProcessor();
        
        void push( engine::Buffer * bufer );
        void flush( );
        
    private:
    
        void process_intenal_buffer( bool finish );
        
        
        
    };

}

#endif
