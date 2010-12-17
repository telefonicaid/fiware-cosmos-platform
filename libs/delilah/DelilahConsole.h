#ifndef _H_DelilahConsole
#define _H_DelilahConsole

/* ****************************************************************************
*
* FILE                     DelilahConsole.h
*
* DESCRIPTION			   Console terminal for delilah
*
*/
#include <cstdlib>				// atexit

#include "logMsg.h"				

#include "Console.h"			// au::Console
#include "DelilahClient.h"      // ss:DelilahClient
#include "Delilah.h"			// ss::Delilah



namespace ss {
	
	void cancel_ncurses(void);
	
	static void lmCursesCancel(int code, void* input, char* logLine, char* stre)
	{
	   cancel_ncurses();
	}

	class DelilahConsole : public au::Console, public DelilahClient
	{
		Delilah* delilah;	// Internal delilah object to interact with SAMSON
		
	public:
		
		DelilahConsole( Delilah *_delilah , bool ncurses) : au::Console( ncurses )
		{
			// Create an internal delilah object to interact with SAMSON
			// and set myself as a client for this delilah object
			
			delilah = _delilah;
			delilah->client =  this;	

			// Prepare the atexit command to cancel ncurses effect over the console
			if( ncurses )
			{
				atexit (cancel_ncurses);
				lmExitFunction(lmCursesCancel, NULL);
			}
		}
		
		~DelilahConsole()
		{
			delete delilah;
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
		
		
		void showQueues( const network::QueueList ql);
		void showDatas( const network::DataList ql);
		void showOperations( const network::OperationList ql);
		void showJobs( const network::JobList ql);
		void showWorkers( const network::ControllerStatus &cs, const network::WorkerStatusList ql);


		
	};

}


#endif
