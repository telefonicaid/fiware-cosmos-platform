#ifndef _H_AU_CONSOLE_COMMAND
#define _H_AU_CONSOLE_COMMAND

#include <string.h>           /* memcpy, ...                                 */
#include <istream>
#include <cstdlib>
#include <iostream>
#include <curses.h>
#include <deque>

#include "CommandLine.h"
#include "Lock.h"                /* Lock                            */

namespace au {
	
	
	class ConsoleCommand
	{
		
	public:
		
		std::deque<std::string> history;
		int pos_history;			// Position to lock up at history
		
		
		char command[1000];		// Vector holding the current command
		int length;				// Full command length
		int cursor_pos;			// Possition of the cursor
		
		
		ConsoleCommand();
		std::string confirmCommand();		
		void notifyString( std::string txt );		
		void notifyChar( char c );		
		void init();
		void moveLeft();
		void moveRigth();
		void delete_character();
		void previous();
		void next();
		
	};
	
}

#endif

	
