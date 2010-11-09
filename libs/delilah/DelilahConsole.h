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

namespace ss {

	class Delilah;

	class DelilahConsole : public au::Console
	{
		Delilah* dalilah;	// Pointer to the main class
		
	public:
		DelilahConsole(  Delilah* _dalilah , bool ncurses) : au::Console( ncurses )
		{
			dalilah = _dalilah;
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
