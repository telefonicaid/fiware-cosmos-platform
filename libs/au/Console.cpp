
#include <termios.h>                // termios

#include "logMsg/logMsg.h"          // LM_M

#include "au/TokenTaker.h"
#include "au/string.h"

#include "au/Console.h"	            // Own interface

namespace au
{
    
    bool isInputReady()
    {
        
        struct termios old_tio, new_tio;
        
        /* get the terminal settings for stdin */
        tcgetattr(STDIN_FILENO,&old_tio);
        
        /* we want to keep the old setting to restore them a the end */
        new_tio=old_tio;
        
        /* disable canonical mode (buffered i/o) and local echo */
        new_tio.c_lflag &=(~ICANON & ~ECHO);
        
        /* set the new settings immediately */
        tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);
        
        struct timeval  timeVal;
        timeVal.tv_sec  = 0;
        timeVal.tv_usec = 0;
        
        fd_set          rFds;
        FD_ZERO(&rFds);
        FD_SET(0, &rFds);
        
        int s = select( 1, &rFds, NULL, NULL, &timeVal);
        
        /* set the new settings immediately */
        tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
        
        return (s==1);
        
    }
    
    
	Console::Console() : token_pending_messages("token_pending_messages")
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
        if( pthread_self() != t )
        {
            // Accumulate message
            au::TokenTaker tt(&token_pending_messages);
            pending_messages.push_back( message );
            return;
        }
        
        printf("%s\n", strToConsole(message).c_str() );
        fflush( stdout );
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
			fprintf(stdout, "\n");
			fflush(stdout);
            
            // Print the prompt
            printf("%s", getPrompt().c_str() );
            fflush(stdout);
            
            while( !isInputReady() )
            {
                {
                    au::TokenTaker tt(&token_pending_messages);
                    if ( pending_messages.size() > 0 )
                    {
                        // remove the prompt
                        //printf("\r" );
                        printf("\r                                                 \r" );
                        fflush(stdout);
                        
                        fprintf(stdout, "\n");
                        fflush(stdout);
                        
                        while( pending_messages.size() > 0 )
                        {   
                            write( pending_messages.front() );
                            pending_messages.pop_front();
                        }
                        
                        fprintf(stdout, "\n");
                        fflush(stdout);
                        
                        
                        // Print the prompt again
                        printf("%s", getPrompt().c_str() );
                        fflush(stdout);
                        
                    }
                }
                
                usleep(100000);
                
            }

            // Remove the promt ( to be printed back by the readLine function
            printf("\r                                     \r" );
            fflush(stdout);
            
			char *line = readline(getPrompt().c_str());
			
			if (line)
			{
				if (line[0] != 0)
					add_history(line);
                
                fprintf(stdout, "\n");
                fflush(stdout);
                
				evalCommand(line);
                
                fprintf(stdout, "\n");
                fflush(stdout);
                
				free(line);
			}
            
		}

		write_history(NULL);
	}
}
