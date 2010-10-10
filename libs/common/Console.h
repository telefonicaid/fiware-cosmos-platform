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
#include "Lock.h"                /* Lock                            */



namespace au {

static char au_clearScreen[] = { 0x1b, '[', '2', 'J', 0x1b, '[', '0', ';' ,'0','H', 0 };
	
class ConsoleCommand
{

public:

	
	std::deque<std::string> history;
	int pos_history;			// Position to lock up at history

	
	char command[1000];		// Vector holding the current command
	int length;				// Full command length
	int cursor_pos;			// Possition of the cursor

	
	ConsoleCommand()
	{
		init();
		pos_history = 0;
		
	}
	
	std::string confirmCommand()
	{
		command[length]='\0';
		std::string cmd = std::string( command );
		
		history.push_front( cmd );
		if( history.size() > 100 )
			history.pop_back();
		
		init();
		
		return cmd;
	}
	
	void notifyString( std::string txt )
	{
		
		if( cursor_pos < length )
			for (int i = length-1 ; i >= cursor_pos  ; i--)
				command[i+txt.length()] = command[i];

		for (size_t i = 0 ; i < txt.length() ; i++)
		{
			command[cursor_pos]=txt[i];
			cursor_pos++;
			length++;
		}
		
		command[length]='\0';

		pos_history = 0;
		
	}
	
	void notifyChar( char c )
	{
		if( cursor_pos < length )
			for (int i = length-1 ; i >= cursor_pos  ; i--)
				command[i+1] = command[i];
		
		command[cursor_pos]=c;
		
		cursor_pos++;
		length++;
		
		// End of string mark
		command[length]='\0';
		
		pos_history = 0;
		
	}

	void init()
	{
		length = 0;
		cursor_pos = 0;
		command[0]='\0';
	}
	
	void moveLeft()
	{
		if( cursor_pos > 0)
			cursor_pos--;
		
		pos_history = 0;
	}
	
	void moveRigth()
	{
		if( cursor_pos < length )
			cursor_pos++;
		
		pos_history = 0;
	}
	
	void delete_character()
	{
		if( cursor_pos > 0 )
		{
			for (int i = cursor_pos ; i < length ; i++)
				command[i] = command[i+1];
			
			length--;
			cursor_pos--;
			
			// End of string mark
			command[length]='\0';

		}
		
		pos_history = 0;
	
	}
	
	
	void previous()
	{
		
		std::string previous_command = history[pos_history];
		length = previous_command.length();
		cursor_pos = length;
		memcpy(command, previous_command.c_str(), length );
		command[length]='\0';
		
		if( pos_history < (int)(history.size()-1) )
			pos_history++;

	}
	
	void next()
	{
		if ( pos_history > 0)
			pos_history--;
		
		std::string previous_command = history[pos_history];
		length = previous_command.length();
		cursor_pos = length;
		memcpy(command, previous_command.c_str(), length );
		command[length]='\0';
		
	}
	
	
};
	

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
	
	Console( bool _ncurses )
	{
		quit_console = false;
		running = false;
		ncurses = _ncurses;
	}
	
	virtual ~Console(){}

	void clearConsole()
	{
		if ( ncurses )
			lines.clear();
		else
			std::cerr << au_clearScreen;
	}

	virtual std::string getPrompt()
	{
		return  "> ";
	}
	
	virtual std::string getHeader()
	{
		return  "AU Console";
	}
	
	void runCommand( std::string command )
	{
		std::stringstream run_command;
		run_command << "Run: " << command;
		writeOnConsole( run_command.str() );
		
		// Proper command of the console
		CommandLine cmdLine;
		cmdLine.parse(command);

		if( cmdLine.get_num_arguments() == 0)
		{
			clearConsole();
			return;
		}
		
		if( cmdLine.get_num_arguments() == 1)
		{
			if( cmdLine.get_argument(0) == "clear" ) {
				clearConsole();
				return;
			}
			/*
			else if( cmdLine.get_argument(0) == "quit" ){
				quit_console=true;
				return;
			}
			 */
		
		}
		
		// Normal command
		evalCommand( command );
		
	}
	
	virtual void evalCommand( std::string command )
	{
		writeWarningOnConsole( "AUConsole method evalCommand not implemented yet.");
		writeWarningOnConsole( std::string("Message to process: ") + command );
	}

	/* Methods to write things on screen */
	
	void writeWarningOnConsole( std::string message )
	{
		if( ncurses )
			addLine( ConsoleLineWarning , message );
		else
			std::cerr << message << std::endl;

	}
	
	void writeErrorOnConsole( std::string message )
	{
		if( ncurses )
			addLine( ConsoleLineError , message );
		else
			std::cerr << "Error: " << message << std::endl;

	}

	void writeOnConsole( std::string message )
	{
		if( ncurses )
			addLine( ConsoleLineNormal , message );
		else
			std::cerr << message << std::endl;
	}

	void writeBlockOnConsole( std::string message )
	{
		if( ncurses )
		{
			std::vector<std::string> lines;

			std::stringstream ss(message);
			std::string item;
			while( std::getline(ss, item, '\n' )) {
				lines.push_back(item);
			}		
			
			addLines( ConsoleLineNormal , lines );
		}
		else
			std::cerr << message << std::endl;
		
		
	}
	
	
	void quit()
	{
		quit_console = true;
	}
	
	/** Main function to run the console */
	
private:
	
	void printLines()
	{
		// Only called in ncurses
		
		std::ostringstream cleanLine;
		for (int i = 0  ; i < ncols-1 ;i++)
			cleanLine << " ";
		
		for (int i = 0 ; i < nrows-5 ; i++)
			mvwprintw(win, nrows-3-i, 0, cleanLine.str().c_str() );			
		
		
		
		for (int i = 0 ; i < nrows-3 ; i++)
		{
			if( i < (int)lines.size() )
			{
				ConsoleLine line =  lines[ i ];
				
				switch (line.type) {
					case ConsoleLineNormal:
						attron(COLOR_PAIR(1));
						break;
					case ConsoleLineWarning:
						attron(COLOR_PAIR(2));
						break;
					case ConsoleLineError:
						attron(COLOR_PAIR(3));
						break;
					default:
						break;
				}

				
				std::ostringstream line_txt;
				line_txt << line.txt;
				
				mvwprintw(win, nrows-3-i, 0, line_txt.str().c_str() );			
				//mvwprintw(win, nrows-3-i, 0, "dsdkasdkajshdkjh" );			
				
				switch (line.type) {
					case ConsoleLineNormal:
						attroff(COLOR_PAIR(1));
						break;
					case ConsoleLineWarning:
						attroff(COLOR_PAIR(2));
						break;
					case ConsoleLineError:
						attroff(COLOR_PAIR(3));
						break;
					default:
						break;
				}
				
			}
			else
			{
				// Empty line
			
			}
			
		}
		
	}

	void printCommand()
	{
		// Only called in ncurses
		
		attron(COLOR_PAIR(1));
		
		if( running ) 
		{
			
			std::ostringstream reset_promt;
			for (int i = 0 ; i < ncols -1 ; i++)
				reset_promt << " ";
			mvwprintw(win, nrows-1, 0, reset_promt.str().c_str() );			
			
			std::ostringstream command_promt;
			command_promt << "RUNNING: " << running_command;
			mvwprintw(win, nrows-1, 0, command_promt.str().c_str() );
			move(nrows-1, 0 );
			
			
		}
		else
		{		
			
			std::ostringstream reset_promt;
			for (int i = 0 ; i < ncols -1 ; i++)
				reset_promt << " ";
			mvwprintw(win, nrows-1, 0, reset_promt.str().c_str() );			
			
			std::ostringstream command_promt;
			std::string p = getPrompt();
			
			command_promt << p << " " << command.command ;
			mvwprintw(win, nrows-1, 0, command_promt.str().c_str() );
			move(nrows-1, command.cursor_pos + 1 + p.length() );
			
		}
		
		attroff(COLOR_PAIR(1));
		
	}	
	
	void _print()
	{
		// Print header
		attron(COLOR_PAIR(1));
		attron(A_BOLD);
		std::stringstream header;
		header << getHeader();
		mvwprintw(win, 0, 0, header.str().c_str() );			
		attroff(A_BOLD);
		attroff(COLOR_PAIR(1));
		
		
		printLines();
		printCommand();
		wrefresh(win);
	}	
	
	void print()
	{
		lock.lock();
		_print();
		lock.unlock();

		
	}
	
	void addLine( int type , std::string txt )
	{
		ConsoleLine line;
		line.type = type;
		line.txt = txt;

		lock.lock();
		
		lines.push_front( line );
		if( (int)lines.size() > nrows )
			lines.pop_back();
		
		_print();

		lock.unlock();
		
	}

	void addLines( int type , std::vector<std::string> txts )
	{
		
		lock.lock();


		for (size_t i = 0 ; i< txts.size();i++)
		{
			ConsoleLine line;
			line.type = type;
			line.txt = txts[i];
			
			lines.push_front( line );
			if( (int)lines.size() > nrows )
				lines.pop_back();
		}
		
		_print();
		
		lock.unlock();
		
	}	
	
public:

	void run()
	{
		if( ncurses )
			run_ncurses();
		else
			run_console();
	}
	
	void run_console()
	{
		 char line[1000];
		 quit_console = false;
		 while( ! quit_console )
		 {
			 std::cout << getPrompt();
			 std::cin.getline(line,1000);
			 runCommand( line );
		 }
		
	}
	
	void run_ncurses()
	{
		win = initscr(); // curses call to initialize window
		if( !win )
		{
			std::cout << "No screen suported";
			exit(0);
		}
		start_color();

		cbreak(); // curses call to set no waiting for Enter key
		noecho(); // curses call to set no echoing
		getmaxyx(win,nrows,ncols); // curses call to find size of window
		clear(); // curses call to clear screen, send cursor to position (0,0)
		refresh(); // curses call to implement all changes since last refresh
		

		// Init pairs of colors
		init_pair(1, COLOR_GREEN, COLOR_BLACK);			// Normal
		init_pair(2, COLOR_MAGENTA, COLOR_BLACK);		// Warning
		init_pair(3, COLOR_RED, COLOR_BLACK);			// Error
		
		// Line on top/bottom of the command line
		attron(COLOR_PAIR(1));
		std::ostringstream line;
		for (int i = 0 ; i < (ncols-1) ; i++)
			line << "-";
		mvwprintw(win, nrows-2, 0, line.str().c_str() );			
		mvwprintw(win, 1, 0, line.str().c_str() );			
		wrefresh(win);
		attroff(COLOR_PAIR(1));

		while( !quit_console )
		{		
			// Empty prompt
			command.init();
			print();
			
			
			int c = getch();
			while( c != '\n' )
			{
				if( c == 27 )
				{
					c = getch();
					if( c == 91)
					{
						c = getch();
						switch (c) {
							case 65:	// UP
								command.previous();
								break;
							case 66:	// DOWN
								command.next();
								break;
							case 68:	// LEFT
								command.moveLeft();
								break;
							case 67:  // RIGTH
								command.moveRigth();
								break;
							default:
								break;
						}
					}
				}
				else if( c==127)	// DELETE
				{	
					command.delete_character();
				}
				else
				{
					// If we are not at the end (insert mode)
					command.notifyChar(c);
					
					/*
					std::ostringstream o;
					o << "(" << c << ")";
					command.notifyString(o.str());
					 */
					
				}
				
				
				// Get the next character
				print();
				c = getch();
			}
			
			
			// Do something with the command
			running_command = command.confirmCommand();
			running = true;
			print();
			runCommand( running_command );
			running = false;
			
			
		}
		
		
		echo();
		nocbreak();
		endwin();
		
		
		
	}
	
};
}	//namespace au

#endif
