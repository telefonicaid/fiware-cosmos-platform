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
#include <string.h>           /* memcpy, ...                                 */
#include <istream>
#include <cstdlib>
#include <iostream>
#include <curses.h>
#include <deque>

#include "CommandLine.h"
#include "ConsoleCommand.h"		// au::ConsoleLine
#include "Lock.h"                /* Lock                            */

namespace au {


	#pragma mark CONSOLELINE
	
	#define ConsoleLineNormal	0
	#define ConsoleLineWarning	1
	#define ConsoleLineError	2
		
		
	class ConsoleLine
	{
	public:
		
		int type;
		std::string txt;
	};
		
	#pragma mark CONSOLE
		
	class Console
	{
		
		Lock lock;	// Lock as a control mechanism to log things into the console
		
		bool ncurses;		//!< Flag to indicate if we should use ncurses
		WINDOW *win;
		int nrows,ncols;
		bool quit_console;

		// Current and history command
		ConsoleCommand command;
		
		// Set of lines to plot of screen
		std::deque<ConsoleLine> lines;
		
		// Flag to indicate that some command is running
		bool running;
		std::string running_command;
		
	public:
		
		/**
		 Create a new console
		 if "ncurses" --> ncurses based interface
		 else --> simple std::cerr interface
		 */
		
		Console( bool _ncurses );
		virtual ~Console(){}

		void clearConsole();
		virtual std::string getPrompt();
		virtual std::string getHeader();
		void runCommand( std::string command );
		virtual void evalCommand( std::string command );
		
		/* Methods to write things on screen */
		
		void writeWarningOnConsole( std::string message );
		void writeErrorOnConsole( std::string message );

		void writeOnConsole( std::string message );

		void writeBlockOnConsole( std::string message );

		static bool checkArgs( int arg , char *argv[]);
		
		void quit();
		
		/** Main function to run the console */
		
	private:
		
		void printLines();
		void printCommand();
		void _print();
		void print();
		
		static std::vector<std::string> getLines( std::string txt );
		void addLines( int type , std::vector<std::string> txts );

		
	public:

		void run();
		void run_console();
		void run_ncurses();

	public:
		static void cancel_ncurses(void);
		
	};
	
}	//namespace au

#endif
