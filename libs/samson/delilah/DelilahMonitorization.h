#ifndef _H_DelilahMonitorization
#define _H_DelilahMonitorization

/* ****************************************************************************
 *
 * FILE                     DelilahMonitorization.h
 *
 * DESCRIPTION			    Monitorization pannel for delilah
 *
 * Portions Copyright (c) 1997 The NetBSD Foundation, Inc. All rights reserved
 */

#include <cstdlib>                      // atexit
#include <sstream>                      // std::ostringstream
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <algorithm>

#include <editline/readline.h>

#include "logMsg/logMsg.h"				// LM_M

#include "samson/delilah/Delilah.h"		// samson::Delilah

#include "DelilahClient.h"              // ss:DelilahClient

#include "au/CursesConsole.h"			// au::CursesConsole


namespace samson {
       
	/**
	 Main class for the DelilahConsole program
	 */
	
	class DelilahMonitorization : public Delilah , public au::CursesConsole
	{
        std::string running_command;
		   
        // Internal list of commands available
        std::vector<std::string>  main_commands;    
        
	public:
        
		DelilahMonitorization( NetworkInterface *network , std::string _command ) : Delilah( network )
		{
			trace_on = true;
            setCommand( _command );    // Default command when starting
            running_command = _command;
            
            // Main commands
            main_commands.push_back("overview");
            
            main_commands.push_back("engine_show");
            main_commands.push_back("ls");
            main_commands.push_back("ls_queues");
            
		}
		
		~DelilahMonitorization()
		{
		}
		
        void runInBackground();
        
        // Function to print content
        void printContent();
        
		// Notifications from delilah
		void uploadDataConfirmation( DelilahUploadComponent *process ){};
		void downloadDataConfirmation( DelilahDownloadComponent *process ){};
		void pushConfirmation( PushDelilahComponent *process ){};
		virtual void notifyFinishOperation( size_t id ){};
        
		// Function to process messages from network elements not handled by Delila class
		int _receive(int fromId, Message::MessageCode msgCode, Packet* packet)
        {
            return 0;
        };		
        
		
		// Show a message on screen
		virtual void showMessage( std::string message)
		{
		}
		
		virtual void showTrace( std::string message)
		{
            /*
			if( trace_on )
				writeWarningOnConsole( message );
             */
		}
        
        // Eval commnad
        void evalComamnd( );
        
        void evalRealTimeComamnd( )
        {
            // do something with the command
        }
        

        // Get header information
        std::string getHeaderLeft();
        std::string getHeaderRight();

        // Autocompletion function
        void auto_complete( std::vector<std::string>& previous_words , std::string& current_word ,std::vector<std::string>& command_options );
        
        
	};
    
}


#endif
