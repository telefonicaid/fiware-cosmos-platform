#ifndef _H_SAMSON_CONNECTOR_COMMON
#define _H_SAMSON_CONNECTOR_COMMON

#include "engine/Buffer.h"
#include "BufferProcessor.h"
#include "samson/client/SamsonClient.h"

namespace samson 
{
    
    typedef enum
    {
        connection_input,
        connection_output
    } ConnectionType;
    
    const char * str_ConnectionType( ConnectionType type );

    class SamsonConnector;
    class Block;
    
    // Base class for all items
    class SamsonConnectorItem
    {
        
        // Rate statistics
        au::Token token_rates;
        au::rate::Rate input_rate;
        au::rate::Rate output_rate;
        
        friend class SamsonConnector;
        int samson_connector_id;
        int parent_samson_connector_id;

        au::Token token_pending_blocks;
        au::list<Block> pending_blocks;      // List of pending packets ( only in output )
        
        BufferProcessor* block_processor;    // Processor for input buffers ( only in input )

    protected:
        
        ConnectionType type;
        au::Cronometer cronometer;             // Global cronometer
        SamsonConnector *samson_connector;     // Pointer to the global samson_connector
        
        
        // Method to recover buffers to be pushed to the output
        Block* getNextOutputBlock();
        void popOutputBlock();

        // Method to push any input buffer
        void pushInputBuffer( engine::Buffer * buffer );
        void flush();
        
    public:
        
        SamsonConnectorItem( SamsonConnector * _samson_connector , ConnectionType _type ) 
        : token_rates("token_rates_SamsonConnectorItem") 
        , token_pending_blocks("token_pending_blocks")
        {
            samson_connector = _samson_connector;
            type = _type;
            samson_connector_id = -1;         // Unassigned until added to SamsonConnector
            parent_samson_connector_id = -1;
            
            
            block_processor = new BufferProcessor( this, _samson_connector );
        }

        virtual ~SamsonConnectorItem();
        
        void setParentId( int _parent_id )
        {
            parent_samson_connector_id = _parent_id;
        }
        
        int getSamsonconnectorId()
        {
            return samson_connector_id;
        }

        int getParentSamsonconnectorId()
        {
            return parent_samson_connector_id;
        }
        
        // Get a name of this element
        virtual std::string getName()=0;
        
        // Get status of this element
        virtual std::string getStatus()=0;
        
        // Can be removed ( no background threads and so... )
        virtual bool canBeRemoved()=0;

        // Check if this item is finished ( if so, no data is push here )
        virtual bool isConnected()=0;
        
        // Method called every 5 seconds to re-connect or whatever is necessary here...
        virtual void review()=0;

        // Method to push data ( only output )
        virtual void push( Block* block );
        
        virtual size_t getOuputBufferSize();
        
        // get type
        ConnectionType getType();
        
        // Methods to get or set information for rates
        void report_input_block( size_t size );
        void report_output_block( size_t size );
        size_t get_input_total();
        size_t get_output_total();
        size_t get_input_rate();
        size_t get_output_rate();
        
        
        std::string getDescription()
        {
            if( type == connection_input )
                return au::str( "[%d] Input from %s" , samson_connector_id , getName().c_str() );
            else
                return au::str( "[%d] Output to %s" , samson_connector_id , getName().c_str() );
        }
        
        std::string str()
        {
            return au::str("%s [%s] In: %s Out: %s" 
                           , getName().c_str()
                           , getStatus().c_str()
                           , au::str( get_input_total() ,"B" ).c_str()
                           , au::str( get_output_total() ,"B" ).c_str()
                           );
        }
        
        
        std::string str_total_statistics()
        {
            return au::str("In: %s Out: %s" 
                           , au::str( get_input_total() ,"B" ).c_str()
                           , au::str( get_output_total() ,"B" ).c_str()
                           );
        }

        SamsonConnector *getSamsonConnector()
        {
            return samson_connector;
        };     

        
    };

    
    class SamsonConnectorItemTotal : public SamsonConnectorItem
    {
        
        public:
        
        SamsonConnectorItemTotal( SamsonConnector * samson_connector  ) : SamsonConnectorItem( samson_connector , connection_input ) // Irrelevant
        {
            
        }
        
        // Get a name of this element
        std::string getName()
        {
            return "TOTAL";
        }
        
        // Get status of this element
        std::string getStatus()
        {
            return "-";
        }
        
        // Can be removed ( no background threads and so... )
        bool canBeRemoved()
        {
            return false; // never removed
        }
        
        bool isConnected()
        {
            return true;
        }
        
        // Method called every 5 seconds to re-connect or whatever is necessary here...
        void review()
        {
            return; // Nothing to review here
        }

    };
    
    
    
}


#endif