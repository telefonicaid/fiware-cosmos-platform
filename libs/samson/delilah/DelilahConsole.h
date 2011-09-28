#ifndef _H_DelilahConsole
#define _H_DelilahConsole

/* ****************************************************************************
*
* FILE                     DelilahConsole.h
*
* DESCRIPTION			   Console terminal for delilah
*
*/
#include <cstdlib>				// atexit

#include <sstream>                  // std::ostringstream
#include <time.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <algorithm>

#include "logMsg/logMsg.h"				

#include "au/TokenTaker.h"                  // au::TokenTake

#include "au/Console.h"                     // au::Console
#include "DelilahClient.h"                  // ss:DelilahClient

#include "samson/delilah/Delilah.h"			// samson::Delilah
#include "samson/delilah/DelilahUtils.h"    // getXXXInfo()

namespace samson {
	
	
    char * command_generator (const char * text, int state);
	char ** readline_completion ( const char* text, int start,int end );	

	
    int common_chars( const char* c1 , const char* c2);
    char * strdup_common(const char* c , int len );

    
	class AutoCompletionOptions
	{
		std::vector<std::string> options;           // Vector of possible options
		        
	public:
		
		void clearOptions()
		{
			options.clear();
		}
		
		void addOption( std::string v )
		{
			options.push_back( v );
		}
        
        
        void addMainCommands()
        {
            addOption("ls");
            addOption("mv");
            addOption("add");
            addOption("operations");
            addOption("datas");
            addOption("ps_jobs");
            addOption("ps_tasks");
            addOption("workers");
            addOption("upload");
            addOption("push");
            addOption("pop");
            addOption("download");
            addOption("load");
            addOption("clear");
            addOption("help");
            addOption("set");
            addOption("unset");
            addOption("info");
            addOption("add_stream_operation");
            addOption("rm_stream_operation");
            addOption("ls_stream_operation");
            addOption("set_stream_operation_property");
            addOption("rm_queue");                          // Remove a queue
            addOption("pause_queue");                       // Pause and Play a state ( to remove it )
            addOption("play_queue");                          
            addOption("ls_queues");                      // Get a list of all current tasks in the system
            addOption("ps_stream");                     // Get a list of stream task
            addOption("ls_modules");                     // Get a list of stream task
            addOption("ls_operations");         
            addOption("ls_datas");         
            addOption("ps_network");         
            addOption("engine_show");
            addOption("ls_local");
            addOption("ls_block_manager");
            addOption("rm_local");
            addOption("run_stream_operation");
            addOption("ls_operation_rates");
            addOption("set_queue_property");
            addOption("ls_queues_info");
        }
        
        void addOperations()
        {
            au::TokenTaker tt( &token_xml_info );
            std::vector<std::string> operation_names = pugi::values( doc , "//controller//operation/name" );

            for ( size_t i = 0 ;  i < operation_names.size() ; i++)
                addOption( operation_names[i] );
        }

        void addQueues( )
        {
            au::TokenTaker tt( &token_xml_info );
            std::vector<std::string> queue_names = pugi::values( doc , "//controller//queue" );
            
            for ( size_t i = 0 ;  i < queue_names.size() ; i++)
                addOption( queue_names[i] );
            
        }        
        
        void addQueueOptions( std::string key_format , std::string value_format )
        {
            au::TokenTaker tt( &token_xml_info );
            std::string c = au::str( "//controller//queue[format/key_format=\"%s\"][format/value_format=\"%s\"]" 
                                    , key_format.c_str() , value_format.c_str() ); 
            
            std::vector<std::string> queue_names = pugi::values( doc , c );
            
            for ( size_t i = 0 ;  i < queue_names.size() ; i++)
                addOption( queue_names[i] );
           
        }        
        
        
        void addQueueForOperation( std::string mainCommand , int argument_pos )
        {
            pugi::xpath_node_set node_set;
            
            {
                au::TokenTaker tt( &token_xml_info );
                std::string c = au::str( "//controller//operation[name=\"%s\"]/input_formats/format[%d]" ,  mainCommand.c_str() , argument_pos+1 );
                node_set = pugi::select_nodes(doc, c );
            }
            
            if( node_set.size() > 0 )
            {
                std::string key_format = pugi::get( node_set[0].node() , "key_format" );
                std::string value_format = pugi::get( node_set[0].node() , "value_format" );
                
                addQueueOptions(key_format, value_format);
            } 
        }
        
        
        /*
         Function to get the real options
         */
        
        char** get( const char* text )
        {
            std::vector<std::string> real_options;      // Vector of final real options
            
			size_t len = strlen(text);
            
            for ( size_t i = 0 ; i < options.size() ; i++)
				if ( strncmp ( options[i].c_str() , text, len) == 0 )
                    real_options.push_back( options[i] );                    

            // If no option... return NULL
            if ( real_options.size() == 0 )
                return (char**) NULL;

            
            // Create the vector of possible solution ( first = "the replacement", last  = NULL )
            char **matches = (char**) malloc( sizeof(char*) * ( real_options.size() + 1 + 1 ) );
            matches[ real_options.size()+1 ] = NULL; // The last should be a NULL

            // let's compute the replacement as the min_common_legth copy of the the options
            int min_common_length = real_options[0].length();
            for ( size_t i = 1 ; i < real_options.size() ; i++)
                min_common_length = std::min( min_common_length , common_chars( real_options[0].c_str()  , real_options[i].c_str() ) );

            matches[0] = strdup_common( real_options[0].c_str() , min_common_length );


            // Rest of options
            for ( size_t i = 0 ; i < real_options.size() ; i++)
            {
                // Rememeber the first is for the "replacement"
                matches[i+1] = strdup( real_options[i].c_str()  );
            }
            
            
            return matches;
        }
        
     
	};

	
	/**
	 Main class for the DelilahConsole program
	 */
	
	class DelilahConsole : public au::Console, public Delilah
	{
		
        std::string commandFileName;
        
	public:
		
        
		DelilahConsole( NetworkInterface *network) : Delilah( network , true )
		{
			/* completion function for readline library */
			rl_attempted_completion_function = readline_completion;

			trace_on = false;
		}
		
		~DelilahConsole()
		{
		}
				
		// Console funciton
		// --------------------------------------------------------
		
		virtual std::string getPrompt()
		{
			return  "Delilah> ";
		}
		
		virtual std::string getHeader()
		{
			return  "Delilah";
		}
		
        // Main run command
        void run();
        
        // Set the command-file
        void setCommandfileName( std::string _commandFileName)
        {
            commandFileName = _commandFileName;
        }
                
		// Eval a command from the command line
		virtual void evalCommand( std::string command );

		// Run asynch command and returns the internal operation in delilah
		size_t runAsyncCommand( std::string command );

		// Functions overloaded from Delilah
		// --------------------------------------------------------
		
        void delilahComponentFinishNotification( DelilahComponent *component);
        void delilahComponentStartNotification( DelilahComponent *component);
        
		// Function to process messages from network elements not handled by Delila class
		int _receive(int fromId, Message::MessageCode msgCode, Packet* packet);		

		// Notify that an operation hash finish
		virtual void notifyFinishOperation( size_t id )
		{
			std::ostringstream output;
			output << "Finished local delilah process with : " << id ;
			writeWarningOnConsole( output.str() );
		}
		
		// Show a message on screen
		void showMessage( std::string message)
		{
			writeOnConsole( message );
		}
		void showWarningMessage( std::string message)
        {
			writeWarningOnConsole( message );
        }
        
		void showErrorMessage( std::string message)
        {
			writeErrorOnConsole( message );
        };

		
		virtual void showTrace( std::string message)
		{
			if( trace_on )
				writeWarningOnConsole( message );
		}

        
    private:
        
        std::string getLsLocal();
        
	};

}


#endif
