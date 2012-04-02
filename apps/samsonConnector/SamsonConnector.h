#ifndef _H_SAMSON_CONNECTOR
#define _H_SAMSON_CONNECTOR

#include <set>

#include "logMsg/logMsg.h"

#include "au/map.h"
#include "au/vector.h"
#include "au/Token.h"
#include "au/string.h"
#include "au/Console.h"

#include "engine/Buffer.h"

#include "samson/network/NetworkListener.h"

#include "common.h"
#include "SamsonConnection.h"
#include "ServerConnection.h"
#include "DiskConnection.h"
#include "BufferProcessor.h"

extern bool interactive; // Defined in the main.cpp

namespace samson {
    
    
    class InputReader;
    class OutputWriter;
    class SamsonConnectorConnection;
    
    
    class SamsonConnector :  public au::Console
    {
        
        au::Token token;

        // All items: listeners, connections , disk connection, samson connections, etc... 
        int items_id; // id for the next item
        au::map<int, SamsonConnectorItem> items;
                
        bool setup_complete;
        
    public:
        
        SamsonConnector() : token("SamsonPushConnectionsManager")
        {
            items_id = 0; // First id for an item

            // Add the total counter with id "0"
            add( new SamsonConnectorItemTotal(this) , -1 );  // Parent is forced to -1 to avoid infinite looping
            
            // Flag to inform that all input & outputs in command line are correclty parsed
            setup_complete = false;
        }
                
        // Generic command to process add_input add_output
        void add_inputs ( std::string input_string , au::ErrorManager* error );
        void add_outputs ( std::string input_string , au::ErrorManager* error );
        
        // Generic method to add an item
        void add( SamsonConnectorItem * item , int parent_id = 0 );
        
        // Methods to report activity...
        void report_input_block( int id , size_t size );
        void _report_output_block( int id , size_t size );
        
        // General review
        void review();
        
        // Common method to push data to all output connections
        void push( engine::Buffer * buffer , SamsonConnectorItem *item );
        
        // au::Console interface
        std::string getPrompt();
        void evalCommand( std::string command );
        void autoComplete( au::ConsoleAutoComplete* info );

        // Generic way to show messages on screen
        void show_message( std::string message )
        {
            if( interactive )
                writeWarningOnConsole(message);
            else
                LM_V(("%s" , message.c_str() ));
        }
        void show_warning( std::string message )
        {
            if( interactive )
                writeWarningOnConsole(message);
            else
                LM_V(("%s" , message.c_str() ));
        }
        
        void show_error( std::string message )
        {
            if( interactive )
                writeErrorOnConsole(message);
            else
                LM_X( 1 , ("%s" , message.c_str() ));
        }
        
        void set_setup_complete()
        {
            setup_complete = true;
        }

        
    private:
        
        void _exit_if_necessary_for_no_inputs();
        void _exit_if_necessary_for_no_outputs();
        void _show_last_messages();
        
    };
}
#endif