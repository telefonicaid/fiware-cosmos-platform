#ifndef _H_SAMSON_CONNECTOR_BLOCK_PROCESSOR
#define _H_SAMSON_CONNECTOR_BLOCK_PROCESSOR

#include "au/Cronometer.h"
#include "engine/Buffer.h"
#include "engine/BufferContainer.h"
#include "samson/module/Operation.h"

namespace samson 
{

    class SamsonConnector;
    class SamsonConnectorItem;
    
    
    class BufferProcessor : public SplitterEmitter 
    {
        
        std::string name;

        SamsonConnectorItem* item;
        SamsonConnector * stream_connector;
        Splitter * splitter;
        std::string splitter_name;
        
        char *buffer;
        size_t max_size;
        size_t size;

        // Cronometer to indicate time since last process
        au::Cronometer cronometer;

        // Buffer used to emit output produced by the splitter
        engine::BufferContainer output_buffer_container;
        
    public:
        
        BufferProcessor( SamsonConnectorItem* item , SamsonConnector * _stream_connector );
        ~BufferProcessor();
        
        void push( engine::Buffer * bufer );
        void flush( );

        // SplitterEmitter interface
        void emit( char* data , size_t length );
        
        // Flush accumulated buffer at the output of splitter
        void flushOutputBuffer();
        
    private:
    
        void process_intenal_buffer( bool finish );
        
        
        
    };

}

#endif
