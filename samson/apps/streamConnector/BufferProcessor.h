#ifndef _H_SAMSON_CONNECTOR_BLOCK_PROCESSOR
#define _H_SAMSON_CONNECTOR_BLOCK_PROCESSOR

#include "au/Cronometer.h"
#include "engine/Buffer.h"

#include "engine/BufferContainer.h"
#include "engine/ProcessItem.h"

#include "samson/module/Operation.h"
#include "BufferList.h"

namespace stream_connector {
    
    class StreamConnector;
    class Channel;
    
    /*
     
     Class BufferProcessor
     
     Element in charge of getting all the blocks from a particular input inside a channel
     and emit output blocks
     
     */
    
    class BufferProcessor : public samson::SplitterEmitter 
    {
        
        Channel* channel;                // Item to send data
        samson::Splitter * splitter;             // Splitter instance
        std::string splitter_name;       // Splitter name
        
        // Cronometer to indicate time since last process
        au::Cronometer cronometer;
        
        // Internal buffer to be processed by splitter
        char *buffer;
        size_t max_size;
        size_t size;
        
        // Buffer used to emit output produced by the splitter
        engine::BufferContainer output_buffer_container;
        
    public:
        
        BufferProcessor( Channel* _channel );
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
