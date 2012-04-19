

#include "Block.h"
#include "SamsonConnector.h"
#include "common.h" // Own interface

namespace samson {

    const char * str_ConnectionType( ConnectionType type )
    {
        switch (type) {
            case connection_input:
                return "Input";
            case connection_output:
                return "Output";
        }
        
        return "Unknown";
    }
    
    SamsonConnectorItem::SamsonConnectorItem( SamsonConnector * _samson_connector , ConnectionType _type ) 
    : token_rates("token_rates_SamsonConnectorItem") 
    , token_pending_buffers("token_pending_blocks")
    {
        samson_connector = _samson_connector;
        type = _type;
        samson_connector_id = -1;         // Unassigned until added to SamsonConnector
        parent_samson_connector_id = -1;
        
        // Block to process all input buffers
        block_processor = new BufferProcessor( this, _samson_connector );
    }

    SamsonConnectorItem::~SamsonConnectorItem()
    {
        delete block_processor;
    }

    
    
    void SamsonConnectorItem::push( engine::Buffer* buffer )
    { 
        if( type == connection_input )
            return; // Nothing to do if we are input
        
        if( !isConnected() )
            return;
        
        {
            au::TokenTaker tt(&token_pending_buffers);
            buffer_list_container.push_back( buffer );
        }
    }

    engine::Buffer* SamsonConnectorItem::getNextOutputBuffer()
    {
        au::TokenTaker tt(&token_pending_buffers);
        return buffer_list_container.front();
    }

    void SamsonConnectorItem::popOutputBuffer()
    {
        au::TokenTaker tt(&token_pending_buffers);
        return buffer_list_container.pop();
    }
    
    void SamsonConnectorItem::pushInputBuffer( engine::Buffer * buffer )
    {
        if( !buffer )
            return;

        /*
        samson_connector->show_message( 
                    au::str("Received %s from \"%s\"" 
                            , au::str( buffer->getSize() ,"Bytes").c_str() 
                            , getDescription().c_str() ) );
         */
        
        // Report input block
        samson_connector->report_input_block( samson_connector_id , buffer->getSize()  );

        // Add the out processor
        block_processor->push(buffer);
    }
    
    void SamsonConnectorItem::flush()
    {
        block_processor->flush();
    }
    
    size_t SamsonConnectorItem::getOuputBufferSize()
    {
        au::TokenTaker tt(&token_pending_buffers);
        return buffer_list_container.getTotalSize();
    }
    
    // get type
    ConnectionType SamsonConnectorItem::getType()
    {
        return type;
    }
    
    // Methods to get or set information for rates
    void SamsonConnectorItem::report_input_block( size_t size )
    {
        au::TokenTaker tt( &token_rates );
        input_rate.push( size );
    }
    void SamsonConnectorItem::report_output_block( size_t size )
    {
        au::TokenTaker tt( &token_rates );
        output_rate.push( size );
    }
    
    size_t SamsonConnectorItem::get_input_total()
    {
        au::TokenTaker tt( &token_rates );
        return input_rate.getTotalSize();
    }
    size_t SamsonConnectorItem::get_output_total()
    {
        au::TokenTaker tt( &token_rates );
        return output_rate.getTotalSize();
    }
    
    size_t SamsonConnectorItem::get_input_rate()
    {
        au::TokenTaker tt( &token_rates );
        return input_rate.getRate();
    }
    size_t SamsonConnectorItem::get_output_rate()
    {
        au::TokenTaker tt( &token_rates );
        return output_rate.getRate();
    }

}