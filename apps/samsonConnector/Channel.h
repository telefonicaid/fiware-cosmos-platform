#ifndef _H_SAMSON_CHANNEL
#define _H_SAMSON_CHANNEL

#include "TrafficStatistics.h"
#include "ConnectorCommand.h"

namespace samson {
    namespace connector {
        
        class Item;
        class SamsonConnector;
        
        class Channel 
        {
            
            // Mutex protection
            au::Token token;
            
            // All items included in this channel
            // listeners, connections , disk connection, samson connections, etc... 
            au::map<int, Item> items;
            
            // id for the next item
            int items_id; 
            
            // Input & Output statistics
            TrafficStatistics traffic_statistics;
            
            SamsonConnector * connector;
            std::string name;
            
            friend class Connection;
            friend class SamsonConnector;
            
        public:
            
            // Constructor
            Channel( SamsonConnector * _connector , std::string _name );
            
            // Generic command line interface to modify this channel
            void process_command( CommandLine* command , au::ErrorManager * error );
            
            // General review to check if there are connections to be closed
            void review();
            
            // Common method to push data to all output connections ( from all items at input )
            void push( engine::Buffer * buffer );

            // Information
            int getNumItems();
            int getNumInputItems();
            int getNumOutputItems();
            int getNumConnections();
            std::string getName();
            
        private:
            
            
            // Command to add or remove items
            void add_inputs ( std::string input_string , au::ErrorManager* error );
            void add_outputs ( std::string input_string , au::ErrorManager* error );
            void add( Item * item );
            
            
        };
        
    }
}
    
#endif