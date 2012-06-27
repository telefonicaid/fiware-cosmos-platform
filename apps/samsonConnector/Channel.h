#ifndef _H_SAMSON_CHANNEL
#define _H_SAMSON_CHANNEL

#include "engine/Buffer.h"
#include "common.h"
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
         
            // Name of this channel
            std::string name_;
            
            // Name of the splitters ( process on all inputs for this channel )
            std::string splitter_; 
            
            // All items ( adapters ) included in this channel
            au::map<std::string, Item> items;
            
            // Input & Output statistics
            TrafficStatistics traffic_statistics;

            // Pointer to the global connector
            SamsonConnector * connector_;
            
            friend class Connection;
            friend class SamsonConnector;
            
        public:
            
            // Constructor
            Channel( SamsonConnector * connector , std::string name , std::string splitter );
            ~Channel();
            
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
            std::string getSplitter();
            std::string getInputsString();
            std::string getOutputsString();
            
            // Remove finished items and connections
            void remove_finished_items_and_connections( au::ErrorManager * error );
            
            void cancel_channel();
            
            void write( au::ErrorManager * error );
            
            // Log system
            void log( std::string type , std::string message );
            void log( Log* log );

        private:
            
            
            // Command to add or remove items
            void add_input ( std::string name , std::string input_string , au::ErrorManager* error );
            void add_output ( std::string name , std::string input_string , au::ErrorManager* error );
            void add( std::string name , Item * item );
            
            
        };
        
    }
}
    
#endif