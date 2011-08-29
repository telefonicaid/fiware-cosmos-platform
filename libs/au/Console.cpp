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
        if( pthread_self() == t )
            write( message );
        else
            write2( message );
	}


	void Console::write( std::string message )
	{
        if( pthread_self() != t )
        {
            // Accumulate message
            pending_messages.push_back( message );
        }
        
		std::ostringstream output;
		output << "\r" << message << "\n";
		std::cout << output.str();
		std::cout.flush();

	}

	void Console::write2( std::string message )
	{
		std::ostringstream output;
		output << "\r" << message << "\n";
		std::cerr << output.str();
		std::cerr.flush();
        
	}
    

	void Console::quitConsole()
	{
		write_history(NULL);
		quit_console = true;
		// Do something to force quit?
	}
	

	void Console::runConsole()
	{
        // Keep the identifier of the thread
        t = pthread_self();
        
		quit_console = false;
		
		while ( !quit_console )
		{
			fflush(stdout);
			fflush(stderr);
			std::cout.flush();
			std::cerr.flush();
			fprintf(stdout, "\n");
            
			char *line = readline(getPrompt().c_str());
			
			if (line)
			{
				if (line[0] != 0)
					add_history(line);
                
				evalCommand(line);
                
				free(line);
                
                if ( pending_messages.size() > 0 )
                {
                    write( "----------------------------------------------------" );
                    while( pending_messages.size() > 0 )
                    {   
                        write( pending_messages.front() );
                        pending_messages.pop_front();
                    }
                    write( "----------------------------------------------------" );
                }
			}
            
		}

		write_history(NULL);
	}
}
