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
namespace ss {

	class Delilah;

	
	void cancel_ncurses(void);
	
	class DelilahConsole : public au::Console
	{
		Delilah* dalilah;	// Pointer to the main class
		size_t id;			// Id counter of the command - messages sent to controller
		
	public:
		DelilahConsole(  Delilah* _dalilah , bool ncurses) : au::Console( ncurses )
		{
			dalilah = _dalilah;
			id = 0;
			
			
			if( ncurses )
				atexit ( cancel_ncurses );
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
		
	};

}


#endif
