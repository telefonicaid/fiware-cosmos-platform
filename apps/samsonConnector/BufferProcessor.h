#ifndef _H_SAMSON_CONNECTOR_BLOCK_PROCESSOR
#define _H_SAMSON_CONNECTOR_BLOCK_PROCESSOR

#include "engine/Buffer.h"

namespace samson 
{

    class SamsonConnector;
    
    
    class BufferProcessor
    {
        
        SamsonConnector * stream_connector;
        
    public:
        
        BufferProcessor( SamsonConnector * _stream_connector );
        
        void push( engine::Buffer * bufer );
        void flush();
        
        
    };

}

#endif