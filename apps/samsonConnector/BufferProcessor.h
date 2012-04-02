#ifndef _H_SAMSON_CONNECTOR_BLOCK_PROCESSOR
#define _H_SAMSON_CONNECTOR_BLOCK_PROCESSOR

#include "engine/Buffer.h"
#include "samson/module/Operation.h"

namespace samson 
{

    class SamsonConnector;
    class SamsonConnectorItem;
    
    class BufferProcessor
    {
        
        std::string name;

        SamsonConnectorItem* item;
        SamsonConnector * stream_connector;
        Splitter * splitter;
        std::string splitter_name;
        
        char *buffer;
        size_t max_size;
        size_t size;
        
    public:
        
        BufferProcessor( SamsonConnectorItem* item , SamsonConnector * _stream_connector );
        ~BufferProcessor();
        
        void push( engine::Buffer * bufer );
        void flush( );
        
    private:
    
        void process_intenal_buffer( bool finish );
        
        
        
    };

}

#endif
