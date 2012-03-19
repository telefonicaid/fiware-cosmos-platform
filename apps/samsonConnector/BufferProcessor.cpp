
#include <string>

#include "logMsg/logMsg.h"

#include "SamsonConnector.h"
#include "BufferProcessor.h" // Own interface

extern char splitter[1024];


namespace samson {
    
    BufferProcessor::BufferProcessor( SamsonConnector * _stream_connector )
    {
        stream_connector = _stream_connector;
        std::string splitter_name = splitter;
        
        LM_V(("Init BlockProcessor with splitter %s" , splitter_name.c_str() ));
    }
    
    void BufferProcessor::push( engine::Buffer * buffer )
    {
        // Still not implemented
        stream_connector->push( buffer );
    }
    
    void BufferProcessor::flush()
    {
        // Nothing at the moment since we are not splitting data
    }
    
}