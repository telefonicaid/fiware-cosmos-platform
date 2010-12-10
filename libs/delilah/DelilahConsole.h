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
#include "logMsg.h"				
#include "Delilah.h"			// ss::Delilah

namespace ss {
	
	void cancel_ncurses(void);
	
	class DelilahConsole : public au::Console, public DelilahClient
	{
		Delilah* delilah;	// Internal delilah object to interact with SAMSON
		
	public:
		
		DelilahConsole( NetworkInterface *_network, const char* controller, int workers, int endpoints , bool ncurses) : au::Console( ncurses )
		{
			// Create an internal delilah object to interact with SAMSON
			// and set myself as a client for this delilah object
			
			delilah = new Delilah( _network , controller , workers , endpoints);
			delilah->client =  this;	

			// Prepare the atexit command to cancel ncurses effect over the console
			if( ncurses )
				atexit ( cancel_ncurses );
		}
		
		~DelilahConsole()
		{
			delete delilah;
		}
		
		int run( )
		{
			au::Console::run();	// au::Console run
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
		
		virtual void notifyFinishOperation( size_t id )
		{
			std::ostringstream output;
			output << "Finished local delilah process with : " << id ;
			writeWarningOnConsole( output.str() );
		}


		
	};

}


#endif
