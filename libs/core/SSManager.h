#ifndef SAMSON_MANAGER_H
#define SAMSON_MANAGER_H

#include <pthread.h>
#include <iostream>
#include <fstream>
#include <queue>
#include <set>
#include <iostream>
#include <assert.h>
#include <map>
#include <pthread.h>
#include <cstdlib>
#include <sys/stat.h>
#include <fstream>
#include <pthread.h>

#include "AUConsole.h"



namespace ss {
	
	/**
	 
	 SAMSON Manager:
	 General element of a samson-node
		- Create all the elements in memory
		- Create all necessary threads
	  */

	
	class SSManagerConsole : public au::Console {
		
	public:
		
		SSManagerConsole( bool ncurses ) : au::Console( ncurses )
		{
		}
		
		// Control of the main console (debug & test only)
		virtual void evalCommand(std::string command);
		virtual std::string getPrompt();
		virtual std::string getHeader();
	};
	
#pragma mark Main Manager
	
	class SSManager
	{

		SSManager( );			
		SSManagerConsole * console;		// Local console if required
		
	public:
		
		bool finish;				//!< Global variable to mark end of execution of all threads

		static SSManager *shared();							//!< Singleton implementation of SSManager
		void run( int args , const char *argv[] );			//!< Init function ( it never returns)
		
		
	};

}

#endif
