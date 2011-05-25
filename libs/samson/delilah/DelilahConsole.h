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

#include "logMsg/logMsg.h"				

#include "au/Console.h"			// au::Console
#include "DelilahClient.h"      // ss:DelilahClient
#include "samson/delilah/Delilah.h"			// samson::Delilah
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
            addOption("jobs");
            addOption("workers");
            addOption("upload");
            addOption("push");
            addOption("download");
            addOption("load");
            addOption("load_clear");
            addOption("clear");
            addOption("help");
            addOption("set");
            addOption("unset");
            addOption("info");
            addOption("info_full");
            addOption("info_net");
            addOption("info_cores");
            addOption("info_task_manager");
            addOption("info_disk_manager");
            addOption("info_process_manager");
            addOption("info_memory_manager");
            addOption("info_load_data_manager");
            addOption("info_queues");
            addOption("info_engine");
        }
        
        void addOperations()
        {
            info_lock.lock();
            
            if( ol )
                for (int i = 0 ; i < ol->operation_size()  ; i++)
                    addOption( ol->operation(i).name() );
            
            info_lock.unlock();
        }
        
        void addQueueOptions( network::KVFormat *format )
        {
                        
            // add available queues...
            info_lock.lock();
            
            if( ql )
                for (int i = 0 ; i < ql->queue_size()  ; i++)
                {
                    
                    const network::Queue &queue = ql->queue(i).queue();
                    
                    //std::cout << "Considering " << queue.name() << "\n";
                    
                    if( !format )
                        addOption( queue.name() );
                    else
                    {
                        //std::cout << "Checkling formats "  << queue.format().keyformat() << " " << queue.format().valueformat()<<   "\n";
                        if( ( queue.format().keyformat() == format->keyformat() ) )
                            if ( queue.format().valueformat() == format->valueformat() )
                            {
                                addOption( ql->queue(i).queue().name() );
                                //std::cout << "added\n";
                            }
                        
                    }
                }
            
            info_lock.unlock();
            
        }        
        
        
        void addQueueForOperation( std::string mainCommand , int argument_pos )
        {
            network::KVFormat *format = NULL;

            // If it is a particular operation... lock for the rigth queue
            info_lock.lock();
            
            if( ol )
                for (int i = 0 ; i < ol->operation_size() ; i++)
                    if( ol->operation(i).name() == mainCommand )
                    {
                        //std::cout << "op found " << ol->operation(i).input_size() << "/" << ol->operation(i).output_size() <<  " ("<< argument_pos << ")\n";
                        
                        if( argument_pos < ol->operation(i).input_size() )
                        {
                            format = new network::KVFormat();
                            format->CopyFrom( ol->operation(i).input(argument_pos) );
                        }
                        else
                        {
                            argument_pos -= ol->operation(i).input_size();
                            if( argument_pos < ol->operation(i).output_size() )
                            {
                                format = new network::KVFormat();
                                format->CopyFrom( ol->operation(i).output(argument_pos) );
                            }
                        }
                        break; // No more for...
                    }
            
            info_lock.unlock();
            
            addQueueOptions(format);
            
            if( format )
                delete format;
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
		
		// Confirmation that a loading process has finished
		virtual void uploadDataConfirmation( DelilahUploadDataProcess *process);
		virtual void downloadDataConfirmation( DelilahDownloadDataProcess *process );
		
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
		virtual void showMessage( std::string message)
		{
			writeWarningOnConsole( message );
		}
		
		virtual void showTrace( std::string message)
		{
			if( trace_on )
				writeWarningOnConsole( message );
		}

		
		// Private functions to show content on the console
		// --------------------------------------------------------
		
		void showQueues( const network::QueueList ql );
		void showAutomaticOperations( const network::AutomaticOperationList aol);
		void showDatas( const network::DataList ql);
		void showOperations( const network::OperationList ql);
		void showJobs( const network::JobList ql);
        
        void showInfo( std::string command );
        
        
	};

}


#endif
