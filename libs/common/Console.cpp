
#include "Console.h"		// Own interface

namespace au
{
	
	Console::Console()
	{
		quit_console  = false;
		writeCallback = NULL;

		// Init history of the readline lib
		read_history(NULL);
	}


	
	std::string Console::getPrompt()
	{
		return  "> ";
	}


	
	void Console::evalCommand( std::string command )
	{
		writeWarningOnConsole( "AUConsole method evalCommand not implemented yet.");
		writeWarningOnConsole( std::string("Message to process: ") + command );
	}
	


	/* Methods to write things on screen */


	
	void Console::writeWarningOnConsole( std::string message )
	{
		if (writeCallback != NULL)
			writeCallback("WARNING", message.c_str());
		else
		{
			std::cout << "\r\033[1;35m"<< message << "\033[0m\n";
			rl_forced_update_display();
		}
	}


	
	void Console::writeErrorOnConsole( std::string message )
	{
		if (writeCallback != NULL)
			writeCallback("ERROR", message.c_str());
		else
		{
			std::cout << "\r\033[1;31m"<< message << "\033[0m\n";
			rl_forced_update_display();
		}
	}



	void Console::writeOnConsole( std::string message )
	{
		if (writeCallback != NULL)
			writeCallback("MESSAGE", message.c_str());
		else
		{
			std::cout << "\r" << message << "\n";
			rl_forced_update_display();
		}
	}



	void Console::quit()
	{
		write_history(NULL);
		quit_console = true;
		// Do something to force quit?
	}


	
	void Console::run()
	{
		quit_console = false;
		
		while (!quit_console)
		{
			fflush(stdout);
			fflush(stderr);
			std::cout.flush();
			std::cerr.flush();
			
			char *line = readline(getPrompt().c_str());
			
			if (line)
			{
				add_history(line);
				evalCommand(line);
				free(line);
			}
			
			// rl_line_buffer[0] = '\0';
		}
		
		write_history(NULL);
	}
}
