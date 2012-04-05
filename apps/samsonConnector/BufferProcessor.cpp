
#include <string>

#include "logMsg/logMsg.h"

#include "SamsonConnector.h"
#include "BufferProcessor.h" // Own interface

#include "samson/module/ModulesManager.h"

#include "common.h"

extern char input_splitter_name[1024];
extern size_t buffer_size;

namespace samson {
    
    BufferProcessor::BufferProcessor( SamsonConnectorItem* _item , SamsonConnector * _stream_connector )
    {
        item = _item;
        stream_connector = _stream_connector;
        splitter_name = input_splitter_name;

        if( splitter_name == "" )
        {
            splitter = NULL;
            buffer = NULL;
        }
        else
        {
        
            Operation* operation = ModulesManager::shared()->getOperation( splitter_name );
            
            if( !operation || ( operation->getType() != Operation::splitter ) )
                LM_X(1, ("Wrong splitter %s" , input_splitter_name ));
            
            // Get instace of the operation
            splitter = (Splitter*) operation->getInstance();
            if( !splitter )
                LM_X(1, ("Error getting instance of the splitter"));
            
            // Internal buffer
            buffer = (char*) malloc( buffer_size );
            max_size = buffer_size;
            size = 0;

        }
        
        
    }
    
    BufferProcessor::~BufferProcessor()
    {
        // Free allocted buffer
        if ( buffer )
            free( buffer );
        
    }
    
    void BufferProcessor::process_intenal_buffer( bool finish )
    {
        
        //LM_W(("Process internal buffer %s" , au::str(size,"B").c_str() ));

        // Split buffer to know what to push
        char * outData;
        size_t outLength;
        char * nextData;
        int c = splitter->split(buffer, size, &outData, &outLength, &nextData , finish );
        
        //LM_W(("Splitter input (%p , %lu)" , buffer , size ));
        //LM_W(("Splitter answer %d out:%p-%lu  Next %p" , c , outData , outLength , nextData ));
        
        LM_READS("client", "input", buffer, size, LmfByte);

        if( outLength > size )
            LM_X(1, ("Error in the splitter implementation. Larger size than the input returned"));
        
        if( c == 0 )
        {
            if( outData && (outLength>0) )
            {
                // Emit at the output
                engine::Buffer *output_buffer = engine::MemoryManager::shared()->newBuffer("output_splitter", "connector", outLength );
                output_buffer->setSize(outLength);
                memcpy( output_buffer->getData(), outData, outLength );
                
                // Push at the output
                stream_connector->push( output_buffer , item );
            }
            
            // Data to be skip after process
            if( nextData )
            {
                size_t skip_size = nextData - buffer;
            
                // Move data at the begining of the buffer
                memmove( buffer + skip_size , buffer, size - skip_size );
                size -= skip_size;
            }
            else
                size = 0;
            
        }
        else
        {
            if( size >= max_size ) 
            {
                LM_W(("Splitter %s is not able to split a full buffer %s. Skipping this buffer" , input_splitter_name , au::str( max_size ).c_str()  ));
                size = 0;
            }
            else
                return; // Not enoutght data for splitter... this is OK if the buffer is not full
        }
        
        
    }

    
    void BufferProcessor::push( engine::Buffer * input_buffer )
    {

        // If no splitter, no process
        if( !splitter )
        {
            stream_connector->push( input_buffer , item );
            return;
        }
        
        //LM_M(("BufferProcessor: Process buffer %s" , au::str( input_buffer->getSize() ,"B").c_str() ));
        
        size_t pos_in_input_buffer = 0; // Readed so far
        while( pos_in_input_buffer < input_buffer->getSize() )
        {
            size_t copy_size = std::min( max_size - size , input_buffer->getSize() - pos_in_input_buffer );
            
            memcpy(buffer+size, input_buffer->getData() + pos_in_input_buffer , copy_size);
            size += copy_size;
            pos_in_input_buffer += copy_size;
            
            // Process internal buffer to push blocks at the output
            process_intenal_buffer( false );
        }
        
        // Destroy input buffer
        engine::MemoryManager::shared()->destroyBuffer( input_buffer );
        
    }
    
    void BufferProcessor::flush()
    {
        // If no splitter is present, we never accumulate nothing here
        if( !splitter )
            return;
            
        
        process_intenal_buffer( true ); // Process internal buffer with the "finish" flag activated
    }
    
}
