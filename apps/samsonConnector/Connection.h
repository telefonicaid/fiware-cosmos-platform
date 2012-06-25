#ifndef _H_SAMSON_CONNECTOR_CONNECTION_BASE
#define _H_SAMSON_CONNECTOR_CONNECTION_BASE

#include "engine/Buffer.h"
#include "engine/BufferContainer.h"
#include "samson/client/SamsonClient.h"
#include "common.h"
#include "TrafficStatistics.h"

namespace samson 
{
    namespace connector
    {
        
        class Channel;
        class Item;
        class BufferProcessor;
        
        class Connection
        {
            BufferProcessor* buffer_processor;     // Processor for input buffers ( only in input )
            BufferList buffer_list;                // List of buffers to be sent ( in output connections )

            std::string name;                      // Description name
            ConnectionType type;                   // Type of item ( input or output )
            au::Cronometer cronometer;             // Global cronometer
            Item * item;                           // My item            
                        
            TrafficStatistics traffic_statistics;  // Information about input & output

            friend class SamsonConnector;
            
            bool removing;            
            
        protected:
            
            // Method to recover buffers to be pushed to the output ( output connections )
            void getNextBufferToSent( engine::BufferContainer * container );
            
            // Method to push any input buffer ( input connections )
            void pushInputBuffer( engine::Buffer * buffer );
            void flushInputBuffers();
            
        public:
            
            Connection( Item  * _item , ConnectionType _type , std::string _name );
            virtual ~Connection();

            ConnectionType getType();       // Get type
            const char* getTypeStr();
            std::string getName();          // Get a name of this element
            virtual size_t getSize();               // Get currect size accumulated here

            
            // Method to push data from channel ( only output )
            virtual void push( engine::Buffer* buffer );
            
            virtual std::string getStatus()=0;      // Get status of this element
            virtual bool canBeRemoved()=0;          // Can be removed ( no background threads and so... )
            virtual void review(){};                // Method called to review this connection
            
            std::string str()
            {
                return au::str("%s [%s] In: %s Out: %s" 
                               , getName().c_str()
                               , getStatus().c_str()
                               , au::str( traffic_statistics.get_input_total() ,"B" ).c_str()
                               , au::str( traffic_statistics.get_output_total() ,"B" ).c_str()
                               );
            }
          
            void set_removing()
            {
                removing =  true;
            }
            
            bool isRemoving()
            {
                return removing;
            }
            
            void report_output_size( size_t );
            void report_input_size( size_t );

            
        };
    }
}

#endif