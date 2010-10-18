/* ****************************************************************************
*
* FILE                     DelilahConsole.h
*
* DESCRIPTION			   Console terminal for delilah
*
*/
#include "Delilah.h"			// ss::Delailh
#include "DelilahConsole.h"		// Own interface



namespace ss
{
	void DelilahConsole::evalCommand( std::string command )
	{
		au::CommandLine commandLine;
		commandLine.parse( command );
		
		if( commandLine.get_num_arguments() == 0)
			return;	// no command
		
		
		std::string mainCommand = commandLine.get_argument(0);
		
		if( mainCommand == "quit" )
			delailh->quit();
		else
		{
			writeWarningOnConsole( "Method evalCommand not implemented yet completelly");
			writeWarningOnConsole( std::string("Message to process: ") + command );
		}
	}
}
