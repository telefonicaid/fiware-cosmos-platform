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
		Delilah* delailh;	// Pointer to the main class
		
	public:
		DelilahConsole(  Delilah* _delailh  , bool ncurses) : au::Console( ncurses )
		{
			delailh = _delailh;
		}
		
		virtual std::string getPrompt()
		{
			return  "Delilah> ";
		}
		
		virtual std::string getHeader()
		{
			return  "Delilah";
		}
		
		virtual void evalCommand( std::string command );

		
	};

}


#endif
