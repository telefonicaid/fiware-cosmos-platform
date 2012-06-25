


#include "Item.h"
#include "SamsonConnector.h"
#include "BufferProcessor.h"
#include "Connection.h" // Own interface

namespace samson {
    namespace connector {
        
        Connection::Connection( Item  * _item , ConnectionType _type , std::string _name )
        {
            // Keep information
            type = _type;
            item = _item;
            name = _name;
            
            // Buffer processor created on demand ( first time )
            buffer_processor = NULL;
            
            removing = false;
            
        }
        
        Connection::~Connection()
        {
            if( buffer_processor )
                delete buffer_processor;
        }
        
        void Connection::report_output_size( size_t size )
        {
            traffic_statistics.push_output(size);
            item->traffic_statistics.push_output( size );
            item->channel->traffic_statistics.push_output( size );
        }
        void Connection::report_input_size( size_t size )
        {
            traffic_statistics.push_input(size);
            item->traffic_statistics.push_input( size );
            item->channel->traffic_statistics.push_input( size );
        }

        
        void Connection::getNextBufferToSent( engine::BufferContainer * container )
        {
            // Extract buffer from the list
            buffer_list.pop( container );
            
            // Report output traffic ( as it is really exported....)
            engine::Buffer* buffer = container->getBuffer();

            if( buffer )
                report_output_size( buffer->getSize() );
        }
        
        void Connection::pushInputBuffer( engine::Buffer * buffer )
        {
            if( !buffer )
                return;
            
            // Create buffer processor to process all input buffers
            if( !buffer_processor )
                buffer_processor = new BufferProcessor( item->channel );

            // Report input block
            report_input_size( buffer->getSize() );
            
            // push the block processor
            buffer_processor->push(buffer);
        }
        
        void Connection::flushInputBuffers()
        {
            buffer_processor->flush();
            
        }

        void Connection::push( engine::Buffer* buffer )
        {
            // Put in the list to be emitted
            buffer_list.push(buffer);
        }
        
        size_t Connection::getSize()
        {
            if( type == connection_output )
                return buffer_list.getSize();
            else
                return 0;
        }
        
        // get type
        ConnectionType Connection::getType()
        {
            return type;
        }
        
        const char* Connection::getTypeStr()
        {
            return str_ConnectionType(type);
        }

        std::string Connection::getName()
        {
            return name;
        }
        
    }
}
