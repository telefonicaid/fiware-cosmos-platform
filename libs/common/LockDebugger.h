#ifndef SAMSON_LOCK_DEBUGGER_H
#define SAMSON_LOCK_DEBUGGER_H

/* ****************************************************************************
*
* FILE                 LockDebugger.h - 
*
* AUTHOR               Ken Zangelin, Andreu Urruela
*
* CREATION DATE        Oct 5 2010
*
*/
#include <string>                /* std::string                              */
#include <map>                   /* map                                      */
#include <set>                   /* set                                      */
#include <pthread.h>             /* pthread_mutex_t                          */




namespace au {

class Lock;

	class LockDebugger
	{
		pthread_mutex_t _lock;
		
		std::map< pthread_t , std::set< void* >* > locks;
		
		LockDebugger();
		~LockDebugger();
		
	public:
		
		pthread_key_t key_title;	// A title for each thread
		
		static void setThreadTitle( std::string );
		
		void add_lock( void* new_lock );
		void remove_lock(  void* new_lock );
		
		static LockDebugger* shared();
		
	private:
		
		std::string _getTitle();
		std::set<void*> * _getLocksVector();	
		bool _cross_blocking( void* new_lock );
		

	};
}

#endif
