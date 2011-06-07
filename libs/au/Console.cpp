#include "logMsg/logMsg.h"         // LM_M
#include "au/Console.h"	           // Own interface


namespace au
{
	Console::Console()
	{
		quit_console  = false;

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
		std::ostringstream output;
		output << "\033[1;35m"<< message << "\033[0m";
		write( output.str() );
	}
	

	void Console::writeErrorOnConsole( std::string message )
	{
		std::ostringstream output;
		output << "\033[1;31m"<< message << "\033[0m";
		write( output.str() );
	}


	void Console::writeOnConsole( std::string message )
	{
		write( message );
	}


	void Console::write( std::string message )
	{
		std::ostringstream output;
		output << "\r" << message << "\n";
		std::cout << output.str();
		std::cout.flush();

		rl_forced_update_display();
		rl_redisplay();
	}


	void Console::quitConsole()
	{
		write_history(NULL);
		quit_console = true;
		// Do something to force quit?
	}
	

	void Console::runConsole()
	{
		quit_console = false;
		
		while ( !quit_console )
		{
			fflush(stdout);
			fflush(stderr);
			std::cout.flush();
			std::cerr.flush();
			
			char *line = readline(getPrompt().c_str());
			
			if (line)
			{
				if (line[0] != 0)
					add_history(line);
				evalCommand(line);
				free(line);
			}

			// rl_line_buffer[0] = '\0';
		}

		write_history(NULL);
	}
}
