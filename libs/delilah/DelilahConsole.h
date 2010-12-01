#ifndef _H_DelilahConsole
#define _H_DelilahConsole

/* ****************************************************************************
 *
 * FILE                     DelilahConsole.h
 *
 * DESCRIPTION			   Console terminal for delilah
 *
 */

#include "Console.h"			// au::Console
#include <cstdlib>				// atexit(.)
#include "DelilahClient.h"      // ss:DelilahClient

namespace ss {

	class Delilah;

	
	void cancel_ncurses(void);
	
	class DelilahConsole : public au::Console, public DelilahClient
	{
		Delilah* dalilah;	// Pointer to the main class
		
	public:
		DelilahConsole(  Delilah* _dalilah , bool ncurses) : au::Console( ncurses )
		{
			dalilah = _dalilah;
			
			
			if( ncurses )
				atexit ( cancel_ncurses );
		}
		
		int run(int argc , const char * argv[] )
		{
			au::Console::run();	// au::Console
			return 0;
		}
		
		virtual std::string getPrompt()
		{
			return  "Delilah> ";
		}
		
		virtual std::string getHeader()
		{
			return  "Delilah";
		}
		
		// Eval a command from the command line
		virtual void evalCommand( std::string command );

		// PacketReceiverInterface
		int receive(int fromId, Message::MessageCode msgCode, Packet* packet);		

		virtual void quit()
		{
			au::Console::quit();
		}
	
		virtual void loadDataConfirmation( DelilahUploadDataProcess *process);		
	
		
		virtual void showMessage( std::string message)
		{
			writeWarningOnConsole( message );
		}

		
	};

}


#endif
