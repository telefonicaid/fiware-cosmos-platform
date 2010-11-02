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
		
		virtual void evalCommand( std::string command );

		
		// Method to process a message received from the controller
		void receivedMessage( size_t id , bool error , bool finished , std::string message )
		{
			// Todo with the received packet
			std::ostringstream txt;
			txt << "----------------------------------------------------------------" << std::endl;
			txt << "Answer for command " << id << ": ";
			
			if( finished )
				txt << "(FINISHED)";
			
			txt << std::endl;
			txt << "----------------------------------------------------------------" << std::endl;
			txt << message << std::endl;
			txt << "----------------------------------------------------------------" << std::endl;
			
			
				if (error)
					writeErrorOnConsole(txt.str());
				else
					writeOnConsole(txt.str());
			
		}
		
		
	};

}


#endif
