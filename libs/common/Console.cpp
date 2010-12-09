
#include "Console.h"		// Own interface

namespace au
{
	
	Console::Console( bool _ncurses )
	{
		quit_console = false;
		running = false;
		ncurses = _ncurses;
	}
	
	void Console::clearConsole()
	{
		if ( ncurses )
			lines.clear();
		else
		{
			char au_clearScreen[] = { 0x1b, '[', '2', 'J', 0x1b, '[', '0', ';' ,'0','H', 0 };
			std::cerr << au_clearScreen;
		}
	}
	
	std::string Console::getPrompt()
	{
		return  "> ";
	}
	
	std::string Console::getHeader()
	{
		return  "AU Console";
	}
	
	void Console::runCommand( std::string command )
	{
		//std::stringstream run_command;
		//run_command << "Run: " << command;
		//writeOnConsole( run_command.str() );
		
		// Proper command of the console
		CommandLine cmdLine;
		cmdLine.parse(command);
		
		/*
		 // No clear command by default
		 if( cmdLine.get_num_arguments() == 0)
		 {
		 clearConsole();
		 return;
		 }
		 */
		
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
	
	void Console::evalCommand( std::string command )
	{
		writeWarningOnConsole( "AUConsole method evalCommand not implemented yet.");
		writeWarningOnConsole( std::string("Message to process: ") + command );
	}
	
	/* Methods to write things on screen */
	
	void Console::writeWarningOnConsole( std::string message )
	{
		if( ncurses )
			addLines( ConsoleLineWarning , getLines(message) );
		else
			std::cerr << message << std::endl;
		
	}
	
	void Console::writeErrorOnConsole( std::string message )
	{
		if( ncurses )
			addLines( ConsoleLineError , getLines(message) );
		else
			std::cerr << ">>> Error:\n" << message << std::endl;
		
	}
	
	void Console::writeOnConsole( std::string message )
	{
		if( ncurses )
			addLines( ConsoleLineNormal , getLines(message) );
		else
			std::cerr << message << std::endl;
	}
	
	void Console::writeBlockOnConsole( std::string message )
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
	
	
	bool Console::checkArgs( int arg , char *argv[])
	{
		au::CommandLine c;
		c.set_flag_boolean("console");
		return !c.get_flag_bool("console");
	}
	
	
	void Console::quit()
	{
		quit_console = true;
	}
	
	
	void Console::printLines()
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
	
	void Console::printCommand()
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
	
	void Console::_print()
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
	
	void Console::print()
	{
		lock.lock();
		_print();
		lock.unlock();
		
		
	}
	
	std::vector<std::string> Console::getLines( std::string txt )
	{
		std::vector<std::string> lines;
		
		
		std::istringstream input( txt );
		std::string line;
		while(std::getline(input, line)) {
			lines.push_back( line );
		}
		
		return lines;
	}
	
	void Console::addLines( int type , std::vector<std::string> txts )
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
	
	void Console::run()
	{
		if( ncurses )
			run_ncurses();
		else
			run_console();
	}
	
	void Console::run_console()
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
	
	void Console::run_ncurses()
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

	void Console::cancel_ncurses(void)	
	{
		echo();
		nocbreak();
		endwin();
	}
	
	
}