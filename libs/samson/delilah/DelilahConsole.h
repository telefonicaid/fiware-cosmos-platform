#ifndef _H_DelilahConsole
#define _H_DelilahConsole

/* ****************************************************************************
*
* FILE                     DelilahConsole.h
*
* DESCRIPTION			   Console terminal for delilah
*
* Portions Copyright (c) 1997 The NetBSD Foundation, Inc. All rights reserved
*/
#include <cstdlib>				// atexit

#include <sstream>                  // std::ostringstream
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <editline/readline.h>
#include <algorithm>

#include "logMsg/logMsg.h"				

#include "au/TokenTaker.h"                  // au::TokenTake

#include "au/Console.h"                     // au::Console
#include "au/ConsoleAutoComplete.h"

#include "DelilahClient.h"                  // ss:DelilahClient

#include "samson/delilah/Delilah.h"			// samson::Delilah
#include "samson/delilah/DelilahUtils.h"    // getXXXInfo()

namespace samson {
	
	/**
	 Main class for the DelilahConsole program
	 */
	
	class DelilahConsole : public au::Console, public Delilah
	{
		
        std::string commandFileName;
        
	public:
		
        
		DelilahConsole( NetworkInterface *network );		
		~DelilahConsole();
				
		// Console funciton
		// --------------------------------------------------------
		
		
        // Main run command
        void run();
        
        // Set the command-file
        void setCommandfileName( std::string _commandFileName)
        {
            commandFileName = _commandFileName;
        }
                
		// Eval a command from the command line
        virtual std::string getPrompt();
		virtual void evalCommand( std::string command );
        virtual void autoComplete( au::ConsoleAutoComplete* info );

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
        
        
        void run_repeat_command( std::string command )
        {
            LM_TODO(("Check what type of messages...."));
            runAsyncCommand(command);
        }

	};

}


#endif
