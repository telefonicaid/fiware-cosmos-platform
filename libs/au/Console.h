#ifndef SAMSON_CONSOLE_H
#define SAMSON_CONSOLE_H

/* ****************************************************************************
*
* FILE                      AUConsole.h
*
* AUTHOR                    Andreu Urruela Planas
*
* Class that implement a simple console to interact with the user.
* Subclass this class implementing the evalCommand( std::string command )
* 
* To run the console, you have the bloquink "run" method.
*/
#include <string.h>           /* memcpy, ... */
#include <istream>
#include <cstdlib>
#include <iostream>
#include <deque>

#include "au/Lock.h"             /* Lock                            */

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sstream>

namespace au {


	class Console
	{
		Lock lock;				//!< Lock as a control mechanism to log things into the console
		bool quit_console;		//!< Flag to indicate that we want the console to quit ( can be set from outside the class with quit )
        
	public:		
        
		Console();
		virtual ~Console(){};

		virtual std::string getPrompt();				//!< Function to give the current prompt (can be overloaded in subclasses )
		virtual void evalCommand( std::string command );//!< function to process a command instroduced by user	
		
		/* Methods to write things on screen	( now it is async ) */		
		void writeWarningOnConsole( std::string message );
		void writeErrorOnConsole( std::string message );
		void writeOnConsole( std::string message );

		void quitConsole(); // Set the console to quit
		
		
	private:
		
		void printLines();
		void printCommand();
		void _print();
		void print();
		
        void write( std::string message );
        
	public:

		void runConsole();
		
	};
	
}	//namespace au

#endif
