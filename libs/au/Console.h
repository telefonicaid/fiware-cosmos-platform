
/* ****************************************************************************
 *
 * FILE            Console.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 * Class that implement a simple console to interact with the user.
 * Subclass this class implementing the evalCommand( std::string command )
 * 
 * To run the console, you have the bloquing "runConsole" method. 
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef SAMSON_CONSOLE_H
#define SAMSON_CONSOLE_H

#include <string.h>           /* memcpy, ... */
#include <istream>
#include <cstdlib>
#include <iostream>
#include <deque>
#include <list>

#include "au/Lock.h"             /* Lock                            */
#include "au/Token.h"                   // au::Token
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

        au::Token token_pending_messages;
        std::list<std::string> pending_messages;
        pthread_t t;
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
