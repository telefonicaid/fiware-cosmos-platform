

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
    
    SamsonConnectorItem::~SamsonConnectorItem()
    {
        // Remove non sent blocks
        au::list<Block>::iterator it_pending_blocks;
        for( it_pending_blocks = pending_blocks.begin() ; it_pending_blocks != pending_blocks.end() ; it_pending_blocks++ )
            (*it_pending_blocks)->release();
        pending_blocks.clear();
        
        
        delete block_processor;
    }

    
    
    void SamsonConnectorItem::push( Block* block )
    { 
        if( type == connection_input )
            return; // Nothing to do if we are input
        
        if( !isConnected() )
            return;
        
        {
            au::TokenTaker tt(&token_pending_blocks);
            block->retain();
            pending_blocks.push_back( block );
        }
    }

    Block* SamsonConnectorItem::getNextOutputBlock()
    {
        au::TokenTaker tt(&token_pending_blocks);
        if( pending_blocks.size() == 0 )
            return NULL;
        
        Block * b = pending_blocks.front();
        return b;
    }

    void SamsonConnectorItem::popOutputBlock()
    {
        au::TokenTaker tt(&token_pending_blocks);
        Block * b = pending_blocks.front();
        b->release();
        pending_blocks.pop_front();
    }
    
    void SamsonConnectorItem::pushInputBuffer( engine::Buffer * buffer )
    {
        if( !buffer )
            return;

        samson_connector->show_message( 
                    au::str("Received %s from \"%s\"" 
                            , au::str( buffer->getSize() ,"Bytes").c_str() 
                            , getDescription().c_str() ) );
        
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
        au::TokenTaker tt(&token_pending_blocks);
        
        size_t total = 0;
        au::list<Block>::iterator it_pending_blocks; 
        for( it_pending_blocks = pending_blocks.begin() ; it_pending_blocks != pending_blocks.end() ; it_pending_blocks++ )
            total += (*it_pending_blocks)->buffer->getSize();
            
        return total;
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